#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>

#define PATTERN_MAX 2048

void handler();
void printout(char*, int, char*);
int findPattern(int, char*, char**, int, int);

jmp_buf env;
int printed = 0;

int main(int argc, char **argv) {
    
    signal(SIGBUS, handler);
    
    int context = 0;
    char *pattern = malloc(sizeof(char)*(PATTERN_MAX+1));
    pattern = "\0";
    char ch;
    int patternfd;
    int patternset = 0;
    struct stat stat_struct;
    struct stat stdin_stats;
    char* stdinfilename;
    int redirect = 0;
    int isStdin = 0;
    while ((ch = getopt(argc, argv, "c:p:")) != -1) {
        switch(ch) {
            case 'c':
                if (sscanf(optarg, "%d", &context) != 1) { //If it gets a string
                    fprintf(stderr, "Invalid argument: %s. Usage: -c should precede an integer.\n", optarg);
                    return -1;
                }
                if (context <= 0) {
                    fprintf(stderr, "Invalid context bytes value %d. Usage: -c should provide a positive integer.\n", context);
                    return -1;
                }
                break;
            case 'p':
                if (strlen(optarg) >= PATTERN_MAX) {
                    fprintf(stderr, "Invalid argument: %s. The pattern should be fewer than 2048 characters.\n", optarg);
                    return -1;
                }
                patternfd = open(optarg, O_RDONLY);

                if (lstat(optarg, &stat_struct) != 0) {
                    fprintf(stderr, "Failed to get stat struct on %s: %s\n", optarg, strerror(errno));
                    return -1;
                }
                pattern = mmap(NULL, stat_struct.st_size, PROT_READ, MAP_SHARED, patternfd, 0);
                patternset = 1;
                break;
        }
    }
    char** files = malloc(sizeof(char)*(argc-optind)*(PATTERN_MAX+1));
    if (patternset == 0) {
        pattern = argv[optind];
    }
    int i = 0;
    for (; i < argc-optind; i++) {
        if (patternset == 0) {
            patternset = 2;
            continue;
        }
        files[(patternset == 2) ? i-1 : i] = argv[optind+i]; //Start at 0 always
    }

    if ((patternset == 1 && argc == optind) || (patternset == 2 && argc-1 == optind)) {
        redirect = 1;
        if (fstat(0, &stdin_stats) != 0) {
            fprintf(stderr, "Failed to get stat struct from standard input. Error: %s.\n", strerror(errno));
            return -1;
        }
        files[0] = mmap(NULL,stdin_stats.st_size, PROT_READ, MAP_SHARED, 0, 0);
        isStdin = 1;
    }

    int numfiles = (redirect == 1 ? 1 : (patternset == 2 ? i-1 : i));

return findPattern(context, pattern, files, numfiles, isStdin);
}

int findPattern(int context, char* pattern, char** files, int numfiles, int isStdin) {

    int fd;
    int ret = 0;
    struct stat stat_struct;
    char* p;
    int ploop = 0;
    int i = 0;
    if (isStdin) {
        numfiles = 1;
        i = 0;
        p = files[0];
        files[0] = "Standard Input"; //for printout
        goto label;
    }
    printed = 0;
    //Use the length of the file to get the block of memory
    labeltwo: for (; i < numfiles; i++) {
        ploop = 0;
        if (lstat(files[i], &stat_struct) != 0) {
            fprintf(stderr, "Failed to get stat struct on %s: %s\n", files[i], strerror(errno));
            return -1;
        }

        fd = open(files[i], O_RDONLY);
        p = mmap(NULL, stat_struct.st_size, PROT_READ, MAP_SHARED, fd, 0);
        label:for (; ploop < (isStdin == 1) ? strlen(files[0]) : stat_struct.st_size; ploop++) {
            if (setjmp(env) != 0) {
                ret = -1;
                continue;
            }
            if (!p[ploop]) {
                i++;
                goto labeltwo;
            }
            if (p[ploop]==pattern[0]) {
                int cnt = 0;
                int numlooped = 1;
                if (strlen(pattern) == 1){
                    if (ploop-1-context < 0) {
                        printout(&p[0], (context+strlen(pattern)), files[i]); //handle overflow off back in print fcn
                        ploop++;
                        goto label;
                    } else {
                        printout(&p[ploop-context], 2*context+strlen(pattern), files[i]);
                        ploop++;
                        goto label;
                    }
                }else{
                    while(++ploop < (isStdin==1) ? strlen(files[0]) : stat_struct.st_size && cnt < strlen(pattern) && p[ploop] == pattern[++cnt]) {
                    if (setjmp(env) != 0) {
                        ret = -1;
                        i++;
                        goto labeltwo;
                    }
                        numlooped++;
                        if (numlooped == strlen(pattern)-1) {
                            if (ploop-context < 1) {
                                printout(&p[0], (2*context+strlen(pattern)+(ploop-2-context)-1), files[i]); //handle overflow off back in print fcn
                            } else {
                                printout(&p[ploop-context-strlen(pattern)+2], 2*context+strlen(pattern), files[i]);
                            }
                            goto label;
                        }
                    }
                }
            }
        }
    }
if (ret == -1)
    return -1;
if (printed != 0)
    return 0;
return 1;
}

void printout(char* printstr, int numchars, char* file) {
    printed=1;
    printf("%s:", file);
    for (int i = 0; i < numchars; i++) {
        if (printstr[i] != '\0') {
            if (printstr[i] > 32 || printstr[i] == ' ')
                printf("%c", printstr[i]);
            else
                printf("?");
        }
    }
    printf("\n");

return;
}

void handler() {
    fprintf(stderr, "SIGBUS received while processing files\n");
    longjmp(env, 0);
    return;
} 

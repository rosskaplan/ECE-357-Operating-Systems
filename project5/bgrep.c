#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define PATTERN_MAX 2048

int findPattern(int, char*, char**, int);

int main(int argc, char **argv) {
    int context = 0;
    char *pattern = malloc(sizeof(char)*(PATTERN_MAX+1));
    pattern = "\0";
    char ch;
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
                pattern = optarg;
                break;
        }
    }
    char** files = malloc(sizeof(char)*(argc-optind)*(PATTERN_MAX+1));
    int i = 0;
    for (; i < argc-optind; i++) {
        files[i] = argv[optind+i];
    }

if (findPattern(context, pattern, files, i) == -1)
    return -1;

return 0;
}

int findPattern(int context, char* pattern, char** files, int numfiles) {

    struct stat stat_struct;
    //Use the length of the file to get the block of memory
    for (int i = 0; i < numfiles; i++) {
        if (lstat(files[i], &stat_struct) != 0) {
            fprintf(stderr, "Failed to get stat struct on %s: %s\n", files[i], strerror(errno));
            return -1;
        }
    int p = mmap(NULL, stat_struct.st_size, 

}

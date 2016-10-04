#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
void findFiles(char*, char*, char*, char*);

int main(int argc, char **argv) {

    int ch;
    int uid = 0;
    int time = 0;
    char* target = NULL;
    struct passwd *usr;
    while ((ch = getopt(argc, argv, "u:m:xl:")) != -1) {
        switch(ch) {
            case 'u':
                if (sscanf(optarg, "%d", &uid) != 1) { //If it gets a string
                    if ((usr = getpwnam(optarg)) == NULL) {
                        fprintf(stderr, "User cannot be found with name %s", optarg);
                        return -1;
                    }
                    uid = usr->pw_uid;
                }
                break;
            case 'm':
                if (sscanf(optarg, "%d", &time) != 1) { //If it gets a string
                    fprintf(stderr, "Invalid argument: %s. Usage: -m should precede integer.\n", optarg);
                    return -1;
                } else {
                    printf("%d\n", time);
                } 
                break;
            case 'x':
                break;
            case 'l':
                target = optarg;
                break;
        }
    }

    char* path = NULL;
    if (optind < argc) {
        path = argv[optind];
    } else {
        fprintf(stderr, "Did not get a starting path.  Please try again.");
        return -1;
    }

return 0;
}

void findFiles(char* uid, char* time, char* target, char* path) {

    printf("%s%s%s%s\n", uid, time, target, path);
    return;
}

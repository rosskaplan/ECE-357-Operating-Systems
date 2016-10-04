#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>
#include <dirent.h>
void findFiles(int, int, char*, char*);

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
                } 
                break;
            case 'x':
                break;
            case 'l':
                target = optarg; //Error checking later when we open the directory
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

    findFiles(uid, time, target, path);

return 0;
}

void findFiles(int uid, int time, char* target, char* path) {

    DIR *directory;
    struct dirent *rd;

    if ((directory = opendir(path)) == NULL) {
        fprintf(stderr, "Failed to open directory %s: %s\n", path, strerror(errno));
        exit(-1);
    }

    while ((rd=readdir(directory)) != NULL) {
        if ((strcmp(rd->d_name, ".") == 0) || (strcmp(rd->dname, "..") == 0)) {
            continue; //Don't recurse over these
        }
        printf("%s\n", rd->d_name);
    }

    //printf("%d %d %s %s\n", uid, time, target, path);
    return;
}

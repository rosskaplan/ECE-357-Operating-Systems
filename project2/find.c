#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void findFiles(char*, char*, char*, char*);

int main(int argc, char **argv) {

    int ch;
    int uid = 0;
    char* time = NULL;
    char* target = NULL;
    while ((ch = getopt(argc, argv, "u:m:xl:")) != -1) {
        switch(ch) {
            case 'u':
                if (sscanf(optarg, "%d", &uid) != 1) {
                    //String -> User's Name
                    printf("User's name\n");
                    printf("%d\n",uid);
                } else {
                    //Integer -> UID
                    printf("User's ID\n");
                    printf("%s\n", optarg); 
                }
                break;
            case 'm':
                time = optarg;
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

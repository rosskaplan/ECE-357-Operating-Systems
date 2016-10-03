#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char **argv) {

    int ch;
    int user, mtime, x, l;
    user = mtime = x = l = 0;
    char *uid = NULL;
    char *time = NULL;
    while ((ch = getopt(argc, argv, "u:m:xl")) != -1) {
        switch(ch) {
            case 'u':
                user = 1;
                uid = optarg;
                printf("found a 'u' and 'uid' is %s\n", uid);
                break;
            case 'm':
                mtime = 1;
                time = optarg;
                printf("found a 'm' and 'time' is %s\n", time);
                break;
            case 'x':
                x = 1;
                printf("found a x\n");
                break;
            case 'l':
                l = 1;
                printf("found a l\n");
                break;
        }
    }

return 0;
}

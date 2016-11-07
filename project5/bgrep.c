#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define PATTERN_MAX 2048

int main(int argc, char **argv) {
    int context;
    char *pattern = malloc(sizeof(char)*(PATTERN_MAX+1));
    char ch;
    while ((ch = getopt(argc, argv, "c:p:")) != -1) {
        switch(ch) {
            case 'c':
                printf("here");
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
    printf("optind: %d", optind);
    char** files;
    for (int i = 0; i < argc-optind; i++) {
        files[i] = argv[optind+i-1];
    }
    printf("%d %s %s %s", context, pattern, files[0], files[1]);

return 0;
}

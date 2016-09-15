#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {

    for(int i = 1; i < argc; i++) {
        if (argc == 1) {
            printf("You didn't provide any input or output files!\n");
            return 0;
        }
        //Next cases: no output file --> standardout
        //get all input files into new array, then start the opening
        for (i = 1; i < argc; i++) {
            if (strcmp(argv[i],"-") == 0) {
                printf("found a \"-o\"\n");
            }

        }
        else if (strcmp(argv[i],"-") == 0) {

            //need to read standard input
            //SPECIAL NOTE: Open once, don't re-open each time, don't close it
            //SPECIAL NOTE: Therefore can be done multiple times
        } else {
            
        }
    }

return 0;
}

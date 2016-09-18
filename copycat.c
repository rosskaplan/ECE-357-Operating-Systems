#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void processInput(int, char **, int, int, char*);

int main(int argc, char **argv) {

    if (argc == 1) {
        printf("You didn't provide any input or output files!\n");
        return 0;
    }

    //Next cases: no output file --> standardout
    //get all input files into new array, then start the opening
    char* outputfile;
    int bytes = -1; 
    int start = 1;
    if (strcmp(argv[1], "-b") == 0) {
        bytes = strtol(argv[2], NULL, 10);
        start = 3; 
        if (strcmp(argv[3], "-o") == 0) {
            outputfile = argv[4];
            start = 5;
        }
    } else if (strcmp(argv[1], "-o") == 0) {
        outputfile = argv[2];
        start = 3;
    }

    processInput(argc, argv, start, bytes, outputfile);
    
return 0;
}

void processInput(int argc, char** argv, int start, int bytes, char* outputfile) {

    char intext[30];
    for (int i = start; i < argc; i++) {
        if (strcmp(argv[i],"-") == 0) {
            scanf("%s\n", inputfile);
            printf("%s\n", inputfile);
        } else {
            printf("found an input file name called \"%s\"\n", argv[i]);
        }
            //SPECIAL NOTE: Open once, don't re-open each time, don't close it
            //SPECIAL NOTE: Therefore can be done multiple times
    }


return;
}

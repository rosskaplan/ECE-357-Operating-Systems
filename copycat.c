#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h> //Defines O_APPEND, O_CREAT, etc. as constants
#include <errno.h>
#include <unistd.h>
#include <sys/types.h> //Necessary on some operating systems
#include <sys/stat.h> //Necessary on some operating systems

void processInput(int, char **, int, int, char*);
void errorOutput(char*, char*, char*);

int main(int argc, char **argv) {

    if (argc == 1) {
        printf("You didn't provide any input or output files!\n");
        return 0;
    }
    char* outputfile;
    int bytes = -1; //Just to note whether bytes was provided.
    int start = 1;
    if (strcmp(argv[1], "-b") == 0) {
        bytes = strtol(argv[2], NULL, 10);
        if (bytes < 2)
            errorOutput("Buffer size inputted is not valid","", "Please enter something larger");
        start = 3; 
        if ((argc > 5) && strcmp(argv[3], "-o") == 0) {
            outputfile = argv[4];
            start = 5;
        }
        
    } else if ((argc > 3) && strcmp(argv[1], "-o") == 0) {
        outputfile = argv[2];
        start = 3;
    }
    processInput(argc, argv, start, bytes, outputfile);
   
return 0;
}

void processInput(int argc, char** argv, int start, int bytes, char* outputfile) {
    bytes = (bytes == -1)?1024:bytes;
    char dataarr[bytes];
    int fdw, byteswritten;
    fdw = open(outputfile, O_CREAT | O_WRONLY | O_TRUNC, 0777);
    if (!outputfile) fdw = STDOUT_FILENO;
    if (fdw < 0 && !(!outputfile)) errorOutput("Function call open failed to open ", (!outputfile) ? "stdout" : outputfile, strerror(errno));
    int fdin, rbytes, attemptwrite;
    for (int i = start; i < argc; i++) {
        if (strcmp(argv[i],"-") == 0) {
            fdin = STDIN_FILENO; 
        } else {
            fdin = open(argv[i], O_RDONLY, 0777);
        }

        while ((rbytes = read(fdin, dataarr, bytes)) > 0) {
            byteswritten =  write(fdw, dataarr, rbytes);
            if (byteswritten <= 0)
                errorOutput("Function call write failed to ", (!outputfile) ? "stdout" : outputfile, strerror(errno));
            else if(byteswritten != rbytes) {
                attemptwrite = 0;
                while(1) {
                    attemptwrite = write(fdw, &dataarr[byteswritten], rbytes-byteswritten);
                    if (attemptwrite < 1)
                        errorOutput("Function call write was partially written to before failing", (!outputfile) ? "stdout" : outputfile, strerror(errno));
                    byteswritten += attemptwrite;
                    if (byteswritten == rbytes)
                        break;
                }
            }
        }
        if (rbytes < 0) {
            errorOutput("Function call read failed from ", argv[i], strerror(errno));
        }
    }

    if (fdw < 0 && !(!outputfile) && close(fdw) < 0)
        errorOutput("Function call close failed on ", (!outputfile) ? "stdout" : outputfile, strerror(errno));
    

return;
}

void errorOutput (char* myOutput, char* whichFile, char* errnoOutput) {

    fprintf(stderr, "Error: %s%s. %s.\n", myOutput, whichFile, errnoOutput);
    exit(-1); 

return;
}

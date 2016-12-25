#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

#define MAX_STRING_LENGTH 2049 //Includes null terminate

int main(int argc, char** argv, char** envp) {

    struct termios term;
    if (tcgetattr(0, &term) == -1) {
        fprintf(stderr, "Error: tcgetattr failed. Error code: %s.\n", strerror(errno));
        exit(-1);
    }
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;

    if (tcsetattr(0, TCSANOW, &term) == -1) {
        fprintf(stderr, "Error: tcsetattr failed. Error code: %s.\n", strerror(errno));
        exit(-1);
    }

    while (my_lineread()) {
        printf("Read a newline, it was echoed back and we're onto the next one!\n");
        return 0;
    }
    printf("Read an end of file. We're done!\n");
    return 0;

}

int my_lineread(int eof_check, string inputline) {

    char c;
    int byte;
    char buf[MAX_STRING_LENGTH];
    int length = 0;
    while((byte = read(0, &c, 1)) > 0) {
        if (strcmp(c, termios.c_cc[VERASE]) == 0) { //Strings match
            if (length == 0)
                continue;
            length--;
            //abcd, length = 4
            //length = 3, abc --> remove char at pos 3
            buf[length] = '\0';
        } else if (strcmp(c, termios.c_cc[VWERASE]) == 0) {
            while(length != 0 && buf[length] != ' ' && buf[length] != '\t') {
                buf[length] = '\0';
                length--;
                //abcd abcd, length = 8
                //then abcd abc\0, length = 7;
                //then abcd ab\0
                //abcd a\0
                //abcd a\0
            }
            buf[length] = '\0';
            length--;
        } else if (strcmp(c, termios.c_cc[VKILL]) == 0) {
            while(length != 0) {
                buf[length] = '\0';
                length--;
            }
            buf[length] = '\0';
        } else if (strcmp(c, termios.c_cc[VEOF]) == 0) {
            buf[length] = '\0';
            return 0;
        } else {
            buf[length++] = c;
        }
    }
return 1;
}

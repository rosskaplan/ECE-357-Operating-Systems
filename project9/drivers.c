#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

#define MAX_STRING_LENGTH 2049 //Includes null terminate

int my_lineread(void);

int main(int argc, char** argv, char** envp) {

    struct termios term;
    if (tcgetattr(0, &term) == -1) {
        fprintf(stderr, "Error: tcgetattr failed. Error code: %s.\n", strerror(errno));
        exit(-1);
    }
    term.c_lflag &= (ICANON | ECHO);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;

    if (tcsetattr(0, TCSANOW, &term) == -1) {
        fprintf(stderr, "Error: tcsetattr failed. Error code: %s.\n", strerror(errno));
        exit(-1);
    }

    while (my_lineread()) {
        printf("Read a newline, it was echoed back and we're onto the next one!\n");
    }
    printf("Read an end of file. We're done!\n");
    return 0;

}

int my_lineread(void) {

    char c;
    char buf[MAX_STRING_LENGTH];
    int length = 0;
    while(read(0, &c, 1) > 0) {
        if (c == VERASE) {
            if (length == 0)
                continue;
            length--;
            buf[length] = '\0';
        } else if (c == VWERASE) {
            while(length != 0 && buf[length] != ' ' && buf[length] != '\t') {
                buf[length] = '\0';
                length--;
            }
            buf[length] = '\0';
        } else if (c == VKILL) {
            while(length != 0) {
                buf[length] = '\0';
                length--;
            }
            buf[length] = '\0';
        } else if (c == '\n') {
            buf[length++] = '\n';
            buf[length++] = '\0';
            write(1, buf, length);
            return 1;
        } else {
            buf[length++] = c;
        }
    }
    //Hit end of file, exit
    return 0;
}

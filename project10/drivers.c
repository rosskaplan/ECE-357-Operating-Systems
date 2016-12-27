#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ttydefaults.h>

#define MAX_STRING_LENGTH 2049 //Includes null terminate

int my_lineread(void);

int main(int argc, char** argv, char** envp) {

    struct termios term;
    if (tcgetattr(0, &term) == -1) {
        fprintf(stderr, "Error: tcgetattr failed. Error code: %s.\n", strerror(errno));
        exit(-1);
    }
    term.c_lflag = 0;
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;

    if (tcsetattr(0, TCSANOW, &term) == -1) {
        fprintf(stderr, "Error: tcsetattr failed. Error code: %s.\n", strerror(errno));
        exit(-1);
    }

    while (my_lineread())
        printf("\n");
    return 0;

}

int my_lineread(void) {

    char c;
    char buf[MAX_STRING_LENGTH];
    int length = 0;
    char newbuf[4] = "\b \b\0";
    while(read(0, &c, 1) > 0) { //Handles EOF
        if (c == 4) { //EOF
            return 0;
        } else if (c == 127) { //VERASE
            if (length == 0)
                continue;
            write(1, newbuf, strlen(newbuf));
            buf[--length] = '\0';
        } else if (c == 23) { //VWERASE
            while(length != 0 && (buf[length] == ' ' || buf[length] == '\t' || buf[length] == '\0')) { //While whitespace
                buf[length--] = '\0';
                write(1, newbuf, strlen(newbuf));
            }  //Clear all spaces, then clear word
            while(length != 0 && ((buf[length] != ' ' && buf[length] != '\t') || buf[length] == '\0')) { //While word
                buf[length--] = '\0';
                write(1, newbuf, strlen(newbuf));
            }
        } else if (c == 21) { //VKILL
            while(length != 0) {
                buf[length] = '\0';
                write(1, newbuf, strlen(newbuf));
                length--;
            }
            buf[length] = '\0';
            write(1, newbuf, strlen(newbuf));
        } else if (c == '\n') {
            buf[length++] = '\n';
            return 1;
        } else {
            buf[length++] = c;
            write(1, &c, 1);
        }
        if (strlen(buf) == 2048) {
            return 1;
        }
    }
    //Hit end of file, exit
    return 0;
}

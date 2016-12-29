#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ttydefaults.h>

#define MAX_STRING_LENGTH 2049 //Includes null terminate

int my_lineread(char buf[MAX_STRING_LENGTH], int len);

int main(int argc, char** argv) {

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

    char buf[MAX_STRING_LENGTH];
    int len = 20;
    int str_len;
    str_len = my_lineread(buf, len); //Yes, single =
    buf[str_len] = '\0';
    printf("Buf contains: \"%s\"\n", buf);
    return 0;

}

int my_lineread(char buf[MAX_STRING_LENGTH], int len) {

    char c;
    int length = 0;
    for (int i = 0; i < len; i++) {
        buf[i] = '\0';
    }
    char newbuf[4] = "\b \b\0";
    while(read(0, &c, 1) > 0) { //Handles EOF
        if (len > 2048) {
            len=2048;
            fprintf(stderr, "Note: len is shortened to 2048 characters\n");
        }
        if (len < 0) {
            len=1;
            fprintf(stderr, "Note: len is lengthened to 1 character\n");
        }
        if (c == 4) { //EOF
            return length;
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
            return length;
        } else {
            buf[length++] = c;
            write(1, &c, 1);
        }
        if (strlen(buf) == len) {
            return length;
        }
    }
    //Hit end of file, exit
    return length;
}

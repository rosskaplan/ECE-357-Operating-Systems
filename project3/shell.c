#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>

void myParse(char* buff);
int myExec(char* input, int is_input, char* output, int is_output, char* err, int is_err, char** commands);

int main(int argc, char **argv) {
    char buff[2048];
    int i = 0;
    char c;
    while (((c = getchar()) != -1) && (i != 2048)) {
        buff[i] = c;
        i++;
        if (c == '\n') {
            buff[i] = '\0';
            i = 0;
            myParse(buff);
        }
    }

return 0;
}

void myParse(char* buff) {
    //Code from `man 3 strtok`
    char *word, *temp;
    char* sep = " ";
    int next_word_in = 0;
    int next_word_out = 0;
    int next_word_err = 0;
    char* input = malloc(sizeof(char)*512);
    char* output = malloc(sizeof(char)*512);
    char* err = malloc(sizeof(char)*512);
    int is_input, is_output, is_err;
    is_input = is_output = is_err = 0;
    int first_redir = 0;
    int i = 0;
    char * commands[32];
    for (word = strtok_r(buff, sep, &temp); word; word = strtok_r(NULL, sep, &temp), i++) {
        if ((i == 0) && (word[0] == '#'))
            return;
        if (word[strlen(word)-1] == '\n')
            word[strlen(word)-1] = '\0';
        if (next_word_in == 1) {
            next_word_in = 0;
            strcpy(input, word);
            if (input[strlen(input)-1] == '\n') {
                input[strlen(input)-1] = '\0';
            }
            continue;
        }
        if (next_word_out == 1) {
            next_word_out = 0;
            strcpy(output, word);
            if (output[strlen(output)-1] == '\n') {
                output[strlen(output)-1] = '\0';
            }
            continue;
        }
        if (next_word_err == 1) {
            next_word_err = 0;
            strcpy(err, word);
            if (err[strlen(err)-1] == '\n') {
                err[strlen(err)-1] = '\0';
            }
            continue;
        }

        if (word[0] == '<') {
            if (first_redir == 0) {
                first_redir = i;
            }
            if (word[1] == '\0') {
                next_word_in = 1;
            } else {
                if (word[strlen(word)-1] == '\n') {
                    word[strlen(word)-1] = '\0';
                }
                strcpy(input, &word[1]);
                is_input = 1;
            }
        }

        if ((word[0] == '>') && (word[1] == '>')) {
            if (first_redir == 0) {
                first_redir = i;
            }
            if (word[2] == '\0') {
                next_word_out = 1;
                is_output = 1;
            } else {
                if (word[strlen(word)-1] == '\n') {
                    word[strlen(word)-1] = '\0';
                }
                strcpy(output, &word[2]);
                is_output = 1;
            }
        } else if (word[0] == '>') {
            if (first_redir == 0) {
                first_redir = i;
            }
            if (word[1] == '\0') {
                next_word_out = 1;
                is_output = 2;
            } else {
                if (word[strlen(word)-1] == '\n') {
                    word[strlen(word)-1] = '\0';
                }
                strcpy(output, &word[1]);
                is_output = 2;
            }
        }

        if ((word[0] == '2') && (word[1] == '>') && (word[2] == '>')) {
            if (first_redir == 0) {
                first_redir = i;
            }
            if (word[3] == '\0') {
                next_word_err = 1;
                is_err = 1;
            } else {
                if (word[strlen(word)-1] == '\n') {
                    word[strlen(word)-1] = '\0';
                }
                strcpy(err, &word[3]);
                is_err = 1;
            }
        } else if ((word[0] == '2') && (word[1] == '>')) {
            if (first_redir == 0) {
                first_redir = i;
            }
            if (word[2] == '\0') {
                next_word_err = 1;
                is_err = 2;
            } else {
                if (word[strlen(word)-1] == '\n') {
                    word[strlen(word)-1] = '\0';
                }
                strcpy(err, &word[2]);
                is_err = 2;
            }
        }
        if (first_redir == 0) {
            commands[i] = word;
        }
    }
    int j = 0;
    if (first_redir == 0) {
        commands[i] = NULL;
    } else {
        commands[first_redir] = NULL;
    }
    myExec((is_input == 1) ? "" : input, is_input, (is_output >= 1) ? output : "", is_output, (is_err >= 1) ? err : "", is_err, commands);
    return;
}

int myExec(char* input, int is_input, char* output, int is_output, char* err, int is_err, char** commands) {
    int pid;
    time_t time1, time2;
    struct rusage stat;
    int wait_amt;
    struct timeval tv1, tv2;
    int fdin;
    int fdout;
    int fderr;
    gettimeofday(&tv1, NULL);  
    double curtime1= (float) tv1.tv_sec+(tv1.tv_usec/1000000.0);
    switch (pid=fork()) {
        case -1:
            fprintf(stderr, "Fork unable to create child process.  Error: %s.\n",strerror(errno)); 
            exit(1);
            break;
        case 0:
            if (is_input) {
                if ((fdin = open(input, O_RDONLY)) < 0) {
                    fprintf(stderr, "Error in opening %s. Error: %s.\n", input, strerror(errno));
                    return 1;
                } else {
                    if (dup2(fdin, 0) >= 0) {
                        close(fdin);
                    } else {
                        fprintf(stderr, "Error in redirecting standard input to %s. Error: %s.\n", input, strerror(errno));
                        return 1;
                    }
                }
            }
            if (is_output) {
                if (is_output == 1) {
                    if ((fdout = open(output, O_WRONLY | O_APPEND | O_CREAT, 0666)) < 0) {
                        fprintf(stderr, "Error in opening %s. Error: %s.\n", output, strerror(errno));
                        return 1; 
                    }
                } else {
                    if ((fdout = open(output, O_WRONLY | O_TRUNC | O_CREAT, 0666)) < 0) {
                        fprintf(stderr, "Error in opening %s. Error: %s.\n", output, strerror(errno));
                        return 1;
                    }
                }
                if (dup2(fdout, 1) >= 0) {
                    close(fdout);
                } else {
                    fprintf(stderr, "Error in redirecting standard output to %s. Error: %s.\n", output, strerror(errno));
                    return 1;
                }
            }
            if (is_err) {
                if (is_err == 1) {
                    if ((fderr = open(err, O_WRONLY | O_APPEND | O_CREAT, 0666)) < 0) {
                        fprintf(stderr, "Error in opening %s. Error: %s.\n", err, strerror(errno));
                        return 1; 
                    }
                } else {
                    if ((fderr = open(err, O_WRONLY | O_TRUNC | O_CREAT, 0666)) < 0) {
                        fprintf(stderr, "Error in opening %s. Error: %s.\n", err, strerror(errno));
                        return 1;
                    }
                }
                if (dup2(fderr, 2) >= 0) {
                    close(fderr);
                } else {
                    fprintf(stderr, "Error in redirecting standard error to %s. Error: %s.\n", err, strerror(errno));
                    return 1;
                }
            }
            execvp(commands[0], commands);
            break;
        default:
            if (wait3(&wait_amt, 0, &stat) == -1) {
                fprintf(stderr, "Error in waiting for child process. Error: %s.\n", strerror(errno));
                return 1;
            }
            gettimeofday(&tv2, NULL);
            double curtime2 = (float) tv2.tv_sec+(tv2.tv_usec/1000000.0);
            fprintf(stderr, "consuming %f real seconds, %f user, %f system\n", (curtime2-curtime1<0)?curtime1-curtime2:curtime2-curtime1, stat.ru_utime.tv_sec + (stat.ru_utime.tv_usec/1000000.0), stat.ru_stime.tv_sec + (stat.ru_stime.tv_usec/1000000.0));
            break;
    }

    return 0;
}



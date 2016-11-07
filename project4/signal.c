#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

void realHandler();
void convHandler();

int real_ct;
int conv_ct;

int main(int argc, char **argv) {

    signal(SIGIO, realHandler);
    signal(35, convHandler);

    int pids[3];
    union sigval value;
    int parent;
    int waitstats;
    int killcheck;
    for (int i = 0; i < 3; i++) {
        switch (pids[i] = fork()) {
            case -1:
                fprintf(stderr, "Error: failure to fork. Error code %s\n", strerror(errno));
                exit(0);
            case 0:
                parent = getppid(); // get parent id
                for (int j = 0; j < 100; j++) {
                    if ((killcheck = kill(parent, SIGIO)) == -1) {
                        fprintf(stderr, "Error: failure to send kill signal (realtime). Error code %s\n", strerror(errno));
                        exit(0);
                    }
                    if ((killcheck = kill(parent, 35)) == -1) {
                        fprintf(stderr, "Error: failure to send kill signal (conventional). Error code %s\n", strerror(errno));
                        exit(0);
                    }
                }
                exit(0);
            default:
                continue;
        }
    }

    for (int i = 0; i < 3; i++) {
        wait (&waitstats);
    }

    printf ("Got %d SIGIO and %d SIGRTMIN\n", real_ct, conv_ct);

return 0;
}

void realHandler() {
    real_ct++;
return;
}

void convHandler() {
    conv_ct++;
return;
}

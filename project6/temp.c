#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "sem.h"
#include "fifo.h"

int main(int argc, char **argv) {

    pid_t pid; 
    int waitstats;
    struct fifo *f1;
    fifo_init(f1);
    for (int i = 0; i < 64; i++) {
        switch(pid = fork()) {
            case -1: 
                fprintf(stderr, "Error: failed to fork.  Error code: %s\n", strerror(errno));
                exit(-1);
            case 0: 
                for (int j = 0; j < 1000; j++) {
                    fifo_wr(f1, i + j*64);
                }

                exit(0);
            default:
                continue;
        }
    }
    for (int i = 0; i < 64; i++) wait(&waitstats);
    printf("\nWith Locking: %d", 1000000*64);

    for (int i = 0; i < MYFIFO_BUFSIZ; i++) {
        printf("%d: %lu\n", i, f1->fifoarr[i]);
    }
    return 0;
}

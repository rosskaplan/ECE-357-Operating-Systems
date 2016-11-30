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
    long* test;
    if ((test = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, 0, 0)) != 0) {
        fprintf(stderr, "Error mapping.  Error code: %s\n", strerror(errno));
        exit(-1);
    }
    int parent;
    int waitstats;
    *test = 0;
    struct fifo *f1;
    
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
    printf("\nWithout Locking: %ld\n", *test);

    if (munmap(test, 4096) != 0) {
        fprintf(stderr, "Error unmapping. Error code: %s\n", strerror(errno));
        exit(-1);
    }
    return 0;
}

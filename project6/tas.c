#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>

int main(int argc, char **argv) {

    pid_t pid; 
    if ((long * test = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, 0, 0)) != 0) {
        fprintf(stderr, "Error mapping.  Error code: %s\n", strerror(errno));
        exit(-1);
    }
    int parent;
    int waitstats;
    *test = 0;

    for (int i = 0; i < 64; i++) {
        switch(pid = fork()) {
            case -1: 
                fprintf(stderr, "Error: failed to fork.  Error code: %s\n", strerror(errno));
                exit(-1);
            case 0: 
                for (int j = 0; j < 1000000; j++) (*test)++;
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

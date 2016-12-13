#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>
#include "fifo.h"

#define NUM_WRITE_PROCESSES 4
#define NUM_WRITES_PER_PROCESS 10000
#define NUM_READS_PER_PROCESS (NUM_WRITE_PROCESSES*NUM_WRITES_PER_PROCESS)

int main() {
    struct fifo *f;
    f = mmap(NULL, sizeof(struct fifo), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
    if (!f) {
        fprintf(stderr, "Error mapping.  Error code: %s\n", strerror(errno));
        exit(-1);
    }
    fifo_init(f);
    
    int pid;

    unsigned long rd_out;
    switch(pid = fork()) {
        case -1: 
            fprintf(stderr, "Error: failure to fork.  Error code %s\n", strerror(errno));
            exit(-1);
        case 0:
            for (int j = 0; j < NUM_READS_PER_PROCESS; ++j) {
                rd_out = fifo_rd(f);
                printf("Read %lu\n", rd_out);
            }
            exit(0);
    }

    for (int i = 0; i < NUM_WRITE_PROCESSES; i++) {
        switch(pid = fork()) {
            case -1: 
                fprintf(stderr, "Error: failure to fork.  Error code %s\n", strerror(errno));
                exit(-1);
            case 0:
                for (int j = 0; j < NUM_WRITES_PER_PROCESS; ++j) {
                    fifo_wr(f, i*NUM_WRITES_PER_PROCESS+j); //Unique numbers generator
                    printf("Wrote %d from process %d\n", i*NUM_WRITES_PER_PROCESS+j, i+1);
                }
                exit(0);
        }
    }

    int waitstats;
    for (int i = 0; i <= NUM_WRITE_PROCESSES; i++) { //+1 is to also wait for 1 reader process
        printf("%d", i);
        wait(&waitstats);
    }

    return 0;
}

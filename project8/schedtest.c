#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "sched.h"
#include <stdlib.h>
#include <time.h>

#define NUM_CHILD 4

void child_handler();
void init_fn() { //The function that sched_init refers to
    srand(time(NULL));
    int pid, i;
    for (i = 0; i < NUM_CHILD; ++i) {
        pid = sched_fork ();
        if (pid == 0) {
            sched_nice(9-(rand() % 20));
            child_handler();
        }
    }
    sched_wait(&i);
    exit(0);
}

int main (int argc, char ** argv) {
  sched_init(init_fn);
  return 0;
}

void child_handler() {
    for (int i = 0; i < 1000000000; i++); //wait a bit
    int pid = sched_getpid();
    sched_exit(pid);
    for (int i = 0; i < 1000000000; i++); //wait a bit
    return;
}

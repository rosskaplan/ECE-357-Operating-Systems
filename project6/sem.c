#include "sem.h"
#include <signal.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

void sem_init(struct sem *s, int count) {
    s->count = count;
    s->spinlock = 0;

    for (int i = 0; i < SEMARR_SIZE; ++i) {
        s->semarr[i] = 0;
    }
    return;
}

void signalhandler(int signalnum) {
}

int sem_try(struct sem*s) {

    if (tas(&(s->spinlock)) == 1)
        return 1;
    else
        return 0;
}

void sem_wait(struct sem *s) {
    for(;;) {
        while(tas(&s->spinlock)) {
            //Getting the lock
        }
        if (s->count >= 1) {
            s->semarr[s->count] = 0;
            s->count--;
            s->spinlock = 0;
            return;
        } else {
            signal(SIGUSR1, signalhandler);
            //Reference: page 5 of notes
            sigset_t oldmask, newmask;
            s->semarr[s->count] = getpid(); //This call cannot fail
            //Allow user to break
            if (sigfillset(&newmask) != 0) {
                fprintf(stderr, "Failed to build new mask. Error: %s.\n", strerror(errno));
                exit(-1);
            }
            if (sigdelset(&newmask, SIGUSR1) != 0) {
                fprintf(stderr, "Failed to enable SIGUSR1. Error: %s.\n", strerror(errno));
                exit(-1);
            }
            if (sigdelset(&newmask, SIGINT) != 0) {
                fprintf(stderr, "Failed to enable SIGINT. Error: %s.\n", strerror(errno));
                exit(-1);
            }
            if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) != 0) {
                fprintf(stderr, "Failed to block. Error: %s.\n", strerror(errno));
                exit(-1);
            }
            s->spinlock = 0;
            sigsuspend(&newmask);
            if (sigprocmask(SIG_UNBLOCK, &oldmask, NULL) != 0) {
                fprintf(stderr, "Failed to unblock. Error: %s.\n", strerror(errno));
                exit(-1);
            }
        }
    }

    return;
}

void sem_inc(struct sem *s) {
    while(tas(&s->spinlock)) {
    } 
    s->count++;
    for (int i = 0; i < SEMARR_SIZE; i++) {
        if (s->semarr[i]) {
            if (kill(s->semarr[i],SIGUSR1) != 0) {
                fprintf(stderr, "Failed to send SIGUSR to process %d. Error: %s.\n", s->semarr[i], strerror(errno));
                exit(-1);
            }
            s->semarr[i] = 0;
        }
    }
    s->spinlock = 0;

    return;
}

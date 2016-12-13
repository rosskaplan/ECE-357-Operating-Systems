#ifndef SEM_H_
#define SEM_H_

#define SEMARR_SIZE 64

struct sem {
    int semarr[SEMARR_SIZE];
    int count;
    char spinlock; //Match the provided tas.S file
};

int tas(volatile char *lock);

void sem_init(struct sem *s, int count);

int sem_try(struct sem *s);

void sem_wait(struct sem *s);

void sem_inc(struct sem *s);

void signalhandler(int signalnum);

#endif // SEM_H_

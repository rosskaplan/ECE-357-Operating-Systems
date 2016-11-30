#ifndef SEM_H_
#define SEM_H_

struct sem {
    int semarr[50];
    int count;
    char* spinlock;
};

int tas(volatile char *lock);

void sem_init(struct sem *s, int count);

int sem_try(struct sem *s);

void sem_wait(struct sem *s);

void sem_inc(struct sem *s);

void signalhandler(void);

#endif // SEM_H_

#include "sem.h"
#include "fifo.h"
#include <stdio.h>

void fifo_init(struct fifo *f) {
    //Fifo is empty
    sem_init(&(f->lock), 1);
    sem_init(&(f->wr), MYFIFO_BUFSIZ);
    sem_init(&(f->rd), 0);

    f->writepos = 0;
    f->readpos = 0;
    f->count = 0;
    return;
}

void fifo_wr(struct fifo *f, unsigned long d) {

    sem_wait(&(f->lock));
    sem_wait(&(f->wr));
    //Now that we have the lock and we block other processes, write.
    //Put the int at the ending spot of the fifo
    f->fifoarr[f->writepos++] = d;
    f->writepos %= MYFIFO_BUFSIZ;
    printf("hi");
    //Wake up all other processes, increment.
    sem_inc(&(f->rd));
    sem_inc(&(f->lock));
    return;
}

unsigned long fifo_rd(struct fifo *f) {

    //Follow a very similar logic to above
    unsigned long temp;

    sem_wait(&(f->lock));
    sem_wait(&(f->rd));
 
    //Now that we have the lock and we block other processes, write.
    //Put the int at the ending spot of the fifo
    temp = f->fifoarr[f->readpos++];
    f->readpos %= MYFIFO_BUFSIZ;

    //Wake up all other processes, increment.
    sem_inc(&(f->wr));
    sem_inc(&(f->lock));
    return temp;
}

#ifndef FIFO_H_
#define FIFO_H_

#include "sem.h"
#include <string.h>

#define MYFIFO_BUFSIZ 4096 //FIFO Length as specified in documentation

struct fifo {
    long int fifoarr[MYFIFO_BUFSIZ];
    int count, writepos, readpos;
    struct sem lock, wr, rd;
};

void fifo_init(struct fifo *f);

void fifo_wr(struct fifo *f, unsigned long d);

unsigned long fifo_rd(struct fifo *f);

#endif // FIFO_H_

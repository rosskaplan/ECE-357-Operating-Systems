#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <sys/types.h>

#define COUNT 100000000

void fcnCall() {
return;
}

int sysCall() {
return getuid();
}

int main() {
    struct timespec first;
    struct timespec second;
   
    clock_gettime(CLOCK_REALTIME, &first);
    for (int i = 0; i < COUNT; i++);
    clock_gettime(CLOCK_REALTIME, &second);
    double timediff = (second.tv_sec - first.tv_sec)*1000000000 + (second.tv_nsec - first.tv_nsec);
    printf("The empty for loop of 1e8: %lf nanoseconds.\n", timediff/COUNT);
    
    clock_gettime(CLOCK_REALTIME, &first);
    for (int i = 0; i < COUNT; i++) {
        fcnCall();
    }
    clock_gettime(CLOCK_REALTIME, &second);
    timediff = (second.tv_sec - first.tv_sec)*1000000000 + (second.tv_nsec - first.tv_nsec);
    printf("The for loop of empty function calls (1e8 iterations): %lf nanoseconds.\n", timediff/COUNT);

    clock_gettime(CLOCK_REALTIME, &first);
    for (int i = 0; i < COUNT; i++) {
        sysCall();
    }
    clock_gettime(CLOCK_REALTIME, &second);
    timediff = (second.tv_sec - first.tv_sec)*1000000000 + (second.tv_nsec - first.tv_nsec);
    printf("The for loop of syscalls (1e8 iterations): %lf nanoseconds.\n", timediff/COUNT);
}

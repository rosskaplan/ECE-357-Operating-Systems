#include "sched.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h> //setitimer
#include <sys/mman.h> //mmap
struct sched_waitq *procs;
struct sched_proc *current;
int tick_count = 0;
sigset_t sig_mask;

void sched_init(void (*init_fn)()) {
    sigemptyset(&sig_mask);
    sigaddset(&sig_mask, SIGVTALRM);
    sigaddset(&sig_mask, SIGABRT);

    //Handle the ticks on setitimer
    if (signal(SIGVTALRM, sched_tick) == SIG_ERR) { 

        fprintf(stderr, "Error: failed to handle SIGVTALRM signal properly. Error code: %s.\n", strerror(errno));
        exit(-1);
    }

    //Establish sched_ps as the signal handler for a SIGABRT so that a ps can be forced at any time by sending the testbed SIGABRT
    if (signal(SIGABRT, sched_ps) == SIG_ERR) {
        fprintf(stderr, "Error: failed to handle SIGABRT signal properly.  Error code: %s.\n", strerror(errno));
        exit(-1);
    }

    struct itimerval mytimer;

    mytimer.it_value.tv_sec = 0;
    mytimer.it_value.tv_usec = 100000; //Told to use tick time of 100ms
    mytimer.it_interval.tv_sec = 0;
    mytimer.it_interval.tv_usec = 100000;

    //Timer triggers SIGVTALRM, so call tick when received
    if (setitimer(ITIMER_VIRTUAL, &mytimer, NULL) == -1) {
        fprintf(stderr, "Error: failed to set itimer. Error code: %s.\n", strerror(errno));
        exit(-1);
    }

    void * newsp;
    if ((newsp = mmap(0, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED) {
        fprintf(stderr, "Error: Failed to mmap newsp. Error code: %s.\n", strerror(errno));
        exit(-1);
    }

    if ((current = malloc(sizeof(struct sched_proc))) == NULL) {
        fprintf(stderr, "Failed to malloc sched_proc. Error code: %s.\n", strerror(errno));
        exit(-1);
    }

    if ((procs = malloc(sizeof(struct sched_waitq))) == NULL) {
        fprintf(stderr, "Failed to malloc sched_waitq. Error code: %s.\n", strerror(errno));
        exit(-1);
    }
    //Set initial stats of current process
    current->task_state = SCHED_RUNNING;
    current->nice = 0;
    current->priority = 0;
    current->accumulated = 0;
    current->cpu_time = 0;
    current->stack = newsp;
    current->pid = 1;
    current->ppid = 0;
    current->exit_status = 0;
    current->num_children = 0;

    savectx(&(current->context));
    
    current->context.regs[JB_BP] = newsp + STACK_SIZE; //newsp is location of "top" of stack
                                                      //so we set JB_BP (base ptr) to the base
    current->context.regs[JB_SP] = newsp + STACK_SIZE; //Same logic
    current->context.regs[JB_PC] = init_fn; //The calling function from schedtest.c 

    procs->myprocs[0] = NULL; //Myprocs' size is SCHED_NPROCS+1 to avoid ugly indexing
    procs->myprocs[1] = current; //pid 1 stores the current process
    
    restorectx(&(current->context), 0);
}

int sched_fork() {
    //Must block signals

    if (sigprocmask(SIG_BLOCK, &sig_mask, NULL) == -1) {
        fprintf(stderr, "Error: failed to block signals in sched_wait. Error code: %s.\n", strerror(errno));
        exit(-1);
    }

    struct sched_proc *newproc;

    int i = 1;
    while(procs->myprocs[i++] != NULL) {
    }
    if (--i > SCHED_NPROC) {
        fprintf(stderr, "Cannot sched_fork another process. \n");
        if (sigprocmask(SIG_UNBLOCK, &sig_mask, NULL) == -1) {
            fprintf(stderr, "Error; failure in sched_fork on sigprocmask. Error code: %s.\n", strerror(errno));
            return -1;
        }
        return -1;
    }

    if ((newproc = malloc(sizeof(struct sched_proc))) == NULL) {
        fprintf(stderr, "Failed to malloc a new child process in sched_fork(). Error code: %s.\n", strerror(errno));
        exit(-1);
    }

    void * newsp;
    if ((newsp = mmap(0, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED) {
        fprintf(stderr, "Error: Failed to mmap newsp. Error code: %s.\n", strerror(errno));
        exit(-1);
    }

    //Initialize the child processes' values;
    
    newproc->task_state = SCHED_READY;
    newproc->priority = current->priority;
    newproc->nice = current->nice;
    newproc->accumulated = 0;
    newproc->cpu_time = current->cpu_time;
    newproc->stack = newsp;
    newproc->pid = i;
    newproc->ppid = current->pid;
    newproc->exit_status = 0;
    newproc->num_children = 0;
    current->num_children++;

    procs->myprocs[i] = newproc;

    //Set new process' stack to parent stack
    if (savectx(&newproc->context)) {
        if (sigprocmask(SIG_UNBLOCK, &sig_mask, NULL) == -1) {
            fprintf(stderr, "Error: failed to block signals in sched_wait. Error code: %s.\n", strerror(errno));
            exit(-1);
        }
        return 0;
    } else {
        //Copy from parent to newproc 
        memcpy(newproc->stack, current->stack, STACK_SIZE);
        //Adjusts from the child stack top to bottom by the difference from parent stack
        //to child stack;
        unsigned long temp = (unsigned long)(newproc->stack - current->stack);
        adjstack(newproc->stack, newproc->stack+STACK_SIZE, temp);
        newproc->context.regs[JB_BP] += temp;
        newproc->context.regs[JB_SP] += temp;
        if (sigprocmask(SIG_UNBLOCK, &sig_mask, NULL) == -1) {
            fprintf(stderr, "Error: failed to block signals in sched_wait. Error code: %s.\n", strerror(errno));
            exit(-1);
        }
        return i;
    }
}

int sched_exit(int exit_status) {

    //Once again, block the signals
    if (sigprocmask(SIG_BLOCK, &sig_mask, NULL) == -1) {
        fprintf(stderr, "Error: failed to block signals in sched_wait. Error code: %s.\n", strerror(errno));
        exit(-1);
    }
    //Update the current process and wake parent
    current->task_state = SCHED_ZOMBIE;
    current->exit_status = exit_status;
   
    for (int i = 1; i <= SCHED_NPROC; i++) {
        int ppid = (current->ppid);
        if ((procs->myprocs[i] != NULL) && (procs->myprocs[i]->task_state == SCHED_SLEEPING) && (procs->myprocs[i]->pid == ppid)) {
            current->task_state = SCHED_READY;
            current->num_children--;
            sched_switch();
            break; //THIS SHOULD NEVER BE REACHED
        }
    }
    return exit_status; //THIS SHOULD ALSO NEVER BE REACHED
}

int sched_wait(int *exit_code) {

    //Block signal
    if (sigprocmask(SIG_BLOCK, &sig_mask, NULL) == -1) {
        fprintf(stderr, "Error: failed to block signals in sched_wait. Error code: %s.\n", strerror(errno));
        exit(-1);
    }

    if (current->num_children == 0)
        return -1;
    
    for (int i=1; i < SCHED_NPROC; i++) {
        int ppid = current->pid;
        if ((procs->myprocs[i]) && (procs->myprocs[i]->ppid = ppid)) {
            if (procs->myprocs[i]->task_state != SCHED_ZOMBIE) {
                //Fill out exit status, and unmap process
                *exit_code = procs->myprocs[i]->exit_status;
                procs->myprocs[i] = NULL;
                sched_switch();
            } else {
                //Set current to sleep, sched_switch() (which returns on dead child)
                if (sigprocmask(SIG_BLOCK, &sig_mask, NULL) == -1) {
                    fprintf(stderr, "Error: failed to block signals in sched_wait. Error code: %s.\n", strerror(errno));
                    exit(-1);
                }
                current->task_state = SCHED_SLEEPING;
                sched_switch();
                *exit_code = procs->myprocs[i]->exit_status;
                if (sigprocmask(SIG_UNBLOCK, &sig_mask, NULL) == -1) {
                    fprintf(stderr, "Error: failed to block signals in sched_wait. Error code: %s.\n", strerror(errno));
                    exit(-1);
                }
                if (munmap(procs->myprocs[i]->stack, STACK_SIZE) == -1) {
                    fprintf(stderr, "Error: failed to munmap in sched_wait. Error code: %s.\n", strerror(errno));
                    exit(-1);
                }
                procs->myprocs[i] = NULL;
            }
        }
    }
    return 0;
}

void sched_nice(int niceval) {
    current->nice = niceval;
    return;
}

int sched_getpid() {
    return current->pid;
}

int sched_getppid() {
    return current->ppid;
}

int sched_gettick() {
    return tick_count;
}

void sched_ps() {
    if (sigprocmask(SIG_BLOCK, &sig_mask, NULL) == -1) {
        fprintf(stderr, "Error: failed to block signals in sched_wait. Error code: %s.\n", strerror(errno));
        exit(-1);
    }
    fprintf(stderr, "pid\tppid\tstate\t\tstack\t\tnice\tprty\tcpu_time\n");
    for (int i=1; i<SCHED_NPROC; i++) {
        if (procs->myprocs[i]) {
            fprintf(stderr, "%d\t", procs->myprocs[i]->pid);
            fprintf(stderr, "%d\t\t", procs->myprocs[i]->ppid);
            if (procs->myprocs[i]->task_state == SCHED_READY) {
                fprintf(stderr, "SCHED_READY\t\t");
            } else if (procs->myprocs[i]->task_state == SCHED_RUNNING) {
                fprintf(stderr, "SCHED_RUNNING\t");
            } else if (procs->myprocs[i]->task_state == SCHED_SLEEPING) {
                fprintf(stderr, "SCHED_SLEEPING\t");
            } else {
                fprintf(stderr, "SCHED_ZOMBIE\t");
            }
            fprintf(stderr, "%x\t", (unsigned int)procs->myprocs[i]->stack);
            fprintf(stderr, "%d\t\t", procs->myprocs[i]->nice);
            fprintf(stderr, "%d\t\t", procs->myprocs[i]->priority);
            fprintf(stderr, "%d\n", procs->myprocs[i]->cpu_time);
        }
    }
    
    if (sigprocmask(SIG_UNBLOCK, &sig_mask, NULL) == -1) {
        fprintf(stderr, "Error: failed to block signals in sched_wait. Error code: %s.\n", strerror(errno));
        exit(-1);
    }
    return;
}        

void sched_switch() {
    for (int i = 1; i< SCHED_NPROC; i++) {
        if (procs->myprocs[i]) {
            procs->myprocs[i]->priority = procs->myprocs[i]->nice + (procs->myprocs[i]->cpu_time); 
            //This gives more time to higher priority processes while still proportioning the time based on how long they ran for
            if (procs->myprocs[i]->priority <= 0) {
                procs->myprocs[i]->priority = 0;
            } else if (procs->myprocs[i]->priority >= 39) {
                procs->myprocs[i]-> priority = 39;
            }
            procs->myprocs[i]->priority -= 20; //Sets all priorities between -20 and 19 like nice values
        }

    }
    int max = 10000; //guaranteed to be below the lowest priority
    int loc = 0;
    int i; 
    for (i = 1; i < SCHED_NPROC; i++) {
        if (procs->myprocs[i]) {
            if (procs->myprocs[i]->task_state == SCHED_READY || procs->myprocs[i]->task_state == SCHED_RUNNING) {
                if (procs->myprocs[i]->priority < max) {
                    max = procs->myprocs[i]->priority;
                    loc = i;
                }
            }
        }
    }
    if (procs->myprocs[loc]->pid == current->pid) { //if same process is still best
        current->task_state = SCHED_READY;
        sched_ps();
        if (sigprocmask(SIG_UNBLOCK, &sig_mask, NULL) == -1) {
            fprintf(stderr, "Error: failed to block signals in sched_wait. Error code: %s.\n", strerror(errno));
            exit(-1);
        }
        return;
    }
    //Context switch to new process, save the context
    if (!savectx(&(current->context))) {
        current = procs->myprocs[loc];
        current->task_state = SCHED_RUNNING;
        if (sigprocmask(SIG_UNBLOCK, &sig_mask, NULL) == -1) {
            fprintf(stderr, "Error: failed to block signals in sched_wait. Error code: %s.\n", strerror(errno));
            exit(-1);
        }
        restorectx(&(current->context), 1);
    }
    return;

}

void sched_tick() {
    if (sigprocmask(SIG_BLOCK, &sig_mask, NULL) == -1) {
        fprintf(stderr, "Error: failed to block signals in sched_wait. Error code: %s.\n", strerror(errno));
        exit(-1);
    }
    current->cpu_time++;
    current->accumulated++;
    tick_count++;
    sched_ps();
    if (sigprocmask(SIG_UNBLOCK, &sig_mask, NULL) == -1) {
        fprintf(stderr, "Error: failed to block signals in sched_wait. Error code: %s.\n", strerror(errno));
        exit(-1);
    }
}

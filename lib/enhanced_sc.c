#include "enhanced_sc.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

/* THREAD FUNCTIONS SECTION */
void lock(pthread_mutex_t *mutex){
    int err;
    if((err = pthread_mutex_lock(mutex)) != 0){
        perror("Error while locking mutex");
        exit(EXIT_FAILURE);
    }
}

void unlock(pthread_mutex_t *mutex){
    int err;
    if((err = pthread_mutex_unlock(mutex)) != 0){
        perror("Error while unlocking mutex");
        exit(EXIT_FAILURE);
    }
}

void create(pthread_t *tid, const pthread_attr_t *attr, void* (*function) (void *), void *args){
    int err;
    if((err = pthread_create(tid, attr, function, args)) != 0){
        perror("Error while creating thread");
        exit(EXIT_FAILURE);
    }
}

void cancel(pthread_t tid){
    int err;
    if((err = pthread_cancel(tid)) != 0){
        perror("Error while canceling thread");
        exit(EXIT_FAILURE);
    }
}

void join(pthread_t thread_id, void **retval){
    int err;
    if((err = pthread_join(thread_id, retval)) != 0){
        perror("Error while joining thread");
        exit(EXIT_FAILURE);
    }
}

void cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex){
    int err;
    if((err = pthread_cond_wait(cond, mutex)) != 0){
        perror("Error while waiting for condition");
        exit(EXIT_FAILURE);
    }
}

void cond_broadcast(pthread_cond_t *cond){
    int err;
    if((err = pthread_cond_broadcast(cond)) != 0){
        perror("Error while broadcasting condition");
        exit(EXIT_FAILURE);
    }
}

void cond_signal(pthread_cond_t *cond){
    int err;
    if((err = pthread_cond_signal(cond)) != 0){
        perror("Error while signaling condition");
        exit(EXIT_FAILURE);
    }
}

void cond_init (pthread_cond_t * cnd, const pthread_condattr_t * attr){
    int err;
    if((err = pthread_cond_init(cnd, attr)) != 0){
        perror("Error while initializing condition");
        exit(EXIT_FAILURE);
    }
}

void mtx_init (pthread_mutex_t * mtx, const pthread_mutexattr_t * attr){
    int err;
    if((err = pthread_mutex_init(mtx, attr)) != 0){
        perror("Error while initializing mutex");
        exit(EXIT_FAILURE);
    }
}

/* MEMORY ALLOCATION SECTION */
void *Malloc(size_t size){
    void *ptr;
    if((ptr = malloc(size)) == NULL){
        perror("Error while allocating memory");
        exit(EXIT_FAILURE);
    }
    return ptr;
}


/* SIGNALS */

int Sigwait(sigset_t *set, int *sig){
    int err;
    if((err = sigwait(set, sig)) != 0){
        perror("Error while waiting for signal");
        exit(EXIT_FAILURE);
    }
    return err;
}

void Sigemptyset(sigset_t *set){
    int err;
    if((err = sigemptyset(set)) != 0){
        perror("Error while emptying signal set");
        exit(EXIT_FAILURE);
    }
}

void Sigaddset(sigset_t *set, int signum){
    int err;
    if((err = sigaddset(set, signum)) != 0){
        perror("Error while adding signal to set");
        exit(EXIT_FAILURE);
    }
}


/* PROCESS */
int Waitpid(pid_t pid, int *status, int options){
    int err;
    if((err = waitpid(pid, status, options)) < 0){
        perror("Error while waiting for process");
        exit(EXIT_FAILURE);
    }
    return err;
}
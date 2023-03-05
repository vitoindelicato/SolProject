#include "enhanced_sc.h"
#include <stdlib.h>


/* THREAD FUNCTIONS SECTION */
int lock(pthread_mutex_t *mutex){
    int err;
    if((err = pthread_mutex_lock(mutex)) != 0){
        perror("Error while locking mutex");
        exit(EXIT_FAILURE);
    }
}

int unlock(pthread_mutex_t *mutex){
    int err;
    if((err = pthread_mutex_unlock(mutex)) != 0){
        perror("Error while unlocking mutex");
        exit(EXIT_FAILURE);
    }
}


int cancel(pthread_t *tid){
    int err;
    if((err = pthread_cancel(*tid)) != 0){
        perror("Error while canceling thread");
        exit(EXIT_FAILURE);
    }
}


static void cleanup(void* arg, pthread_mutex_t *mtx) {
    free(arg);
    unlock(mtx);
}


int create(pthread_t *tid, const pthread_attr_t *attr, void* (*function) (void *), void *args){
    int err;
    if((err = pthread_create(tid, attr, function, args)) != 0){
        perror("Error while creating thread");
        exit(EXIT_FAILURE);
    }
}

int join(pthread_t thread_id, void **retval){
    int err;
    if((err = pthread_join(thread_id, retval)) != 0){
        perror("Error while joining thread");
        exit(EXIT_FAILURE);
    }
}


int cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex){
    int err;
    if((err = pthread_cond_wait(cond, mutex)) != 0){
        perror("Error while waiting for condition");
        exit(EXIT_FAILURE);
    }
}



int cond_signal(pthread_cond_t *cond){
    int err;
    if((err = pthread_cond_signal(cond)) != 0){
        perror("Error while signaling condition");
        exit(EXIT_FAILURE);
    }
}


int cond_init (pthread_cond_t * cnd, const pthread_condattr_t * attr){
    int err;
    if((err = pthread_cond_init(cnd, attr)) != 0){
        perror("Error while initializing condition");
        exit(EXIT_FAILURE);
    }
}


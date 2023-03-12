#include "enhanced_sc.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
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


void *Malloc(size_t size){
    void *ptr;
    if((ptr = malloc(size)) == NULL){
        perror("Error while allocating memory");
        exit(EXIT_FAILURE);
    }
    return ptr;
}



size_t readn(int fd, void *ptr, size_t n) {
    size_t   nleft;
    ssize_t  nread;

    nleft = n;
    while (nleft > 0) {
        if((nread = read(fd, ptr, nleft)) < 0) {
            if (nleft == n) return -1; /* error, return -1 */
            else break; /* error, return amount read so far */
        } else if (nread == 0) break; /* EOF */
        nleft -= nread;
        ptr   += nread;
    }
    return(n - nleft); /* return >= 0 */
}


size_t writen(int fd, void *ptr, size_t n) {
    size_t   nleft;
    ssize_t  nwritten;

    nleft = n;
    while (nleft > 0) {
        if((nwritten = write(fd, ptr, nleft)) < 0) {
            if (nleft == n) return -1; /* error, return -1 */
            else break; /* error, return amount written so far */
        } else if (nwritten == 0) break;
        nleft -= nwritten;
        ptr   += nwritten;
    }
    return(n - nleft); /* return >= 0 */
}
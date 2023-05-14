#ifndef ENHANCED_SC_H
#define ENHANCED_SC_H

#include <pthread.h>
#include <signal.h>



/* THREADS SYSTEM CALLS */
void lock(pthread_mutex_t *mutex);
void unlock(pthread_mutex_t *mutex);
void cancel(pthread_t tid);
//static void cleanup(void* arg);

void create(pthread_t *tid, const pthread_attr_t *attr, void* (*function) (void *), void *args);
void join(pthread_t thread_id, void **retval);

void cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
void cond_signal(pthread_cond_t *cond);
void cond_broadcast(pthread_cond_t *cond);
void cond_init (pthread_cond_t * cnd, const pthread_condattr_t * attr);
void mtx_init (pthread_mutex_t * mtx, const pthread_mutexattr_t * attr);
// static void cleanup_handler(void *arg);

/* MEMORY ALLOCATION SYSTEM CALLS */
void *Malloc(size_t size);

/*   SIGNALS   */
int Sigwait(sigset_t *set, int *sig);
void Sigemptyset(sigset_t *set);
void Sigaddset(sigset_t *set, int signum);


/* PROCESS */
int Waitpid(pid_t pid, int *status, int options);

#endif



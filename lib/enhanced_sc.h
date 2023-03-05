#include <pthread.h>


/* THREADS SYSTEM CALLS */
void lock(pthread_mutex_t *mutex);
void unlock(pthread_mutex_t *mutex);
void cancel(pthread_t *tid);
//static void cleanup(void* arg);

void create(pthread_t *tid, const pthread_attr_t *attr, void* (*function) (void *), void *args);
void join(pthread_t thread_id, void **retval);

void cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
void cond_signal(pthread_cond_t *cond);
void cond_init (pthread_cond_t * cnd, const pthread_condattr_t * attr);

/* MEMORY ALLOCATION SYSTEM CALLS */
void *Malloc(size_t size);



/*   READ AND WRITES   */
size_t readn(int fd, void *ptr, size_t n);
size_t writen(int fd, void *ptr, size_t n);





#include <pthread.h>


/* THREADS SYSTEM CALLS */
int lock(pthread_mutex_t *mutex);
int unlock(pthread_mutex_t *mutex);
int cancel(pthread_t *tid);
static void cleanup(void* arg);

int create(pthread_t *tid, const pthread_attr_t *attr, void* (*function) (void *), void *args);
int join(pthread_t thread_id, void **retval);

int cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int cond_signal(pthread_cond_t *cond);
int cond_init (pthread_cond_t * cnd, const pthread_condattr_t * attr);

/* MEMORY ALLOCATION SYSTEM CALLS */
void *Malloc(size_t size);



/*   READ AND WRITES   */
ssize_t readn(int fd, void *ptr, size_t n);
ssize_t writen(int fd, void *ptr, size_t n);





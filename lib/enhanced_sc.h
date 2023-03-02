#include <pthread.h>

int lock(pthread_mutex_t *mutex);
int unlock(pthread_mutex_t *mutex);
int cancel(pthread_t *tid);


int create(pthread_t *thread_id, const pthread_attr_t *attr, void* (*function) (void *), void *args);
int join(pthread_t thread_id, void **retval);

int cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int cond_signal(pthread_cond_t *cond);







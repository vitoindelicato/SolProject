#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>


void *worker_function(void *args){
    /* Here I will write the thread function
     * Each thread will wait untill the concurrent queue is not empty
     * Then it will take one filename, will open the file, and will make its calculations based on the data contained by the file
     * then it will send the data somewhere...*/
    return;
}
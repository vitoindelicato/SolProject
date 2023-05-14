#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "../lib/tools.h"
#include "master_worker.h"
#include "collector.h"
#include "../lib/socket_utils.h"
#include <sys/un.h>
#include <sys/socket.h>


#define TD_POOL_SIZE  4
#define QUEUE_SIZE 8
#define TIME_DELAY 0
#define SOCKNAME "farm.sck"


pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;


int n_threads = TD_POOL_SIZE;
int q_size = QUEUE_SIZE;
int t_delay = TIME_DELAY;
char *dir_name = NULL;
int queue_interrupt = 0;

struct sockaddr_un client_addr;



static void signal_handler(int signum){
    /* In order to skip useless acquire of queue's lock I'm setting queue_interrupt flag to have more values
     * 0 - no signal has arrived yet
     * 1 - signal arrived, I acquire the queue's lock, and set queue->done = 1, immediatly after I set queue_interrupt =2
     * 2 - if queue_interrupt == 2 I can skip enqueue function and also explorer. */
    if(queue_interrupt == 0){
        queue_interrupt = 1;
        printf("Queue is being closed, please wait thread to finish\n");
    }

    /* When these kinds of signals arrive, I do not permit any file to be inserted into queue anymore
     * But still I have to wait every thread to empty queue, so after SIGINT program does not stop immediately*/

    else return;
}


static void *thread_signal_handler(void *arg){
    int sig;
    sigset_t *set = (sigset_t*)arg;
    int fd = client_connection(&client_addr);

    while(1){

        Sigwait(set, &sig);

        if(sig == SIGUSR1){
            printf("SIGUSR1 received\n");
            writen(fd, "PRINT", 5);
            continue;
        }
        else if(sig == SIGINT || sig == SIGQUIT || sig == SIGTERM || sig == SIGHUP){
            signal_handler(sig);
            continue;
        }

        else if(sig == SIGUSR2){
            break;

        }
        else{
            printf("Unknown signal received\n");
            continue;
        }

    }
    close(fd);
    return NULL;
}

void farm_clean() {
    unlink(SOCKNAME);
    //pthread_join(signal_thread,NULL);
}


int main (int argc, char **argv) {
    //TODO: 'https://stackoverflow.com/questions/4584904/what-causes-the-broken-pipe-error' EPIPE error

    printf("[FARM] PID: %d\n", getpid());

    if (atexit(farm_clean) != 0 ){
        perror("atexit");
        exit(EXIT_FAILURE);
    };


    int opt;


    client_addr.sun_family = AF_UNIX;
    strncpy(client_addr.sun_path, SOCKNAME, strlen(SOCKNAME)+1);

    sigset_t mask;
    Sigemptyset(&mask);
    Sigaddset(&mask, SIGINT);
    Sigaddset(&mask, SIGQUIT);
    Sigaddset(&mask, SIGTERM);
    Sigaddset(&mask, SIGHUP);
    Sigaddset(&mask, SIGUSR1);
    Sigaddset(&mask, SIGUSR2);
    //Sigaddset(&mask, SIGPIPE);

    if(pthread_sigmask(SIG_BLOCK, &mask, NULL) != 0)
    {
        perror("pthread_sigmask");
        exit(EXIT_FAILURE);
    }

    struct sigaction s;
    memset(&s, 0, sizeof(s));
    s.sa_handler=SIG_IGN;

    if(sigaction(SIGPIPE, &s, NULL) == -1 ){
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    pthread_t signal_thread;
    create(&signal_thread, NULL, thread_signal_handler, &mask);



    while ((opt = getopt(argc, argv, "n:q:d:t:")) != -1) {

        switch (opt) {

            case 'n':
                n_threads = strtol(optarg, NULL, 10);
                //printf("n_threads: %d\n", n_threads);
                break;

            case 'q':
                q_size = strtol(optarg, NULL, 10);
                //printf("q_size: %d\n", q_size);
                break;

            case 't':
                t_delay = strtol(optarg, NULL, 10);
                //printf("t_delay: %d\n", t_delay);
                break;

            case 'd':
                dir_name = optarg;
                //printf("dir_name: %s\n", dir_name);
                break;

            case '?':
                printf("Unknown option: %c\nExiting...\n", optopt);
                exit(EXIT_FAILURE);

            default:
                printf("entered default \n");
                break;
        }

    }
    pid_t pid = fork();

    if (pid > 0) { //PADRE
        //printf("PID: %d\n", getpid());
        master_worker(argc, argv, dir_name);

        Waitpid(pid, NULL, 0);
        //once that every thread has finished, thread to monitor signals can be cancelled
        //I send SIGUSR2 in order to break while(1) loop
        pthread_kill(signal_thread, SIGUSR2);

        join(signal_thread, NULL);
        cancel(signal_thread);

    }
    else if (pid == 0) { //FIGLIO
        collector();

    }
    else {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKNAME);
    return 0;
}

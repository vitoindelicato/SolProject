#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "../lib/tools.h"
#include "../lib/enhanced_sc.h"
#include "master_worker.h"
#include "collector.h"
#include "worker.h" // I really don't like this import, but I'm running out of time :/
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/wait.h>



#define TD_POOL_SIZE  4
#define QUEUE_SIZE 8
#define TIME_DELAY 0
#define SOCKNAME "farm.sck"


pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
volatile sig_atomic_t queue_interrupt = 0;


struct sockaddr_un saddr;
int n_threads = TD_POOL_SIZE;
int q_size = QUEUE_SIZE;
int t_delay = TIME_DELAY;
char *dir_name = NULL;
int stop_thread = 0;



static void signal_handler(int signum){
    queue_interrupt = 1;
}


static void *thread_signal_handler(void *arg){
    int sig;
    sigset_t *set = (sigset_t*)arg;

    while(1){

        sigwait(set, &sig);

        if(sig == SIGUSR1){
            printf("SIGUSR1 received\n");
            //signal_handler(sig);
            int fd = connect_wrapper();
            writen(fd, "PRINT", 5);
            close(fd);
            continue;
        }
        else if(sig == SIGINT || sig == SIGQUIT || sig == SIGTERM || sig == SIGHUP){
            printf("SIGINT, SIGQUIT, SIGTERM or SIGHUP received\n");
            signal_handler(sig);
            continue;
        }

    }
    return NULL;
}




int main (int argc, char **argv) {

    int opt;

    saddr.sun_family = AF_UNIX;
    strcpy(saddr.sun_path, SOCKNAME);



    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGHUP);
    sigaddset(&mask, SIGUSR1);

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
        master_worker(argc, argv, dir_name, optind);
        waitpid(pid, NULL, 0);
        pthread_cancel(signal_thread);
        join(signal_thread, NULL);

    } else if (pid == 0) { //FIGLIO
        collector();
    }
    else {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    return 0;
}

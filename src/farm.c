#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/tools.h"
#include "../lib/enhanced_sc.h"
#include "master_worker.h"
#include "collector.h"
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



static void signal_handler(int signum){
    queue_interrupt = 1;
}


int main (int argc, char **argv) {

    int opt;

    saddr.sun_family = AF_UNIX;
    strcpy(saddr.sun_path, SOCKNAME);
    //int fd;
    //char *filename;


    sigset_t set;
    int sig;
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sigemptyset(&set);
    sigaddset(&set, SIGTSTP);

    sa.sa_handler = signal_handler;
    if ( sigaction(SIGINT, &sa, NULL) == -1 ) {
        perror("Couldn't set SIGINT handler");
        exit(EXIT_FAILURE);
    }

    if ( sigaction(SIGQUIT, &sa, NULL) == -1 ) {
        perror("Couldn't set SIGINT handler");
        exit(EXIT_FAILURE);
    }


    if ( sigaction(SIGTERM, &sa, NULL) == -1 ) {
        perror("Couldn't set SIGINT handler");
        exit(EXIT_FAILURE);
    }

    if ( sigaction(SIGHUP, &sa, NULL) == -1 ) {
        perror("Couldn't set SIGINT handler");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = SIG_IGN;
    if ( sigaction(SIGPIPE, &sa, NULL) == -1 ) {
        perror("Couldn't set SIGINT handler");
        exit(EXIT_FAILURE);
    }



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
        master_worker(argc, argv, dir_name, optind);
        waitpid(pid, NULL, 0);

    } else if (pid == 0) { //FIGLIO
        collector();

    }
    return 0;
}
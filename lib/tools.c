#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include "queue.h"


int isDir(char *path){
    struct stat info;

    if(stat(path, &info) == -1){
        perror("stat");
        exit(EXIT_FAILURE);
    }
    else {
        return S_ISDIR(info.st_mode);
    }
}

void explorer(char *dir_name, _queue *queue){
    DIR *dir;
    char updated_path[PATH_MAX];
    if(  (dir = opendir(dir_name)) == NULL  ){
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    else{
        errno = 0;
        struct dirent *file;

        while ( (file = readdir(dir)) != NULL && (errno == 0 ) ){

            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
                continue;
            }
            strncpy(updated_path, dir_name, strlen(dir_name) + 1);
            strncat(updated_path, "/", 2);
            strncat(updated_path, file->d_name, strlen(file->d_name) + 1);
            if ( isDir(updated_path) ) {
                explorer(updated_path, queue);
            }
            else {
                enqueue(queue, updated_path);
            }
        }
        //free(updated_path);

        if (errno != 0) {
            perror("readdir");

        }
        else {
            closedir(dir);
        }
    }
}

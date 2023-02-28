#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>


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



void explorer(char *dir_name){
    DIR *dir;

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

            char *updated_path = malloc(PATH_MAX * sizeof(char));

            strncpy(updated_path, dir_name, strlen(dir_name) + 1);
            strncat(updated_path, "/", 2);
            strncat(updated_path, file->d_name, strlen(file->d_name) + 1);

            if ( isDir(updated_path) ) {
                explorer(updated_path);
            }
            else {
                printf("file: %s\n", updated_path);
                //closedir(dir);
                //return updated_path;
            }

        }

        if (errno != 0) {
            perror("readdir");
            //return NULL;
        }
        else {
            closedir(dir);
            //return NULL;
        }
    }
}

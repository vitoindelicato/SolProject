#ifndef TOOLS_H
#define TOOLS_H
#include "queue.h"

int isDir(char *path);
void explorer(char *dir_name, _queue *queue);
int isRegular(char *filename);

#endif

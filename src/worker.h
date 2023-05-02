#ifndef WORKER_H
#define WORKER_H

#include "../lib/enhanced_sc.h"

void *worker_function(void *args);
long calculator(char *filename);
int connect_wrapper();

#endif
#ifndef _LIBRARY_ROUTINES_
#define _LIBRARY_ROUTINES_
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#define MAX_SHOP 5
#define MAX_BUYER 3
extern FILE *file;
extern int shop[MAX_SHOP];
extern pthread_mutex_t shop_mutex[MAX_SHOP];
extern bool flagsloader = true;
void *routine(void *n);
#endif
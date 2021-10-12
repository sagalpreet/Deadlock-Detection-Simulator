#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>

#include "log.h"

extern pthread_mutex_t MUTEX_LOG;

void log(FILE *f, char *format, ...)
{
    pthread_mutex_lock(&MUTEX_LOG);
    time_t t;
    va_list args;
    va_start(args, format);

    time(&t);
    char *time = ctime(&t);
    time[24] = 0;
    fprintf(f, "%s : ", time);
    vfprintf(f, format, args);

    va_end(args);
    pthread_mutex_unlock(&MUTEX_LOG);
}
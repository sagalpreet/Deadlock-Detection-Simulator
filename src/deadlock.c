#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "deadlock.h"
#include "worker.h"
#include "resource.h"

extern int MAX_THREADS;
extern double DELAY;
extern pthread_mutex_t MUTEX;
extern int **THREAD_RESOURCES_REQUESTED;
extern int **THREAD_RESOURCES_REQUIRED;
extern pthread_t *WORKERS;

void* detect_deadlock(void* arg)
{
    // typecasting to get reference to resources
    struct resource_pool *POOL = (struct resource_pool *) arg;

    // getting reference to the array of resources
    struct resource *resources = POOL -> resources;

    // getting the count of number of different resources available
    // and making a check if details of all the resources are available
    int num_resources = POOL -> size_cur;
    if (num_resources != POOL -> size_max)
    {
        printf("warning: worker thread trying to access the details of resources before all of them are set\n");
    }

    while (1)
    {
        sleep(DELAY);

        pthread_mutex_lock(&MUTEX);

        // code for deadlock detection goes here

        pthread_mutex_unlock(&MUTEX);
    }
}



#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "worker.h"
#include "resource.h"

extern double DELAY;
extern pthread_mutex_t MUTEX;
extern int **THREAD_RESOURCES_REQUESTED;
extern int **THREAD_RESOURCES_REQUIRED;
extern pthread_t *WORKERS;

void* worker_routine(void * arg)
{
    // typecasting to get reference to resources
    struct resource_pool *POOL = (struct resource_pool *) arg;

    // initializing random number generator
    time_t t;
    srand((unsigned) time(&t));

    // getting reference to the array of resources
    struct resource *resources = POOL -> resources;
    
    // getting the count of number of different resources available
    // and making a check if details of all the resources are available
    int num_resources = POOL -> size_cur;
    if (num_resources != POOL -> size_max)
    {
        printf("warning: worker thread trying to access the details of resources before all of them are set\n");
    }

    // getting the thread id and mapping of this thread in workers thread pool
    pthread_t tid = pthread_self();
    int tmap = -1;
    for (;tmap < num_resources; tmap++) if (WORKERS[tmap] == tid) break;

    if (tmap == num_resources) printf("warning: unmapped thread found\n");

    // creating the request array
    int *request = THREAD_RESOURCES_REQUIRED[tmap];
    int *request_copy = THREAD_RESOURCES_REQUESTED[tmap];
    
    while (1)
    {
        // setting up request array
        for (int i = 0; i < num_resources; i++) request[i] = request_copy[i] = rand() % (resources[i].r_count);
        
        // acquiring resources
        int acquired = 0;
        while (acquired < num_resources)
        {
            int resource_id = rand() % num_resources; // inherent random pauses between allocations

            if (request[resource_id] == 0) continue;
            
            pthread_mutex_lock(&MUTEX);
            if (request[resource_id] > resources[resource_id].r_free)
            {
                request[resource_id] -= resources[resource_id].r_free;
                resources[resource_id].r_free = 0;
                pthread_mutex_unlock(&MUTEX);
                continue;
            }

            resources[resource_id].r_free -= request[resource_id];
            request[resource_id] = 0;
            pthread_mutex_unlock(&MUTEX);

            acquired++;
        }

        // randomly choosing sleep time
        float timer = DELAY * ((rand() % 9) + 7.0) / 10;
        sleep(timer);

        // deallocating resources used
        for (int i = 0; i < num_resources; i++)
        {
            pthread_mutex_lock(&MUTEX);
            resources[i].r_free += request_copy[i];
            pthread_mutex_unlock(&MUTEX);
        }

        printf("Successfully completed the request and deallocated all the resources\n");
    }
}
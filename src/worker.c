#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include "worker.h"
#include "resource.h"
#include "log.h"

const int FORCE_DEADLOCK = 1;

extern double DELAY;
extern int MAX_THREADS;
extern pthread_mutex_t MUTEX;
extern int **THREAD_RESOURCES_REQUESTED;
extern int **THREAD_RESOURCES_REQUIRED;
extern pthread_t *WORKERS;
extern char *WORKER_STATUS;
extern pthread_mutex_t MUTEX_BEAUTIFUL_LOG;

void* worker_routine(void * arg)
{
    // getting the thread id and mapping of this thread in workers thread pool
    pthread_t tid = pthread_self();
    int tmap = 0;
    for (;tmap < MAX_THREADS; tmap++) if (WORKERS[tmap] == tid) break;

    if (tmap == MAX_THREADS) printf("warning: unmapped thread found\n");

    // opening log file
    FILE* fptr = fopen("../log/log.txt", "a");
    setbuf(fptr, NULL);
    
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

    // creating the request array
    int *request = THREAD_RESOURCES_REQUIRED[tmap];
    int *request_copy = THREAD_RESOURCES_REQUESTED[tmap];
    
    while (1)
    {
        // setting up request array
        if (FORCE_DEADLOCK)
        {
            for (int i = 0; i < num_resources; i++) request[i] = request_copy[i] = resources[i].r_count;
        }
        else
        {
            for (int i = 0; i < num_resources; i++) request[i] = request_copy[i] = rand() % (resources[i].r_count + 1);
        }

        pthread_mutex_lock(&MUTEX_BEAUTIFUL_LOG);
        fprintf(fptr, "\n");
        log(fptr, "Thread %d (%lu): Resource Requests made:\n", tmap, tid);
        for (int i = 0; i < num_resources; i++) log(fptr, "Resource %d: %d\n", i, request[i]);
        fprintf(fptr, "\n");
        pthread_mutex_unlock(&MUTEX_BEAUTIFUL_LOG);
        
        // zero requirement resource count
        int z_count = 0;
        for (int i = 0; i < num_resources; i++) if (request[i] == 0) z_count++;

        // acquiring resources
        int acquired = 0;
        while (acquired + z_count < num_resources)
        {
            if (WORKER_STATUS[tmap] == 0)
            {
                pthread_mutex_lock(&MUTEX_BEAUTIFUL_LOG);
                log(fptr, "Thread %d (%lu): killed and all the resources acquired by it released\n", tmap, tid);
                pthread_mutex_unlock(&MUTEX_BEAUTIFUL_LOG);
                WORKERS[tmap] = 0;
                for (int r = 0; r < num_resources; r++)
                {
                    resources[r].r_free += THREAD_RESOURCES_REQUESTED[tmap][r] - THREAD_RESOURCES_REQUIRED[tmap][r];
                    THREAD_RESOURCES_REQUESTED[tmap][r] = THREAD_RESOURCES_REQUIRED[tmap][r] = 0;
                }
                pthread_exit(NULL);
            }

            if (FORCE_DEADLOCK) sleep(2); // debug statement to force deadlock

            int resource_id = rand() % num_resources; // inherent random pauses between allocations

            if (request[resource_id] == 0) continue;
            
            pthread_mutex_lock(&MUTEX);
            if (request[resource_id] > resources[resource_id].r_free)
            {
                if (resources[resource_id].r_free > 0)
                {
                    pthread_mutex_lock(&MUTEX_BEAUTIFUL_LOG);
                    log(fptr, "Thread %d (%lu): %d units of Resource %d acquired\n", tmap, tid, resources[resource_id].r_free, resource_id);
                    pthread_mutex_unlock(&MUTEX_BEAUTIFUL_LOG);
                }
                
                request[resource_id] -= resources[resource_id].r_free;
                resources[resource_id].r_free = 0;

                pthread_mutex_unlock(&MUTEX);
                continue;
            }

            if (request[resource_id] > 0)
            {
                pthread_mutex_lock(&MUTEX_BEAUTIFUL_LOG);
                log(fptr, "Thread %d (%lu): %d units of Resource %d acquired\n", tmap, tid, request[resource_id], resource_id);
                pthread_mutex_unlock(&MUTEX_BEAUTIFUL_LOG);
            }
            resources[resource_id].r_free -= request[resource_id];
            request[resource_id] = 0;
            pthread_mutex_unlock(&MUTEX);

            acquired++;
        }
        
        pthread_mutex_lock(&MUTEX_BEAUTIFUL_LOG);
        log(fptr, "Thread %d (%lu): All the requirements for the process fulfilled. Initiating Process (sleep time)\n", tmap, tid);
        pthread_mutex_unlock(&MUTEX_BEAUTIFUL_LOG);

        // randomly choosing sleep time
        float timer = DELAY * ((rand() % 9) + 7.0) / 10;
        usleep(timer);

        // deallocating resources used
        for (int i = 0; i < num_resources; i++)
        {
            pthread_mutex_lock(&MUTEX);
            resources[i].r_free += request_copy[i];
            pthread_mutex_unlock(&MUTEX);
        }

        pthread_mutex_lock(&MUTEX_BEAUTIFUL_LOG);
        log(fptr, "Successfully completed the request and deallocated all the resources\n\n");
        pthread_mutex_unlock(&MUTEX_BEAUTIFUL_LOG);
    }
    fclose(fptr);
}
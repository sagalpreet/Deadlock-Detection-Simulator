#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include "worker.h"
#include "resource.h"

extern double DELAY;
extern int MAX_THREADS;
extern pthread_mutex_t MUTEX;
extern int **THREAD_RESOURCES_REQUESTED;
extern int **THREAD_RESOURCES_REQUIRED;
extern pthread_t *WORKERS;

void* worker_routine(void * arg)
{
    // getting the thread id and mapping of this thread in workers thread pool
    pthread_t tid = pthread_self();
    int tmap = 0;
    for (;tmap < MAX_THREADS; tmap++) if (WORKERS[tmap] == tid) break;

    if (tmap == MAX_THREADS) printf("warning: unmapped thread found\n");

    // opening log file
    char location[100] = "../log/";
    char str_tid[20];
    itoa(tid, str_tid);
    strcat(location, str_tid);

    FILE* log = fopen(location, "w");
    setbuf(log, NULL);
    fprintf(log, "Thread Id: %lu\n\n", tid);
    
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
        for (int i = 0; i < num_resources; i++) request[i] = request_copy[i] = rand() % (resources[i].r_count);
        fprintf(log, "Resource Requests made:\n");
        for (int i = 0; i < num_resources; i++) fprintf(log, "Resource %d: %d\n", i, request[i]);
        
        // zero requirement resource count
        int z_count = 0;
        for (int i = 0; i < num_resources; i++) if (request[i] == 0) z_count++;

        // acquiring resources
        int acquired = 0;
        while (acquired + z_count < num_resources)
        {
            int resource_id = rand() % num_resources; // inherent random pauses between allocations

            if (request[resource_id] == 0) continue;
            
            pthread_mutex_lock(&MUTEX);
            if (request[resource_id] > resources[resource_id].r_free)
            {
                if (resources[resource_id].r_free > 0) fprintf(log, "%d units of Resource %d acquired\n", resources[resource_id].r_free, resource_id);
                
                request[resource_id] -= resources[resource_id].r_free;
                resources[resource_id].r_free = 0;

                pthread_mutex_unlock(&MUTEX);
                continue;
            }

            if (request[resource_id] > 0) fprintf(log, "%d units of Resource %d acquired\n", request[resource_id], resource_id);
            resources[resource_id].r_free -= request[resource_id];
            request[resource_id] = 0;
            pthread_mutex_unlock(&MUTEX);

            acquired++;
            fprintf(log, "%d resources requirement fulfilled\n", acquired);
        }

        fprintf(log, "All the requirements for the process fulfilled. Initiating Process (sleep time)\n");

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

        fprintf(log, "Successfully completed the request and deallocated all the resources\n\n");
    }
    fclose(log);
}

char* itoa(unsigned long long num, char* str)
{
    // converts an integer into string and stores it in the second argument (char* pointer), also returns the same
    int i = 0;
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    while (num != 0)
    {
        int rem = num % 10;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/10;
    }
    str[i] = '\0';
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++)
    {
        char temp = str[i];
        str[i] = str[len-i-1];
        str[len-i-1] = temp;
    }
    return str;
}
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "main.h"
#include "resource.h"
#include "worker.h"
#include "deadlock.h"

struct resource_pool *POOL; // pool of resources
int MAX_THREADS; // limit on the number of threads
double DELAY; // delay
pthread_t *WORKERS; // worker threads
extern int **THREAD_RESOURCES_REQUESTED; // maintains track of resources requested up by each thread
extern int **THREAD_RESOURCES_REQUIRED; // maintains track of resources used up by each thread

pthread_mutex_t MUTEX = PTHREAD_MUTEX_INITIALIZER; // mutex lock

int main(int argc, char const *argv[])
// format example: A 1 B 2 C 3 10 5 // make sure none of the instance count should be equal to 0
{
    argc--;
    if (argc % 2 || argc < 2)
    {
        printf("Illegal Command-Line Arguments\n");
        exit(1);
        // failure to run ==> even number of arguments required 
        // (name, quantity) pairs, number of threads and interval
    }
    
    int num_resources = (argc / 2) - 1;
    POOL = resource_pool(num_resources);
    
    for (int i = 0; i < num_resources; i++)
    {
        int r_count = atoi(argv[2*i + 2]);
        if (r_count == 0)
        {
            printf("Illegal value for number of instances of a resource\n");
            exit(1);
        }
        append(POOL, r_count, argv[2*i+1]);
    }

    MAX_THREADS = atoi(argv[argc-2]);
    if (MAX_THREADS == 0)
    {
        printf("The Number of threads cannot be equal to 0\n");
        exit(1);
    }

    DELAY = atof(argv[argc-1]);

    // initialize worker threads
    WORKERS = (pthread_t *) malloc (sizeof(pthread_t) * MAX_THREADS);
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&WORKERS[i], NULL, &worker_routine, POOL);
    
    THREAD_RESOURCES_REQUESTED = (int **) malloc (sizeof(int *) * MAX_THREADS);
    THREAD_RESOURCES_REQUIRED = (int **) malloc (sizeof(int *) * MAX_THREADS);
    for (int i = 0; i < MAX_THREADS; i++)
    {
        THREAD_RESOURCES_REQUESTED[i] = (int *) malloc (sizeof (int) * num_resources);
        THREAD_RESOURCES_REQUIRED[i] = (int *) malloc (sizeof (int) * num_resources);
        for (int j = 0; j < num_resources; j++) THREAD_RESOURCES_REQUESTED[i][j] = THREAD_RESOURCES_REQUIRED[i][j] = 0;
    }

    pthread_t deadlock;
    pthread_create(&deadlock, NULL, &detect_deadlock, POOL);

    // not exiting the program until threads are running
    for (int i = 0; i < MAX_THREADS; i++) pthread_join(WORKERS[i], NULL);
    pthread_join(deadlock, NULL);
}
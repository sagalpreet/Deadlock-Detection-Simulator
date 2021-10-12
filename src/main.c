#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "main.h"
#include "resource.h"
#include "worker.h"
#include "deadlock.h"
#include "log.h"

struct resource_pool *POOL; // pool of resources
int MAX_THREADS; // limit on the number of threads
double DELAY; // delay
pthread_t *WORKERS; // worker threads
char *WORKER_STATUS; // shared memory for communication between worker threads and main function
int **THREAD_RESOURCES_REQUESTED; // maintains track of resources requested up by each thread
int **THREAD_RESOURCES_REQUIRED; // maintains track of more resources required by thread

pthread_mutex_t MUTEX = PTHREAD_MUTEX_INITIALIZER; // mutex lock for making resources array thread-safe
pthread_mutex_t MUTEX_LOG = PTHREAD_MUTEX_INITIALIZER; // mutex lock for making file logging thread-safe
pthread_mutex_t MUTEX_BEAUTIFUL_LOG = PTHREAD_MUTEX_INITIALIZER; // mutex lock for making file logging of resource details continuous

int main(int argc, char const *argv[])
// format example: A 1 B 2 C 3 10 5 // make sure none of the instance count should be equal to 0
{
    FILE* fptr_clean_log = fopen("../log/log.txt", "w");
    FILE* fptr_clean_deadlock = fopen("../log/deadlock.txt", "w");
    fclose(fptr_clean_log);
    fclose(fptr_clean_deadlock);

    FILE* fptr = fopen("../log/main.txt", "w");
    setbuf(fptr, NULL);
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

    MAX_THREADS = atoi(argv[argc-1]);
    if (MAX_THREADS == 0)
    {
        printf("The Number of threads cannot be equal to 0\n");
        exit(1);
    }

    DELAY = atof(argv[argc]);
    
    // allocate memory for storing resource consumptioon information
    THREAD_RESOURCES_REQUESTED = (int **) malloc (sizeof(int *) * MAX_THREADS);
    THREAD_RESOURCES_REQUIRED = (int **) malloc (sizeof(int *) * MAX_THREADS);
    for (int i = 0; i < MAX_THREADS; i++)
    {
        THREAD_RESOURCES_REQUESTED[i] = (int *) malloc (sizeof (int) * num_resources);
        THREAD_RESOURCES_REQUIRED[i] = (int *) malloc (sizeof (int) * num_resources);
        for (int j = 0; j < num_resources; j++) THREAD_RESOURCES_REQUESTED[i][j] = THREAD_RESOURCES_REQUIRED[i][j] = 0;
    }

    // initialize worker threads
    WORKERS = (pthread_t *) malloc (sizeof(pthread_t) * MAX_THREADS);
    WORKER_STATUS = (char *) malloc (sizeof(char) * MAX_THREADS);
    for (int i = 0; i < MAX_THREADS; i++)
    {
        WORKER_STATUS[i] = 1;
        pthread_create(&WORKERS[i], NULL, &worker_routine, POOL);
    }

    pthread_t deadlock;
    pthread_create(&deadlock, NULL, &detect_deadlock, POOL);

    while (1) // constantly check if a worker thread has been destroyed by deadlock thread
    {
        for (int i = 0; i < MAX_THREADS; i++)
        {
            if (WORKERS[i] == 0)
            {
                log(fptr, "Thread %d is being restarted...\n", i);
                WORKER_STATUS[i] = 1;
                pthread_create(&WORKERS[i], NULL, &worker_routine, POOL);
            }
        }
    }

    // not exiting the program until interrupted
   pthread_join(deadlock, NULL);

   fclose(fptr);
}
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

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
    // opening log file
    FILE* log = fopen("../log/deadlock_detection", "w");
    setbuf(log, NULL);

    // typecasting to get reference to resources
    struct resource_pool *POOL = (struct resource_pool *) arg;

    // getting reference to the array of resources
    struct resource *resources = POOL -> resources;

    // getting the count of number of different resources available
    // and making a check if details of all the resources are available
    int num_resources = POOL -> size_cur;
    if (num_resources != POOL -> size_max)
    {
        fprintf(log, "warning: worker thread trying to access the details of resources before all of them are set\n");
    }

    while (1)
    {
        usleep(DELAY);
        
        fprintf(log, "Deadlock Checking Begins\n");

        pthread_mutex_lock(&MUTEX);

        // code for deadlock detection goes here
        
        int done[MAX_THREADS]; // indicate which threads are trapped in deadlock
        for (int i = 0; i < MAX_THREADS; i++) done[i] = 0;

        //  0 -> not processed yet
        //  1 -> not to be killed
        // -1 -> to be killed

        int available[num_resources];
        for (int i = 0; i < num_resources; i++) available[i] = resources[i].r_free;

        char is_first_iteration = 1;

        while (1)
        {
            while (1)
            {
                char has_converged = 1; // boolean variable to indicate whether or not while loop should be broken
                for (int t = 0; t < MAX_THREADS; t++)
                {
                    if (done[t] != 0) continue;

                    char is_deadlocked = 0;
                    for (int r = 0; r < num_resources; r++)
                    {
                        if (available[r] < THREAD_RESOURCES_REQUIRED[t][r])
                        {
                            is_deadlocked = 1;
                            break;
                        }
                    }

                    if (is_deadlocked == 0)
                    {
                        has_converged = 0;
                        done[t] = 1;
                        for (int r = 0; r < num_resources; r++) available[r] += THREAD_RESOURCES_REQUESTED[t][r] - THREAD_RESOURCES_REQUIRED[t][r];
                    }
                }

                if (has_converged == 1) break;
            }
            
            if (is_first_iteration == 1)
            {
                is_first_iteration = 0;
                fprintf(log, "The processes involved in deadlock reside on the following threads:\n");
                for (int i = 0; i < MAX_THREADS; i++)
                {
                    if (done[i] == 0)
                    {
                        fprintf(log, "%lu ", WORKERS[i]);
                    }
                }
                fprintf(log, "\n");
            }

            // heuristic function decides which process to kill -> returns -1 if already all processes are in either states: out of deadlock or killed
            int index = first_thread_heuristic(num_resources, done, available);
            if (index == -1) break;

            for (int i = 0; i < num_resources; i++) available[i] += THREAD_RESOURCES_REQUESTED[index][i] - THREAD_RESOURCES_REQUIRED[index][i];
            fprintf(log, "Process on thread %lu killed\n", WORKERS[index]);
        }

        // kill (decided to be) killed threads
        for (int i = 0; i < MAX_THREADS; i++)
        {
            if (done[i] == -1)
            {
                while(pthread_cancel(WORKERS[i]));
                for (int r = 0; r < num_resources; r++)
                {
                    resources[r].r_free += THREAD_RESOURCES_REQUESTED[i][r] - THREAD_RESOURCES_REQUIRED[i][r];
                    THREAD_RESOURCES_REQUESTED[i][r] = THREAD_RESOURCES_REQUIRED[i][r] = 0;
                }
                pthread_create(&WORKERS[i], NULL, &worker_routine, POOL);
            }
        }

        fprintf(log, "\n-----------Deadlock Checking Ends------------\n\n\n");

        pthread_mutex_unlock(&MUTEX);
    }

    fclose(log);
}

int first_thread_heuristic(int num_resources, int* done, int* available)
{
    int not_done = -1;
    for (int t = 0; t < MAX_THREADS; t++)
    {
        if (done[t] == 0)
        {
            done[t] = -1;
            not_done = t;
            break;
        }
    }

    return not_done;
}

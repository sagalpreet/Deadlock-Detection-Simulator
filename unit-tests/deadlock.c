#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#include "deadlock.h"

int MAX_THREADS;
double DELAY;
pthread_mutex_t MUTEX;
int THREAD_RESOURCES_REQUESTED[4][3] = {{3, 4, 5}, {3, 2, 5}, {2, 2, 0}, {3, 3, 3}};;
int THREAD_RESOURCES_REQUIRED[4][3] = {{2, 4, 5}, {3, 0, 5}, {0, 2, 0}, {3, 3, 0}};
pthread_t *WORKERS;
char *WORKER_STATUS;

int main()
{
    printf("Running Unit Tests for Deadlock.c functions...\n\n");

    printf("SET 1\n");

    {
        // SET 1
        int num_resources = 3;
        MAX_THREADS = 4;
        
        int available[] = {0, 0, 0};
        int done[] = {0, 0, 0, 0};

        int ans;

        ans = first_thread_heuristic(num_resources, done, available);
        if (ans == 0) printf("TEST CASE 1.a PASSED\n");
        else printf("TEST CASE 1.a FAILED\n");
        if (done[0] == -1) printf("TEST CASE 1.b PASSED\n");
        else printf("TEST CASE 1.b FAILED\n");

        ans = last_thread_heuristic(num_resources, done, available);
        if (ans == 3) printf("TEST CASE 2.a PASSED\n");
        else printf("TEST CASE 2.a FAILED\n");
        if (done[3] == -1) printf("TEST CASE 2.b PASSED\n");
        else printf("TEST CASE 2.b FAILED\n");

        ans = heaviest_thread_heuristic(num_resources, done, available);
        if (ans == 2) printf("TEST CASE 3.a PASSED\n");
        else printf("TEST CASE 3.a FAILED\n");
        if (done[2] == -1) printf("TEST CASE 3.b PASSED\n");
        else printf("TEST CASE 3.b FAILED\n");
    }

    printf("\n\n");

    printf("SET 2\n");
    
    {
        // SET 2
        int num_resources = 3;
        MAX_THREADS = 4;
        
        int available[] = {0, 0, 4};
        int done[] = {1, 0, 0, -1};

        int ans;

        ans = first_thread_heuristic(num_resources, done, available);
        if (ans == 1) printf("TEST CASE 1.a PASSED\n");
        else printf("TEST CASE 1.a FAILED\n");
        if (done[1] == -1) printf("TEST CASE 1.b PASSED\n");
        else printf("TEST CASE 1.b FAILED\n");

        ans = last_thread_heuristic(num_resources, done, available);
        if (ans == 2) printf("TEST CASE 2.a PASSED\n");
        else printf("TEST CASE 2.a FAILED\n");
        if (done[2] == -1) printf("TEST CASE 2.b PASSED\n");
        else printf("TEST CASE 2.b FAILED\n");

        ans = heaviest_thread_heuristic(num_resources, done, available);
        if (ans == -1) printf("TEST CASE 3.a PASSED\n");
        else printf("TEST CASE 3.a FAILED\n");
        ans = 0;
        for (int i = 0; i < num_resources; i++) if (done[i] == 0) ans = 1;
        if (ans == 0) printf("TEST CASE 3.b PASSED\n");
        else printf("TEST CASE 3.b FAILED\n");
    }

    return 0;
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

int last_thread_heuristic(int num_resources, int* done, int* available)
{
    int not_done = -1;
    for (int t = MAX_THREADS - 1; t > -1; t--)
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

int heaviest_thread_heuristic(int num_resources, int* done, int* available)
{
    int not_done = -1;
    int max_weight = 0;

    for (int t = MAX_THREADS - 1; t > -1; t--)
    {
        if (done[t] == 0)
        {
            int sm = 0;
            for (int r = 0; r < num_resources; r++) sm += THREAD_RESOURCES_REQUESTED[t][r] - THREAD_RESOURCES_REQUIRED[t][r];
            if (sm <= max_weight) continue;
            not_done = t;
            max_weight = sm;
        }
    }

    if (not_done != -1) done[not_done] = -1;

    return not_done;
}
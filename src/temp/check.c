#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

void* f(void* arg_)
{
    pthread_t tid = pthread_self();
    printf("Inside thread function: %lu\n", tid);
    return NULL;
}

int main()
{
    pthread_t x;
    pthread_create(&x, NULL, &f, "1\0");
    printf("Outside thread function: %lu\n", x);
    pthread_join(x, NULL);
}
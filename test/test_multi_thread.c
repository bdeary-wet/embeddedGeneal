#include "unity.h"

#include "multi_thread.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


void setUp(void)
{
}

void tearDown(void)
{
}

void test_multi_thread_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement multi_thread");
}


int go = 1;

void *threadfunction(void *arg)
{
    uint32_t *cnt = (uint32_t*) arg;
    while(go)
    {
        //nanosleep(&(struct timespec){.tv_nsec=10000}, NULL);
        (*cnt)++;
        sched_yield();
    }
    return 0;
}
#define THREADS 16

void test_multi_thread(void)
{
    pthread_t td[THREADS];
    uint32_t ticks[THREADS] = {0};


    int err;
    for (int i=0; i<THREADS;i++)
    {
        TEST_ASSERT_EQUAL_MESSAGE(0, 
            (err = pthread_create(&td[i], NULL, threadfunction, &ticks[i])),
            strerror(err) );                 
    }


    nanosleep(&(struct timespec){.tv_sec=5}, NULL);
    go = 0;
    for (int i=0; i<THREADS;i++)
    {
        pthread_join(td[i], NULL);
    }
    for (int i=0; i<THREADS;i++)
    {
        printf("%i %u, ",i, ticks[i]);
    }
    printf("\n");
}
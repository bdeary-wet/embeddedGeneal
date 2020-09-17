#include "unity.h"

#include "multi_thread.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


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


void *threadfunction(void *arg)
{
    static int cnt = (int){0};

    cnt++;
    char *str = arg;
    char scpy[10];
    strcpy(scpy, str);
    for(int i = 0; i<strlen(str);i++)
    {
        printf("Hello, World! %s -> %i\n", str, cnt); /*printf() is specified as thread-safe as of C11*/
        sleep(1);
    }
    strcpy(str,"done ");
    strcpy(&str[5], scpy);
    cnt--;
    return 0;
}


void test_multi_thread(void)
{
    pthread_t t1,t2,t3;
    int err;
    char s1[20] = "one";
    char s2[20] = "two";
    char s3[20] = "three";
    TEST_ASSERT_EQUAL_MESSAGE(0, 
        (err = pthread_create(&t3, NULL, threadfunction, s3)),
        strerror(err) );                 
    TEST_ASSERT_EQUAL_MESSAGE(0, 
        (err = pthread_create(&t1, NULL, threadfunction, s1)),
        strerror(err) ); 
    TEST_ASSERT_EQUAL_MESSAGE(0, 
        (err = pthread_create(&t2, NULL, threadfunction, s2)),
        strerror(err) ); 

    sleep(2);
    pthread_join(t2, NULL);
    pthread_join(t1, NULL);
    pthread_join(t3, NULL);
    printf("1-%s 2-%s 3-%s .\n", s1, s2, s3);
    printf("1-%s 2-%s 3-%s .\n", s1, s2, s3);
    printf("1-%s 2-%s 3-%s .\n", s1, s2, s3);
}
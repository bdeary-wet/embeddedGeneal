#include "unity.h"
#include "tasker.h"
#include <string.h>

int *called;
int *eventUsed;


static void task0(int event) { called[0]++; eventUsed[event]++; }
static void task1(int event) { called[1]++; eventUsed[event]++; }
static void task2(int event) { called[2]++; eventUsed[event]++; }
static void task3(int event) { called[3]++; eventUsed[event]++; }
static void task4(int event) { called[4]++; eventUsed[event]++; }

taskerFunction_f jumpTable[] =
{
    task0, // 0
    task1, // 1
    task2, // 2
    task3, // 3
    task4, // 4
    task3, // 5
    task2, // 6
    task1, // 7
    task0  // 8
};

tasker_t tasker =
{
    {jumpTable, DIM(jumpTable)},
    0,
    0
};



int wasCalled[5];
int eventWasUsed[DIM(jumpTable)];

void setUp(void)
{
    // clear counters
    called = wasCalled;
    memset(wasCalled,0,sizeof(wasCalled));
    eventUsed = eventWasUsed;
    memset(eventWasUsed,0,sizeof(eventWasUsed));
}

void tearDown(void)
{
}

void test_TaskerRoundRobin(void)
{
    TaskerSetEvent(&tasker, 0);
    TEST_ASSERT_EQUAL(0x1, tasker.event);
    TaskerSetEvent(&tasker, 2);
    TEST_ASSERT_EQUAL(0x5, tasker.event);
    TaskerSetEvent(&tasker, 7);
    TEST_ASSERT_EQUAL(0x85, tasker.event);    
    TaskerSetEvent(&tasker, 5);
    TEST_ASSERT_EQUAL(0xA5, tasker.event);      
    TEST_ASSERT_EQUAL(0, TaskerRoundRobin(&tasker));
    TEST_ASSERT_EQUAL(0xA4, tasker.event);     
    TEST_ASSERT_EQUAL(2, TaskerRoundRobin(&tasker));
    TEST_ASSERT_EQUAL(0xA0, tasker.event);       
    TEST_ASSERT_EQUAL(5, TaskerRoundRobin(&tasker));
    TEST_ASSERT_EQUAL(0x80, tasker.event);       
    TEST_ASSERT_EQUAL(7, TaskerRoundRobin(&tasker));   
    TEST_ASSERT_EQUAL(0x0, tasker.event);    
    TEST_ASSERT_EQUAL(1, wasCalled[0]);
    TEST_ASSERT_EQUAL(1, wasCalled[1]);
    TEST_ASSERT_EQUAL(1, wasCalled[2]);
    TEST_ASSERT_EQUAL(1, wasCalled[3]);
    TEST_ASSERT_EQUAL(0, wasCalled[4]);
    TEST_ASSERT_EQUAL(1,eventWasUsed[0]);
    TEST_ASSERT_EQUAL(0,eventWasUsed[1]);
    TEST_ASSERT_EQUAL(1,eventWasUsed[2]);
    TEST_ASSERT_EQUAL(0,eventWasUsed[3]);
    TEST_ASSERT_EQUAL(0,eventWasUsed[4]);
    TEST_ASSERT_EQUAL(1,eventWasUsed[5]);
    TEST_ASSERT_EQUAL(0,eventWasUsed[6]);
    TEST_ASSERT_EQUAL(1,eventWasUsed[7]);
    TEST_ASSERT_EQUAL(0,eventWasUsed[8]);    
}

void test_TaskerPrioritized(void)
{
    TaskerSetEvent(&tasker, 0);
    TEST_ASSERT_EQUAL(0x1, tasker.event);
    TaskerSetEvent(&tasker, 2);
    TEST_ASSERT_EQUAL(0x5, tasker.event);
    TaskerSetEvent(&tasker, 7);
    TEST_ASSERT_EQUAL(0x85, tasker.event);    
    TaskerSetEvent(&tasker, 5);
    TEST_ASSERT_EQUAL(0xA5, tasker.event);      
    TEST_ASSERT_EQUAL(0, TaskerPrioritized(&tasker));
    TEST_ASSERT_EQUAL(0xA4, tasker.event);     
    TEST_ASSERT_EQUAL(2, TaskerPrioritized(&tasker));
    TEST_ASSERT_EQUAL(0xA0, tasker.event);       
    TEST_ASSERT_EQUAL(5, TaskerPrioritized(&tasker));
    TEST_ASSERT_EQUAL(0x80, tasker.event);       
    TEST_ASSERT_EQUAL(7, TaskerPrioritized(&tasker));   
    TEST_ASSERT_EQUAL(0x0, tasker.event);   
    TEST_ASSERT_EQUAL(1, wasCalled[0]);
    TEST_ASSERT_EQUAL(1, wasCalled[1]);
    TEST_ASSERT_EQUAL(1, wasCalled[2]);
    TEST_ASSERT_EQUAL(1, wasCalled[3]);
    TEST_ASSERT_EQUAL(0, wasCalled[4]);
    TEST_ASSERT_EQUAL(1,eventWasUsed[0]);
    TEST_ASSERT_EQUAL(0,eventWasUsed[1]);
    TEST_ASSERT_EQUAL(1,eventWasUsed[2]);
    TEST_ASSERT_EQUAL(0,eventWasUsed[3]);
    TEST_ASSERT_EQUAL(0,eventWasUsed[4]);
    TEST_ASSERT_EQUAL(1,eventWasUsed[5]);
    TEST_ASSERT_EQUAL(0,eventWasUsed[6]);
    TEST_ASSERT_EQUAL(1,eventWasUsed[7]);
    TEST_ASSERT_EQUAL(0,eventWasUsed[8]);      
}

void test_TaskerOnePass(void)
{
    TaskerSetEvent(&tasker, 0);
    TEST_ASSERT_EQUAL(0x1, tasker.event);
    TaskerSetEvent(&tasker, 2);
    TEST_ASSERT_EQUAL(0x5, tasker.event);
    TaskerSetEvent(&tasker, 7);
    TEST_ASSERT_EQUAL(0x85, tasker.event);    
    TaskerSetEvent(&tasker, 5);
    TEST_ASSERT_EQUAL(0xA5, tasker.event);      
    TEST_ASSERT_EQUAL(4, TaskerOnePass(&tasker));
    TEST_ASSERT_EQUAL(0x0, tasker.event);   
    TEST_ASSERT_EQUAL(1, wasCalled[0]);
    TEST_ASSERT_EQUAL(1, wasCalled[1]);
    TEST_ASSERT_EQUAL(1, wasCalled[2]);
    TEST_ASSERT_EQUAL(1, wasCalled[3]);
    TEST_ASSERT_EQUAL(0, wasCalled[4]);
    TEST_ASSERT_EQUAL(1,eventWasUsed[0]);
    TEST_ASSERT_EQUAL(0,eventWasUsed[1]);
    TEST_ASSERT_EQUAL(1,eventWasUsed[2]);
    TEST_ASSERT_EQUAL(0,eventWasUsed[3]);
    TEST_ASSERT_EQUAL(0,eventWasUsed[4]);
    TEST_ASSERT_EQUAL(1,eventWasUsed[5]);
    TEST_ASSERT_EQUAL(0,eventWasUsed[6]);
    TEST_ASSERT_EQUAL(1,eventWasUsed[7]);
    TEST_ASSERT_EQUAL(0,eventWasUsed[8]);      
}

void test_TaskerTestRecusive(void)
{
    TEST_IGNORE_MESSAGE("Need to test cases where tasks set events");
}
#include "unity.h"
#include "taskertick.h"
#include "swtimer.h"
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


taskerTick_t tickTasker =
{
    &tasker,
    {0,10},  // every 10 ticks
    0x9      // two events 0 and 3
};


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

void test_taskertick(void)
{
    uint32_t time = 0;
    int cnt = 0;
    
    for (int i=0; i<25; i++)
    {
        
        TT_ProcessTick(&tickTasker, time++ );
        int events = TaskerOnePass(tickTasker.tasker);
        if( events != -1)
        {
            TEST_ASSERT_EQUAL(2, events);
            cnt++;
        }
    }
    TEST_ASSERT_EQUAL(2,cnt);
}

#include "unity.h"
#include "swbgtimers.h"
#include "swtimers.h"
#include "swtimer.h"
#include "mock_taskService.h"

__IO uint32_t uwTick;

extern swtBg_t *timerList;

int32_t called1;
intptr_t context1;
void callback1(intptr_t context)
{
    called1++;
    context1 = context;
}

int32_t called2;
intptr_t context2;

void callback2(intptr_t context)
{
    called2++;
    context2 = context;
}


void setUp(void)
{
}

void tearDown(void)
{
}

void test_swbgtimers_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement swbgtimers");
}


void test_swbgtimers_can_do_callbacks(void)
{
    swtBg_t t1;
    swtBg_t t2;
    
    SWT_BackgroundTimerCallback( &t1, callback1, 5, 3, 100);
    TEST_ASSERT_EQUAL(&t1, timerList);
    SWT_BackgroundTimerCallback( &t2, callback2, 7, 1, 200);
    TEST_ASSERT_EQUAL(&t2, timerList);
    TEST_ASSERT_EQUAL(&t1,t2.next);
    while(timerList)
    {
        SWT_Background();    
    }
    TEST_ASSERT_EQUAL(3, called1);
    TEST_ASSERT_EQUAL(1, called2);
    TEST_ASSERT_EQUAL(100, context1);
    TEST_ASSERT_EQUAL(200, context2);   

   
    
    
}
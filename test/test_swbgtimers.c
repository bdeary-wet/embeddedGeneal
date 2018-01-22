#include "unity.h"
#include "swbgtimers.h"
#include "swtimers.h"
#include "swtimer.h"
#include "mock_taskService.h"

__IO uint32_t uwTick;
__IO uint32_t *someCounter = &uwTick;

taskHandle_t task1 = 1000, task2 = 2000;

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
    called1=0;
    called2=0;
    context1 =0;
    context2 =0;
    uwTick = 0; // reset timer
    SWT_BackgroundTimersReset();
}

void tearDown(void)
{
}




void test_swbgtimers_can_do_callbacks(void)
{
    swtBg_t t1;
    swtBg_t t2;
    TEST_ASSERT_EQUAL(0, called1);
    TEST_ASSERT_EQUAL(0, called2);
    TEST_ASSERT_EQUAL(0, context1);
    TEST_ASSERT_EQUAL(0, context2);    
    
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

void test_swbgtimers_can_do_tasks(void)
{
    swtBg_t t1;
    swtBg_t t2;
    // should see 4 task calls in this order
    TS_SignalTask_ExpectAndReturn(task1,0); // 5000
    TS_SignalTask_ExpectAndReturn(task2,0); // 7000
    TS_SignalTask_ExpectAndReturn(task1,0); // 10000
    TS_SignalTask_ExpectAndReturn(task1,0); // 15000   
    // call task 3 times 5000 apart
    SWT_BackgroundTimerTask( &t1, task1, 5, 3);
    TEST_ASSERT_EQUAL(&t1, timerList);
    // call task once at 7000
    SWT_BackgroundTimerTask( &t2, task2, 7, 1);
    TEST_ASSERT_EQUAL(&t2, timerList);
    TEST_ASSERT_EQUAL(&t1,t2.next);
    
    while(timerList)
    {
        SWT_Background();    
    }

}

void test_swbgtimers_a_mix_of_countdown_and_continious(void)
{
    swtBg_t t1,t2,t3,t4;
    
    TEST_ASSERT_EQUAL(0, called1);
    TEST_ASSERT_EQUAL(0, called2);
    TEST_ASSERT_EQUAL(0, context1);
    TEST_ASSERT_EQUAL(0, context2);      

    // should see 7 task calls in this order
    TS_SignalTask_ExpectAndReturn(task1,0); 
    TS_SignalTask_ExpectAndReturn(task2,0); 
    TS_SignalTask_ExpectAndReturn(task1,0); 
    TS_SignalTask_ExpectAndReturn(task1,0);  
    TS_SignalTask_ExpectAndReturn(task1,0);  
    TS_SignalTask_ExpectAndReturn(task1,0);  
    TS_SignalTask_ExpectAndReturn(task1,0);   

    
    SWT_BackgroundTimerCallback( &t1, callback1, 5, 3, 100);
    TEST_ASSERT_EQUAL(&t1, timerList);

    // call task continuous, 5000 apart
    SWT_BackgroundTimerTask( &t2, task1, 5, 0);
    TEST_ASSERT_EQUAL(&t2, timerList);
    // call task once at 7000
    SWT_BackgroundTimerTask( &t3, task2, 7, 1);
    TEST_ASSERT_EQUAL(&t3, timerList);
    
    SWT_BackgroundTimerCallback( &t4, callback2, 6, 1, 200);
    TEST_ASSERT_EQUAL(&t4, timerList);    
    
    TEST_ASSERT_EQUAL(&t2,t3.next);    
    
    
    while(timerList && uwTick < 35000)
    {
        SWT_Background();    
    }    
    
    TEST_ASSERT_EQUAL(&t2, timerList);
    
    TEST_ASSERT_EQUAL(3, called1);
    TEST_ASSERT_EQUAL(1, called2);
    TEST_ASSERT_EQUAL(100, context1);
    TEST_ASSERT_EQUAL(200, context2);     
    timerList = NULL;
}

void test_swbgtimers_can_be_reset_and_return_status(void)
{
    swtBg_t t1,t2,t3,t4;
    
    TEST_ASSERT_EQUAL(0, called1);
    TEST_ASSERT_EQUAL(0, called2);
    TEST_ASSERT_EQUAL(0, context1);
    TEST_ASSERT_EQUAL(0, context2);      

    // should see 7 task calls in this order
    TS_SignalTask_ExpectAndReturn(task1,0); 
    TS_SignalTask_ExpectAndReturn(task2,0); 
    TS_SignalTask_ExpectAndReturn(task1,0); 
    TS_SignalTask_ExpectAndReturn(task1,0);  
    TS_SignalTask_ExpectAndReturn(task1,0);  
    TS_SignalTask_ExpectAndReturn(task1,0);  
    TS_SignalTask_ExpectAndReturn(task1,0);   

    
    SWT_BackgroundTimerCallback( &t1, callback1, 5, 3, 100);
    TEST_ASSERT_EQUAL(&t1, timerList);

    // call task continious, 5000 apart
    SWT_BackgroundTimerTask( &t2, task1, 5, 0);
    TEST_ASSERT_EQUAL(&t2, timerList);
    // call task once at 7000
    SWT_BackgroundTimerTask( &t3, task2, 7, 1);
    TEST_ASSERT_EQUAL(&t3, timerList);
    
    SWT_BackgroundTimerCallback( &t4, callback2, 6, 1, 200);
    TEST_ASSERT_EQUAL(&t4, timerList);    
    
    TEST_ASSERT_EQUAL(&t2,t3.next);    
    
    
    while(timerList && uwTick < 35000)
    {
        SWT_Background();    
        if (uwTick > 20000)
        {
            TEST_ASSERT_TRUE(SWT_IsTimerActive(&t2));
            TEST_ASSERT_FALSE(SWT_IsTimerActive(&t3));
            TEST_ASSERT_TRUE(SWT_IsTimerRunning(&t2));
            TEST_ASSERT_FALSE(SWT_IsTimerRunning(&t1));
        }
    }    
    
    TEST_ASSERT_EQUAL(&t2, timerList);
    
    TEST_ASSERT_EQUAL(3, called1);
    TEST_ASSERT_EQUAL(1, called2);
    TEST_ASSERT_EQUAL(100, context1);
    TEST_ASSERT_EQUAL(200, context2);         
    
    
}

void test_swbgtimers_NeedToImplement_pause_and_stop_tests(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement pause_and_stop_tests");
}



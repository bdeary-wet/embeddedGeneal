#include "unity.h"
#include "timercb.h"
#include "swtimer.h"

void setUp(void)
{
}

void tearDown(void)
{
}

uint32_t timer32(void)
{
    static uint32_t time;
    return time++;
}

uint16_t timer16(void)
{
    static uint16_t time;
    return time++;
}

int callbackCalled1;
int callbackCalled2;

void theCallback1(uintptr_t context)
{
    timerCb32_t *timercb = (timerCb32_t*)context;
    TEST_ASSERT_EQUAL(context, timercb->context);
    callbackCalled1++;
}

void theCallback2(uintptr_t context)
{
    timerCb16_t *timercb = (timerCb16_t*)context;
    TEST_ASSERT_EQUAL(context, timercb->context);
    callbackCalled2++;
}


void test_timercb32(void)
{
    timerCb32_t timercb;
    TC_Timer32Init(&timercb, timer32);
    TEST_ASSERT_EQUAL(timer32, timercb.timer.timer);
    TEST_ASSERT_EQUAL(0,timercb.timer.timeObj.start);
    TEST_ASSERT_EQUAL(0,timercb.timer.timeObj.duration);
    TC_Timer32Set(&timercb, theCallback1, (uintptr_t)&timercb);
    TC_Timer32Start(&timercb, 3);
    TEST_ASSERT_FALSE(TC_Timer32Test(&timercb));
    TEST_ASSERT_FALSE(TC_Timer32Test(&timercb));
    TEST_ASSERT_EQUAL(0, callbackCalled1);    
    TEST_ASSERT_TRUE(TC_Timer32Test(&timercb));
    TEST_ASSERT_EQUAL(1, callbackCalled1);
    // test cb
    TC_Timer32Start(&timercb, 4);
    TEST_ASSERT_FALSE(TC_Timer32Test(&timercb));
    TEST_ASSERT_FALSE(TC_Timer32Test(&timercb));
    TEST_ASSERT_FALSE(TC_Timer32Test(&timercb));
    TEST_ASSERT_EQUAL(1, callbackCalled1);    
    TEST_ASSERT_TRUE(TC_Timer32Test(&timercb));
    TEST_ASSERT_EQUAL(2, callbackCalled1);    
    // test stop
    TC_Timer32Start(&timercb, 3);
    TEST_ASSERT_FALSE(TC_Timer32Test(&timercb));
    TEST_ASSERT_FALSE(TC_Timer32Test(&timercb));
    TC_Timer32Stop(&timercb);
    TEST_ASSERT_EQUAL(2, callbackCalled1);    
    TEST_ASSERT_FALSE(TC_Timer32Test(&timercb));
    TEST_ASSERT_FALSE(TC_Timer32Test(&timercb));
    TEST_ASSERT_FALSE(TC_Timer32Test(&timercb));    
    TEST_ASSERT_EQUAL(2, callbackCalled1);        
    
}



void test_timercb16(void)
{
    timerCb16_t timercb;
    TC_Timer16Init(&timercb, timer16);
    TEST_ASSERT_EQUAL(timer16, timercb.timer.timer);
    TEST_ASSERT_EQUAL(0,timercb.timer.timeObj.start);
    TEST_ASSERT_EQUAL(0,timercb.timer.timeObj.duration);
    TC_Timer16Set(&timercb, theCallback2, (uintptr_t)&timercb);
    TC_Timer16Start(&timercb, 3);
    TEST_ASSERT_FALSE(TC_Timer16Test(&timercb));
    TEST_ASSERT_FALSE(TC_Timer16Test(&timercb));
    TEST_ASSERT_EQUAL(0, callbackCalled2);    
    TEST_ASSERT_TRUE(TC_Timer16Test(&timercb));
    TEST_ASSERT_EQUAL(1, callbackCalled2);
    // test cb
    TC_Timer16Start(&timercb, 4);
    TEST_ASSERT_FALSE(TC_Timer16Test(&timercb));
    TEST_ASSERT_FALSE(TC_Timer16Test(&timercb));
    TEST_ASSERT_FALSE(TC_Timer16Test(&timercb));
    TEST_ASSERT_EQUAL(1, callbackCalled2);    
    TEST_ASSERT_TRUE(TC_Timer16Test(&timercb));
    TEST_ASSERT_EQUAL(2, callbackCalled2);    
    // test stop
    TC_Timer16Start(&timercb, 3);
    TEST_ASSERT_FALSE(TC_Timer16Test(&timercb));
    TEST_ASSERT_FALSE(TC_Timer16Test(&timercb));
    TC_Timer16Stop(&timercb);
    TEST_ASSERT_EQUAL(2, callbackCalled2);    
    TEST_ASSERT_FALSE(TC_Timer16Test(&timercb));
    TEST_ASSERT_FALSE(TC_Timer16Test(&timercb));
    TEST_ASSERT_FALSE(TC_Timer16Test(&timercb));    
    TEST_ASSERT_EQUAL(2, callbackCalled2);        
    
}
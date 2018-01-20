#include "unity.h"
#include "swbgtimers.h"
#include "swtimers.h"
#include "swtimer.h"

__IO uint32_t uwTick;

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
    SWT_BackgroundTimerCallback( &t2, callback2, 7, 0, 200);
    
    
   
    
    
}
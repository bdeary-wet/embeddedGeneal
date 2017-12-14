#include "unity.h"
#include "swtimer.h"

void setUp(void)
{
}

void tearDown(void)
{
}

uint16_t time16()
{
    static uint16_t time;
    return time++;
}

uint32_t time32()
{
    static uint32_t time;
    return time++;
}


void test_swtimer16(void)
{
    uint16_t temp;
    timerObj16_t timer;
    SW_Timer16Init(&timer, time16);
    TEST_ASSERT_EQUAL(time16, timer.timer);
    TEST_ASSERT_EQUAL(0,timer.timeObj.start);
    TEST_ASSERT_EQUAL(0,timer.timeObj.duration);
    SW_SetTimer16(&timer, 4);
    TEST_ASSERT_EQUAL(1,time16()); // time16() now 2
    TEST_ASSERT_FALSE(SW_Timer16Passed(&timer)); // 3
    TEST_ASSERT_FALSE(SW_Timer16Passed(&timer)); // 4
    TEST_ASSERT_TRUE(SW_Timer16Passed(&timer));  // 5, returns 4  
    SW_SetTimer16(&timer, 5);
    TEST_ASSERT_FALSE(SW_Timer16Passed(&timer)); 
    TEST_ASSERT_FALSE(SW_Timer16Passed(&timer)); 
    TEST_ASSERT_FALSE(SW_Timer16Passed(&timer)); 
    TEST_ASSERT_FALSE(SW_Timer16Passed(&timer));   
    TEST_ASSERT_TRUE(SW_Timer16Passed(&timer));
    
    SW_SetTimer16(&timer, 5);
    TEST_ASSERT_FALSE(SW_Timer16Passed(&timer)); 
    TEST_ASSERT_FALSE(SW_Timer16Passed(&timer)); 
    TEST_ASSERT_FALSE(SW_Timer16Passed(&timer)); 
    SW_ResetTimer16(&timer);
    temp = time16();
    TEST_ASSERT_EQUAL(0,timer.timeObj.duration);    
    TEST_ASSERT_EQUAL(4,timer.timeObj.start); 
    TEST_ASSERT_FALSE(SW_Timer16Passed(&timer));  // timer is off   
    TEST_ASSERT_FALSE(SW_Timer16Passed(&timer));  // timer is off
    TEST_ASSERT_FALSE(SW_Timer16Passed(&timer));  // timer is off    
    // confirm running passed with timer disabled does not call the timer
    TEST_ASSERT_EQUAL(temp+1, time16());

}


void test_swtimer32(void)
{
    uint32_t temp;
    timerObj32_t timer;
    SW_Timer32Init(&timer, time32);
    TEST_ASSERT_EQUAL(time32, timer.timer);
    TEST_ASSERT_EQUAL(0,timer.timeObj.start);
    TEST_ASSERT_EQUAL(0,timer.timeObj.duration);
    SW_SetTimer32(&timer, 4);
    TEST_ASSERT_EQUAL(1,time32()); // time16() now 2
    TEST_ASSERT_FALSE(SW_Timer32Passed(&timer)); // 3
    TEST_ASSERT_FALSE(SW_Timer32Passed(&timer)); // 4
    TEST_ASSERT_TRUE(SW_Timer32Passed(&timer));  // 5, returns 4  
    SW_SetTimer32(&timer, 3);
    TEST_ASSERT_FALSE(SW_Timer32Passed(&timer)); // 7
    TEST_ASSERT_FALSE(SW_Timer32Passed(&timer)); // 8
    TEST_ASSERT_TRUE(SW_Timer32Passed(&timer));  // 9, returns 8

    SW_SetTimer32(&timer, 5);
    TEST_ASSERT_FALSE(SW_Timer32Passed(&timer)); 
    TEST_ASSERT_FALSE(SW_Timer32Passed(&timer)); 
    TEST_ASSERT_FALSE(SW_Timer32Passed(&timer)); 
    SW_ResetTimer32(&timer);
    temp = time32(); // get timer, advances it 
    TEST_ASSERT_EQUAL(0,timer.timeObj.duration);    
    TEST_ASSERT_EQUAL(4,timer.timeObj.start); 
    TEST_ASSERT_FALSE(SW_Timer32Passed(&timer));  // timer is off   
    TEST_ASSERT_FALSE(SW_Timer32Passed(&timer));  // timer is off
    TEST_ASSERT_FALSE(SW_Timer32Passed(&timer));  // timer is off    
    // confirm running passed with timer disabled does not call the timer
    TEST_ASSERT_EQUAL(temp+1, time32());
    
    
    
}
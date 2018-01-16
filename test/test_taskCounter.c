#include "unity.h"
#include "taskCounter.h"
#define __Testable extern

uintptr_t space[4];
tcHandle_t task1;

__Testable TC_Counter_t volatile **counterPointer;
__Testable size_t counters;
__Testable size_t allocated;

static TC_Counter_t fixedCounter;

void setUp(void)
{
    TC_InitHandleService(space, sizeof(space));
    TEST_ASSERT_EQUAL(1, task1 = TC_AssociateHandle(&fixedCounter));
}

void tearDown(void)
{
}

void test_tc_init(void)
{
    TC_Counter_t autoCounter;
    TC_Counter_t *autoCounterAdd = &autoCounter;
    TEST_ASSERT_EQUAL(2, TC_AssociateHandle(&autoCounter));
    TEST_ASSERT_EQUAL(3, TC_AssociateHandle(autoCounterAdd));
    TEST_ASSERT_EQUAL(4, TC_AssociateHandle(autoCounterAdd));
    // out of space, should return 0
    TEST_ASSERT_EQUAL(0, TC_AssociateHandle(autoCounterAdd));
}

uint32_t TC_misuse(void);

void test_tc_counter(void)
{
    TEST_ASSERT_EQUAL(0, TC_Test(task1));
    TC_SignalTask(task1);
    TEST_ASSERT_EQUAL(1, TC_Test(task1));
    TC_SignalTask(task1);
    TEST_ASSERT_EQUAL(2, TC_Test(task1));
    TC_SignalTask(task1);
    TEST_ASSERT_EQUAL(3, TC_TestAndClear(task1));
    TEST_ASSERT_EQUAL(0, TC_TestAndClear(task1));
    TEST_ASSERT_EQUAL(0, TC_misuse());
    // test legal but not assigned
    TEST_ASSERT_EQUAL(0, TC_Test(3));
    // test illegal value
    TEST_ASSERT_EQUAL(0, TC_Test(7));
    // should have logged 2 errors
    TEST_ASSERT_EQUAL(2, TC_misuse());
}


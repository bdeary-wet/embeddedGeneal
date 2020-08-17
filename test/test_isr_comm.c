/**
 * @file test_isr_comm.c
 * @author bdeary (bdeary@wetdesign.com)
 * @brief Test the comm functions for sending data between isr and user layers
 * @date 2020-08-16
 * 
 * Copyright 2020, WetDesigns
 * 
 */
#include "unity.h"

#include "isr_comm.h"
#include <genQ.h>
#include <genPool.h>
#include <mock_pool_support.h>

void setUp(void)
{
}

void tearDown(void)
{
}

void test_isr_comm_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement isr_comm");
}

typedef struct 
{   
    char *str;
    uint8_t flag;
    int32_t i1;
} myMes;


DefineSingleIsrObjectQueue(Isr1Pipe, myMes, 5, poolTestCallback );


void test_GenerationSingle(void)
{
    int avail, total;
    // peer into the pool object
    TEST_ASSERT_EQUAL(Status_OK, GenPool_status(PIsr1Pipe, &avail, &total) );
    TEST_ASSERT_EQUAL(5,avail);
    TEST_ASSERT_EQUAL(5,total);
    myMes *mes = Allocate_Isr1Pipe_Object();        // get object from pool
    mes->str = "hello world";
    mes->flag = 1;
    mes->i1 = 42;
    TEST_ASSERT_NOT_NULL(mes);
    TEST_ASSERT_EQUAL(Status_OK, GenPool_status(PIsr1Pipe, &avail, &total) );
    TEST_ASSERT_EQUAL(4,avail);
    TEST_ASSERT_EQUAL(5,total);    
    myMes *mesIsr;
    TEST_ASSERT_EQUAL(Status_OK, Send_Isr1Pipe_Object(mes));  // send object to isr
    TEST_ASSERT_EQUAL(Status_OK, Dequeue_Isr1Pipe_Object(&mesIsr));  // dequeue and object in isr    
    TEST_ASSERT_EQUAL_PTR(mesIsr, mes);
    TEST_ASSERT_EQUAL(42, mesIsr->i1);

    Free_Isr1Pipe_Object(mesIsr);  // used by isr to free pool object
    TEST_ASSERT_EQUAL(Status_OK, GenPool_status(PIsr1Pipe, &avail, &total) );
    TEST_ASSERT_EQUAL(4,avail);
    TEST_ASSERT_EQUAL(5,total);
    // calling ProcessReturnPoolObjects will free all items in queue and call their 
    // callback functions which we pointed at a mocked function.
    poolTestCallback_Expect((Context_t){.v_context=(intptr_t)mes});
    ProcessReturnPoolObjects();
    TEST_ASSERT_EQUAL(Status_OK, GenPool_status(PIsr1Pipe, &avail, &total) );
    TEST_ASSERT_EQUAL(5,avail);
    TEST_ASSERT_EQUAL(5,total);

}

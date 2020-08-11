#include "unity.h"
#include "genPool.h"

#include <stdbool.h>

void setUp(void)
{
}

void tearDown(void)
{
}

void test_genPool_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement genPool");
}

typedef struct 
{
    char *str;
    int i1;
    bool flag;
} myBlk_t;



GenPoolDefine(myPool, myBlk_t, 10, NULL);

void test_defined_object(void)
{
    TEST_ASSERT_EQUAL_PTR(myPool, &myPool_pool);
    TEST_ASSERT_EQUAL_PTR((void*)myPool->base, (void*)myPool_pool.pool);
    TEST_ASSERT_EQUAL_PTR((void*)&(myPool->next->obj[0]), (void*)&(myPool_pool.pool[0].poolObject));    
}



#include "unity.h"
#include "genPool.h"

#include <stdbool.h>

typedef struct 
{
    char *str;
    int i1;
    bool flag;
} myBlk_t;


#define POOL_LEN 10

void myGenCallback(Context_t context)
{
    PreBufMeta_t meta = GenPool_object_meta((void*)context.v_context);
    TEST_ASSERT_EQUAL(sizeof(myBlk_t), meta.objectSize);
    TEST_ASSERT_TRUE(meta.index < POOL_LEN);
    TEST_ASSERT_EQUAL_PTR(myGenCallback, meta.onRelease.cb);
    TEST_ASSERT_EQUAL_MEMORY(&context, &(meta.onRelease.context), sizeof context);
}


GenPoolDefine(myPool, myBlk_t, POOL_LEN, myGenCallback);

void setUp(void)
{
    GenPool_reset(myPool);
}

void tearDown(void)
{
}

void test_genPool_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement genPool");
}

void test_defined_object(void)
{
    TEST_ASSERT_EQUAL_PTR(myPool, &myPool_pool);
    TEST_ASSERT_EQUAL_PTR((void*)myPool->base, (void*)myPool_pool.pool);
    TEST_ASSERT_EQUAL_PTR((void*)&(myPool->next->obj[0]), (void*)&(myPool_pool.pool[0].poolObject));    
}

// use some normally static internals for tests
PoolObjId_t getPoolId(void *p_obj);

void test_GenPool_allocate(void)
{
    void *obj = GenPool_allocate(myPool);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj).pool, myPool);
    TEST_ASSERT_EQUAL_PTR(getPoolId(obj).index, 0);
    GenPool_return(obj);
}

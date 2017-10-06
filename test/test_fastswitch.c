#include "unity.h"
#include "fastswitch.h"

void setUp(void)
{
}

void tearDown(void)
{
}



typedef struct
{
    uint32_t var1;
    uint8_t b1;
    uint16_t short1;
    
    
}myType_t;

MAKE_FAST_SWITCH(myVar, myType_t, 2);

void test_fastswitch(void)
{

    myType_t localCpy = {32, 45, 67};
    TEST_ASSERT_EQUAL(myVar, &myVar_buf[1]);
    
    fsCopy(&myVar_fs, &localCpy);
    TEST_ASSERT_EQUAL_MEMORY (&localCpy, myVar_buf, sizeof(myType_t));
    TEST_ASSERT_EQUAL(myVar, &myVar_buf[0]);
    localCpy.b1++;
    fsCopy(&myVar_fs, &localCpy);
    TEST_ASSERT_EQUAL_MEMORY (&localCpy, &myVar_buf[1], sizeof(myType_t));
    TEST_ASSERT_EQUAL(localCpy.b1, myVar->b1);
    
}
#include "unity.h"
#include "optable.h"

void setUp(void)
{
}

void tearDown(void)
{
}

typedef struct
{
    uint8_t opCode;
    uint8_t *payload;
    uint8_t payloadLen;
} myMessage_t;

typedef void (*myProcessorType_f)(myMessage_t *mes);

void static tp(opTableEntry_t const *entry, uintptr_t context)
{
    myProcessorType_f func = entry->value;
    if(func) func((myMessage_t*)context);
}

uint8_t lastKey;
void proc5(myMessage_t *mes) {lastKey = 5;}
void proc12(myMessage_t *mes) {lastKey = 12;}
void proc15(myMessage_t *mes) {lastKey = 15;}
void proc99(myMessage_t *mes) {lastKey = 99;}
void proc145(myMessage_t *mes) {lastKey = 145;}

const opTableEntry_t theTable[] =
{
    {5,proc5},
    {12,proc12},
    {15,proc15},
    {99,proc99},
    {145,proc145}
};


const opTable_t tableObject = 
{
    theTable,
    sizeof(theTable)/sizeof(theTable[0]),
    tp
};

void test_const_optable(void)
{
    myMessage_t context;
    TEST_ASSERT_EQUAL(0, lastKey);
    TEST_ASSERT_NULL(OPT_FindEntry(&tableObject, 23));
    TEST_ASSERT_NOT_NULL(OPT_FindEntry(&tableObject, 99));
    TEST_ASSERT_NULL(OPT_FindEntry(&tableObject, 0));
    TEST_ASSERT_NOT_NULL(OPT_FindEntry(&tableObject, 145));
    TEST_ASSERT_NULL(OPT_FindEntry(&tableObject, 0xffff));
    TEST_ASSERT_NOT_NULL(OPT_FindEntry(&tableObject, 12));
    TEST_ASSERT_NULL(OPT_FindEntry(&tableObject, 0x7fff));
    TEST_ASSERT_NOT_NULL(OPT_FindEntry(&tableObject, 15));
    TEST_ASSERT_NULL(OPT_FindEntry(&tableObject, 67));
    TEST_ASSERT_NOT_NULL(OPT_FindEntry(&tableObject, 5));
    TEST_ASSERT_NULL(OPT_FindEntry(&tableObject, 23));
    TEST_ASSERT_NOT_NULL(OPT_FindEntry(&tableObject, 99));
    
    TEST_ASSERT_NULL(OPT_ProcessViaTable(&tableObject, 23,(uintptr_t)&context));
    TEST_ASSERT_NOT_NULL(OPT_ProcessViaTable(&tableObject, 99,(uintptr_t)&context));
    TEST_ASSERT_EQUAL(99, lastKey);
    TEST_ASSERT_NULL(OPT_ProcessViaTable(&tableObject, 0,(uintptr_t)&context));
    TEST_ASSERT_EQUAL_PTR(&theTable[4],OPT_ProcessViaTable(&tableObject, 145,(uintptr_t)&context));
    TEST_ASSERT_EQUAL(145, lastKey);    
    TEST_ASSERT_NULL(OPT_ProcessViaTable(&tableObject, 0xffff,(uintptr_t)&context));
    TEST_ASSERT_NOT_NULL(OPT_ProcessViaTable(&tableObject, 12,(uintptr_t)&context));
    TEST_ASSERT_EQUAL(12, lastKey);    
    TEST_ASSERT_NULL(OPT_ProcessViaTable(&tableObject, 0x7fff,(uintptr_t)&context));
    TEST_ASSERT_EQUAL_PTR(&theTable[2],OPT_ProcessViaTable(&tableObject, 15,(uintptr_t)&context));
    TEST_ASSERT_EQUAL(15, lastKey);    
    TEST_ASSERT_NULL(OPT_ProcessViaTable(&tableObject, 67,(uintptr_t)&context));
    TEST_ASSERT_NOT_NULL(OPT_ProcessViaTable(&tableObject, 5,(uintptr_t)&context));
    TEST_ASSERT_EQUAL(5, lastKey);    
    TEST_ASSERT_NULL(OPT_ProcessViaTable(&tableObject, 23,(uintptr_t)&context));
    TEST_ASSERT_NOT_NULL(OPT_ProcessViaTable(&tableObject, 99,(uintptr_t)&context));    
    TEST_ASSERT_EQUAL(99, lastKey);    
}

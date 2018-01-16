#include "unity.h"
#include "generalQueue.h"
#include <string.h>

// copied from generalQueue.c for testing internals
typedef struct genQ_s
{
    uint8_t *base;      // buffer start
    uint8_t *next;      // next entry point
    uint8_t *last;      // next source point
    uint8_t *end;       // address of last object in buffer
    uint_fast16_t objectSize; // size of the object
} genQ_t;

void setUp(void)
{
}

void tearDown(void)
{
}


#define OBJECTS 7

void test_generalQueue_Init(void)
{
    genQ_t queue;
    struct myObj{uint8_t v8; uint16_t v16;} bufSpace[OBJECTS];
    
    TEST_ASSERT_NOT_EQUAL(0, GenQ_Init(&queue, bufSpace, sizeof (struct myObj), 0));
    TEST_ASSERT_NOT_EQUAL(0, GenQ_Init(&queue, bufSpace, sizeof (struct myObj), 1));  
    TEST_ASSERT_EQUAL(0, GenQ_Init(&queue, bufSpace, sizeof (struct myObj), 2));    
    TEST_ASSERT_EQUAL(0, GenQ_Init(&queue, bufSpace, sizeof (struct myObj), OBJECTS));
    TEST_ASSERT_EQUAL(sizeof(struct myObj), queue.objectSize);
    TEST_ASSERT_EQUAL(bufSpace, queue.base);
    TEST_ASSERT_EQUAL(bufSpace, queue.next);
    TEST_ASSERT_EQUAL(bufSpace, queue.last);
    TEST_ASSERT_EQUAL(&bufSpace[OBJECTS-1], queue.end);
    
    TEST_ASSERT_EQUAL(sizeof(struct myObj), GenQ_ObjectSize(&queue));
    
    
}


void test_generalQueue_Put(void)
{
    genQ_t queue;
    struct myObj{uint8_t v8; uint16_t v16;} bufSpace[OBJECTS], anObj, anotherObj;
    anObj.v8 = 123;
    anObj.v16 = 4567;
    memset(bufSpace, 0, sizeof(bufSpace));
    TEST_ASSERT_EQUAL(0, GenQ_Init(&queue, bufSpace, sizeof (struct myObj), OBJECTS));
    TEST_ASSERT_EQUAL(0, GenQ_Put(&queue, &anObj));
    TEST_ASSERT_EQUAL(123, bufSpace[0].v8);
    TEST_ASSERT_EQUAL(4567, bufSpace[0].v16);    
    TEST_ASSERT_EQUAL(0, GenQ_Put(&queue, &anObj));
    TEST_ASSERT_EQUAL(0, GenQ_Put(&queue, &anObj));
    TEST_ASSERT_EQUAL(0, GenQ_Put(&queue, &anObj));
    TEST_ASSERT_EQUAL(0, GenQ_Put(&queue, &anObj));
    TEST_ASSERT_EQUAL(0, GenQ_Put(&queue, &anObj));
    // only holds OBJECTS -1 items
    TEST_ASSERT_NOT_EQUAL(0, GenQ_Put(&queue, &anObj));
    
    // verify data in queue elements
    for (int i=0; i<OBJECTS-1; i++)
    {
        TEST_ASSERT_EQUAL(123, bufSpace[i].v8);
        TEST_ASSERT_EQUAL(4567, bufSpace[i].v16);
    }
    TEST_ASSERT_NOT_EQUAL(123, bufSpace[OBJECTS-1].v8);
    TEST_ASSERT_NOT_EQUAL(4567, bufSpace[OBJECTS-1].v16);
    
    // test roll over pointer addressing
    // first last slot 
    TEST_ASSERT_EQUAL(&bufSpace[OBJECTS-1], queue.next);
    GenQ_Get(&queue, &anotherObj);  // free first slot
    TEST_ASSERT_EQUAL(0, GenQ_Put(&queue, &anObj)); // write to last slot
    TEST_ASSERT_EQUAL(&bufSpace[0], queue.next); // show pointer rolled over
    
    TEST_ASSERT_EQUAL(sizeof(struct myObj), GenQ_ObjectSize(&queue));
    
}

void test_generalQueue_Get(void)
{
    genQ_t queue;
    struct myObj{uint8_t v8; uint16_t v16;} bufSpace[OBJECTS], anObj, anotherObj;
    anObj.v8 = 123;
    anObj.v16 = 4567;
    memset(bufSpace, 0, sizeof(bufSpace));
    TEST_ASSERT_EQUAL(0, GenQ_Init(&queue, bufSpace, sizeof (struct myObj), OBJECTS));
    // test IsData
    TEST_ASSERT_EQUAL(0, GenQ_IsData(&queue));
    
    TEST_ASSERT_EQUAL(sizeof(struct myObj), GenQ_ObjectSize(&queue));
    
    while (0 == GenQ_Put(&queue, &anObj))
    {
        GenQ_Put(&queue, &anObj);
    }
    TEST_ASSERT_NOT_EQUAL(0, GenQ_IsData(&queue));
    
    // verify data in queue elements
    for (int i=0; i<OBJECTS-1; i++)
    {
        TEST_ASSERT_EQUAL(123, bufSpace[i].v8);
        TEST_ASSERT_EQUAL(4567, bufSpace[i].v16);
    }
    TEST_ASSERT_NOT_EQUAL(123, bufSpace[OBJECTS-1].v8);
    TEST_ASSERT_NOT_EQUAL(4567, bufSpace[OBJECTS-1].v16);
    TEST_ASSERT_EQUAL(&bufSpace[OBJECTS-1], queue.next);
    TEST_ASSERT_EQUAL(&bufSpace[0], queue.last);
    
    for (int i=0; i<OBJECTS-1; i++)
    {
        TEST_ASSERT_EQUAL(0, GenQ_Get(&queue, &anotherObj));
        TEST_ASSERT_EQUAL(123,anotherObj.v8);
        TEST_ASSERT_EQUAL(4567, anotherObj.v16);
    }
    // queue should be empty
    TEST_ASSERT_NOT_EQUAL(0, GenQ_Get(&queue, &anotherObj));
    // pointing at last entry in array
    TEST_ASSERT_EQUAL(&bufSpace[OBJECTS-1], queue.last);
    // put another entry then get it
    GenQ_Put(&queue, &anObj);
    TEST_ASSERT_EQUAL(0, GenQ_Get(&queue, &anotherObj));
    // last should roll over back to start of buffer 
    TEST_ASSERT_EQUAL(&bufSpace[0], queue.last);    
    // queue should be empty
        // test IsData
    TEST_ASSERT_EQUAL(0, GenQ_IsData(&queue));
    
    
}

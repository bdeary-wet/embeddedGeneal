#include "unity.h"
#include "generalQueue.h"
#include <string.h>
#include <malloc.h>

// copied from generalQueue.c for testing internals

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
    
    // Test the helper MACROs for allocating queues
    // Allocate a 5 space queue of 32bit ints on the stack
    QBUILDER_ALLOCA(alq_pnt, int32_t, 5);
    TEST_ASSERT_EQUAL(4, GenQ_ObjectSize(alq_pnt));

    // allocate a 7 space queue of 16bit ints on heap
    QBUILDER_MALLOC(mlq_pnt, int16_t, 7);
    TEST_ASSERT_EQUAL(2, GenQ_ObjectSize(mlq_pnt));
    free(mlq_pnt);
   
    // static allocation of byte queue length 10
    QBUILDER_STATIC(bq_pnt, uint8_t, 10);
    TEST_ASSERT_EQUAL(1, GenQ_ObjectSize(bq_pnt));
    
    // attach a queue object to an array of struct
    struct {int i1; char c2; short int s3;} sarray[7];
    genQ_t sq;
    ARRAY_TO_Q(sarray, sq);
    TEST_ASSERT_EQUAL(sizeof(sarray[0]), GenQ_ObjectSize(&sq));
    
    struct {int i1; char c2; uint64_t ll3;} sarray2[9];
    QBUILDER_ARRAY(s2Q, sarray2);
    TEST_ASSERT_EQUAL(sizeof(sarray2[0]), GenQ_ObjectSize(&s2Q));
    
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
    
    TEST_ASSERT_EQUAL(0, ARRAY_TO_Q(bufSpace, queue));
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

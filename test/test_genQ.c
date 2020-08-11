#include "unity.h"
#include "genQ.h"
#include <string.h>

void setUp(void)
{
}

void tearDown(void)
{
}


#define OBJECTS 7  // intentionally prime for pool testing

void test_GenQ_Init(void)
{
    GenQ_t queue;
    struct myObj{uint8_t v8; uint16_t v16;} bufSpace[OBJECTS];
    
    TEST_ASSERT_EQUAL(Queue_PARAMETER, GenQ_Init(&queue, bufSpace, sizeof (struct myObj), 0));
    TEST_ASSERT_EQUAL(Queue_PARAMETER, GenQ_Init(&queue, bufSpace, sizeof (struct myObj), 1));  
    TEST_ASSERT_EQUAL(Queue_OK, GenQ_Init(&queue, bufSpace, sizeof (struct myObj), 2));    
    TEST_ASSERT_EQUAL(Queue_OK, GenQ_Init(&queue, bufSpace, sizeof (struct myObj), OBJECTS));
    TEST_ASSERT_EQUAL(sizeof(struct myObj), queue.objectSize);
    TEST_ASSERT_EQUAL_PTR(bufSpace, queue.base_add);
    TEST_ASSERT_EQUAL_PTR(bufSpace, queue.next);
    TEST_ASSERT_EQUAL_PTR(bufSpace, queue.last);
    TEST_ASSERT_EQUAL_PTR(&bufSpace[OBJECTS-1], queue.end);
    
    TEST_ASSERT_EQUAL(sizeof(struct myObj), GenQ_ObjectSize(&queue));
    
    }


void test_GenQ_Put(void)
{
    GenQ_t queue;
    struct myObj{uint8_t v8; uint16_t v16;} ;
    
    struct myObj bufSpace[OBJECTS] = {0};
    struct myObj anObj = (struct myObj){.v8 = 123, .v16=4567};
    struct myObj anotherObj = {0};

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
    TEST_ASSERT_EQUAL(Queue_FULL, GenQ_Put(&queue, &anObj));
    
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
    TEST_ASSERT_EQUAL_PTR(&bufSpace[OBJECTS-1], queue.next);
    TEST_ASSERT_NOT_EQUAL(0, memcmp(&anObj, &anotherObj, sizeof anObj));
    TEST_ASSERT_EQUAL(Queue_OK, GenQ_Get(&queue, &anotherObj));  // free first slot
    TEST_ASSERT_EQUAL(0, memcmp(&anObj, &anotherObj, sizeof anObj));
    TEST_ASSERT_EQUAL(0, GenQ_Put(&queue, &anObj)); // write to last slot
    TEST_ASSERT_EQUAL(&bufSpace[0], queue.next); // show pointer rolled over
    
    TEST_ASSERT_EQUAL(sizeof(struct myObj), GenQ_ObjectSize(&queue));
}

void test_static_GenQ_def(void)
{
    StaticGenQDef(testQ, void*, 10);
    TEST_ASSERT_EQUAL_PTR(testQ_space, testQ.next);
    TEST_ASSERT_EQUAL_PTR(&testQ_space[9], testQ.end);

}

typedef struct test_genQ
{
    char *str;
    int i1;
    double d1;
} gStr_t;

gStr_t global_blk[23];
GenQDef(globalQ, global_blk, 23);

void test_global_GenQ_def(void)
{
    TEST_ASSERT_EQUAL_PTR(global_blk, globalQ.next);
    TEST_ASSERT_EQUAL_PTR(&global_blk[22], globalQ.end);
    GenQ_Put(&globalQ, &(gStr_t){.str="hello",.d1=4.3,.i1=7});
    TEST_ASSERT_TRUE(GenQ_HasData(&globalQ));
    gStr_t out;
    TEST_ASSERT_EQUAL(0,GenQ_Get(&globalQ, &out));
    printf("%s",out.str);
    TEST_ASSERT_EQUAL(Queue_EMPTY,GenQ_Get(&globalQ, &out));    
}



void test_GenQ_Get(void)
{
    struct myObj{uint8_t v8; uint16_t v16;} ;
    
    struct myObj anObj = (struct myObj){.v8 = 123, .v16=4567};
    struct myObj anotherObj = {0};
    
    StaticGenQDef(queue, struct myObj, OBJECTS);

    // test IsData
    TEST_ASSERT_FALSE(GenQ_HasData(&queue));
    
    TEST_ASSERT_EQUAL(sizeof(struct myObj), GenQ_ObjectSize(&queue));
    
    while (Queue_OK == GenQ_Put(&queue, &anObj))
    {
        GenQ_Put(&queue, &anObj);
    }
    TEST_ASSERT_TRUE(GenQ_HasData(&queue));
    TEST_ASSERT_FALSE(GenQ_HasSpace(&queue));
    
    // verify data in queue elements
    for (int i=0; i<OBJECTS-1; i++)
    {
        TEST_ASSERT_EQUAL(123, queue_space[i].v8);
        TEST_ASSERT_EQUAL(4567, queue_space[i].v16);
    }
    TEST_ASSERT_NOT_EQUAL(123, queue_space[OBJECTS-1].v8);
    TEST_ASSERT_NOT_EQUAL(4567, queue_space[OBJECTS-1].v16);
    TEST_ASSERT_EQUAL(&queue_space[OBJECTS-1], queue.next);
    TEST_ASSERT_EQUAL(&queue_space[0], queue.last);
    
    for (int i=0; i<OBJECTS-1; i++)
    {
        TEST_ASSERT_EQUAL(0, GenQ_Get(&queue, &anotherObj));
        TEST_ASSERT_EQUAL(123,anotherObj.v8);
        TEST_ASSERT_EQUAL(4567, anotherObj.v16);
    }
    // queue should be empty
    TEST_ASSERT_EQUAL(Queue_EMPTY, GenQ_Get(&queue, &anotherObj));
    // pointing at last entry in array
    TEST_ASSERT_EQUAL(&queue_space[OBJECTS-1], queue.last);
    // put another entry then get it
    GenQ_Put(&queue, &anObj);
    TEST_ASSERT_EQUAL(0, GenQ_Get(&queue, &anotherObj));
    // last should roll over back to start of buffer 
    TEST_ASSERT_EQUAL(&queue_space[0], queue.last);    
    // queue should be empty
        // test IsData
    TEST_ASSERT_EQUAL(0, GenQ_HasData(&queue));
}


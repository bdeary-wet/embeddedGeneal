#include "unity.h"
#include "generalQueue.h"
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

// copied from generalQueue.c for testing internals

void setUp(void)
{
}

void tearDown(void)
{
}


#define OBJECTS 7  // intentionally prime for pool testing

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
   
    typedef struct{int32_t i1; uint8_t b1;} mixed_t;   
    GENERAL_QUEUE(gq2, mixed_t, 8);
    TEST_ASSERT_EQUAL(sizeof(mixed_t),GenQ_ObjectSize(&gq2)); 

   
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

#define SizeUp4(val) ((((val)+3)>>2)<<2)
genPool_t *genBufLooksGood(genBuf_t *gbuf);
int objReleased;
void onRelease(intptr_t obj)
{
    objReleased++;
    TEST_ASSERT_NOT_EQUAL(0, ((genBuf_t*)obj -1)->size);
    TEST_ASSERT_NOT_EQUAL(0, ((genBuf_t*)obj -1)->guard);
    TEST_ASSERT_NOT_NULL(genBufLooksGood((genBuf_t*)obj -1));
}

void test_generalPool_init(void)
{
    // object with a 32 bit object, should force 4 byte boundary sizing 
    typedef struct {uint8_t v8; uint16_t v16; uint32_t v32; uint8_t arr32[5];} myObj_t;
    // object with only odd number of bytes, should byte align
    typedef struct {uint8_t arr[5];} myObj5_t;
    genPool_t *pool;
    genPool_t *pool5;
    GenPoolAllocate(pool, myObj_t, OBJECTS);
    GenPoolAllocate(pool5, myObj5_t, OBJECTS);
    
    TEST_ASSERT_EQUAL(sizeof(uint32_t),sizeof(genBuf_t)); // fundamental assumption
    
    TEST_ASSERT_EQUAL(sizeof(myObj_t), pool->objectSize);
    TEST_ASSERT_EQUAL(sizeof(myObj5_t), pool5->objectSize);
    TEST_ASSERT_EQUAL(5, pool5->objectSize); // fundamental assumption
    // verify size of cells falls on 4 byte boundaries
    TEST_ASSERT_EQUAL(sizeof(myObj_t)+sizeof(genBuf_t), pool->cellSize);
    TEST_ASSERT_EQUAL(SizeUp4(sizeof(myObj5_t))+sizeof(genBuf_t), pool5->cellSize);
    TEST_ASSERT_NOT_EQUAL(sizeof(myObj5_t)+sizeof(genBuf_t), pool5->cellSize);
    // verily location of last cell
    TEST_ASSERT_EQUAL((uint8_t*)pool + sizeof(genPool_t) + 
                      pool->cellSize*(OBJECTS-1)  ,
                      (uint8_t*)pool->end);
    TEST_ASSERT_EQUAL((uint8_t*)pool5 + sizeof(genPool_t) + 
                      pool5->cellSize*(OBJECTS-1)  ,
                      (uint8_t*)pool5->end);
    TEST_ASSERT_EQUAL(pool->base, pool->next);
    TEST_ASSERT_EQUAL(pool5->base, pool5->next);
}

void test_generalPool_Get(void)
{
    // object with a 32 bit object, should force 4 byte boundary sizing 
    typedef struct {uint8_t v8; uint16_t v16; uint32_t v32; uint8_t arr32[5];} myObj_t;
    // object with only odd number of bytes, should byte align
    typedef struct {uint8_t arr[5];} myObj5_t;
    genPool_t *pool;
    genPool_t *pool5;
    GenPoolAllocate(pool, myObj_t, OBJECTS);
    GenPoolAllocate(pool5, myObj5_t, OBJECTS);
    
    uint32_t *cell = (uint32_t*)pool->base + 1;
    uint32_t *cell5 = (uint32_t*)pool5->base + 1;
    for (int i=0; i<OBJECTS; i++)
    {
        genBuf_t *gbuf = GenPool_GetGenBuf(pool);
        genBuf_t *gbuf5 = GenPool_GetGenBuf(pool5);
        TEST_ASSERT_NOT_NULL(gbuf);
        TEST_ASSERT_NOT_NULL(gbuf5);
        TEST_ASSERT_EQUAL(sizeof(myObj_t), gbuf->size);
        TEST_ASSERT_EQUAL(sizeof(myObj5_t), gbuf5->size);
        TEST_ASSERT_EQUAL(cell - (uint32_t*)pool->base, gbuf->guard);
        TEST_ASSERT_EQUAL(cell5 - (uint32_t*)pool5->base, gbuf5->guard);  
        cell += pool->cellSize/4;
        cell5 += pool5->cellSize/4;
    }
    
    // this should fail since we checked them all out
    genBuf_t *gbuf = GenPool_GetGenBuf(pool);
    genBuf_t *gbuf5 = GenPool_GetGenBuf(pool5);
    TEST_ASSERT_NULL(gbuf);
    TEST_ASSERT_NULL(gbuf5);
}

// fill a space with random
void fillRnd(void *buf, size_t size)
{
    for (int i=0; i<size;i++)
    {
        ((uint8_t*)buf)[i] = (uint8_t)rand();
    }
}

void test_generalPool_Return(void)
{
    // object with a 32 bit object, should force 4 byte boundary sizing 
    typedef struct {uint8_t v8; uint16_t v16; uint32_t v32; uint8_t arr32[5];} myObj_t;
    // object with only odd number of bytes, should byte align
    typedef struct {uint8_t arr[5];} myObj5_t;
    genPool_t *pool;
    genPool_t *pool5;
    GenPoolAllocate(pool, myObj_t, OBJECTS);
    GenPoolAllocateWithCallback(pool5, myObj5_t, OBJECTS,onRelease);
    genPool_t *pools[2] = {pool, pool5};
    
    uint32_t *cell = (uint32_t*)pool->base + 1;
    uint32_t *cell5 = (uint32_t*)pool5->base + 1;
    
    genBuf_t *bufs[11];
    int out = 3;
    // wind up allocation
    for (int i=0; i<11; i++)
    {
        bufs[i] = GenPool_GetGenBuf(pools[i%2]);
        fillRnd(bufs[i]->buf, bufs[i]->size);
    }

    int test = 0;
    objReleased = 0;
    while(test < 1000)
    {
        TEST_ASSERT_EQUAL(0,GenPool_ReturnGenBuf(bufs[out]));
        TEST_ASSERT_NOT_NULL(bufs[out] = GenPool_GetGenBuf(pools[test%2]));
        fillRnd(bufs[out]->buf, bufs[out]->size);
        out = (++out)%11;
        test++;
    }
    
    TEST_ASSERT_EQUAL(499, objReleased);
}



// object with a 32 bit object, should force 4 byte boundary sizing 
typedef struct {uint8_t v8; uint16_t v16; uint32_t v32; uint8_t arr32[5];} myObj_t;
// object with only odd number of bytes, should byte align
typedef struct {uint8_t arr[5];} myObj5_t;

uint32_t pCalled;
void processSet(genBuf_t *obj)
{
    myObj_t *test = (myObj_t*)obj->buf;
    test->v32 = test->v16 + test->v8;
    pCalled++;
}

void processTest(genBuf_t *obj)
{
    myObj_t *test = (myObj_t*)obj->buf;
    TEST_ASSERT_EQUAL(test->v32, test->v16 + test->v8);
    pCalled++;
}

void test_generalPool_void_pointer(void)
{

    genPool_t *pool;
    genPool_t *pool5;
    GenPoolAllocateWithCallback(pool, myObj_t, OBJECTS, onRelease);
    GenPoolAllocate(pool5, myObj5_t, OBJECTS);
    genPool_t *pools[2] = {pool, pool5};
    
    
    void *bufs[11];
    int out = 2;
    // wind up allocation
    for (int i=0; i<11; i++)
    {
        bufs[i] = GenPool_Get(pools[i%2]);
        fillRnd(bufs[i], GenPool_GetSize(bufs[i]));
    }

    int test = 0;
    objReleased = 0;
    while(test < 1000)
    {
        TEST_ASSERT_EQUAL(0,GenPool_Return(bufs[out]));
        TEST_ASSERT_NOT_NULL(bufs[out] = GenPool_Get(pools[test%2]));
        fillRnd(bufs[out], GenPool_GetSize(bufs[out]));
        out = (++out)%11;
        test++;
    }
    TEST_ASSERT_EQUAL(501, objReleased);
    // make sure we have at least 3 inactive
    GenPool_Return(bufs[3]);
    GenPool_Return(bufs[6]);
    GenPool_Return(bufs[1]);
    
    // At this point data should be full of random.
    // Test the on each functions by comparing all to sets of active and inactive
    pCalled = 0;
    GenPool_OnEach(pool, processSet);
    TEST_ASSERT_EQUAL(OBJECTS, pCalled);
    pCalled = 0;
    GenPool_OnEachActive(pool, processTest);
    int found = pCalled;
    TEST_ASSERT_TRUE(pCalled < OBJECTS);
    GenPool_OnEachInactive(pool, processTest);
    TEST_ASSERT_EQUAL(OBJECTS, pCalled);
    
    
}

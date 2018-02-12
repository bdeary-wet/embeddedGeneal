#include "unity.h"
#include "genlists.h"

void setUp(void)
{
}

void tearDown(void)
{
}

typedef struct sltt_s slistTest_t;
typedef struct dltt_s dlistTest_t;
typedef struct sltt_s
{
    slistTest_t *next;
    uint32_t someVal;
} slistTest_t;

typedef struct dltt_s
{
    dlistTest_t *next;
    dlistTest_t *prev;
    uint8_t someVal8;
    uint32_t someVal;    
} dlistTest_t;


void test_genlists_slist_adds(void)
{
    // test first head
    slist_t slist = {NULL,NULL};
    slistTest_t sarr[10];
    sl_addHead(&slist, &sarr[5]);
    TEST_ASSERT_EQUAL_PTR(slist.head, &sarr[5]);
    TEST_ASSERT_EQUAL_PTR(slist.tail, &sarr[5]);
    TEST_ASSERT_NULL(sarr[5].next);    
    // test a tail
    sl_addTail(&slist, &sarr[7]);
    TEST_ASSERT_EQUAL_PTR(slist.head, &sarr[5]);
    TEST_ASSERT_EQUAL_PTR(slist.tail, &sarr[7]);
    TEST_ASSERT_EQUAL_PTR(sarr[5].next, &sarr[7]);
    TEST_ASSERT_NULL(sarr[7].next); 
    // test a head
    sl_addHead(&slist, &sarr[9]);
    TEST_ASSERT_EQUAL_PTR(slist.head, &sarr[9]);
    TEST_ASSERT_EQUAL_PTR(slist.tail, &sarr[7]);
    TEST_ASSERT_EQUAL_PTR(sarr[5].next, &sarr[7]);
    TEST_ASSERT_EQUAL_PTR(sarr[9].next, &sarr[5]);    
    TEST_ASSERT_NULL(sarr[7].next);  

    // clear the list, reuse an element we know is dirty
    slist.head = NULL;
    slist.tail = NULL;
    sarr[5].someVal = 0x55555555;
    TEST_ASSERT_NOT_NULL(sarr[5].next);
    sl_addTail(&slist, &sarr[5]);
    TEST_ASSERT_EQUAL_PTR(slist.head, &sarr[5]);
    TEST_ASSERT_EQUAL_PTR(slist.tail, &sarr[5]);
    TEST_ASSERT_NULL(sarr[5].next);  
    TEST_ASSERT_EQUAL(0x55555555, sarr[5].someVal);


    
}

void test_genlists_dlist_adds(void)
{
    dlist_t dlist = {NULL,NULL};
    dlistTest_t darr[10];
    dl_addHead(&dlist, (dlNode_t*)&darr[5]);        // first item at head
    TEST_ASSERT_EQUAL_PTR(dlist.head, &darr[5]);    // list pointers updated
    TEST_ASSERT_EQUAL_PTR(dlist.tail, &darr[5]);
    TEST_ASSERT_NULL(darr[5].next);                 // node pointers updated
    TEST_ASSERT_NULL(darr[5].prev);   
    // test a tail
    dl_addTail(&dlist, (dlNode_t*)&darr[7]);        // second item at tail
    TEST_ASSERT_EQUAL_PTR(dlist.head, &darr[5]);    // head not changed
    TEST_ASSERT_EQUAL_PTR(dlist.tail, &darr[7]);    // tail was changed
    TEST_ASSERT_EQUAL_PTR(darr[5].next, &darr[7]);  // existing node updated
    TEST_ASSERT_NULL(darr[5].prev);     
    TEST_ASSERT_NULL(darr[7].next);                 // new node set
    TEST_ASSERT_EQUAL_PTR(&darr[5], darr[7].prev); 
    // test a head
    dl_addHead(&dlist, (dlNode_t*)&darr[9]);        // third item at head
    TEST_ASSERT_EQUAL_PTR(dlist.head, &darr[9]);    // head changed
    TEST_ASSERT_EQUAL_PTR(dlist.tail, &darr[7]);    // tail not changed
    TEST_ASSERT_NULL(darr[7].next);      
    TEST_ASSERT_EQUAL_PTR(&darr[7], darr[5].next);  
    TEST_ASSERT_EQUAL_PTR(&darr[5], darr[9].next);  // new node correct
    TEST_ASSERT_NULL(darr[9].prev);     
    TEST_ASSERT_EQUAL_PTR(&darr[9], darr[5].prev);   // old head node updated 
    TEST_ASSERT_EQUAL_PTR(&darr[7], darr[5].next);   // next still same 


    // clear the list, reuse an element we know is dirty
    dlist.head = NULL;
    dlist.tail = NULL;
    darr[5].someVal8 = 55;
    TEST_ASSERT_NOT_NULL(darr[5].next);
    TEST_ASSERT_NOT_NULL(darr[5].prev);
    dl_addTail(&dlist, (dlNode_t*)&darr[5]);        // add tail when empty
    TEST_ASSERT_EQUAL_PTR(dlist.head, &darr[5]);
    TEST_ASSERT_EQUAL_PTR(dlist.tail, &darr[5]);
    TEST_ASSERT_NULL(darr[5].next);  
    TEST_ASSERT_NULL(darr[5].prev);  
    TEST_ASSERT_EQUAL(55, darr[5].someVal8);    
}

void test_genlists_dlist_remove(void)
{
    dlist_t dlist = {NULL,NULL};
    dlistTest_t darr[10];
    for (int i=0; i<10; i+=2)
    {
        dl_addHead(&dlist, (dlNode_t*)&darr[i]);        // first item at head
        dl_addTail(&dlist, (dlNode_t*)&darr[i+1]);        // first item at head
        TEST_ASSERT_EQUAL_PTR((dlNode_t*)&darr[i],dlist.head);
        TEST_ASSERT_EQUAL_PTR((dlNode_t*)&darr[i+1],dlist.tail);
    }
    // 8642013579 
    
    // remove middle
    TEST_ASSERT_EQUAL_PTR(darr[4].next, &darr[2]);
    TEST_ASSERT_EQUAL_PTR(darr[0].prev, &darr[2]);    
    dl_remove(&dlist, (dlNode_t*)&darr[2]);
    TEST_ASSERT_EQUAL_PTR(darr[4].next, &darr[0]);
    TEST_ASSERT_EQUAL_PTR(darr[0].prev, &darr[4]); 
    TEST_ASSERT_EQUAL_PTR(dlist.head, &darr[8]); 
    TEST_ASSERT_EQUAL_PTR(dlist.tail, &darr[9]);     
    
    // remove head
    TEST_ASSERT_NULL(darr[8].prev);
    TEST_ASSERT_EQUAL_PTR(darr[6].prev, &darr[8]);    
    dl_remove(&dlist, (dlNode_t*)&darr[8]);
    TEST_ASSERT_NULL(darr[6].prev)
    TEST_ASSERT_EQUAL_PTR(dlist.head, &darr[6]); 
    TEST_ASSERT_EQUAL_PTR(dlist.tail, &darr[9]);      
    
    // remove tail 
    TEST_ASSERT_NULL(darr[9].next);
    TEST_ASSERT_EQUAL_PTR(darr[7].next, &darr[9]);    
    dl_remove(&dlist, (dlNode_t*)&darr[9]);
    TEST_ASSERT_NULL(darr[7].next)
    TEST_ASSERT_EQUAL_PTR(dlist.head, &darr[6]); 
    TEST_ASSERT_EQUAL_PTR(dlist.tail, &darr[7]);     

    dl_remove(&dlist, (dlNode_t*)&darr[1]);  
    dl_remove(&dlist, (dlNode_t*)&darr[3]);  
    dl_remove(&dlist, (dlNode_t*)&darr[7]);      
    dl_remove(&dlist, (dlNode_t*)&darr[6]);  
    dl_remove(&dlist, (dlNode_t*)&darr[4]);  
    dl_remove(&dlist, (dlNode_t*)&darr[0]);  
    TEST_ASSERT_EQUAL_PTR(dlist.head, &darr[5]);
    TEST_ASSERT_EQUAL_PTR(dlist.tail, &darr[5]);
    dl_remove(&dlist, (dlNode_t*)&darr[5]);  
    TEST_ASSERT_NULL(dlist.head);
    TEST_ASSERT_NULL(dlist.tail);
}



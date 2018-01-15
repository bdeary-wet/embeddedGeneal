#include "generalQueue.h"
#include <string.h>

#define BUMP(val) \
val >= q->end ? q->base: val + q->objectSize;


int GenQ_Init(genQ_t *q, void *buffer, uint16_t objectSize, uint16_t totalObjects)
{
    if (totalObjects < 2) return -1;
    q->base = buffer;
    q->next = buffer;
    q->last = buffer;
    q->end = (uint8_t*)buffer + objectSize * (totalObjects-1);
    q->objectSize = objectSize;
    return 0;
    
}

int GenQ_Put(genQ_t *q, void const *obj)
{
    uint8_t *next = BUMP(q->next);
    if (next ==  q->last) return -1;  // queue is full
    memcpy(q->next, obj, q->objectSize);
    q->next = next;
    return 0;
}

int GenQ_Get(genQ_t *q, void *obj)
{
    if (q->next == q->last) return -1; // queue empty
    memcpy(obj, q->last, q->objectSize);
    q->last = BUMP(q->last);
    return 0;    
}

int GenQ_IsData(genQ_t *q)
{
    return q->next != q->last;
}

uint16_t GenQ_ObjectSize(genQ_t *q)
{
    return q->objectSize;
}
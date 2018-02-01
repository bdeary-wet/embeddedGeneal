#include "generalQueue.h"
#include <string.h>



#define BUMP(val) \
(val) >= q->end ? q->base: (val) + q->objectSize;

#define BUMPP(val) \
(val) >= p->end ? p->base: (val) + p->objectSize;


int GenQ_Init(genQ_t *q, void *buffer, uint16_t objectSize, uint16_t totalObjects)
{
    if (!q || totalObjects < 2) return -1;
    q->base = buffer;
    q->next = buffer;
    q->last = buffer;
    q->end = (uint8_t*)buffer + objectSize * (totalObjects-1);
    q->objectSize = objectSize;
    return 0;
}

// always leaves one empty space, so putter only modifies next
// and getter only modifies last
int GenQ_Put(genQ_t *q, void const *obj)
{
    uint8_t *next = BUMP(q->next);
    if (!q || next ==  q->last) return -1;  // queue is full
    memcpy(q->next, obj, q->objectSize);
    q->next = next;
    return 0;
}

int GenQ_Get(genQ_t *q, void *obj)
{
    if (!q || q->next == q->last) return -2; // queue empty
    memcpy(obj, q->last, q->objectSize);
    q->last = BUMP(q->last);
    return 0;    
}

int GenQ_IsData(genQ_t *q)
{
    return q && (q->next != q->last);
}

int GenQ_IsSpace(genQ_t *q)
{
    uint8_t *next = BUMP(q->next);
    return q && (next !=  q->last);
}

uint16_t GenQ_ObjectSize(genQ_t *q)
{
    return q->objectSize;
}

size_t GenQ_Size(void) {return sizeof(genQ_t);}




#define SizeUp4(val) ((((val)+3)>>2)<<2)


genPool_t *GenPool_Init(uint32_t *space, size_t spaceSize, uint16_t objectSize)
{
    genPool_t *pool = (genPool_t*)space;
    size_t objects = (spaceSize-sizeof(genPool_t)) /
                     (SizeUp4(objectSize)+sizeof(genBuf_t));
    if(objects == 0 || space == NULL || spaceSize < sizeof(genPool_t)+4) return NULL;
    memset(space, 0, spaceSize); // clear the space
    pool->objectSize = objectSize;
    pool->cellSize = SizeUp4(objectSize) + sizeof(genBuf_t);
    pool->end = (genBuf_t*)((uint8_t*)pool->base + (objects-1) * pool->cellSize);
    pool->next = (genBuf_t*)pool->base;
    return pool;
}

genBuf_t *GenPool_GetGenBuf(genPool_t *p)
{
    if (!p) return NULL; // bad pool
    
    genBuf_t *check = p->next;
    genBuf_t *end = check;
    do
    {
        genBuf_t *next = (genBuf_t*)((uint8_t*)check + p->cellSize);
        if (next > p->end) next = (genBuf_t*)p->base;
        if(!check->size) 
        {
            check->size = p->objectSize;
            if(check->guard == 0)
            {
                check->guard = check - (genBuf_t*)p->base + 1; // add 1 so != 0
                p->next = next;
                return check;
            }
        }
        check = (genBuf_t*)next;
    }while (check != end);
    // final hail mary
    check = p->next;
    if(!check->size) 
    {
        check->size = p->objectSize;
        if(check->guard == 0)
        {
            check->guard = check - (genBuf_t*)p->base + 1;
            return check;
        }
    }
    return NULL;
}

void *GenPool_Get(genPool_t *p)
{
    genBuf_t *buf = GenPool_GetGenBuf(p);
    if(buf) return buf->buf;
    return NULL;
}

static genPool_t *genBufLooksGood(genBuf_t *gbuf)
{
    genBuf_t *base = gbuf - (gbuf->guard-1); // offset one so not zero
    genPool_t *pool = ((genPool_t*)base)-1;
    if(pool->objectSize != gbuf->size) return NULL;    
    return pool;
}

int GenPool_ReturnGenBuf(genBuf_t *gbuf)
{
    genPool_t *pool = genBufLooksGood(gbuf);
    if(!pool) return -1;
    ReleaseGenBuf(gbuf);
    pool->next = gbuf; // update suggestion 
    return 0;
}

int GenPool_Return(void *buf)
{
    return GenPool_ReturnGenBuf(((genBuf_t*)buf)-1);
}

size_t GenPool_GetSize(void *buf)
{
    genBuf_t *gbuf = (genBuf_t*)buf - 1;
    if (genBufLooksGood(gbuf)) return gbuf->size;
    return 0;
}


void GenPool_ReturnNoCheck(void *buf)
{
    ReleaseGenBuf(((genBuf_t*)buf)-1);
}


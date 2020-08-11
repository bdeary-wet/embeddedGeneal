/**
 * @file genPool.h
 * @author bdeary (bdeary@wetdesign.com)
 * @brief Generic pool object for creating a pool of any type
 * @version 0.1
 * @date 2020-08-10
 * 
 * Copyright 2020, WetDesigns
 * 
 */
#ifndef _GENPOOL_H
#define _GENPOOL_H
#include <stdint.h>

/**
 * @brief This is a object type that can hold any single
 *        value usefull as a context 
 *        (usually a pointer to data or pointer to code)
 */
typedef union Context_t
{
    intptr_t v_context;     // can hold any data pointer or reasonable integer
    void (*f_context)(union Context_t *context); // can hold a function pointer
} Context_t;

// generic callback function pointer
typedef void (*GenCallback_t)(Context_t context);

typedef struct
{
    volatile uintptr_t size:16;   // a non zero number 1 to some max size
    volatile uintptr_t guard:16;  // the address of the buffer area
    uintptr_t obj[];              // Not actually part of the structure
} genBuf_t;


typedef struct genPool_t
{
    genBuf_t *next;      // next entry point
    genBuf_t * const end;       // address of last object in buffer
    GenCallback_t onRelease; // optional function to call on release
    size_t const objectSize;  // size of the object 
    size_t const cellSize;    // size of the cell holding the object
    uintptr_t base[];     // buffer start, not part of the structure
} genPool_t;


/**
 * @brief define a pool object pointer and the associated pool object
 *        so if we use name fooPool of type MyChunk with 10 elements
 */
#define GenPoolDefine(name, type, len, onRel) \
enum {name##_len = (len)}; \
typedef struct { \
    genBuf_t genBuf; \
    type poolObject[1]; /* the MyChunk object */ \
} name##_objtype; /* creates type fooPool_objtype */ \
typedef struct { \
    genPool_t poolHeader; /* the base class object */ \
    name##_objtype pool[name##_len]; /* the pool storage */ \
} name##_pool_t; /* fooPool_pool_t */ \
/* actually instantiate and initialize */   \
name##_pool_t name##_pool = (name##_pool_t){ \
    .pool = {0}, /* zero the storage */ \
    .poolHeader.next=(genBuf_t *)name##_pool.pool, /* set first */ \
    .poolHeader.end=(genBuf_t *const)&name##_pool.pool[name##_len-1], /* assign const */ \
    .poolHeader.objectSize=sizeof(type), /* fill in derived info in base class */ \
    .poolHeader.cellSize = sizeof(name##_objtype), /* needed to find base from instance */ \
    .poolHeader.onRelease=onRel, \
}; \
genPool_t * const name = (genPool_t*)&name##_pool








#endif // _GENPOOL_H

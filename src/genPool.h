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

#ifdef TEST
#define STATIC
#endif
#ifndef STATIC 
#define STATIC static
#endif

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
    volatile uintptr_t theboss:16;// a non zero number 1 to some max size
    volatile uintptr_t guard:16;  // the address of the buffer area
    GenCallback_t onRelease;      // optional per buffer callback
    Context_t context;            // optional context variable
    uintptr_t obj[];              // Not actually part of the structure
} PreBuf_t;

// Pool object meta data
typedef struct 
{
    size_t objectSize;  // size of the object 
    int index;          // the offset in the pool
    GenCallback_t onRelease; // optional per buffer callback
    Context_t context;  // optional context variable    
} PreBufMeta_t;

typedef struct GenPool_t
{
    PreBuf_t *next;      // next entry point
    PreBuf_t * const end;       // address of last object in buffer
    GenCallback_t onRelease; // optional function to call on release
    size_t const objectSize;  // size of the object 
    size_t const cellSize;    // size of the cell holding the object
    uintptr_t base[];     // buffer start, not part of the structure
} GenPool_t;

typedef struct
{
    GenPool_t *pool;
    PreBuf_t  *pre;
    int index;
} PoolObjId_t;

/**
 * @brief define a pool object pointer and the associated pool object
 *        so if we use name fooPool of type MyChunk with 10 elements
 */
#define GenPoolDefine(name, type, len, onRel) \
enum {name##_len = (len)}; \
typedef struct { \
    PreBuf_t genBuf; \
    type poolObject[1]; /* the MyChunk object */ \
} name##_objtype; /* creates type fooPool_objtype */ \
typedef struct { \
    GenPool_t poolHeader; /* the base class object */ \
    name##_objtype pool[name##_len]; /* the pool storage */ \
} name##_pool_t; /* fooPool_pool_t */ \
/* actually instantiate and initialize */   \
name##_pool_t name##_pool = (name##_pool_t){ \
    .pool = {0}, /* zero the storage */ \
    .poolHeader.next=(PreBuf_t *)name##_pool.pool, /* set first */ \
    .poolHeader.end=(PreBuf_t *const)&name##_pool.pool[name##_len-1], /* assign const */ \
    .poolHeader.objectSize=sizeof(type), /* fill in derived info in base class */ \
    .poolHeader.cellSize = sizeof(name##_objtype), /* needed to find base from instance */ \
    .poolHeader.onRelease=onRel, \
}; \
GenPool_t * const name = (GenPool_t*)&name##_pool

// General calls using base class
/**
 * @brief Allocate an object from the pool container
 * 
 * @param self - pointer to base class of pool object
 * @return void* - returns pointer to allocated object as void pointer or NULL if none available
 */
void *GenPool_allocate(GenPool_t *self);

/**
 * @brief Allocate an object from the pool container with optional
 *   callback function on attempt to return to pool.
 * 
 * @param self - pointer to base class fo pool container
 * @param cb - optional function address to call of type GenCallback_t
 * @param context - optional context used with call back function
 * @return void* - returns pointer to allocated object or NULL in none are available
 */
void *GenPool_allocate_with_callback(GenPool_t *self, GenCallback_t cb, Context_t context);

/**
 * @brief return a object to its pool
 * 
 * @param obj pointer to original allocated object
 * @details fails silently if object was not a pool object or was
 *   already returned.
 */
void GenPool_return(void *obj);

/**
 * @brief Get copy of the pool object meta data
 * 
 * @param obj the pool object pointer
 * @return PreBufMeta_t a copy of the data, zero filled on error
 */
PreBufMeta_t GenPool_object_meta(void *obj);




#endif // _GENPOOL_H

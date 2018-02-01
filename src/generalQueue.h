/**
 *  @file generalQueue.h
 *  @brief General purpose queue functions for sending data between
 *         tasks or between ISR functions and background functions.
 *  
 *  @details This uses the queue design of allowing 1 less than the allocated
 *           space to be queued. This design allows the functions to operate
 *           without the use of protected regions or device supported atomic
 *           operations.  
 *           This requires that the totalObjects parameter and allocated
 *           memory buffer must be one object size larger than required.
 *           This also implies that a Queue length of 1 is illegal. 
 */
 /** \addtogroup usefulObjects
 *  @{
 */
#ifndef _GENERALQUEUE_H
#define _GENERALQUEUE_H
#include <stdint.h>
#include <stddef.h>


/// incomplete def of a generic Q object.
typedef struct genQ_s
{
    uint8_t *end;       // address of last object in buffer
    size_t objectSize;  // size of the object    
    uint8_t *next;      // next entry point
    uint8_t *last;      // next source point
    uint8_t *base;     // buffer start
} genQ_t;

/** @addtogroup usefulMacros */

// /helper macro to build a queue and define a pointer to it
#define QBUILDER_ALLOCA(nameP,type, length) \
genQ_t *nameP = alloca(sizeof(genQ_t)+sizeof(type)*length); \
GenQ_Init(nameP, nameP+1, sizeof(type), length)


/// uses malloc into an automatic variable so need to free before var goes
/// out of scope or make other arrangements 
#define QBUILDER_MALLOC(nameP,type, length) \
genQ_t *nameP = malloc(sizeof(genQ_t)+sizeof(type)*length); \
GenQ_Init(nameP, nameP+1, sizeof(type), length)

/// instantiate a queue into a static variable and initialize it once
#define QBUILDER_STATIC(nameP,type, length) \
static struct { \
    genQ_t q; \
    uint8_t buf[sizeof(type)*length]; \
} nameP##structure; \
static genQ_t *nameP = (genQ_t*)&nameP##structure;\
if (!nameP##structure.q.end) \
GenQ_Init(nameP, nameP##structure.buf, sizeof(type), length)

/// initialize an existing queue object with an existing array
#define ARRAY_TO_Q(arr, qObj) \
GenQ_Init(&qObj, arr, sizeof(arr[0]), sizeof(arr)/sizeof(arr[0]))

/// instantiate an automatic queue and attach an existing array.
#define QBUILDER_ARRAY(qObj, arr) \
static genQ_t qObj; \
GenQ_Init(&qObj, arr, sizeof(arr[0]), sizeof(arr)/sizeof(arr[0]))

 /** @} */

/**
 *  @brief Initialize a queue object
 *  
 *  @param [in] q            pointer to the q object to initialize
 *  @param [in] buffer       pointer to the memory buffer used to hold the objects
 *                           being queued.
 *  @param [in] objectSize   The size in bytes of one object.
 *  @param [in] totalObjects The number of objects the queue will hold plus one.
 *  @return return 0 if successful 
 *  
 *  @details Initializes a queue object with a memory area and an object size.
 *           Sets the queue to empty.  
 *           Remember the queue must be sized one larger than needed and queue
 *           sizes of less than 2 are not allowed. 
 */
int GenQ_Init(genQ_t *q, void *buffer, uint16_t objectSize, uint16_t totalObjects);

/**
 *  @brief Copy a user object into the queue.
 *  
 *  @param [in] q   pointer to the queue object.
 *  @param [in] obj pointer to the user object to copy
 *  @return 0 if successful or non-zero if queue is full or for other errors
 *  
 *  @details Copies the user object into the queue if there is space available. 
 */
int GenQ_Put(genQ_t *q, void const *obj);

/**
 *  @brief Copy a user object out of the queue
 *  
 *  @param [in] q   pointer tot he queue object
 *  @param [out] obj pointer to location where object should be copied.
 *  @return 0 if successful or non-zero if queue is empty
 *  
 *  @details De-queues an object if available.      
 */
int GenQ_Get(genQ_t *q, void *obj);

/**
 *  @brief Test if data is available in the queue.
 *  
 *  @param [in] q pointer to the queue object
 *  @return non zero if data is available
 *  
 *  @details Tests if data is available in the queue.
 */
int GenQ_IsData(genQ_t *q);

/**
 *  @brief Get the configured object size of the queue.
 *  
 *  @param [in] q pointer tothe queue pbject
 *  @return The configure size of queued objects 
 *  
 *  @details Returns the original objectSize parameter used to initialize the queue.
 *           This is useful for intermediate functions that don't care about the
 *           contained object but must do some buffer operation on an entry,
 *           like copy it to another buffer or message structure.
 */
uint16_t GenQ_ObjectSize(genQ_t *q);

size_t GenQ_Size(void);

typedef struct
{
    volatile uint32_t size:16;   // a non zero number 1 to some max size
    volatile uint32_t guard:16;  // the address of the buffer area
    uint32_t buf[];
} genBuf_t;

typedef struct genPool_s
{
    genBuf_t *next;      // next entry point
    genBuf_t *end;       // address of last object in buffer
    size_t objectSize;  // size of the object 
    size_t cellSize;
    uint32_t base[];     // buffer start
} genPool_t;

/**

 */
#define GenPoolSpace(type, number) \
(((((sizeof(type)+sizeof(genBuf_t) +3)/4) * number)*4)+sizeof(genPool_t))
    
genPool_t *GenPool_Init(uint32_t *space, size_t spaceSize, uint16_t objectSize);

void *GenPool_Get(genPool_t *p);
genBuf_t *GenPool_GetGenBuf(genPool_t *p);
int GenPool_Return(void *buf);
int GenPool_ReturnGenBuf(genBuf_t *buf);
static inline void ReleaseGenBuf(genBuf_t *gbuf)
{
    gbuf->guard = 0;    // order is important
    gbuf->size = 0;     
}
void GenPool_ReturnNoCheck(void *buf);


/** @}*/
#endif // _GENERALQUEUE_H

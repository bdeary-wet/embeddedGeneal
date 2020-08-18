/**
 * @file isr_comm.h
 * @author bdeary (bdeary@wetdesign.com)
 * @brief module for doing communication between user and isr layers.
 * @version 0.1
 * @date 2020-08-16
 * @details  Includes data queues, object pool handlers and the isr_blocking calls
 *      Includes helper macros to generate user/isr message queues and type specific
 *      access functions.
 *      Allows either single direction or double (bi-directional) message passing from a
 *      generated pool of user specified message types.
 *      The single direction is intended for transmit only isr and the double is
 *      intended for transmit/receive messaging.  
 *      Receive only or receive/transmit use cases can be handled by either type depending on
 *      the buffering requirements. 
 *      The pooled objects have an embedded callback facility that allows for creation of perpetual
 *      processing if desired.
 * 
 *      For Example, a single direction isrProcessTarget named Foo that takes
 *      messages of type myMessage will generate functions:
 * 
 *      myMessage* Allocate_Foo_Object(void);        // get object from pool
 *      Status_t   Send_Foo_Object(myMessage *obj);  // send object to isr
 *      Status_t   Dequeue_Foo_Object(void);         // dequeue and object in isr
 *      void       Free_Foo_Object(myMessage *obj);  // used by isr to free pool object
 * 
 *      For a double direction isrProcessTarget name Bar that takes
 *      messages of type aThing will generate functions:
 * 
 *      aThing*  Allocate_Bar_Object(void);           // get object from pool
 *      Status_t Send_Bar_Object(aThing *obj);        // send object to isr
 *      Status_t Dequeue_Bar_Object(void);            // dequeue and object in isr
 *      void     Return_Bar_Object(aThing *obj);      // return object from isr to user
 *      Status_t Receive_Bar_Object(aThing **obj);    // dequeue returned object from isr
 *      Status_t PoolReturn_Bar_Object(aThing *obj);  // return object to pool
 * 
 * Copyright 2020, WetDesigns
 * 
 */
#ifndef ISR_COMM_H
#define ISR_COMM_H
#include <config.h>

#include <genPool.h>
#include <genQ.h>

typedef void *PoolQType;

typedef struct IsrProcessTarget_t
{
    GenPool_t * const iptPool;
    GenQ_t * const toIsrQ;     // for sending user->isr
    GenQ_t * const fromIsrQ;   // for sending isr->user (only used when one to one)
    uint32_t droppedTo;     // bumped if tried to send and no room 
    uint32_t droppedFrom;   // bumped if could not return
} IsrProcessTarget_t;


/**
 * @brief Helper macros to auto generate user/isr queue prototypes
 * 
 */
#define DefineSingleIsrObjectQueuePrototypes(name, type) \
type *Allocate_##name##_Object(void); \
Status_t Send_##name##_Object(type *obj); \
Status_t Dequeue_##name##_Object(type **obj); \
void Free_##name##_Object(type *obj)

#define DefineDoubleIsrObjectQueuePrototypes(name, type) \
type *Allocate_##name##_Object(void); \
Status_t Send_#name##_Object(type *obj); \
Status_t Dequeue_##name##_Object(type **obj); \
void Return_##name##_Object(type *obj); \
Status_t Receive_##name##_Object(type **obj); \
Status_t PoolReturn_##name##_Object(type *obj)

/**
 * @brief helper macros to autogenerate and initialize user/isr queues
 * @details A Single is designed for Isr that are transmit only
 */
#define DefineSingleIsrObjectQueue(name, type, depth, complete) \
StaticGenPoolDefine(P##name, type, depth, complete); \
StaticGenQDef(name##_toQ, type*, depth); \
STATIC IsrProcessTarget_t name##_instance = (IsrProcessTarget_t){ \
    .iptPool = (GenPool_t*)&P##name##_pool, \
    .toIsrQ = &name##_toQ_instance, \
    .droppedTo = 0, \
}; \
IsrProcessTarget_t *name = &name##_instance; \
/* Single Q functions */ \
type *Allocate_##name##_Object(void) \
{ return (type*) GenPool_allocate(name->iptPool); } \
Status_t Send_##name##_Object(type *obj) \
{ return Ipt_SendToIsr(name, obj); } \
Status_t Dequeue_##name##_Object(type **obj) \
{ return Ipt_DequeueInIsr(name, (void**)obj); } \
void Free_##name##_Object(type *obj) \
{ Ipt_ReturnToUserProcess(name, obj); }


#define DefineDoubleIsrObjectQueue(name, type, depth, complete) \
StaticGenPoolDefine(P##name, type, depth, complete); \
StaticGenQDef(name##_toQ, type*, depth); \
StaticGenQDef(name##_fromQ, type*, depth); \
IsrProcessTarget_t name##_instance = (IsrProcessTarget_t){ \
    .iptPool = (GenPool_t*)&P##name##_pool, \
    .toIsrQ = &name##_toQ_instance, \
    .droppedTo = 0, \
    .fromIsrQ = &name##_fromQ_instance, \
    .droppedFrom = 0 \
}; \
IsrProcessTarget_t * const name = &name##_instance; \
/* Double Q functions */ \
type *Allocate_##name##_Object(void) \
{ return (type*) GenPool_allocate(name->iptPool); } \
Status_t Send_##name##_Object(type *obj) \
{ return Ipt_SendToIsr(name, obj); } \
void Return_##name##_Object(type *obj) \
{ Ipt_ReturnToUserProcess(name, obj);} \
Status_t Dequeue_##name##_Object(type **obj) \
{ return Ipt_DequeueInIsr(name, (void**)obj); } \
Status_t Receive_##name##_Object(type **obj) \
{ return Ipt_ReceiveFromIsr(name, (void**)obj); }\
Status_t PoolReturn_##name##_Object(type *obj) \
{ return GenPool_return(obj); }


/****************************************/
/* Functions callable in the USER layer */
/****************************************/

void Ipt_Reset(IsrProcessTarget_t *ipt);

/**
 * @brief Send a pool object to an Isr via its Q
 * 
 * @param ipt - the Isr process object
 * @param object - The pool object pointer
 * @return Status_t - return Ok if successful otherwise FULL or other error 
 */
Status_t Ipt_SendToIsr(IsrProcessTarget_t *ipt, void *object);

/**
 * @brief 
 * 
 * @param ipt 
 * @param object 
 * @return Status_t 
 */
Status_t Ipt_ReceiveFromIsr(IsrProcessTarget_t *ipt, void **object);

/**
 * @brief backgroud user space function to dequeue
 *   returned pool objects from the isr or user
 *   space 
 * 
 */
void ProcessReturnPoolObjects(void);

/****************************************/
/* Functions callable in the ISR layer */
/****************************************/

/**
 * @brief This sends an pool object back to the user layer before
 *  calling any embedded callbacks.
 * 
 * @param obj - pool object pointer
 * @details 
 */
void ReturnPoolObject(void *obj);

/**
 * @brief This sends an pool object back to the user layer via
 *  a pre-defined queue or the general queue if no queue is available.
 * 
 * @param ipt - the Isr process object
 * @param object The object to return
 */
void Ipt_ReturnToUserProcess(IsrProcessTarget_t *ipt, void *object);

/**
 * @brief Dequeue object sent from User to Isr
 * 
 * @param ipt - the Isr process object
 * @param object [out] address to copy object to
 * @return Status_t Ok, if successful, EMPTY if no data available
 */
Status_t Ipt_DequeueInIsr(IsrProcessTarget_t *ipt, void **object);




#endif // ISR_COMM_H

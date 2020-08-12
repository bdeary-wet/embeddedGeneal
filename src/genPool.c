/**
 * @file genPool.c
 * @author bdeary (bdeary@wetdesign.com)
 * @brief functions implimenting the general Pool
 * @version 0.1
 * @date 2020-08-10
 * 
 * Copyright 2020, WetDesigns
 * 
 */
#include "genPool.h"

#include <stdlib.h>
#include <assert.h>


STATIC PoolObjId_t getPoolId(void *buf);


// given the user's object pointer, work back to find if it
// belongs to a pool and if it does return the meta data
STATIC PoolObjId_t getPoolId(void *p_obj)
{
    PoolObjId_t id;
    const int offset = offsetof(PreBuf_t, obj); // address adjustment
    id.pre = (PreBuf_t*)((intptr_t)p_obj - offset); // possible address of PreBuf
    if (id.pre->theboss) // if guard is not zero
    {   // theboss is offset back in 32bit words, guard is offset forward as pool index
        id.pool = (GenPool_t*)(((intptr_t)id.pre) - ((int32_t)(id.pre->theboss) * 4));
        int delta = (intptr_t)id.pre - (intptr_t)id.pool->base;
        div_t ans = div(delta, id.pool->cellSize); // 
        if (ans.rem == 0 && ans.quot == id.pre->guard)
        {
            id.index = ans.quot;
            return id;
        }
    }
    return (PoolObjId_t){0}; // Not an allocated pool object
}


PreBufMeta_t GenPool_object_meta(void *obj)
{
    PoolObjId_t id = getPoolId(obj);
    if (id.pool != NULL)
        return (PreBufMeta_t){
            .context=id.pre->context,
            .onRelease=id.pre->onRelease,
            .index=id.index,
            .objectSize=id.pool->objectSize
            };
    else
        return (PreBufMeta_t){
            .objectSize=0
            };
}

void *GenPool_allocate(GenPool_t *self)
{
    return GenPool_allocate_with_callback(self, self->onRelease, (Context_t){.v_context=-1});
}

void *GenPool_allocate_with_callback(GenPool_t *self, GenCallback_t cb, Context_t context)
{
    int index = self->next->guard;
    int first = index;
    PreBuf_t *next = self->next;
    
    while(next->theboss)
    {
        if(next >= self->end)
        {
            next = (PreBuf_t*)self->base;
            index = 0;
        }
        else
        {
            next = (PreBuf_t*)( (intptr_t)next + self->cellSize );
            index++;
        }
        // if we wrapped around. give up
        if (index == first)
        {
            return NULL;
        }
    }
    next->theboss++; // claim it
    next->guard = index;
    intptr_t delta = (intptr_t)next - (intptr_t)self;
    assert((delta & 0x3) == 0 );  // divisible by 4
    self->next = next; // save where we where
    next->onRelease = cb;
    // if -1 then use this object as context
    next->context = (context.v_context==-1)?(Context_t){.v_context=(intptr_t)next->obj}: context;
    next->theboss = (uint16_t)(delta >> 2); // mark the field for recovery(validates the allocation)
    return next->obj; // return the object
}



void GenPool_return(void *obj);

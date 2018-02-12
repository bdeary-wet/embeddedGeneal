#ifndef _GENLISTS_H
#define _GENLISTS_H

typedef struct dlh_s dlNode_t;
typedef struct dlh_s
{
    dlNode_t *next;
    dlNode_t *prev;
} dlNode_t;
typedef struct
{
    dlNode_t* head;
    dlNode_t* tail;
} dlist_t;

typedef struct
{
    void** head;
    void** tail;
} slist_t;


void sl_addHead(slist_t *sl, void *node);


void sl_addTail(slist_t *sl, void *node);


void dl_addHead(dlist_t *dl, dlNode_t *node);


void dl_addTail(dlist_t *dl, dlNode_t *node);


void dl_remove(dlist_t *dl, dlNode_t *node);

void* dl_removeTail(dlist_t *dl);

void* dl_removeHead(dlist_t *dl);

#endif // _GENLISTS_H

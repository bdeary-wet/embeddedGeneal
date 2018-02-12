#include "genlists.h"
#include <stddef.h>


void sl_addHead(slist_t *sl, void *nd)
{
    void **node = nd;
    *node = sl->head;
    sl->head = node;
    if (*node == NULL) sl->tail = node;
}

void sl_addTail(slist_t *sl, void *nd)
{
    void **node = nd;
    if (!sl->tail)
    {
        sl->head = node;
    }    
    else
    {
        *(sl->tail) = node;
    }
    *node = NULL;
    sl->tail = node;
}

void dl_addHead(dlist_t *dl, dlNode_t *node)
{
    node->prev = NULL;
    node->next = dl->head;
    dl->head = node;
    if (node->next == NULL) dl->tail = node;
    else node->next->prev = node;
}

void dl_addTail(dlist_t *dl, dlNode_t *node)
{
    node->next = NULL;
    node->prev = dl->tail;
    dl->tail = node;
    if (node->prev == NULL) dl->head = node;
    else node->prev->next = node;
}

void dl_remove(dlist_t *dl, dlNode_t *node)
{
    if(dl->head == node) // then no prev
    {
        dl->head = node->next;
    }
    else if(node->prev)
    {
        (node->prev)->next = node->next;
    }
    if(dl->tail == node) // no next
    {
        dl->tail = node->prev;
    }    
    else if(node->next)
    {
        (node->next)->prev = node->prev;
        node->next = NULL;  // book keeping
    }
    node->prev = NULL;  // book keeping


}

void* sl_removeHead(slist_t *sl)
{
    void **next = sl->head;
    if(sl->head)
    {
        sl->head = *(sl->head);
        *next = NULL;
    }
    return next;
}

void* dl_removeHead(dlist_t *dl)
{
    dlNode_t *node = dl->head;
    dl_remove(dl, node);
    return node;
}

void* dl_removeTail(dlist_t *dl)
{
    dlNode_t *node = dl->tail;
    dl_remove(dl, node);
    return node;    
}

// run the list front to back looking for item and remove it.
void sl_remove(slist_t *sl, void *nd)
{
    void **node = nd;
    void **next = sl->head;
    void **prev = NULL;
    while(next)
    {
        if(next == node)
        {
            if(prev)
            {
                *prev = *next;
            }
            else
            {
                sl->head = *next;
            }
            if (sl->tail == node) sl->tail = prev;
            return;
        }
        prev = next;
        next = *next;
    }
    return;
}

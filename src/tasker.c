#include "tasker.h"

// Do only one each time in order
// return last slot run or -1 if none
int TaskerRoundRobin(tasker_t *to)
{
    if(!to->event) return -1;

    unsigned stop = to->jumpTable.tableLen;
    unsigned start = to->next;
    do {
        for(taskerMask_t mask = 1 << to->next; 
            to->event && to->next < stop;
            to->next++, mask <<= 1
           )
        {   // if event set, clear event
            if (to->event & mask)
            {
                to->event &= ~mask; // clear event
                // if function defined, call it and quit
                if (to->jumpTable.table[to->next])
                {
                    to->jumpTable.table[to->next](to->next); // make jump
                    int last = to->next++;
                    if(to->next >= to->jumpTable.tableLen) to->next = 0;
                    return last;
                }
            }
        }
        stop = start;
        start = 0;
    } while(stop);
    return -1;
}

// Do one each time, restarting at 0 each entry
// return last slot run or -1 if none
int TaskerPrioritized(tasker_t *to)
{
    unsigned next = 0;
    for(taskerMask_t mask = 1;
        to->event && next < to->jumpTable.tableLen; 
        next++, mask <<= 1
       )
    {   // if event set and function defined clear event and jump
        if (to->event & mask) 
        {
            to->event &= ~mask; // clear event
            if (to->jumpTable.table[next])
            {
                to->jumpTable.table[next](next); // make jump
                to->next = next + 1;
                if(to->next >= to->jumpTable.tableLen) to->next = 0;
                return next;
            }
        }
    }  
    return -1;
}

// run through entire list once based on event mask at entry
int TaskerOnePass(tasker_t *to)
{
    int ran = 0;
    unsigned next = 0;
    for(taskerMask_t events = to->event;
        events != 0;
        next++, events >>= 1
       )
    {   // if event set and function defined clear event and jump
        if (events & 1) 
        {
            to->event &= ~(1 << next);
            if (to->jumpTable.table[next]) to->jumpTable.table[next](next); 
            ran++;  // mark events where set
        }
    } 
    if (!ran) return -1;
    return ran;
}

// if tasks always also set events this will never terminate 
void TaskerRoundRobinTillNone(tasker_t *to)
{
    while(TaskerRoundRobin(to) != -1){}
}

// if tasks always also set events this will never terminate 
void TaskerPrioritizedTillNone(tasker_t *to)
{
    while(TaskerPrioritized(to) != -1){}
}


// convenience function for init. Could also static int at compile time. 
void TaskerInit(tasker_t *to, taskerFunction_f *jumpTable, unsigned int tableLen)
{
    to->jumpTable.table = jumpTable;
    to->jumpTable.tableLen = tableLen;
    to->event = 0;
    to->next = 0;
}

#include "eventlog.h"
#include "ecodef.h"
//#include "stm32f0xx_hal.h"
#include <string.h>

extern __IO uint32_t seconds;

eventStore_t *eventStore;

void setEvent(eventDes_t evt, uintptr_t val)
{
    if (!eventStore) return;
    // log the new event if there is space
    if (eventStore->next < &eventStore->events[EVENTS_TO_STORE])
    {
        eventStore->next->event = evt;
        eventStore->next->val = val;
        eventStore->next->seconds = EVENT_SECONDS;
        eventStore->next++;
    }
    // else log the event overrun and the last event and time
    else
    {
        eventRec_t *next = eventStore->next-1;
        next->event = EventsOverrun;
        next->val = evt;
        next->seconds = EVENT_SECONDS;
    }
    if (evt < Events) 
    {
        USAT_INC(eventStore->counts[evt]);
        //if (!(++eventStore->counts[evt])) --eventStore->counts[evt];
    }
}


uint8_t getEventCnt(void)
{
    if (!eventStore) return 0;
    return (uint8_t)(eventStore->next - eventStore->events);

}
    
// copy events into a buffer, clear the ones copied
// copy only enough that will fit in the buffer
// This is for building status messages
uint8_t getEvents(void *buf, size_t bufLen)
{
    if(eventStore)
    {
        uint32_t max = bufLen / sizeof(eventRec_t);
        // actual 
        uint32_t events = (uint32_t)(eventStore->next - eventStore->events);
        if (max > events) max = events;
        memcpy(buf, eventStore->events, max*sizeof(eventStore->events[0]));
        memmove(eventStore->events, &eventStore->events[max], 
                (events-max)*sizeof(eventStore->events[0]));
        eventStore->next -= max;
        return max;
    }
    return 0;
}

void initEvents(eventStore_t *baseAddress)
{
    eventStore = baseAddress;
    // if next is already correctly pointing into event buffer, just leave it
    for (int i=1; i<DIM(eventStore->events); i++)
    {
        if (eventStore->next == &(eventStore->events[i]))
        {
            setEvent(SystemReset, EVENT_SECONDS_HIGH);
            return;
        }
    }
    // else clean everything up
    memset(eventStore, 0, sizeof(*eventStore));
    eventStore->next = eventStore->events;
    eventTime = 0;
    setEvent(SystemReset, *((uint32_t*)&eventTime));
    return;
}

// let the user set the time
// log the event using the old time frame
void setEventTime(uint64_t time)
{
    uint32_t mask = __get_PRIMASK();
    setEvent(TimeSet, EVENT_SECONDS_HIGH);
    eventTime = time;
    __set_PRIMASK(mask);
}

/** @file eventlog.h
 *  @brief Event reporting system for saving important diagnostic information.
 *         While generally there is no way to report events, certain interfaces
 *         and specifically RDM have a facility to report to the controller
 *         that information is available to read.
 */
#ifndef _EVENTLOG_H_
#define _EVENTLOG_H_
#include <stdint.h>
#include <stddef.h>

#define EVENTS_TO_STORE (16)


// Add event types here
typedef enum {
    InternalError,  // Some internal software error
    ParameterError, // Call parameter error
    AssertError,    // An assert line
    SystemReset,    // A system reset
    BufferNotAvailable, // non fatal buffer busy 
    TimeSet,
    
    //----------------------- internal to event system
    EventsOverrun,
    Events
} eventDes_t;

typedef struct
{
    uint32_t seconds;
    eventDes_t event;
    uintptr_t val;
} eventRec_t;

typedef struct
{
    eventRec_t* next;
    eventRec_t events[EVENTS_TO_STORE];
    uint8_t counts[Events];
} eventStore_t;

extern eventStore_t *eventStore;
extern eventStore_t eventSpace;
extern uint64_t eventTime;

#define EVENT_SECONDS_HIGH  (*(((uint32_t*)&eventTime)+1))
#define EVENT_SECONDS  (*((uint32_t*)&eventTime))

void initEvents(eventStore_t *baseAddress);
void setEvent(eventDes_t evt, uintptr_t val);
uint8_t getEventCnt(void);
uint8_t getEvents(void *buf, size_t bufLen);
void setEventTime(uint64_t time); // set time in milliseconds












#endif

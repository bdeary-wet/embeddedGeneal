/** @file 
 *   @brief file purpose
 *
 *
 */
#ifndef _SW_TIMER_H_
#define _SW_TIMER_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint32_t start;
    uint32_t duration;
} swTime32_t;

typedef struct
{
    uint16_t start;
    uint16_t duration;
} swTime16_t;

#define SW_TIMER_PASSED(timer, time) \
  (time - (timer).start >= (timer).duration)
  
#define SW_TIMER_SET(timer, duration, time) \
  (timer).start = time; (timer).duration = duration;  
  
static inline void swTime32Set(swTime32_t *t, uint32_t duration, uint32_t time)  
{
    t->start = time;
    t->duration = duration;
}

static inline void swTime16Set(swTime16_t *t, uint16_t duration, uint16_t time)  
{
    t->start = time;
    t->duration = duration;
}
  
static inline bool swTimer32Passed(swTime32_t *t, uint32_t time)
{
    return time - t->start >= t->duration;
}

static inline bool swTimer16Passed(swTime16_t *t, uint16_t time)
{
    return time - t->start >= t->duration;
}


typedef uint32_t (*timer32_f)(void);
typedef uint16_t (*timer16_f)(void);


typedef struct
{
    timer32_f timer;
    swTime32_t timeObj;
} timerObj32_t;

typedef struct
{
    timer16_f timer;
    swTime32_t timeObj;
} timerObj16_t;

static inline void SW_Timer32Init(timerObj32_t *t, timer32_f timeBase)
{
    t->timer = timeBase;
    t->timeObj.start = 0;
    t->timeObj.duration = 0;
}

static inline void SW_Timer16Init(timerObj16_t *t, timer16_f timeBase)
{
    t->timer = timeBase;
    t->timeObj.start = 0;
    t->timeObj.duration = 0;
}

static inline void SW_SetTimer16(timerObj16_t *t, uint16_t duration)
{
    t->timeObj.start = t->timer();
    t->timeObj.duration = duration;
}

static inline void SW_SetTimer32(timerObj32_t *t, uint32_t duration)
{
    t->timeObj.start = t->timer();
    t->timeObj.duration = duration;
}

static inline bool SW_Timer32Passed(timerObj32_t *t)
{
    return t->timeObj.duration && SW_TIMER_PASSED(t->timeObj, t->timer());
}

static inline bool SW_Timer16Passed(timerObj16_t *t)
{
    return t->timeObj.duration && SW_TIMER_PASSED(t->timeObj, t->timer());
}

static inline void SW_ResetTimer16(timerObj16_t *t)
{
    t->timeObj.start = t->timer() - t->timeObj.start; // save how far we progressed
    t->timeObj.duration = 0;
}

static inline void SW_ResetTimer32(timerObj32_t *t)
{
    t->timeObj.start = t->timer() - t->timeObj.start; // save how far we progressed
    t->timeObj.duration = 0;
}












#endif
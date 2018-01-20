/**
 *  @file swtimer.h
 *  @brief does unsigned rollover math and tests for continuous timers
 *  
 *  @details assumes a timer of some sort either 16 or 32 bits and keeps track
 *  of a start time and a duration such that when:
 *  
 *  timer - start > duration
 *  
 *  the timer is considered passed.
 *  
 *  Assumes unsigned rollover math.
 *  
 *  
 */
/** \addtogroup usefulObjects
 *  @{
 */
#ifndef _SW_TIMER_H_
#define _SW_TIMER_H_

#include <stdint.h>
#include <stdbool.h>

/// type to hold a 32bit software timer.
typedef struct
{
    uint32_t start;
    uint32_t duration;
} swTime32_t;

/// type to hold a 16bit software timer.
typedef struct
{
    uint16_t start;
    uint16_t duration;
} swTime16_t;

/// helper macro used by the inline functions
/// test if timer has passed
#define SW_TIMER_PASSED(theTimer, timeNow) \
  (timeNow - (theTimer).start >= (theTimer).duration)
  
/// Start a timer   
#define SW_TIMER_SET(theTimer, theDuration, timeNow) \
  (theTimer).start = timeNow; (theTimer).duration = theDuration;
  
/// set new match point one duration past last match point  
#define SW_TIMER_CYCLE(theTimer) \
  (theTimer).start += (theTimer).duration;
  
/// extend timer using existing duration  
#define SW_TIMER_RESTART(theTimer, timeNow) \
  (theTimer).start = timeNow
  
  
/** @brief set a timer relative to a time source
 *  
 *  @param [in] t        pointer to timer struct
 *  @param [in] duration timer relative duration
 *  @param [in] time     the current timer
 *  
 *  @details starts a sw timer relative to a time value.
 */
static inline void swTime32Set(swTime32_t *t, uint32_t duration, uint32_t time)  
{
    t->start = time;
    t->duration = duration;
}

/** @brief set a timer relative to a time source
 *  
 *  @param [in] t        pointer to timer struct
 *  @param [in] duration timer relative duration
 *  @param [in] time     the current timer
 *  
 *  @details starts a sw timer relative to a time value.
 */
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

/// defines a 32bit time source function
typedef uint32_t (*timer32_f)(void);
/// defines a 16bit time source function
typedef uint16_t (*timer16_f)(void);

/// further abstracts the timer by including the 32 bit time source function
typedef struct
{
    timer32_f timer;
    swTime32_t timeObj;
} timerObj32_t;

/// further abstracts the timer by including the 16 bit time source function
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

 /** @} */

#endif

/** @file future.h
 *  @brief General purpose software callback system tied to timer2
 */
#ifndef _FUTURE_H_
#define _FUTURE_H_

#include "ecodef.h"
#include "timDev.h"
#include "main.h"


#define TICKS_PER_US (1000/T2_PERIOD_NS)  // should typically be 2
#define LARGEST_FUTURE ((uint32_t)(0xffffffff/TICKS_PER_US))



typedef void (*timerCallback_t)(uintptr_t context);
typedef struct
{
    timerCallback_t callback;  // function to call
    uintptr_t context;            // the context object (probable an object address)
    uint32_t target;            
    uint32_t volatile *hwTarget;
} future_t;


// for background timer callbacks
typedef struct
{
    timerCallback_t callback;  // function to call
    uintptr_t context;         // the context object of the callback
    uint32_t startTime;
    uint32_t duration;
    int32_t repeat;
} bkgCallback_t;




/** @brief have a function called back in the future in timer interrupt
  * @param uSec The number of uSec in the future (time from now)
  * @param context A user specified data item to return to callback in the future
  * @param callback The function to call in the future
  * @return handle to the future if successful, NULL if could not schedule the callback
  */
future_t* FutureCallbackIsr(uint32_t uSec, uintptr_t context, timerCallback_t callback);

/** @brief have a function called back in the future in the background
  * @param uSec The minimum number of uSec in the future, could be more
  * @param context A user specified data item to return to callback in the future
  * @param callback The function to call in the future
  * @return handle to the future if successful, NULL if could not schedule the callback
  */
bkgCallback_t* FutureCallbackBackground(uint32_t uSec, uintptr_t context, timerCallback_t callback);


// Delay existing callback set time from now
bool FutureReset(future_t *fut, uint32_t delay);
// Kill the future callback
bool FutureKill(future_t *fut);

bool DelayCallback(bkgCallback_t *bcb, uint32_t delay);
bool KillCallback(bkgCallback_t *bcb);





// Function called in Startup and Background loop
void FutureBackground(void);
void InitFuture(void);

#endif

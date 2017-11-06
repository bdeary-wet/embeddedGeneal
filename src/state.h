#ifndef _STATE_H
#define _STATE_H

#include <stdint.h>
#include <stddef.h>

typedef struct sm_s sm_t;

typedef enum
{
    
    SM_OK,          // normal return
    SM_ERR,         // something went wrong,
    SM_OFF,         // state machine terminated at null state
    SM_AT_TARGET,   // state machine reached target state
    SM_BAD_PARAMETER,

   
    //------
    SmReturns
} stRet_t;

typedef enum
{
    SMP_RUN,        // normal entry
    SMP_INIT,       // state first entry
    SMP_EXIT,       // state tear down
    SMP_TIMEOUT,    // state reached timeout
    SMP_LIMIT,      // state reached call limit
    //-------- 
    SmPhases
} smPhase_t;


typedef stRet_t (*stateFunc_f)(sm_t *st, smPhase_t phase);
typedef void (*smTrigCb)(sm_t *st);
typedef uint32_t (*stTimeFunc_t)(void);

typedef struct
{
    stateFunc_f trigState;
    smTrigCb callback;
} smTrigger_t;

typedef struct sm_s
{
    stateFunc_f *stateList;
    stateFunc_f current;  // the current state
    stateFunc_f next;     // the next state
    stateFunc_f prev;     // the last state
    smTrigger_t *target; // optional target state trigger
    smTrigger_t *onErr;  // optional error state trigger
    stTimeFunc_t getTime;  
    smTrigCb timeoutCb;
    void *context;  // the associated statefull object
    size_t states;    // number of states in stateList
    uint32_t currentStateEntries; 
    uint32_t stateStartTime; // time state was entered in provided time base  
    uint32_t currentTimeout; // if non-zero check for timeouts    
    uint32_t prevStateDuration;
    uint32_t prevStateEntries;
    uint32_t errors;    // count of calls to processError
    uint8_t doBefore;   // flag to do state switch at start of cycle
    uint8_t timedOut;
} sm_t;

// user provided timebase function
uint32_t SmTimeBase(void);


/**
 *  Basic Form
 *  stRet_t aState(sm_t *st, smPhase_t phase)
 *  {
 *      stRet_t rt = SM_ERR;
 *      switch (phase)
 *      {
 *          case SMP_INIT:  // any state initialization 
 *  
 *          // optional initialization code here
 *  
 *          rt =  SM_OK;
 *          break;
 *   
 *          case SMP_RUN:   // normal state code
 *  
 *          // normal state code here
 *  
 *          rt =  SM_OK;
 *          break;
 *  
 *          case SMP_EXIT:  // any state tear down code
 *  
 *          // optional state tear down code here
 *  
 *          rt = SM_OK;
 *          break;
 *      }
 *      return rt;
 *  }
 *  
 */

 
 
 
 
 /**
 *  @brief Initialize the state machine object 
 *  
 *  @param [in] st pointer to state machine object
 *  @param [in] stateList pointer to array of all the function pointers
 *  @param [in] states number of elements in stateList (all possible states)
 *  @param [in] context void pointer to user's underlying context object
 *  @param [in] onErr optional special error handler. pointer to a smTrigger_t object
 *  @param [in] target optional special target handler. pointer to a smTrigger_t object
 *  @return SM_OR or SM_ERR if initialization reported an error.
 *  
 *  @details Details
 */
stRet_t SmInit(sm_t *st, 
            stateFunc_f *stateList, 
            size_t states,
            void *context,
            smTrigger_t *onErr, 
            smTrigger_t *target);
 
 /**
 *  @brief process the state machine
 *  
 *  @param [in] st pointer to the state machine object
 *  @return SM_OK, SM_ERR, or SM_AT_TARGET
 *  
 *  @details Details
 */
stRet_t SmProcess(sm_t *st);
 
 /**
 *  @brief User convenience helper function allows user to assign state by 
 *         number, probable an associated enumeration.
 *  
 *  @param [in] st the state machine pointer
 *  @param [in] state The application specific state number to set as next
 *  @return SM_OK or SM_BAD_PARAMETER
 *  
 *  @details Since the state machine system does not know about the user 
 *           implementation except for a table of functions. The user may
 *           use this function to set the next state machine function pointer
 *           by table index rather than function pointer. 
 *           It is assumed the user would maintain an associated enumeration
 *           associating a value to a index in the function table.
 *           In general this is not necessary and the function names themselves
 *           form an set of good enumerations albeit instance specific ones.
 *           If no state table was provided, setting to state 0 will always
 *           succeed with setting next to NULL (the off state)
 */
stRet_t SmSetNext(sm_t *st, uint32_t state);
 
stRet_t SmSetTimeout(sm_t *st, uint32_t ticks, smTrigCb cb);
 
 
 
 
#endif // _STATE_H

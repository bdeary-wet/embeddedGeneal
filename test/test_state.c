#include "unity.h"
#include "state.h"
#include <stdio.h>

#define DIM(arr) (sizeof(arr)/sizeof(arr[0]))

uint32_t SmTimeBase(void)
{
    static uint32_t time;
    return ++time;
}

uint32_t stateNo = 0;

static stRet_t stateGen(sm_t *st, smPhase_t phase)
{
    stRet_t rt = SM_ERR;
    uint32_t *stateNo = (uint32_t*)st->context;

    switch (phase)
    {
        case SMP_INIT:  // any state initialization 

        printf("Init for state %i\n", *stateNo);

        rt =  SM_OK;
        break;
 
        case SMP_RUN:   // normal state code

        rt = SmSetNext(st, ++(*stateNo) );

        break;

        case SMP_EXIT:  // any state tear down code

        printf("Exit before state %i\n", *stateNo);

        rt = SM_OK;
        break;
    }
    return rt;
}

static stRet_t state0(sm_t *st, smPhase_t phase)
{
    return stateGen(st,phase);
}
static stRet_t state1(sm_t *st, smPhase_t phase)
{
    return stateGen(st,phase);
}
static stRet_t state2(sm_t *st, smPhase_t phase)
{
    return stateGen(st,phase);
}
static stRet_t state3(sm_t *st, smPhase_t phase)
{
    return stateGen(st,phase);
}
static stRet_t state4(sm_t *st, smPhase_t phase)
{
    return stateGen(st,phase);
}
static stRet_t state5(sm_t *st, smPhase_t phase)
{
    return stateGen(st,phase);
}


stateFunc_f table1[] = 
{
    state0,
    state1,
    state2,
    state3,
    state4,
    NULL
};

uint32_t collatzVal;

static stRet_t odd(sm_t *st, smPhase_t phase)
{
    uint32_t *val = (uint32_t*)st->context;
    if (phase == SMP_RUN)
    {
        if (*val == 1) return SmSetNext(st, 2);
        *val = *val*3 + 1;
        return SmSetNext(st, (*val & 1));
    }
    return 0;
}

static stRet_t even(sm_t *st, smPhase_t phase)
{
    uint32_t *val = (uint32_t*)st->context;
    if (phase == SMP_RUN)
    {
        *val >>= 1;
        return SmSetNext(st, (*val & 1));
    }
    return 0;
}


stateFunc_f collatzTable[] = 
{
    even,
    odd,
    NULL
};




void setUp(void)
{
}

void tearDown(void)
{
}

void test_state_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement state");
}

// basic functionality 5 states in sequence
void test_state_0(void)
{
    sm_t st;
    
    TEST_ASSERT_EQUAL(SM_OK, 
                      SmInit(&st, table1, DIM(table1), &stateNo, NULL, NULL));
    TEST_ASSERT_EQUAL(SM_OK, SmProcess(&st));
    TEST_ASSERT_EQUAL(1, stateNo);
    TEST_ASSERT_EQUAL(SM_OK, SmProcess(&st));
    TEST_ASSERT_EQUAL(2, stateNo);
    TEST_ASSERT_EQUAL(SM_OK, SmProcess(&st));
    TEST_ASSERT_EQUAL(3, stateNo);
    TEST_ASSERT_EQUAL(SM_OK, SmProcess(&st));
    TEST_ASSERT_EQUAL(4, stateNo);
    TEST_ASSERT_EQUAL(SM_OFF, SmProcess(&st));
    TEST_ASSERT_EQUAL(5, stateNo);    

}

void test_state_collatz(void)
{
    sm_t st;
    collatzVal = 127 * 2;
    uint32_t cnt;
    uint32_t max = 0;
    
    TEST_ASSERT_EQUAL(SM_OK, 
                  SmInit(&st, collatzTable, DIM(collatzTable), &collatzVal, NULL, NULL));
                      
    st.doBefore = 1;
    stRet_t rt;
    for (int i=2; i<1200; i++)
    {
        collatzVal = i;
        SmSetNext(&st, i & 1);
        cnt = 0;
        while (!(rt = SmProcess(&st))) 
        {
            cnt++;
   //         printf("%i,",collatzVal);
        }
        TEST_ASSERT_EQUAL(SM_OFF, rt);
        cnt --;
        if (cnt > max)
        {
            max = cnt;
            printf("%i->%i\n",i,cnt);
        }
    }
}


static stRet_t stateTimed(sm_t *st, smPhase_t phase);
static stRet_t stateTimedErr(sm_t *st, smPhase_t phase);

int timedCallbackCalled = 0;
static void timedCallback(sm_t *st)
{
    TEST_ASSERT_NOT_EQUAL(0,st->timedOut);
    timedCallbackCalled++;
    
}

static stRet_t stateTimed(sm_t *st, smPhase_t phase)
{
    stRet_t rt = SM_ERR;
    uint32_t *stateNo = (uint32_t*)st->context;

    switch (phase)
    {
        case SMP_INIT:  // any state initialization 
        rt = SmSetTimeout(st, 5, timedCallback);
        printf("Init stateTimed, to = 5");


        break;
 
        case SMP_RUN:   // normal state code

        TEST_ASSERT_EQUAL(0, st->timedOut);
        rt =  SM_OK;

        break;
        
        case SMP_TIMEOUT:
        
        TEST_ASSERT_NOT_EQUAL(0, st->timedOut);
        st->next = stateTimedErr;
        rt =  SM_OK;
        
        break;

        case SMP_EXIT:  // any state tear down code

        printf("Exit from stateTimed");

        rt = SM_OK;
        break;
    }
    return rt;
}

static stRet_t stateTimedErr(sm_t *st, smPhase_t phase)
{
    stRet_t rt = SM_ERR;
    uint32_t *stateNo = (uint32_t*)st->context;

    switch (phase)
    {
        case SMP_INIT:  // any state initialization 
        rt = SmSetTimeout(st, 3, timedCallback);
        printf("Init stateTimedErr, to = 3");

        break;
 
        case SMP_RUN:   // normal state code

        TEST_ASSERT_EQUAL(0, st->timedOut);
        rt =  SM_OK;

        break;
        
        case SMP_TIMEOUT:
        
        TEST_ASSERT_NOT_EQUAL(0, st->timedOut);
        st->next = stateTimed; // try to go back but
        //rt =  SM_OK;         // should exit with SM_ERR and go to state 0
        
        break;

        case SMP_EXIT:  // any state tear down code

        printf("Exit from stateTimedErr");

        rt = SM_OK;
        break;
    }
    return rt;
}


stateFunc_f timeoutTable[]=
{
    NULL,
    stateTimed,
    stateTimedErr
};




void test_timeout(void)
{
    sm_t st;
    
    TEST_ASSERT_EQUAL(SM_OFF, 
                      SmInit(&st, timeoutTable, DIM(timeoutTable), &stateNo, NULL, NULL));
    st.next = stateTimed;
    TEST_ASSERT_EQUAL(SM_OK, SmProcess(&st));
    TEST_ASSERT_EQUAL(SM_OK, SmProcess(&st));
    TEST_ASSERT_EQUAL(SM_OK, SmProcess(&st));
    TEST_ASSERT_EQUAL(SM_OK, SmProcess(&st));    
    TEST_ASSERT_EQUAL(SM_OK, SmProcess(&st)); 
    TEST_ASSERT_EQUAL(stateTimed, st.current);    
    TEST_ASSERT_EQUAL(SM_OK, SmProcess(&st));    
    TEST_ASSERT_EQUAL(stateTimedErr, st.current);
    TEST_ASSERT_EQUAL(1, timedCallbackCalled );    
    TEST_ASSERT_EQUAL(0,st.errors);    
    TEST_ASSERT_EQUAL(SM_OK, SmProcess(&st));    
    TEST_ASSERT_EQUAL(SM_OK, SmProcess(&st));  
    TEST_ASSERT_EQUAL(SM_OFF, SmProcess(&st)); // handled error move to state 0
    TEST_ASSERT_EQUAL(2, timedCallbackCalled );
    TEST_ASSERT_EQUAL(1,st.errors);
    TEST_ASSERT_EQUAL(NULL, st.current);
}

int errorStateEntered = 0;
stRet_t errorState(sm_t *st, smPhase_t phase)
{
    stRet_t rt = SM_OK;
    if (phase == SMP_RUN)
    {
        errorStateEntered++;
        st->next = NULL;    // should stop the SM
    }
    return rt;
}

int targetStateEntered =0;
stRet_t targetState(sm_t *st, smPhase_t phase)
{
    stRet_t rt = SM_OK;
    if (phase == SMP_RUN)
    {
        targetStateEntered++;
        st->next = state4;    
        rt = SM_ERR;
    }

    return rt;
}



int errorCbCalled = 0;
void errCb(sm_t *st)
{
    errorCbCalled++;
}

int targetCbCalled = 0;
void targetCb(sm_t *st)
{
    targetCbCalled++;
}



smTrigger_t targetTrigger = {targetState, targetCb };
smTrigger_t errorTrigger = {errorState, errCb };

stateFunc_f cbTable1[] = 
{
    state0,
    state1,
    state2,
    targetState,
    state4,
};



void test_callbacks(void)
{
    sm_t st;
    stateNo = 0;
    TEST_ASSERT_EQUAL(SM_OK, 
                      SmInit(&st, 
                      cbTable1, 
                      DIM(cbTable1), 
                      &stateNo, 
                      &errorTrigger, 
                      &targetTrigger)
                      );  

    TEST_ASSERT_EQUAL(SM_OK, SmProcess(&st)); // 0->1
    TEST_ASSERT_EQUAL(SM_OK, SmProcess(&st)); // 1->2
    TEST_ASSERT_EQUAL(SM_AT_TARGET, SmProcess(&st)); // 2->3 
    TEST_ASSERT_EQUAL(0, st.errors);
    TEST_ASSERT_EQUAL(targetState, st.current);
    TEST_ASSERT_EQUAL(SM_OK, SmProcess(&st)); 
    TEST_ASSERT_EQUAL(1, st.errors);
    TEST_ASSERT_EQUAL(SM_OFF, SmProcess(&st)); 
    TEST_ASSERT_EQUAL(1, targetCbCalled);
    TEST_ASSERT_EQUAL(1, errorCbCalled);
    TEST_ASSERT_EQUAL(1, targetStateEntered);
    TEST_ASSERT_EQUAL(1, errorStateEntered);    
    
}

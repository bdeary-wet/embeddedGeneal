#include "unity.h"
#include "swtimers.h"
#include "generalQueue.h"
#include "mock_taskService.h"



void SWT_MicroSetup(future_t *fut, int chans){}
void SWT_start(future_t *fut){}
void SWT_stop(future_t *fut){}
void SWT_recycle(future_t *fut){}
int SWT_delay(future_t *fut, fastTimer_t newTime){return 0;}

uint64_t uSec;
uint64_t SWT_GetTimeUsecLL(void){return uSec++;}
uint32_t SWT_GetTimeUsec(void){return (uint32_t)uSec;}







fastTimer_t getFastCounter(void)
{
    static fastTimer_t count;
    return count++;
}

int pendSet;

void setUp(void)
{
}

void tearDown(void)
{
}

void test_swtimers_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement swtimers");
}

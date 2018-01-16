#include "swtimers.h"
#include "swtimer.h"

typedef struct swtH_s 
{
    swTime32_t timer;
 
} swtHandle_t;


void SWT_InitSysTimers(void *space, size_t spaceSize);

size_t SWT_sizeofTimer(void);

swtHandle_t *SWT_SysTimerCallback(
            uint32_t timeInMicroSeconds, 
            timerCallback cb, intptr_t context,
            uint32_t runCount)
                    {return NULL;}           
            
swtHandle_t *SWT_SysTimerCount(
                    uint32_t timeInMicroSeconds,
                    tcHandle_t task, 
                    uint32_t runCount)
                    {return NULL;}

swtHandle_t *SWT_FastTimerCallback(
                    uint32_t timeInNs, 
                    timerCallback cb, intptr_t context,
                    uint32_t runCount)
                    {return NULL;}                    
                    
swtHandle_t *SWT_FastTimerCount(
                    uint32_t timeInNs, 
                    tcHandle_t taskNo,
                    uint32_t runCount)
                    {return NULL;}
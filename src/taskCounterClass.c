#include "taskCounterClass.h"
#include <string.h>
#include "generalDef.h"



tcHandle_t TCC_AssociateHandle(tcc_t *tcc, tcCounter_t volatile * myCounter,
                               intptr_t context)
{
    tcc->counter = myCounter;
    tcc->context = context;
    return (tcHandle_t)tcc;
}

intptr_t TCC_SignalUp(tcHandle_t task)
{
    if (task)
    {
        // do saturated inc
    	USAT_INC(*((tcc_t*)task)->counter);
    	return ((tcc_t*)task)->context;
    }
    return 0;
}

intptr_t TCC_SignalDown(tcHandle_t task)
{
	if (task)
	{
		// do saturated dec
		USAT_DEC(*((tcc_t*)task)->counter);
		return ((tcc_t*)task)->context;
	}
    return 0;
}


tcCounter_t TCC_Test(tcHandle_t task) 
{
    if (task)
    {
        return *(((tcc_t*)task)->counter);
    }
    return 0;
}

tcCounter_t TCC_TestAndClear(tcHandle_t task)
{
    if (task)
    {
        tcCounter_t temp = *((tcc_t*)task)->counter;
        *((tcc_t*)task)->counter = 0;
        return temp;
    }
    return 0;
}

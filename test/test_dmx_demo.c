#include "unity.h"

#include "dmx_demo.h"
#include "micro_p_sim.h"
#include <pthread.h>
#include <time.h>
#include <genQ.h>
#include <genPool.h>
#include <isr_comm.h>
#include <rs485_dmx.h>

pthread_t sim_main_thread;

void stop(void)
{
    dd_model.model_base.sim_enabled = 0;
    pthread_join(sim_main_thread, NULL);
}

void setUp(void)
{

    dd_model.ms_tick=0;
    dd_model.model_base.main_tick=0;
    dd_model.model_base.tick=0;
    dd_model.model_base.in_isr=1;
    dd_model.model_base.sim_enabled = 1;
    for(int i=0; i<TOTAL_ISRs; i++) 
    {
        dd_model.model_base.isr_flags[i]=0;
    }

    TEST_ASSERT_FALSE_MESSAGE(
        pthread_create(&sim_main_thread, NULL, Micro_p_sim_main, NULL) ,
        "sim_main did not start"
    );
    
    while(!dd_model.model_base.main_tick)
        sched_yield(); // wait to get started and setup
}

void tearDown(void)
{
    dd_model.model_base.sim_enabled = 0;
    pthread_join(sim_main_thread, NULL);
}

void test_wait_100_ms(void)
{
    while(dd_model.ms_tick < 100) sched_yield();
    printf("100ms=%u, isr %u, main %u\n",
    dd_model.ms_tick,
    dd_model.model_base.tick,
    dd_model.model_base.main_tick );
}

void off_test_nothing(void)
{
//    while(dd_model.model_base.sim_enabled){}
}
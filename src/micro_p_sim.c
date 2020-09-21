/**
 * @file micro_p_sim.c
 * @author bdeary (bdeary@wetdesign.com)
 * @brief Function to simulate the setup, run_loop and interrupts
 *   of a micro_processor
 * @date 2020-09-16
 * 
 * Copyright 2020, WetDesigns
 * 
 */
#include <config.h>
#include "micro_p_sim.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

pthread_t isr_stimulus_thread;

void *_isr_stimulus(void *arg)
{
    ModelBase_t *model =  arg;
    while(model->sim_enabled)
    {
        if(model->isr_stimulus)
        {
            model->isr_stimulus(model);
            model->tick++;
        }
        else 
        {
            model->tick++;
        }
        // do pretend isrs
        {
            model->in_isr = 1;  // lock out background in multithread (need to fix with condition vars)
            int isr_run = 1;
            while(isr_run)
            {
                isr_run = 0;
                for(int i=0; i < model->isr_table_size; i++)
                {
                    if(model->isr_flags[i])
                    {
                        if(model->isr_table[i])
                        {
                            if(model->isr_auto_clear) model->isr_flags[i]=0;
                            model->isr_table[i]();
                            isr_run = 1;
                            if(model->isr_are_prioritized) break;
                        }
                    }
                }
            }
            model->in_isr = 0; // release background
        }
        sched_yield(); // spread out the execution
    }
    return 0;
}

// invoke functions once at the start. Use to put system into a known
// initialized state, may be used by unit tests in setup to reload 
// variables and structures
ModelBase_t *sim_init(void)
{
    ModelBase_t *model = Micro_p_sim_init();
    model->sim_enabled = 1;
    model->tick = 0;
    return model;
}

// invoke functions that may do dynamic startup like build linked lists or
// open and read files. More dynamic than init but may rely on the state 
// created by init.  Also where we start our isr stimulation thread.
ModelBase_t *sim_start(ModelBase_t *model)
{
    if(model->setup)
    {
        model = model->setup(model);
    }
    return model;
}

// the program main and loop, runs init, startup, then loops forever 
// on the enable flag
void *Micro_p_sim_main(void* arg)
{
    // Do one time functions
    ModelBase_t *model = sim_init();
    model = sim_start(model);
    assert(model);
    if (model)
    {
        model->in_isr = 1; // hold off main below from running until isr runs
        int status = pthread_create(&isr_stimulus_thread, 
                                    NULL, 
                                    _isr_stimulus, 
                                    model);
        assert(status == 0);
        model->main_tick = 0;
    }
    
    sched_yield();

    uint32_t wd = WD_RESET;
    uint32_t last_tick = model->tick;

    // loop until killed
    while(model && model->sim_enabled)
    {
        // This loop runs at speed, but isr sim should be running also
        // do a watchdog to make sure isr is also running
        if(last_tick == model->tick) // make sure clock is running
        {
            assert(--wd);
        }
        else
        {
            last_tick = model->tick;
            wd = WD_RESET;
        }

        if(!model->in_isr)  // if isr prevent background, need to do this with mutex
        {
            model->main_tick++;
            // actual background process
            if(model && model->main_loop) 
            {
                model = model->main_loop(model);
            }
        }
        // optional user diagnostic to check state
        if(model && model->diagnostics) 
        {
            model = model->diagnostics(model);
        }
        sched_yield();
    }
    pthread_join(isr_stimulus_thread, NULL);
    return NULL;
}

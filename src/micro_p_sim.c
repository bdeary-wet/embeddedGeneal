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
            assert(!pthread_mutex_lock(&(model->isr_mutex)));
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
            assert(!pthread_mutex_unlock(&(model->isr_mutex)));
        }
        sched_yield(); // spread out the execution
    }
    model->in_isr = 0;
    return 0;
}

// invoke functions once at the start. Use to put system into a known
// initialized state, may be used by unit tests in setup to reload 
// variables and structures
ModelBase_t *sim_init(void)
{
    ModelBase_t *model = Micro_p_sim_init(); // user defined init
    //model->sim_enabled = 1;
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
    ModelBase_t *model = sim_init();  // This gets the model memory from the user
    pthread_mutex_init(&(model->isr_mutex), NULL);  // init our mutexes
    pthread_mutex_init(&(model->sim_mutex), NULL);
    model = sim_start(model); 
    assert(model);  // can't be NULL

    // init our internal main loop watchdog to catch stuck threads
    uint32_t wd = WD_RESET;
    uint32_t last_tick;

    if (model) // if not NULL start sim interrupt system
    {
        model->in_isr = 1; // hold off main below from running until isr runs
        int status = pthread_create(&isr_stimulus_thread, 
                                    NULL, 
                                    _isr_stimulus, 
                                    model);
        assert(status == 0);
        model->main_tick = 0;  // clear the main loop counter
        last_tick = model->tick; // part of WD system
    }
    
    sched_yield();  // increase probability interrupt system will run

    // loop until killed
    while(model && model->sim_enabled)
    {
        // This loop runs at speed, but isr sim should be running also
        // do a watchdog to make sure isr is also running
        if(last_tick == model->tick) // make sure clock is running
        {
            --wd;
            assert(wd); 
            if(!wd) 
            {
                model->sim_enabled = 0;
                pthread_cancel(isr_stimulus_thread);
                return NULL;
            }
        }
        else
        {
            last_tick = model->tick;
            wd = WD_RESET;
        }

        if(!model->in_isr)  // There is also a mutex
        {
            MICRO_P_INT_DISABLE(model); // just here to catch a race
            model->main_tick++;
            MICRO_P_INT_ENABLE(model);
            // actual background process
            if(model && model->main_loop) 
            {
                model = model->main_loop(model);
            }
        }

        // optional user diagnostic to check state, 
        // it can run regardless of isr state
        if(model && model->diagnostics) 
        {
            model = model->diagnostics(model);
        }
        // give isr system a turn
        sched_yield();
    }
    pthread_join(isr_stimulus_thread, NULL);
    return NULL;
}

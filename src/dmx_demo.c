/**
 * @file dmx_demo.c
 * @author bdeary (bdeary@wetdesign.com)
 * @brief A Demonstration of a DMX512 system for the purpose of end-to-end 
 *   testing of a microprocess based system independent of micro-p
 * @date 2020-09-20
 * 
 * Copyright 2020, WetDesigns
 * 
 * @details This file contains the setup and main loop of the system. 
 *   It sits on top of the micro_p_sim structure that simulates a 
 *   micro processor with some current issues related to isr exclusion.
 * 
 *   In general it creates a framework to test a DMX receiver and 
 *   associated device processing (processing the dmx slots into one or more
 *   device response or tranlation to a second communication channel)
 *   
 *   the dmx_demo.h derives its DdModel_t from the ModelBase_t defined in
 *   micro_p_sim that has the isr_vector table pointer and functions for 
 *   setup and loop (similar to other micro P frameworks like Arduino)
 * 
 * 
 * 
 */

#include <config.h>
#include "dmx_demo.h"
#include <pthread.h>
#include <time.h>
#include <genQ.h>

// This is the instantiation of the isr vector table and
// the isr flags
void_func_t isr_table[TOTAL_ISRs];
uint8_t isr_flags[TOTAL_ISRs];

// a signal that an isr triggered
DefineGenQ(isrQ_, uint8_t, 20);
DefineGenQWrappers(isrQ_, uint8_t);

// this is the instantiation of the user derived class from the ModelBase_t class
// compile time setup, only oppertunity to assign const variables in our model
DdModel_t dd_model = (DdModel_t)
{
    // derived model variables
    .dmx512_receiver.first_device=&dd_model.dev1,  // these are immutable
    .dmx512_receiver.slot0=0,                      // these are immutable
    .dev1.next_dev=&dd_model.dev2,                 
    .dev2.next_dev=NULL,
    
    // provide the isr vector table
    .model_base.isr_table_size=TOTAL_ISRs,
    .model_base.isr_table=isr_table,
    .model_base.isr_flags=isr_flags,
    // this sets how ISRs are processed in the sim
    .model_base.in_isr=1,   // This blocks user main until Isr runs at least once
    .model_base.isr_are_prioritized=0, // changes how ISRs trigger other ISRs
    .model_base.isr_auto_clear=1,  // frees isr handlers from clearing their flags
};

// the user ms tick isr handler, called every ms in user time
void ms_isr(void)
{
    dd_model.ms_tick++;
}

/////// The following four functions define the users model //////
ModelBase_t *DD_setup(ModelBase_t *model) // the user setup code
{
    DdModel_t *self = (DdModel_t*)model;
    model->isr_table[MS_ISR] = ms_isr;
    self->ms_tick = 0;
}


#define CLOCKS_PER_MS (CLOCKS_PER_SEC/1000)
ModelBase_t *DD_isr_stimulus(ModelBase_t *model) 
{
    DdModel_t *self = (DdModel_t*)model;

    // Do any user stimulus generation or get external commands
    static clock_t last=0;
    if(last)
    {
        if (clock() - last > CLOCKS_PER_MS)
        {
            last += CLOCKS_PER_MS;
            model->isr_flags[MS_ISR] = 1;
        }
    }
    else
    {
        last = clock();
    }
    
    return model;
}

// the user background loop
ModelBase_t *DD_main(ModelBase_t *model)
{
    DdModel_t *self = (DdModel_t*)model;
    // do background processing here.



    // if model is returned as NULL, should start abort() processes
    return model;
}

// This is background level diagnostic, runs after each user DD_main call
ModelBase_t *DD_diag(ModelBase_t *model)
{
    DdModel_t *self = (DdModel_t*)model;

    // also has ability to invoke abort() by returning NULL.
    return model;
}


// This is the user provide function to setup the model with the
// above functions and any initializations or invariants to the user data model.
// runs once before DD_setup above and injects the user data model into the
// micro_p_sim.
ModelBase_t *Micro_p_sim_init(void)
{
    // set all our simulation invariants at run time, vs the compile time
    // setup above.
    dd_model.model_base.setup=DD_setup;
    dd_model.model_base.main_loop=DD_main;
    dd_model.model_base.diagnostics=DD_diag;
    dd_model.model_base.isr_stimulus=DD_isr_stimulus;
    dd_model.model_base.sim_enabled=1;
    dd_model.model_base.tick=0;
    dd_model.model_base.main_tick=0;

// any other user init goes here

// dev 1 setup
    dd_model.dev1.next_dev=&dd_model.dev2;
    dd_model.dev1.first_address=8;
    dd_model.dev1.slots=8 ;
// dev 2 setup
    dd_model.dev2.next_dev=NULL;
    dd_model.dev2.first_address=20;
    dd_model.dev2.slots=4;

    // return pointer to base class object
    return (ModelBase_t*)&dd_model;
}


#ifndef TEST

int main(int argc, char const *argv[])
{
    Micro_p_sim_main(&dd_model);
    return 0;
}

#endif

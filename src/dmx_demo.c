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
 *   It sits on top of the micro_p_sim structure that can then be 
 * 
 */

#include <config.h>
#include "dmx_demo.h"
#include <pthread.h>
#include <time.h>
#include <genQ.h>


void_func_t isr_table[TOTAL_ISRs];
uint8_t isr_flags[TOTAL_ISRs];

// a signal that an isr triggered
DefineGenQ(isrQ_, uint8_t, 20);
DefineGenQWrappers(isrQ_, uint8_t);

// this is the instantiation of the user derived class from the ModelBase_t class
DdModel_t dd_model = (DdModel_t)
{
    .dmx512_receiver.first_device=&dd_model.dev1,
    .dmx512_receiver.slot0=0,
    .dev1.next_dev=&dd_model.dev2,
    .dev2.next_dev=NULL,
    .model_base.isr_table_size=TOTAL_ISRs,
    .model_base.isr_table=isr_table,
    .model_base.isr_flags=isr_flags,
    .model_base.in_isr=1,
    .model_base.isr_are_prioritized=0,
    .model_base.isr_auto_clear=1,

};




void ms_isr(void)
{
    dd_model.ms_tick++;
}

/////// The following four functions define the users model //////
ModelBase_t *DD_setup(ModelBase_t *model)
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

ModelBase_t *DD_main(ModelBase_t *model)
{
    DdModel_t *self = (DdModel_t*)model;




    return model;
}

ModelBase_t *DD_diag(ModelBase_t *model)
{
    DdModel_t *self = (DdModel_t*)model;


    return model;
}


// This is the user provide function to setup the model with the
// above functions and any initializations or invariants to the user data model.
ModelBase_t *Micro_p_sim_init(void)
{
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

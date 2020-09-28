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
#include <genPool.h>
#include <isr_comm.h>
#include <rs485_dmx.h>
#include <stdio.h>


// This is the instantiation of the isr vector table and
// the isr flags
void_func_t isr_table[TOTAL_ISRs];
uint8_t isr_flags[TOTAL_ISRs];

// a signal that an isr triggered
DefineGenQ(isrQ_, uint8_t, 20);
DefineGenQWrappers(isrQ_, uint8_t);

DefineDoubleIsrObjectQueue(dev1_ipt, dev1_buf_t, 3, NULL);
DefineDoubleIsrObjectQueue(dev2_ipt, dev2_buf_t, 3, NULL);

// this is the instantiation of the user derived class from the ModelBase_t class
// compile time setup, only oppertunity to assign const variables in our model
DdModel_t dd_model = (DdModel_t)
{
    // derived model variables
    .dmx512_receiver.first_device=&dd_model.dev1,  // these are immutable
    .dmx512_receiver.slot0=0,                      // these are immutable
    .dev1.next_dev=&dd_model.dev2,                 
    .dev2.next_dev=NULL,
    .dev1.ipt = &dev1_ipt_instance,
    .dev2.ipt = &dev2_ipt_instance,
    
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

#define BYTES_PER_SIM 14

#define SIM_TIME (DMX_BYTES_PER_SEC / BYTES_PER_SIM)

uint8_t dmx_sim_buffer[BYTES_PER_SIM];
size_t dmx_sim_len;
int sim_slot = -1;

void dmx1_isr_b_warpper(void)
{
    DMX_process_break(&dd_model.dmx512_receiver);
    dd_model.frame++;
}

void dmx1_isr_s_warpper(void)
{
    DMX_use_raw_data(&dd_model.dmx512_receiver, dmx_sim_buffer, dmx_sim_len);
}


/////// The following four functions define the users model //////
ModelBase_t *DD_setup(ModelBase_t *model) // the user setup code
{
    DdModel_t *self = (DdModel_t*)model;
    model->isr_table[MS_ISR] = ms_isr;
    self->ms_tick = 0;

    model->isr_table[DMX_BREAK] = dmx1_isr_b_warpper;
    model->isr_table[DMX_SERIAL] = dmx1_isr_s_warpper;
    DMX_receiver_reset(&dd_model.dmx512_receiver);

    Status_t status;
    void *working_buffer;
    while(NULL != (working_buffer = Allocate_dev1_ipt_Object()))
    {
        status = Send_dev1_ipt_Object(working_buffer);
    }

    while(NULL != (working_buffer = Allocate_dev2_ipt_Object()))
    {
        status = Send_dev2_ipt_Object(working_buffer);
    }

    return model;
}


void Dev1_background(ModelBase_t *model)
{
    DdModel_t *self = (DdModel_t*)model;
    Status_t status;
    dev1_buf_t *workspace = Allocate_dev1_ipt_Object();
    if(workspace) status = Send_dev1_ipt_Object(workspace);
    status = Receive_dev1_ipt_Object(&workspace);
    if(status == Status_OK) // if Ok then data was received
    {
        // process the data
        // process the data
        printf("Dev1 = 0x");
        for(int i=0; i<SARR_LEN(workspace->buf);i++)
        {
            printf("%02x",workspace->buf[i]);
        }
        printf("--> %u\n",self->ms_tick);

        // return the workspace to driver
        status = Send_dev1_ipt_Object(workspace);
    }
}

void Dev2_background(ModelBase_t *model)
{
    Status_t status;
    DdModel_t *self = (DdModel_t*)model;
    dev2_buf_t *workspace = Allocate_dev2_ipt_Object();
    if(workspace) status = Send_dev2_ipt_Object(workspace);
    status = Receive_dev2_ipt_Object(&workspace);
    if(status == Status_OK) // if Ok then data was received
    {
        // process the data
        printf("Dev2 = 0x");
        for(int i=0; i<SARR_LEN(workspace->buf);i++)
        {
            printf("%02x",workspace->buf[i]);
        }
        printf("--> %u\n",self->ms_tick);

        // return the workspace to driver
        status = Send_dev2_ipt_Object(workspace);
    }
}

#if 0
#define CLOCKS_PER_MS (CLOCKS_PER_SEC/1000)
#define CLOCKS_PER_SIM (CLOCKS_PER_SEC / SIM_TIME)
#else
#define CLOCKS_PER_MS 100
#define CLOCKS_PER_SIM 45
#endif
ModelBase_t *DD_isr_stimulus(ModelBase_t *model) 
{
    DdModel_t *self = (DdModel_t*)model;
    clock_t clk = clock();

    // Do any user stimulus generation or get external commands

    // Here simulate the 1 ms tick interrupt
    static clock_t last=0;
    if(last)
    {
        if (clk - last > CLOCKS_PER_MS)
        {
            last += CLOCKS_PER_MS;
            model->isr_flags[MS_ISR] = 1;  // trigger the ISR
        }
    }
    else
    {
        last = clock();
    }

    static clock_t dmx_last=0;
    if(dmx_last)
    {
        if (clk - dmx_last > CLOCKS_PER_SIM)
        {
            dmx_last += CLOCKS_PER_SIM;

            if(sim_slot < 0)
            {
                model->isr_flags[DMX_BREAK] = 1;  // trigger a break
                sim_slot = 0;
            }
            else
            {
                static int sim_shift;
                for(int i=0; i < SARR_LEN(dmx_sim_buffer); i++, sim_slot++)
                {
                    if(sim_slot == 0)
                    {
                        dmx_sim_buffer[0] = 0; 
                    }
                    else
                    {
                        dmx_sim_buffer[i] = sim_slot + sim_shift;
                    }
            
                }
                dmx_sim_len = SARR_LEN(dmx_sim_buffer);
                if(sim_slot > 500) 
                {
                    sim_slot = -1;
                    sim_shift = (sim_shift + 1) % 5;
                }
                model->isr_flags[DMX_SERIAL] = 1; 
            }
        }
    }
    else
    {
        dmx_last = clock();
    }
    
     
    return model;
}

// the user background loop
ModelBase_t *DD_main(ModelBase_t *model)
{
    DdModel_t *self = (DdModel_t*)model;
    // do background processing here.

    Dev1_background(model);
    Dev2_background(model);

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
    dd_model.model_base.in_isr = 1;
    dd_model.model_base.tick=0;
    dd_model.model_base.main_tick=0;

// any other user init goes here

// dev 1 setup
    dd_model.dev1.next_dev=&dd_model.dev2;
    dd_model.dev1.first_slot=DEV1_FIRST;
    dd_model.dev1.slots=DEV1_SLOTS;
    dd_model.dev1.ipt=dev1_ipt;
// dev 2 setup
    dd_model.dev2.next_dev=NULL;
    dd_model.dev2.first_slot=DEV2_FIRST;
    dd_model.dev2.slots=DEV2_SLOTS;
    dd_model.dev2.ipt=dev2_ipt;

    dd_model.dmx512_receiver.slot_cnt=0;


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

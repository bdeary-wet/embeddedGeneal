/**
 * @file micro_p_sim.h
 * @author bdeary (bdeary@wetdesign.com)
 * @brief Function to simulate the setup, run_loop and interrupts
 *   of a micro_processor
 * @date 2020-09-16
 * 
 * Copyright 2020, WetDesigns
 * 
 */
#ifndef MICRO_P_SIM_H
#define MICRO_P_SIM_H

#define WD_RESET (0xffffffff)

struct ModelBase_t; // forward ref

/**
 * @brief User provided function to setup the model and set invariants
 * 
 * @return struct ModelBase_t* returns the model structure which is passed
 *      to all other threads and modules as global memory space
 *      derived from ModelBase_t.
 */
extern struct ModelBase_t *Micro_p_sim_init(void);

/**
 * @brief Main method of micro_p_sim.  Run this directly or in a thread
 * 
 * @param pointer to the ModelBase_t derived memory area.
 * @return void* returns NULL
 */
void *Micro_p_sim_main(void* model);

// prototype for other user provided functions below.
typedef struct ModelBase_t *(*model_fun_t)(struct ModelBase_t *model);

typedef void (*void_func_t)(void);

// Base class for the sim model shared memory object.
typedef struct ModelBase_t
{
    // base class immutables 
    void_func_t *isr_table;     // not const but could be
    uint8_t *isr_flags;         // not const but could be
    uint8_t isr_table_size;     // not const but could be
    uint8_t isr_are_prioritized;// not const but could be
    uint8_t isr_auto_clear;     // not const but could be
    uint8_t config_spare;       // filler for potential const

    // base class vars
    uint32_t tick;              // sim tick by isr_stimulus
    uint32_t main_tick;         // sim tick by background
    uint8_t sim_enabled;        // master run flag
    uint8_t in_isr;             // future condition var or mutex
    
    // the 4 user provided simulator functions ()
    model_fun_t setup;          // more traditional setup function
    model_fun_t isr_stimulus;   // for sim of stimulus, starts after setup
    model_fun_t main_loop;      // user provided main background loop
    model_fun_t diagnostics;    // optional, runs after each loop for testing
} ModelBase_t;

#endif // MICRO_P_SIM_H

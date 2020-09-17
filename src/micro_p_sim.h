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

struct ModelBase_t;

struct ModelBase_t *sim_init(void);
struct ModelBase_t *sim_start(struct ModelBase_t*);
void *sim_main(void* arg);

extern struct ModelBase_t *model_init(void);
extern struct ModelBase_t *model_start(struct ModelBase_t *);

typedef struct ModelBase_t *(*model_fun_t)(struct ModelBase_t *model);

typedef struct ModelBase_t
{
    int sim_enabled;            // master run flag
    uint32_t tick;              // sim tick by isr
    uint32_t main_tick;         // sim tick by background

    model_fun_t isr_stimulus;   // user provided isr_stimulator task
    model_fun_t main;           // user provided main function
    model_fun_t diagnostics;    // user provided diagnostic function
} ModelBase_t;

#endif // MICRO_P_SIM_H

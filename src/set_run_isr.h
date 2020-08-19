/**
 * @file set_run_isr.h
 * @author bdeary (bdeary@wetdesign.com)
 * @brief header for a simple embedded tasker
 * @version 0.1
 * @date 2020-08-18
 * 
 * @details
 * Idea is traditional setup section and run section where the run section is a
 * one or more queues that dequeue a task object and run the contained function.
 * 
 * 
 */
#ifndef SET_RUN_ISR_H
#define SET_RUN_ISR_H

#include <config.h>
#include <genQ.h>

#define FUTURE_RUNNERS 20

extern bool system_run;
extern uint32_t time_tick;

Status_t Run_Later(CbInstance_t cb, uint32_t future);
Status_t Run_Periodically(CbInstance_t cb, int32_t interval);
void User_Setup(void);
void User_Loop(void);
void ProcessUser(void);

#endif // SET_RUN_ISR_H

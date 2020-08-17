/**
 * @file config.h
 * @brief This is a project config file and is different than normal module
 *        config files. It should normally be place at the top of any 
 *        include lists in a translation unit, before the module include.
 *        It contains defines and other compile time switches and helper
 *        macros, including those used in test.

 * @date 2020-08-16
 * 
 * Copyright 2020, WetDesigns
 * 
 */
#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <stdbool.h>

// With unit test we want to expose static symbols
// so remove STATIC. Will potentially have name collisions
// for very large tests incluing many modules
#ifdef TEST
#define STATIC
#endif
// STATIC, if not reassigned, should be static
#ifndef STATIC 
#define STATIC static
#endif

// Common helper macros
#define ARR_LEN(arr) (sizeof arr / sizeof arr[0])
#define SARR_LEN(arr) ((int_fast32_t)(sizeof arr / sizeof arr[0]))
#define DIM(arr) SARR_LEN(arr)  // common alias

/**
 * @brief This is a master collection of all possible Status return values
 * defined for the project, Generally negative are serious while positive
 * ones are expected
 * 
 */
typedef enum Status_t
{
    Status_BadState = -3,
    Status_Param = -2,      // A function all parameter was incorrect
    Status_Failed = -1,     // general non specifc fail
    Status_OK = 0,          // 0 means all is well
    Status_Timeout = 1,     // expected timeout
    Status_FULL,
    Status_EMPTY,
    Status_BUSY,
    Status_Unexpected,      // did you try to free soemthing twice? or use after free?
} Status_t;

/**
 * @brief This is a object type that can hold any single
 *        value usefull as a context.
 * @details A union because on some systems intptr_t can't hold
 *          a function pointer correctly, and C99 does not require it.
 *          (remember segment hell from old intel 16bit days)
 */
typedef union Context_t
{
    intptr_t v_context;     // can hold any data pointer or reasonable integer
    void (*f_context)(union Context_t *context); // can hold a function pointer
} Context_t;

// generic callback function pointer
typedef void (*GenCallback_t)(Context_t context);

// usefull for building queues of functions or as a function return value.
typedef struct CbInstance_t
{
    GenCallback_t cb;
    Context_t context;
} CbInstance_t;





#endif
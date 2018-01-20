/**
 *  @file gencmdef.h
 *  @brief General defines useful in cortex M development
 */
#ifndef _GENCMDEF_H_
#define _GENCMDEF_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef TEST
 #include "core_cmInstr.h"
#else
 #define __get_PRIMASK() (0)
 #define __set_PRIMASK(pri) 
#endif

#ifndef __IO
#define __IO volatile
#endif

/// @brief Get dimension of an array
#define DIM(arr) (sizeof((arr))/sizeof((arr)[0]))
    
/// @brief saturated unsigned inc
#define USAT_INC(ui) (++(ui)?(ui):--(ui))
/// @brief saturated unsigned dec
#define USAT_DEC(ui) ((ui)?--(ui):(ui))

/// @brief Start a protected region, must end with END_PROTECTED
#define START_PROTECTED \
{ uint32_t __protect_var_28734 = __get_PRIMASK(); __set_PRIMASK(0);

/// @brief End a protected region started with START_PROTECTED
#define END_PROTECTED \
__set_PRIMASK(__protect_var_28734); }

typedef void (*voidFunc_f)(void);
typedef void (*objFunc_f)(void *obj);
typedef void *(*chainObjFunc_f)(void *obj);

#endif



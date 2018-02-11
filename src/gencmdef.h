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
#include "gpio.h"
 //#include "core_cmInstr.h"
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
#define USAT_INC(ui) \
do{ui = (ui)+1?(ui)+1:(ui);}while(0)
/// @brief saturated unsigned dec
#define USAT_DEC(ui) \
do{ ui = (ui)?(ui)-1:(ui);}while(0)

/// @brief Start a protected region, must end with END_PROTECTED
#define START_PROTECTED \
{ uint32_t __protect_var_28734 = __get_PRIMASK(); __set_PRIMASK(1);

/// @brief End a protected region started with START_PROTECTED
#define END_PROTECTED \
__set_PRIMASK(__protect_var_28734); }

/// plain void function prototype
typedef void (*voidFunc_f)(void);
/// void function that takes one parameter (usually an object pointer)
typedef void (*objFunc_f)(intptr_t obj);
/// a chainable object function (for making wrappers, decorators, or just returning status
typedef intptr_t (*chainObjFunc_f)(intptr_t obj);

typedef struct
{
    objFunc_f cb;
    intptr_t obj;
} objCallbackWrapper_t;

#define HEX_NIBBLE(val) \
(char)((val & 0xf) > 9?(val & 0xF)+'7':(val & 0xF) + '0')

#define BYTE2HEX(byte) \
(((byte & 0xF0) < 0xA0)? \
    (((uint16_t)(byte & 0xF0)>>4)+'0') + ((uint16_t)(((byte & 0xF) < 0xA)?\
        (byte & 0xF) + '0': (byte & 0xF) + '7')<<8): \
    (((uint16_t)(byte & 0xF0)>>4)+'7') + ((uint16_t)(((byte & 0xF) < 0xA)?\
        (byte & 0xF) + '0': (byte & 0xF) + '7')<<8))

   

#endif



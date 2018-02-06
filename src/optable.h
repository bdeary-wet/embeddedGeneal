/**
 *  @file optable.h
 *  @brief OpCode jump table functions.
 *  
 *  This function is for message opcode decoding. It takes a tableObject and a 
 *  key value and returns the entry in the table associated with the key.
 *  The table is a sparse sorted table with a key field and a pointer field
 *  assumed to be some function pointer for processing data based on the key.
 *  
 */
 /** \addtogroup usefulObjects
 *  @{
 */
#ifndef _OPTABLE_H
#define _OPTABLE_H

#include <stddef.h>
#include <stdint.h>

/// the default key size is 16 bits but could be 8 or 32.
/// if a string is required, the cmpfunc must be changed to use strcmp.
typedef uint16_t opTblKey_t;
#define OP_KEY_MSB 0x8000 // used to do the unsigned compare using the sign bit.

/// One table entry, key and associated pointer
typedef struct
{
    opTblKey_t key; // the key
    void *value;    // associated value
} opTableEntry_t;

/// prototype of a user supplied function to 
typedef void (*opTblEntryProcessor_f)(opTableEntry_t const *entry, uintptr_t context);

/// object defines a lookup table and associated processor function
typedef struct
{
    const opTableEntry_t *table; 
    size_t tableLen;
    opTblEntryProcessor_f processor;
} opTable_t;


/**
 *  @brief process a context via a table based on a key
 *  
 *  @param [in] table The opcode table object pointer
 *  @param [in] key the key to find
 *  @param [in] context The context value (probable an object pointer)
 *  @return returns a pointer to the object table entry or NULL if key not found
 *  
 *  @details Details
 */
opTableEntry_t const *OPT_ProcessViaTable(opTable_t const *table, 
                                    opTblKey_t key, 
                                    uintptr_t context);


/**
 *  @brief Look up a key in the table
 *  
 *  @param [in] table The opcode table object pointer
 *  @param [in] key the key to find
 *  @return a pointer to the object table entry or NULL if key not found
 *  
 *  @details Details
 */
opTableEntry_t const *OPT_FindEntry(opTable_t const *to, 
                              opTblKey_t key);

/**
 *  @brief optional Function for runtime init of table, in general the table will 
 *         probable be const in rom.
 *  
 *  @param [in] to The table objct to init
 *  @param [in] table pointer to table array
 *  @param [in] tableLen entries in the table array
 *  @param [in] processor optional processor function or NULL if not needed
 *  
 *  @details Details
 */
void OPT_InitTable(opTable_t *to, 
                   opTableEntry_t const *table, 
                   size_t tableLen, 
                   opTblEntryProcessor_f processor);

 /** @} */
                   
#endif // _OPTABLE_H

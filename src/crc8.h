/**
 *  @file crc8.h
 *  @brief CRC8 calculator for our inter processor bus.
 *  adaptable to any polynomial, we are using generator 
 *  x^8+x^5+x^3+x^2+x+1 (Baicheva C2) referred to as 0x97 by Koopman -
 *  "Cyclic Redundancy Code (CRC) Polynomial Selection For Embedded Networks"
 *  which can detect burst errors of up to 4 bits in messages up to 119 bits and
 *  has reasonable detection up to 2048 bits.
 *  Koopman 0x97 is 0x2f in traditional representation.
 *  
 */
#ifndef _CRC8_H
#define _CRC8_H

#include <stdint.h>
#include <stddef.h>

//#define CRC_HARD_TABLE  // use default hard table poly 0x2f

#ifdef CRC_HARD_TABLE
extern uint8_t const * const crc8Table;
#else
extern uint8_t const *crc8Table;  
void crc8Init(uint8_t gen);  
void crc8RestoreHardTable(void);
#endif    

/**
 *  @brief add a value to ongoing CRC8
 *  
 *  @param [in] lastCrc previous calculated crc8 or initialization value 
 *  @param [in] val byte to add to crc8
 *  @return the updated crc8 value
 *  
 *  @details Made this inline because it is a simple xor and lookup but wanted
 *           type enforcement of u8 to protect table access.
 */
static inline uint8_t crc8(uint8_t lastCrc, uint8_t val)
{
    return crc8Table[val ^ lastCrc];
}


uint8_t calcCrc8(uint8_t lastCrc, uint8_t *buf, size_t bufLen);


#endif // _CRC8_H

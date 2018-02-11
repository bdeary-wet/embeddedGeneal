#ifndef _HASHES_H
#define _HASHES_H
#include <stdint.h>

/** @brief Hash a string into a 32bit hash
 *  
 *  @param [in] str sting pointer
 *  @return 32 bit hash of the string
 *  
 *  @details Details
 */
uint32_t keyHash(char const *str);




#endif // _HASHES_H

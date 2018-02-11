#include "hashes.h"

static uint32_t myRotr32(uint32_t val, uint8_t bits)
{
    bits &= 31; // Mod 32
    return (val >> bits) | (val << (32-bits));
}

uint32_t keyHash(char const *str)
{
    uint32_t target = 0, working = 0;
    int place = 0, cnt = 0;
    while (*str)
    {
        working |= *(uint8_t*)str << 8*place;
        str++;
        if (++place == 4)
        {
            place = 0;
            working = myRotr32(working, 32 - (31 &(cnt * 3))); // 3 is rel prime to 32.
            target ^= working;
            working = 0;
            if(++cnt == 32)
            {
                return target; // probably a bug key was longer than 128
            }
        }
    }
    if (place != 0)
    {
        working = myRotr32(working, 32 - (31 &(cnt * 3)));
        target ^= working;
    }
    return target;
}
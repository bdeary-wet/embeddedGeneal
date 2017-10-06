/** @brief Unprotected Q macros for static Qs intended to pass bytes with 
 *         additional special case values between isr and background layers.
 
 * Queues (fifo) of 4, 8, and 16 signed 16 bit values with no checking for 
 * Q overflow. But optional test macros are included. 
 * These Qs really only hold n-1 values so if you need to actually hold
 * 8 values then use the Q16
 *
 */


#ifndef QMACS_H
#define QMACS_H

#include <stdint.h>
#include <stdbool.h>
        
#define DEFQ4(name) \
  static int16_t name##Q4[4], name##Q4From, name##Q4To
#define INQ4(name, val) \
do { name##Q4[ name##Q4To++ ] = val; name##Q4To &= 3;}while(0)
#define OUTQ4(name,val) \
do {val = name##Q4[ name##Q4From++ ]; name##Q4From &= 3;}while(0c
#define IS_DATAQ4(name) ( name##Q4To != name##Q4From)    
#define IS_SPACEQ4(name) ((name##Q4To+1)&3 != name##Q4From)
#define RESETQ4(name) name##Q4To = name##Q4From = 0
    
#define DEFQ8(name) \
  static int16_t name##Q8[8], name##Q8From, name##Q8To
#define INQ8(name, val) \
do { name##Q8[ name##Q8To++ ] = val; name##Q8To &= 7;}while(0)
#define OUTQ8(name,val) \
do {val = name##Q8[name##Q8From++]; name##Q8From &= 7;}while(0)
#define IS_DATAQ8(name) (name##Q8To != name##Q8From)    
#define IS_SPACEQ8(name) ((name##Q8To+1)&7 != name##Q8From)
#define RESETQ8(name) name##Q8To = name##Q8From = 0        
    
#define DEFQ16(name) \
  static int16_t name##Q16[16], name##Q16From, name##Q16To
#define INQ16(name, val) \
do { name##Q16[ name##Q16To++ ] = val; name##Q16To &= 15;}while(0)
#define OUTQ16(name,val) \
do {val = name##Q16[ name##Q16From++ ]; name##Q16From &= 15;}while(0)
#define IS_DATAQ16(name) (name##Q16To != name##Q16From)    
#define IS_SPACEQ16(name) ((name##Q16To+1)&15 != name##Q16From)
#define RESETQ16(name) name##Q16To = name##Q16From = 0
        
        
typedef struct
{
    int16_t *qbuf;
    int to;
    int from;
    int mask;
} qi16_t;    
        
// bufLen must be at least 4       
static inline void defqi16(qi16_t *q, int16_t *buf, uint8_t bufLen)
{
    q->qbuf=buf;
    q->to=0;
    q->from=0;
    int i;
    bufLen /= sizeof(int16_t); // buflen to DIM
    for (i=0; bufLen; i++) bufLen >>= 1;
    q->mask = (1 << (i-1)) -1;
}

static inline void inqi16(qi16_t *q, int16_t val)
{
    q->qbuf[q->to++] = val;
    q->to &= q->mask;
}

static inline int16_t outqi16(qi16_t *q)
{
    int16_t val = q->qbuf[q->from++];
    q->from &= q->mask;
    return val;
}
        
static inline bool isdataqi16(qi16_t *q)
{
    return q->to != q->from;
}

static inline bool isspaceqi16(qi16_t *q)
{
    return ((q->to+1)&q->mask) != q->from;
}

static inline void resetqi16(qi16_t *q)
{
    q->to = q->from = 0;
}


#endif



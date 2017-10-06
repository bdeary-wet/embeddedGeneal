#ifndef _FASTSWITCH_H
#define _FASTSWITCH_H
#include <stdint.h>

// Makes a pointer to an object given a name and a type where
// the name points to an object of type. Provides a buffered
// copy function where an new copy of the object is created
// and the name pointer updated to point at it.
// useful in slow modification of data in low priority task where
// the data is used in a higher priority task.


#define MAKE_FAST_SWITCH(name, type, copies) \
type name##_buf[copies]; \
type * name = &name##_buf[copies-1]; \
fastSwitch_t name##_fs = \
{(void*)&name, (uint8_t*)name##_buf, sizeof( type ), copies, 0}


typedef struct
{
    void **current;
    uint8_t *root;
    uint16_t size;
    uint8_t copies;
    uint8_t next;
} fastSwitch_t;

void *fsCopy(fastSwitch_t *fs, void *buf);

#endif // _FASTSWITCH_H

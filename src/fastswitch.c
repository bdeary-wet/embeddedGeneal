#include "fastswitch.h"
#include <string.h>


void *fsCopy(fastSwitch_t *fs, void *buf)
{
    uint8_t *next = fs->root + (fs->next*fs->size);
    memcpy(next, buf, fs->size);
    fs->next++;
    *fs->current = next;
    if(fs->next >= fs->copies) fs->next=0;
    return next;
}
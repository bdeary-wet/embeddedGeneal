/**
 * @file rs485_dmx.c
 * @author Bryce Deary (github@bd2357.org)
 * @brief 
 * @version 0.1
 * @date 2020-08-30
 * 
 * Copyright 2020, bdPrime
 * 
 */
#include <config.h>
#include "rs485_dmx.h"


STATIC size_t _add_to_device(DmxDevice_t *dev, uint8_t const *raw_data, size_t const raw_data_len)
{
    // If we need a buffer get it from queue 
    size_t remaining = raw_data_len;
    if(dev->data == NULL)
    {
        if(Status_OK == Ipt_DequeueInIsr(dev->ipt, (void**)&dev->data))
        {
            dev->next_data = dev->data;
        }
        else
        {
            dev->missed++; // for debug, can remove after validation
            return 0;
        }
    }

    uint8_t *end = dev->data + dev->slots;
    while(remaining && dev->next_data < end)
    {
        *dev->next_data = *raw_data++;
        dev->next_data++;
        remaining--;
    }
    // if we are done, return the dev data to user
    if(dev->next_data == end)
    {
        Ipt_ReturnToUserProcess(dev->ipt, dev->data);
        dev->data = NULL; // not ours anymore
    }
    return raw_data_len - remaining; // return used
}

/**
 * @brief Pass a raw block of data to the receiver where the data is only a
 *   stream of bytes.  A sepearate call to the function with a length of zero
 *   is needed to indicate a break condition to reset the internal byte counter.
 * 
 * @param self 
 * @param raw_data pointer to byte array
 * @param new_data_len number of byte in the array
 * @details This function is intended for systems with DMA or large fixed arrays
 *   that may have sepearate break detection.
 */
void DMX_use_raw_data(DmxReceiver_t *self, uint8_t const *raw_data, size_t raw_data_len)
{
    Status_t status;
    if(raw_data_len)
    {
        if (self->slot_cnt >= 0)
        {
            // if new frame, make sure it is our type
            if(self->slot_cnt == 0)
            {
                // if slot0 match start the processing
                if (*raw_data == self->slot0)
                {
                    raw_data++;     // effectivly drop first cell in buffer
                    raw_data_len--;
                    self->slot_cnt = 1;
                }
                else // this data is not for us
                {
                    self->slot_cnt = -1; // take us off line till break
                    return;
                }
            }


            for (DmxDevice_t *dev = self->first_device; 
                dev && raw_data_len; 
                dev = dev->next_dev)
            {
                if(self->slot_cnt >= (dev->first_slot + dev->slots)) continue;

                if(self->slot_cnt < dev->first_slot)
                {
                    int skip = dev->first_slot - self->slot_cnt;
                    // if not enough data to reach next device, quit
                    if(skip > raw_data_len)
                    {
                        self->slot_cnt += raw_data_len;
                        return;
                    }
                    // else advance to next device
                    self->slot_cnt += skip;
                    raw_data_len -= skip;
                    raw_data += skip;
                }
                // process the data into device, 
                size_t used = _add_to_device(dev, raw_data, raw_data_len );
                // setup for any remaining data
                self->slot_cnt += used;
                raw_data_len -= used;
                raw_data += used;
            }
            // advance based on any remaining data, means we done till break.
            self->slot_cnt += raw_data_len;
            // self->slot_cnt = -1; // take us off line till break
        }
    }
    else // zero length, interpret as a serial break
    {
        DMX_process_break(self);
    }
}

/**
 * @brief This function is called by GPIO ISR that deals with the 
 *   unusual DMX512 break or by the serial break detection.
 * 
 * @param self DmxReceiver object
 */
void DMX_process_break(DmxReceiver_t *self)
{
    // set slot counter back to 0 and clear any and all pending data collections
    self->slot_cnt = 0;
    // walk the list and set data pointers back to zero offset
    for (DmxDevice_t *dev = self->first_device; dev; dev = dev->next_dev)
    {
        if(dev->data == NULL) // if no buffer allocated see if we can get one
        {
            Ipt_DequeueInIsr(dev->ipt, (void**)&dev->data);
        }
        dev->next_data = dev->data;
    }
}

void DMX_receiver_reset(DmxReceiver_t *self)
{
    for (DmxDevice_t *dev = self->first_device; dev; dev = dev->next_dev)
    {
        dev->data = NULL;
        Ipt_Reset(dev->ipt);
    }
    self->slot_cnt=0;
}















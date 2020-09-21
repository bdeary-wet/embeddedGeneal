/**
 * @file rs485_dmx.h
 * @author bdeary (bdeary@wetdesign.com)
 * @brief This is the dmx main function that in turn uses the rs485 system
 *   and optional a GPIO pin to enforce the break and other timing requirements
 *   of DMX512. 
 * @date 2020-09-15
 * @details note that the strict timing may be disabled if not needed.
 * 
 * Copyright 2020, WetDesigns
 * 
 */
#ifndef RS485_DMX_H
#define RS485_DMX_H

#include <isr_comm.h>

typedef struct DmxDeviceContext_t
{
    uint8_t *data;
} DmxDeviceContext_t;

typedef struct DmxDevice_t
{
    struct DmxDevice_t *next_dev;
    uint16_t first_address; // first slot to look for
    uint16_t slots;         // the number of slots to collect
    uint8_t *data;
    uint8_t *next_data;
    GenCallback_t *cb;  // cb could be device specific or generic contect is copy of this structure
} DmxDevice_t;

typedef struct DmxReceiver_t
{
    DmxDevice_t * const first_device; // pointer to first device in list
    DmxDevice_t *current_device;      // pointer to current or next device, or NULL if complete
    IsrProcessTarget_t *buffer_pool;  // source of our working buffers
    uint8_t *working_buffer; // pointer to a buffer big enough to hold data for all devices
    uint8_t *working_next;   // next location in buffer
    int16_t slot_cnt;        // slot counter advances in blocks
    uint8_t const slot0;     // slot 0 value to trigger decoding
} DmxReceiver_t;









#endif // RS485_DMX_H

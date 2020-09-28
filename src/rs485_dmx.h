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
    struct DmxDevice_t *next_dev;  // linked list of devices, NULL if last in chain
    IsrProcessTarget_t *ipt;    // Isr process target, data queues
    uint8_t *data;          // pointer to device specific working buffer from ipt
    uint8_t *next_data;     // next destination in working buffer
    uint32_t missed;
    uint16_t first_slot;    // first slot to look for
    uint16_t slots;     // the number of slots to collect
} DmxDevice_t;

typedef struct DmxReceiver_t
{
    DmxDevice_t * const first_device; // pointer to first device in list
    int16_t slot_cnt;        // slot counter advances in blocks
    uint8_t const slot0;     // slot 0 value to trigger decoding
} DmxReceiver_t;


/**
 * @brief handler called by specific isr wrapper.
 * 
 * @param buf pointer to data to process
 * @param buf_len length of data to process
 * @details this is the general purpose function, called by a specific wrapper.
 * 
 */
void DMX_use_raw_data(DmxReceiver_t *self, uint8_t const *raw_data, size_t raw_data_len);

void DMX_process_break(DmxReceiver_t *self);

void DMX_receiver_reset(DmxReceiver_t *self);

void *DMX_background_device_process(DmxDevice_t *self);





#endif // RS485_DMX_H

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

struct _drop_t;
typedef void (*drop_processor_t)(struct _drop_t *self, uint8_t slot);

typedef struct _drop_t
{
    struct _drop_t *next;
    uint16_t first;
    uint16_t range;
    drop_processor_t processor;
} _drop_t;

typedef enum {
    DMX_NoBreak=0, 
    DMX_BreakWithData=1,
    DMX_DataWithBreak=2,
} BreakState_t;

struct _DmxStateMachine_t;
typedef void (*dmx_state_process_t)(struct _DmxStateMachine *self, uint8_t *slot, size_t slots, BreakState_t saw_break);

typedef struct _DmxStateMachine_t
{
    _drop_t *first_drop;
    uint16_t slot;
    dmx_state_process_t process_state;
    uint8_t zero_slot;
} _DmxStateMachine_t;


STATIC void _waitBreak(struct _DmxStateMachine *self, uint8_t *slot, size_t slots, bool saw_break);
STATIC void _waitSlotZero(struct _DmxStateMachine *self, uint8_t *slot, size_t slots, bool saw_break);
STATIC void _waitSlots(struct _DmxStateMachine *self, uint8_t *slot, size_t slots, bool saw_break);

extern void drop3_processor(struct _drop_t *self, uint8_t slot);
extern void drop2_processor(struct _drop_t *self, uint8_t slot);
extern void drop1_processor(struct _drop_t *self, uint8_t slot);

_drop_t drop3 = (_drop_t){.first=48, .range=4, .processor=drop3_processor};
_drop_t drop2 = (_drop_t){.first=48, .range=4, .processor=drop2_processor, .next=&drop3};
_drop_t drop1 = (_drop_t){.first=48, .range=4, .processor=drop1_processor, .next=&drop2};

_DmxStateMachine_t dmx = (_DmxStateMachine_t){.first_drop=&drop1, .process_state=_waitBreak};



void ProcessDmxSlotData(_DmxStateMachine_t *self, uint8_t *slot, size_t slots, bool saw_break)
{
    // if self is NULL, system is OFF
    if(!self) return;
    // if state is defined, run state process
    if(self->process_state)
    {
        self->process_state(self, slot, slots, saw_break);
    }
    // if no state was defined, a break resets the state machine
    else if(saw_break)
    {
        self->process_state = _waitBreak;
        self->process_state(self, slot, slots, saw_break);
    }
}

// system is waiting for start of sequence
STATIC void _waitBreak(struct _DmxStateMachine *self, uint8_t *slot, size_t slots, bool saw_break)
{

}

//
STATIC void _waitSlotZero(struct _DmxStateMachine *self, uint8_t *slot, size_t slots, bool saw_break)
{

}

STATIC void _waitSlots(struct _DmxStateMachine *self, uint8_t *slot, size_t slots, bool saw_break)
{

}


STATIC int16_t _process_dev(DmxDevice_t *self, uint8_t *buf)

STATIC int16_t _process_data(DmxReceiver_t *self, uint8_t const *raw_data, size_t raw_data_len)
{
    size_t processed = raw_data_len;
    DmxDevice_t *dev = self->current_device;
    if (dev)
    {
        int16_t past = self->slot_cnt + raw_data_len;
        if(dev->first_address < past) // some of the data will fit
        {
            int16_t skip = dev->first_address - self->slot_cnt;
            if(skip > 0) 
            {
                raw_data += skip;
            }
            else 
            {
                int16_t last = dev->first_address + dev->slots;
                if (last > past)
                {
                    for(int slot=self->slot_cnt; slot < past; slot++ ) 
                    {
                        *(dev->next_data)++ = *raw_data++;
                    }
                }
                else
                {
                    for(int slot=self->slot_cnt; slot < last; slot++ ) 
                    {
                        *(dev->next_data)++ = *raw_data++;
                    }
                    _next_dev(self);
                    processed -= (past-last);
                }
            }
         }
    }

    return processed; 
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
        if(self->slot_cnt == 0)
        {
            // if slot0 match start the processing
            if (*raw_data == self->slot0)
            {
                raw_data++;     // effectivly drop first cell in buffer
                raw_data_len--;
                if(!self->working_buffer)
                {
                    status = Ipt_DequeueInIsr(self->buffer_pool, 
                                              &(self->working_buffer));
                    if(status != Status_OK)
                    {
                        // data loss !!!!!
                        self->slot_cnt = -1; // take us off line till next break
                    }
                    else
                    {
                        self->slot_cnt = 1;  // next data to be processed
                        self->current_device = self->first_device;
                    }
                }
            }
            else // this data is not for us
            {
                self->slot_cnt = -1; // take us off line till break
            }
        }
        while(raw_data_len)
        {
            int16_t used = _process_data(self, raw_data, raw_data_len);
            raw_data_len -= used;
            raw_data += used;
        }

    }
    else // process break state
    {
        // if slot_cnt is pos we need to return the buffer
        if (self->slot_cnt > 0)
        {
            // flush the working data set
            Ipt_ReturnToUserProcess(self->buffer_pool, self->working_buffer);
            self->slot_cnt = 0;
            self->current_device = NULL;
        }
    }
}

/**
 * @brief Pass a processed set of raw received data to the receiver where
 *   processes means breaks are inserted into the stream as -1 tokens.
 * 
 * @param self 
 * @param new_data short array of processed data
 * @param new_data_len the number of words in the array
 * @details This function is intended for systems with short fifo like IO
 *   where mutiple interrupts are expected per DMX data frame.
 */
void DMX_use_processed_data(DmxReceiver_t *self, int16_t new_data, size_t new_data_len)
{

}














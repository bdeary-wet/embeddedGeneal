#ifndef DMX_DEMO_H
#define DMX_DEMO_H
#include "micro_p_sim.h"  // model base class
#include <rs485_dmx.h>
#include <genQ.h>


DeclareGenQWrappers(isrQ_, uint8_t);


enum {
    NMI_PLACE = 0,
    DMX_BREAK,
    DMX_SERIAL,
    MS_ISR = 5,
    TOTAL_ISRs,
};

enum {
    DEV1_SLOTS = 4,
    DEV2_SLOTS = 7,

    DEV1_FIRST = 10,
    DEV2_FIRST = 124,
};

#define DMX_BYTES_PER_SEC (250000/11)


typedef struct 
{
    uint8_t buf[DEV1_SLOTS];
} dev1_buf_t;

typedef struct 
{
    uint8_t buf[DEV2_SLOTS];
} dev2_buf_t;


DeclareDoubleIsrObjectQueue(dev1_ipt, dev1_buf_t);
DeclareDoubleIsrObjectQueue(dev2_ipt, dev2_buf_t);

// this is the declaration of the user derived class from the ModelBase_t class
typedef struct DdModel_t
{
    ModelBase_t model_base; // parent object

    DmxDevice_t dev1;
    DmxDevice_t dev2;
    DmxReceiver_t dmx512_receiver;
    uint32_t ms_tick;
    uint16_t frame;

} DdModel_t;

extern DdModel_t dd_model;

/////// The following four functions define the users model and are included
// in micro_p_sim 
ModelBase_t *DD_setup(ModelBase_t *model);
ModelBase_t *DD_isr_stimulus(ModelBase_t *model);
ModelBase_t *DD_main(ModelBase_t *model);
ModelBase_t *DD_diag(ModelBase_t *model);


#endif // DMX_DEMO_H

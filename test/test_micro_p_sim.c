#include "unity.h"

#include "micro_p_sim.h"

typedef struct MyModel_t
{
    ModelBase_t model_base;
    int my_int;
} MyModel_t;

MyModel_t my_model;

ModelBase_t *my_main(ModelBase_t *model)
{
    return model;
}

ModelBase_t *my_diag(ModelBase_t *model)
{
    return model;
}

ModelBase_t *my_isr_sim(ModelBase_t *model)
{
    return model;
}

ModelBase_t *model_init(void)
{
    my_model.model_base.main = my_main;
    my_model.model_base.diagnostics = my_diag;
    my_model.model_base.isr_stimulus = my_isr_sim;
    return (ModelBase_t*)&my_model;
}

ModelBase_t *model_start(ModelBase_t *model)
{
    MyModel_t *self = (MyModel_t*)model;
}



void setUp(void)
{
}

void tearDown(void)
{
}

void test_micro_p_sim_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement micro_p_sim");
}

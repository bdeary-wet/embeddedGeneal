#include "unity.h"

#include "micro_p_sim.h"
#include <pthread.h>
#include <unistd.h>

typedef struct MyModel_t
{
    ModelBase_t model_base;
    int my_int;
} MyModel_t;

MyModel_t my_model;

ModelBase_t *my_main(ModelBase_t *model)
{
    TEST_ASSERT_EQUAL_PTR((ModelBase_t*)&my_model, model);
    return model;
}

ModelBase_t *my_diag(ModelBase_t *model)
{
    TEST_ASSERT_EQUAL_PTR((ModelBase_t*)&my_model, model);
    return model;
}

ModelBase_t *my_isr_sim(ModelBase_t *model)
{
    TEST_ASSERT_EQUAL_PTR((ModelBase_t*)&my_model, model);    
    return model;
}

ModelBase_t *model_init(void)
{
    my_model.model_base.main = my_main;
    my_model.model_base.diagnostics = my_diag;
    my_model.model_base.isr_stimulus = my_isr_sim;
    return (ModelBase_t*)&my_model;
}

pthread_t sim_main_thread;
void stop(void)
{
    my_model.model_base.sim_enabled = 0;
    pthread_join(sim_main_thread, NULL);
}

ModelBase_t *model_start(ModelBase_t *model)
{
    MyModel_t *self = (MyModel_t*)model;
    TEST_ASSERT_EQUAL_PTR(&my_model, self);    
}


void setUp(void)
{
    TEST_ASSERT_FALSE_MESSAGE(
        pthread_create(&sim_main_thread, NULL, sim_main, NULL) ,
        "sim_main did not start"
    );
}

void tearDown(void)
{
}

void test_micro_p_sim(void)
{
    sleep(1);
    printf("isr %u, main %u\n", my_model.model_base.tick,
           my_model.model_base.main_tick);
    stop();
    printf("isr %u, main %u\n", my_model.model_base.tick,
           my_model.model_base.main_tick);    
}

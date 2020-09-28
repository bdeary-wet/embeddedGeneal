#include "unity.h"

#include "micro_p_sim.h"
#include <pthread.h>
#include <time.h>



// this is the declaration of the user derived class from the ModelBase_t class
typedef struct MyModel_t
{
    ModelBase_t model_base; // parent object
    int my_int;
} MyModel_t;

enum {ISR_TABLE_SIZE= 10,};
void_func_t isr_table[ISR_TABLE_SIZE];
uint8_t isr_flags[ISR_TABLE_SIZE];

// this is the instantiation of the user derived class from the ModelBase_t class
MyModel_t my_model = (MyModel_t){
    .model_base.isr_table_size=ISR_TABLE_SIZE,
    .model_base.isr_table=isr_table,
    .model_base.isr_flags=isr_flags,
};

/////// The following four functions define the users model //////
ModelBase_t *model_setup(ModelBase_t *model)
{
    MyModel_t *self = (MyModel_t*)model;
}

ModelBase_t *my_isr_sim(ModelBase_t *model)
{
    return model;
}

ModelBase_t *my_main(ModelBase_t *model)
{
    return model;
}

ModelBase_t *my_diag(ModelBase_t *model)
{
    return model;
}



// This is the user provide function to setup the model with the
// above functions and any initializations or invariants to the user data model.
ModelBase_t *Micro_p_sim_init(void)
{
    my_model = (MyModel_t) {
        .model_base.setup=model_setup,
        .model_base.main_loop=my_main,
        .model_base.diagnostics=my_diag,
        .model_base.isr_stimulus=my_isr_sim,
        .model_base.isr_table_size=ISR_TABLE_SIZE,
        .model_base.isr_table=isr_table,
        .model_base.isr_flags=isr_flags,
        .model_base.in_isr=1,
        .model_base.sim_enabled=1,
        .model_base.isr_are_prioritized=0,
        .model_base.isr_auto_clear=1,
        .my_int=42,
    };
    // any other user init goes here

    // return pointer to base class object
    return (ModelBase_t*)&my_model;
}

pthread_t sim_main_thread;
void stop(void)
{
    my_model.model_base.sim_enabled = 0;
    pthread_join(sim_main_thread, NULL);
}

int trials={0};

void isr_example(void)
{
    printf("In Isr Example\n");
    fflush(stdout);
}


// before each test, create a new initialized model
void setUp(void)
{

    my_model.model_base.setup=model_setup;
    my_model.model_base.main_loop=my_main;
    my_model.model_base.diagnostics=my_diag;
    my_model.model_base.isr_stimulus=my_isr_sim;
    my_model.model_base.main_tick=0;
    my_model.model_base.tick=0;
    my_model.model_base.in_isr=1;
    my_model.model_base.sim_enabled=1;
    my_model.my_int=trials++;

    for(int i=0; i<ISR_TABLE_SIZE; i++) 
    {
        my_model.model_base.isr_flags[i]=0;
    }

    isr_table[2] = isr_example;
    isr_flags[2] = 1;

    TEST_ASSERT_FALSE_MESSAGE(
        pthread_create(&sim_main_thread, NULL, Micro_p_sim_main, &my_model) ,
        "sim_main did not start"
    );
    printf("set up, isr %u, main %u\n", my_model.model_base.tick,
        my_model.model_base.main_tick);

    while(!my_model.model_base.main_tick)
        sched_yield(); // wait to get started and setup
}

void tearDown(void)
{
    stop(); // command it to stop
    printf("tear down, isr %u, main %u\n", my_model.model_base.tick,
           my_model.model_base.main_tick);
}

void test_micro_p_sim(void)
{
    nanosleep(&(struct timespec){.tv_sec=2}, NULL);  // let it run for 2 sec
    printf("test_micro_p_sim isr %u, main %u\n", my_model.model_base.tick,
           my_model.model_base.main_tick);
 
}

ModelBase_t *wait_for_time(ModelBase_t *model)
{
    MyModel_t *self = (MyModel_t*)model;
    static int first = {1};
    uint32_t check;
    if (first)
    {
        first = 0;
        printf("main tick is %u \n", model->main_tick);
        check = model->main_tick;
    }
    self->my_int++;
    if(model->main_tick - check > 1000000)
    {
        first = 1;
        model->sim_enabled = 0;
    }
    return model;
}

// This is an example of how to inject a test into the running system
// the setup gets the model up and running then this test waits for
// indication the main loop has started and then inserts a function
// at the diagnostic postion at the end of the main loop.
void test_injected_test(void)
{
    while(!my_model.model_base.main_tick){} // wait for background to start
    
    my_model.model_base.diagnostics = wait_for_time; // add our function

    // monitor (read only) state of the model and quit when certain state
    // or after a certain time.
    while(my_model.model_base.sim_enabled) 
    {
        nanosleep(&(struct timespec){.tv_nsec=10000000}, NULL);  
        printf("inj test %i\n", my_model.my_int);
        //sched_yield(); // wait for shut down
    }
}


void test_up_then_down(void)
{
    // do nothing, just run setup and teardown
    while(!my_model.model_base.main_tick){} // wait for background to start
    printf("tick is %u\n", my_model.model_base.tick);
    sched_yield(); // before we tear_down, let stuff happen
}

void test_do_nothing(void)
{
    
}
#include "unity.h"

#include "rs485_dmx.h"
#include <isr_comm.h>
#include <genQ.h>
#include <genPool.h>


enum {
    DEV1_SLOTS = 4,
    DEV2_SLOTS = 7,
    DEV3_SLOTS = 15,
    DEV1_FIRST = 10,
    DEV2_FIRST = 20,
    DEV3_FIRST = 30,

};

typedef struct 
{
    uint8_t buf[DEV1_SLOTS];
} dev1_buf_t;

typedef struct 
{
    uint8_t buf[DEV2_SLOTS];
} dev2_buf_t;

typedef struct 
{
    uint8_t buf[DEV3_SLOTS];
} dev3_buf_t;

DefineDoubleIsrObjectQueue(dev1_ipt, dev1_buf_t, 3, NULL);
DefineDoubleIsrObjectQueue(dev2_ipt, dev2_buf_t, 3, NULL);
DefineDoubleIsrObjectQueue(dev3_ipt, dev3_buf_t, 3, NULL);

DmxDevice_t dev3 = (DmxDevice_t){
    .next_dev=NULL,
    .ipt=&dev3_ipt_instance,
    .first_slot=DEV3_FIRST,
    .slots=DEV3_SLOTS,
};

DmxDevice_t dev2 = (DmxDevice_t){
    .next_dev=&dev3,
    .ipt=&dev2_ipt_instance,
    .first_slot=DEV2_FIRST,
    .slots=DEV2_SLOTS,
};

DmxDevice_t dev1 = (DmxDevice_t){
    .next_dev=&dev2,
    .ipt=&dev1_ipt_instance,
    .first_slot=DEV1_FIRST,
    .slots=DEV1_SLOTS,
};

DmxReceiver_t myRx = (DmxReceiver_t){
    .first_device=&dev1,
    .slot0=0,
    .slot_cnt=0,
};

dev1_buf_t *first_dev_1_buf;
void setUp(void)
{
    DMX_receiver_reset(&myRx);
    void *item = Allocate_dev1_ipt_Object();
    first_dev_1_buf = item;
    TEST_ASSERT_EQUAL(Status_OK, Send_dev1_ipt_Object(item));
    item = Allocate_dev2_ipt_Object();
    TEST_ASSERT_EQUAL(Status_OK, Send_dev2_ipt_Object(item));
    item = Allocate_dev3_ipt_Object();
    TEST_ASSERT_EQUAL(Status_OK, Send_dev3_ipt_Object(item));
}

void tearDown(void)
{
}

void test_rs485_dmx_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement rs485_dmx");
}


size_t _add_to_device(DmxDevice_t *dev, uint8_t *raw_data, size_t raw_data_len);
void test_add_to_device_1(void)
{
    uint8_t buf[10]={1,2,3,4,5,6,7,8,9,10};
    TEST_ASSERT_EQUAL(1,_add_to_device(&dev1, buf, 1));
    TEST_ASSERT_NOT_NULL(dev1.data);
    TEST_ASSERT_EQUAL_PTR(dev1.data+1, dev1.next_data);
    TEST_ASSERT_EQUAL(1,_add_to_device(&dev1, &buf[1], 1));
    TEST_ASSERT_NOT_NULL(dev1.data);
    TEST_ASSERT_EQUAL_PTR(dev1.data+2, dev1.next_data);
    TEST_ASSERT_EQUAL(1,_add_to_device(&dev1, &buf[2], 1));
    TEST_ASSERT_NOT_NULL(dev1.data);
    TEST_ASSERT_EQUAL_PTR(dev1.data+3, dev1.next_data); 
    TEST_ASSERT_EQUAL_HEX8_ARRAY(buf, dev1.data, 3);
    TEST_ASSERT_EQUAL(1,_add_to_device(&dev1, &buf[3], 7));
    TEST_ASSERT_NULL(dev1.data);
    dev1_buf_t *from;
    Receive_dev1_ipt_Object(&from);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(buf, from->buf, DEV1_SLOTS);    
}

void test_add_to_device_many(void)
{
    uint8_t buf[10]={1,2,3,4,5,6,7,8,9,10};
    TEST_ASSERT_EQUAL(DEV2_SLOTS,_add_to_device(&dev2, buf, 10));
    TEST_ASSERT_NULL(dev2.data);
    dev2_buf_t *from;
    Receive_dev2_ipt_Object(&from);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(buf, from->buf, DEV2_SLOTS);    
}

void test_DMX_process_break(void)
{
    TEST_ASSERT_EQUAL_PTR(NULL, myRx.first_device->data);
    DMX_process_break(&myRx);
    TEST_ASSERT_EQUAL_PTR(first_dev_1_buf, myRx.first_device->data);

}

void test_DMX_use_raw_data(void)
{
    uint8_t buf[80];
    for(int i=0; i<80; i++) buf[i] = i;
    DMX_use_raw_data(&myRx, &buf[ 0],  5);
    DMX_use_raw_data(&myRx, &buf[ 5],  9);
    DMX_use_raw_data(&myRx, &buf[14], 11);
    DMX_use_raw_data(&myRx, &buf[25], 14);
    DMX_use_raw_data(&myRx, &buf[39], 41);

    dev1_buf_t *from1;
    dev2_buf_t *from2;
    dev3_buf_t *from3;
    Receive_dev1_ipt_Object(&from1);
    Receive_dev2_ipt_Object(&from2);
    Receive_dev3_ipt_Object(&from3);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(&buf[DEV1_FIRST], from1->buf, DEV1_SLOTS);  
    TEST_ASSERT_EQUAL_HEX8_ARRAY(&buf[DEV2_FIRST], from2->buf, DEV2_SLOTS);  
    TEST_ASSERT_EQUAL_HEX8_ARRAY(&buf[DEV3_FIRST], from3->buf, DEV3_SLOTS);  
}


void test_DMX_use_raw_data_loop(void)
{
    uint8_t buf[80];
    for(int i=0; i<80; i++) buf[i] = i;

// allocate and queue a second set of buffers
    void *item = Allocate_dev1_ipt_Object();
    TEST_ASSERT_EQUAL(Status_OK, Send_dev1_ipt_Object(item));
    item = Allocate_dev2_ipt_Object();
    TEST_ASSERT_EQUAL(Status_OK, Send_dev2_ipt_Object(item));
    item = Allocate_dev3_ipt_Object();
    TEST_ASSERT_EQUAL(Status_OK, Send_dev3_ipt_Object(item));

    for (int i = 0; i<10; i++)
    {
        DMX_use_raw_data(&myRx, &buf[0], 1);
        DMX_use_raw_data(&myRx, &buf[1+i], 60);

        dev1_buf_t *from1;
        dev2_buf_t *from2;
        dev3_buf_t *from3;
        Receive_dev1_ipt_Object(&from1);
        Receive_dev2_ipt_Object(&from2);
        Receive_dev3_ipt_Object(&from3);
        printf("Set # %i, %i %i %i\n",i, from1->buf[0], from2->buf[0], from3->buf[0]);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(&buf[DEV1_FIRST+i], from1->buf, DEV1_SLOTS);  
        TEST_ASSERT_EQUAL_HEX8_ARRAY(&buf[DEV2_FIRST+i], from2->buf, DEV2_SLOTS);  
        TEST_ASSERT_EQUAL_HEX8_ARRAY(&buf[DEV3_FIRST+i], from3->buf, DEV3_SLOTS); 
        // send the buffers back
        TEST_ASSERT_EQUAL(Status_OK, Send_dev1_ipt_Object(from1));
        TEST_ASSERT_EQUAL(Status_OK, Send_dev2_ipt_Object(from2));
        TEST_ASSERT_EQUAL(Status_OK, Send_dev3_ipt_Object(from3));
        DMX_use_raw_data(&myRx, buf, 0); // This is a break
    }
}



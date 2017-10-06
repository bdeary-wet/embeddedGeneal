#include "fastswitch.h"
#include "unity.h"


void setUp(void)

{

}



void tearDown(void)

{

}







typedef struct

{

    uint32_t var1;

    uint8_t b1;

    uint16_t short1;





}myType_t;



myType_t myVar_buf[2]; myType_t * myVar = &myVar_buf[2 -1]; fastSwitch_t myVar_fs = {(void*)&myVar, (uint8_t*)myVar_buf, sizeof( myType_t ), 2, 0};



void test_fastswitch(void)

{



    myType_t localCpy = {32, 45, 67};

    UnityAssertEqualNumber((UNITY_INT)((myVar)), (UNITY_INT)((&myVar_buf[1])), (

   ((void *)0)

   ), (UNITY_UINT)(29), UNITY_DISPLAY_STYLE_INT);



    fsCopy(&myVar_fs, &localCpy);

    UnityAssertEqualMemory(( const void*)((&localCpy)), ( const void*)((myVar_buf)), (UNITY_UINT32)((sizeof(myType_t))), 1, (

   ((void *)0)

   ), (UNITY_UINT)(32));

    UnityAssertEqualNumber((UNITY_INT)((myVar)), (UNITY_INT)((&myVar_buf[0])), (

   ((void *)0)

   ), (UNITY_UINT)(33), UNITY_DISPLAY_STYLE_INT);

    localCpy.b1++;

    fsCopy(&myVar_fs, &localCpy);

    UnityAssertEqualMemory(( const void*)((&localCpy)), ( const void*)((&myVar_buf[1])), (UNITY_UINT32)((sizeof(myType_t))), 1, (

   ((void *)0)

   ), (UNITY_UINT)(36));

    UnityAssertEqualNumber((UNITY_INT)((localCpy.b1)), (UNITY_INT)((myVar->b1)), (

   ((void *)0)

   ), (UNITY_UINT)(37), UNITY_DISPLAY_STYLE_INT);



}

#include "crc8.h"
#include "unity.h"






void dumpTable(void)

{

    for (int r =0; r<16; r++)

    {

        for (int c =0; c<16; c++)

        {

            printf("0x%02x,", crc8Table[(r<<4)+c]);

        }

        printf("\n");

    }



}



void setUp(void)

{

}



void tearDown(void)

{

}



char test[10]="123456789";



void test_from_source(void)

{

    crc8Init(0x1d);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((0xf)), (UNITY_INT)(UNITY_UINT8 )((crc8(0x00, 0xC2))), (

   ((void *)0)

   ), (UNITY_UINT)(32), UNITY_DISPLAY_STYLE_UINT8);

    uint8_t crc = crc8(0,1);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((0x76)), (UNITY_INT)(UNITY_UINT8 )((crc8(crc, 0x02))), (

   ((void *)0)

   ), (UNITY_UINT)(34), UNITY_DISPLAY_STYLE_UINT8);

}





void test_from_source2(void)

{

    uint8_t buf[2] = {1,2};

    crc8Init(0x1d);



    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((0x76)), (UNITY_INT)(UNITY_UINT8 )((calcCrc8(0, buf, sizeof(buf)))), (

   ((void *)0)

   ), (UNITY_UINT)(51), UNITY_DISPLAY_STYLE_UINT8);

}



void test_k83(void)

{

    crc8Init(0x7);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((0xf4)), (UNITY_INT)(UNITY_UINT8 )((calcCrc8(0, test, 9))), (

   ((void *)0)

   ), (UNITY_UINT)(57), UNITY_DISPLAY_STYLE_UINT8);

}



void test_kea(void)

{

    crc8Init(0xd5);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((0xbc)), (UNITY_INT)(UNITY_UINT8 )((calcCrc8(0, test, 9))), (

   ((void *)0)

   ), (UNITY_UINT)(63), UNITY_DISPLAY_STYLE_UINT8);

}



void test_iCode(void)

{

    crc8Init(0x1d);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((0x7e)), (UNITY_INT)(UNITY_UINT8 )((calcCrc8(0xfd, test, 9))), (

   ((void *)0)

   ), (UNITY_UINT)(69), UNITY_DISPLAY_STYLE_UINT8);

}



void test_build2f(void)

{



    crc8Init(0x2f);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((0x3e)), (UNITY_INT)(UNITY_UINT8 )((test[9]=calcCrc8(0x00, test, 9))), (

   ((void *)0)

   ), (UNITY_UINT)(76), UNITY_DISPLAY_STYLE_UINT8);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((0)), (UNITY_INT)(UNITY_UINT8 )((calcCrc8(0x00, test, 10))), (

   ((void *)0)

   ), (UNITY_UINT)(77), UNITY_DISPLAY_STYLE_UINT8);

    test[4]++;

    if (((0) != (calcCrc8(0x00, test, 10)))) {} else {UnityFail( ((" Expected Not-Equal")), (UNITY_UINT)((UNITY_UINT)(79)));};

    test[4]++;

    if (((0) != (calcCrc8(0x00, test, 10)))) {} else {UnityFail( ((" Expected Not-Equal")), (UNITY_UINT)((UNITY_UINT)(81)));};

    test[4]++;

    if (((0) != (calcCrc8(0x00, test, 10)))) {} else {UnityFail( ((" Expected Not-Equal")), (UNITY_UINT)((UNITY_UINT)(83)));};

    test[4]++;

    if (((0) != (calcCrc8(0x00, test, 10)))) {} else {UnityFail( ((" Expected Not-Equal")), (UNITY_UINT)((UNITY_UINT)(85)));};

    test[4] = '5';

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((0)), (UNITY_INT)(UNITY_UINT8 )((calcCrc8(0x00, test, 10))), (

   ((void *)0)

   ), (UNITY_UINT)(87), UNITY_DISPLAY_STYLE_UINT8);



    dumpTable();

}



void test_table(void)

{

    crc8RestoreHardTable();

    test[sizeof(test)-1]=calcCrc8(0xec, test, sizeof(test)-1);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((0)), (UNITY_INT)(UNITY_UINT8 )((calcCrc8(0xec, test, sizeof(test)))), (

   ((void *)0)

   ), (UNITY_UINT)(96), UNITY_DISPLAY_STYLE_UINT8);

}

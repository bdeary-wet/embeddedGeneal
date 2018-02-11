#include "unity.h"
#include "hashes.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_hashes_keyhash(void)
{
    char str1[] = {0x44, 0x33, 0x22, 0x11, 0};
    TEST_ASSERT_EQUAL_HEX(0x11223344,keyHash(str1));
    str1[3] = 0;
    TEST_ASSERT_EQUAL_HEX(0x223344,keyHash(str1));
    str1[2] = 0;
    TEST_ASSERT_EQUAL_HEX(0x3344,keyHash(str1));
    str1[1] = 0;
    TEST_ASSERT_EQUAL_HEX(0x44,keyHash(str1));
    str1[0] = 0;
    TEST_ASSERT_EQUAL_HEX(0x0,keyHash(str1));    
    
    char str2[] = {0x44, 0x33, 0x22, 0x11, 0x55, 0x66, 0x77, 0x88, 0xef, 0x5a, 0};
    TEST_ASSERT_EQUAL_HEX(0x528fba28,keyHash(str2));
    str2[9] += 1;
    TEST_ASSERT_EQUAL_HEX(0x528ffa28,keyHash(str2));    
    str2[9] = 0;
    str2[8] = 0;
    TEST_ASSERT_EQUAL_HEX(0x529901e8,keyHash(str2));
    str2[7] = 0;
    TEST_ASSERT_EQUAL_HEX(0x129901ec,keyHash(str2));
    str2[6] = 0;    
    TEST_ASSERT_EQUAL_HEX(0x112101ec,keyHash(str2));
    str2[5] = 0;    
    TEST_ASSERT_EQUAL_HEX(0x112231ec,keyHash(str2));


}

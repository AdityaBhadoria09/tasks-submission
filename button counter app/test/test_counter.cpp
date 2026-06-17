#include "unity.h"
#include "../src/counter.h"

Counter myCounter;

void setUp(void) {
    myCounter.reset(); 
}

void tearDown(void) {}

void test_Counter_Increment(void) {
    myCounter.increment();
    TEST_ASSERT_EQUAL(1, myCounter.getValue());
}

void test_Counter_Reset(void) {
    myCounter.increment();
    myCounter.increment();
    myCounter.reset();
    TEST_ASSERT_EQUAL(0, myCounter.getValue());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_Counter_Increment);
    RUN_TEST(test_Counter_Reset);
    return UNITY_END();
}
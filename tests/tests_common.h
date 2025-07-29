#ifndef __TESTS_COMMON__
#define __TESTS_COMMON__
#include<stdio.h>

#define CREATE_TEST(suit_name, test_name) \
int test_suit_##suit_name##_test_case_##test_name(void)

#define TEST_SUCCES   return 0;
#define TEST_FAILURE  return 1;
#define TEST_MEM_FAIL return 2;
#define TEST_EXPECT_FAIL return 3;

#define TEST_MEM_CHECK(func,...)                            \
if (func(__VA_ARGS__) == 0) {                               \
    printf("function %s failed to alloc memory", #func);    \
    TEST_MEM_FAIL                                           \
}                                                           


#define TEST_ASSERT(condition, err_message) \
if (!(condition)) {                         \
    printf("%s\n",err_message);             \
    TEST_FAILURE                            \
}

#define TEST_ASSERT_EQ(expr1, expr2, err_message)   \
if ((expr1) != (expr2)) {                           \
    printf("%s\n",err_message);                     \
    TEST_FAILURE                                    \
}

#define TEST_ASSERT_NEQ(expr1,expr2, err_message)   \
if ((expr1) == (expr2)) {                           \
    printf("%s\n",err_message);                     \
    TEST_FAILURE                                    \
}

#define LIT_LENGTH(x) sizeof(x)-1
#endif
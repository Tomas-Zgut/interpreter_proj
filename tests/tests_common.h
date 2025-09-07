#ifndef __TESTS_COMMON__
#define __TESTS_COMMON__
#include<stdio.h>
#include<string.h>
/**
 * @brief Macro that defines a test case
 * 
 * @par Macro creates a new fucntion witha standadrd nameing
 * scheme for tests, alos used for test discovery
 * 
 * @param suit_name: name of a test suit the test belongs to
 * @param test_name: name of the test it self
 */
#define CREATE_TEST(suit_name, test_name) \
int test_suit_##suit_name##_test_case_##test_name(void)

/**
 * @brief test ended successfuly
 */
#define TEST_SUCCES   return 0;

/**
 * @brief test unexpectadly failed
 */
#define TEST_FAILURE  return 1;

/**
 * @brief test failed to allocate memory
 */
#define TEST_MEM_FAIL return 2;
/**
 * @brief test failed as expected
 */
#define TEST_EXPECT_FAIL return 3;

/**
 * @brief test encountered an error
 */
#define TEST_ERR return 2;

/**
 * @brief Macro checks for mem allocation failure, and ends test early
 * 
 * @param func: function to call
 * @param ...: function arguments
 */
#define TEST_MEM_CHECK(func,...)                            \
if (func(__VA_ARGS__) == 0) {                               \
    printf("function %s failed to alloc memory\n", #func);  \
    TEST_MEM_FAIL                                           \
}                                                           

/**
 * @brief Macro to assert a condition, on fail prints error message
 * 
 * @param condition: condition that must hold
 * @param err_message: message to print if condition doesn't hold
 */
#define TEST_ASSERT(condition, err_message) \
if (!(condition)) {                         \
    printf("%s\n",err_message);             \
    TEST_FAILURE                            \
}

/**
 * @brief Macro to assert to expr to be equal, on fail prints erro message
 * 
 * @param epxr1: 1st expr to compare
 * @param expr2: 2nd expr to compare
 * @param err_message: message to print if condition doesn't hold
 */
#define TEST_ASSERT_EQ(expr1, expr2, err_message)   \
if ((expr1) != (expr2)) {                           \
    printf("%s\n",err_message);                     \
    TEST_FAILURE                                    \
}

/**
 * @brief Macro to assert to expr to be not equal, on fail prints error message
 * 
 * @param epxr1: 1st expr to compare
 * @param expr2: 2nd expr to compare
 * @param err_message: message to print if condition doesn't hold
 */
#define TEST_ASSERT_NEQ(expr1,expr2, err_message)   \
if ((expr1) == (expr2)) {                           \
    printf("%s\n",err_message);                     \
    TEST_FAILURE                                    \
}

/**
 * @brief Macro returns the length of a string literal without the null terminator
 * 
 * @param x: string literal to get length of
 * 
 * @returns length of pased in string literal (no null terminator)
 */
#define LIT_LENGTH(x) (sizeof(x)-1)

/**
 * @brief helper Macro to put a string literal into a string type
 * 
 * @param[out] buff: string type to init with a literal
 * @param[in] lit: string literal
 */
#define BUFF_FROM_LIT(buff,lit)                     \
    memcpy((void *)buff.data,lit,LIT_LENGTH(lit));  \
    buff.length = LIT_LENGTH(lit);

/**
 * @brief Macro creates a String from a string literal, on alloc fail ends test early
 * 
 * @param[out] buff: String to initialize with a string literal
 * @param[in] lit: string literal
 */
#define STRING_FROM_LIT(buff,lit)       \
    buff.data = malloc(LIT_LENGTH(lit));    \
    if(!buff.data) {                        \
        TEST_MEM_FAIL                       \
    }                                       \
    BUFF_FROM_LIT(buff,lit)

/**
 * @brief Macro creates a StringView from a string literal
 * @param lit: string literal
 */    
#define VIEW_FROM_LIT(lit)  \
(const StringView) {.data = lit, .length = LIT_LENGTH(lit)}

/**
 * @brief Macro creates a StringMut from a string literal, on alloc fail ends test early
 * 
 * @param[out] buff: StringMut to initialize with a string literal
 * @param[in] lit: string literal
 */
#define STRING_MUT_FROM_LIT(buff,lit)               \
    TEST_MEM_CHECK(sb_init,&buff,LIT_LENGTH(lit))   \
    BUFF_FROM_LIT(buff,lit)

/**
 * @brief Macro to singal unfinished work, prints message
 * 
 * @param message: message to print
 */
#define TODO(message)           \
    printf("%s\n",message);     \
    TEST_ERR

#endif
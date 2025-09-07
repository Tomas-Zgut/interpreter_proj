#include "tests_common.h"
#include<headders/memory_value.h>

CREATE_TEST(memory_value_create_tests,create_from_string) {
    const char lit[] = "string lit";
    String out;
    STRING_FROM_LIT(out, lit)
    memory_value_t val = memory_value_create(&out);

    const StringView stored_string = memory_value_get_string(&val);
    TEST_ASSERT_EQ(memcmp(stored_string.data,lit,sb_length(&stored_string)),0,"data in strings should be the same")
    TEST_ASSERT_EQ(sb_length(&stored_string),LIT_LENGTH(lit),"lenght should equal as well")

    memory_value_free(&val);
    sb_free(&out);

    TEST_SUCCES
}

CREATE_TEST(memory_value_create_tests,create_from_string_mut) {
    const char lit[] = "string mut";
    StringMut out;
    STRING_MUT_FROM_LIT(out, lit)
    memory_value_t val = memory_value_create(&out);

    StringMut stored_string;
    TEST_MEM_CHECK(memory_value_get_str_mut,&stored_string, &val);
    TEST_ASSERT_EQ(memcmp(stored_string.data,lit,sb_length(&stored_string)),0,"data in strings should be the same")
    TEST_ASSERT_EQ(sb_length(&stored_string),LIT_LENGTH(lit),"lenght should equal as well")

    memory_value_free(&val);
    sb_free(&out);
    sb_free(&stored_string);
    TEST_SUCCES
}

CREATE_TEST(memory_value_create_tests,create_from_nill) {
    nill_t nill = {0};
    memory_value_t val = memory_value_create(&nill);

    nill_t stored_nill = memory_value_get_nill(&val);
    (void) stored_nill; //removes a waring
    memory_value_free(&val);

    TEST_SUCCES
}

CREATE_TEST(memory_value_create_tests,create_from_int) {
    int64_t int_Val = 69;
    memory_value_t val = memory_value_create(&int_Val);
    int64_t stored_val = memory_value_get_int(&val);
    TEST_ASSERT_EQ(stored_val,int_Val,"stored data should equal")

    memory_value_free(&val);

    TEST_SUCCES
}

CREATE_TEST(memory_value_create_tests,create_from_double) {
    double double_Val = 69.0;
    memory_value_t val = memory_value_create(&double_Val);
    double stored_val = memory_value_get_double(&val);

    TEST_ASSERT_EQ(stored_val,double_Val,"stored data should equal")

    memory_value_free(&val);

    TEST_SUCCES
}

CREATE_TEST(memory_value_create_tests,create_from_type_string) {
    double double_Val = 69.0;
    memory_value_t val = memory_value_create(&double_Val);
    type_string type_str = memory_value_get_type_string(&val);
    memory_value_t val_type_str = memory_value_create(&type_str);
    const StringView stored_type_str = memory_value_get_type_view(&val_type_str);
    const StringView actual_type_str = VIEW_FROM_LIT("string");
    TEST_ASSERT_EQ(sb_cmp(&stored_type_str,&actual_type_str),0,"type strings should match")
    memory_value_free(&val);

    TEST_SUCCES
}


CREATE_TEST(memory_value_create_tests,create_uninitialized) {

    memory_value_t val = memory_value_create_default();

    TEST_ASSERT_EQ(memory_value_is_type(&val,VALUE_UNDEFINED),true,"value should be undefined!")

    memory_value_free(&val);

    TEST_SUCCES
}


CREATE_TEST(memory_value_move_tests,move) {
    const char lit[] = "string lit";
    String out;
    STRING_FROM_LIT(out, lit)
    memory_value_t val = memory_value_create(&out);
    memory_value_t new_val = memory_value_move(&val);
    
    const StringView stored_string = memory_value_get_string(&new_val);
    TEST_ASSERT_EQ(memcmp(stored_string.data,lit,sb_length(&stored_string)),0,"data in strings should be the same")
    TEST_ASSERT_EQ(sb_length(&stored_string),LIT_LENGTH(lit),"lenght should equal as well")
    TEST_ASSERT_EQ(memory_value_get_type(&val),VALUE_UNDEFINED,"moved out value should be undefined");

    memory_value_free(&val);
    memory_value_free(&new_val);
    sb_free(&out);

    TEST_SUCCES    
}

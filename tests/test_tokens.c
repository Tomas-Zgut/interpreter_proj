#include"tests_common.h"
#include<string.h>
#include<headders/tokens.h>
CREATE_TEST(token_tests,token_free) {
    token_t token_int = {.type=TOKEN_INT,.value.int_val = 5};
    token_t token_string = {.type = TOKEN_STRING};
    token_t token_var = {.type = TOKEN_VARIABLE, .value.var_data.var_frame = GF};
    STRING_FROM_LIT(token_string.value.string_val,"string")
    STRING_FROM_LIT(token_var.value.var_data.var_name,"variable")

    token_free(&token_int);
    token_free(&token_string);
    token_free(&token_var);

    TEST_SUCCES
}

CREATE_TEST(token_tests,token_free_double) {
    token_t token_int = {.type=TOKEN_INT,.value.int_val = 5};
    token_t token_string = {.type = TOKEN_STRING};
    token_t token_var = {.type = TOKEN_VARIABLE, .value.var_data.var_frame = GF};
    STRING_FROM_LIT(token_string.value.string_val,"string")
    STRING_FROM_LIT(token_var.value.var_data.var_name,"variable")

    token_free(&token_int);
    token_free(&token_string);
    token_free(&token_var);

    token_free(&token_int);
    token_free(&token_string);
    token_free(&token_var);
    TEST_SUCCES
}

CREATE_TEST(token_tests,token_move) {
    token_t token_int = {.type=TOKEN_INT,.value.int_val = 5};
    token_t token_string = {.type = TOKEN_STRING};
    token_t token_var = {.type = TOKEN_VARIABLE, .value.var_data.var_frame = GF};
    const char str_lit[] = "string";
    const char var_lit[] = "variable";
    STRING_FROM_LIT(token_string.value.string_val,str_lit)
    STRING_FROM_LIT(token_var.value.var_data.var_name,var_lit)
    
    token_t token_int_empty;
    token_t token_string_empty;
    token_t token_var_empty;

    token_move(&token_int,&token_int_empty);
    token_move(&token_string,&token_string_empty);
    token_move(&token_var,&token_var_empty);

    TEST_ASSERT_EQ(token_int_empty.type,TOKEN_INT,"new token should be int token")
    TEST_ASSERT_EQ(token_string_empty.type,TOKEN_STRING,"new token should be string token")
    TEST_ASSERT_EQ(token_var_empty.type,TOKEN_VARIABLE,"new token should be var token")

    const StringView str_val = VIEW_FROM_LIT(str_lit);
    const StringView var_val = VIEW_FROM_LIT(var_lit);

    TEST_ASSERT_EQ(token_int_empty.value.int_val,5,"new int token should have the correct value")
    TEST_ASSERT_EQ(sb_cmp(&token_string_empty.value.string_val,&str_val),0,"new string token should have the correct value")
    TEST_ASSERT_EQ(sb_cmp(&token_var_empty.value.var_data.var_name,&var_val),0,"new token should have the corrcet str value")
    TEST_ASSERT_EQ(token_var_empty.value.var_data.var_frame,GF,"new var token should have the correct frame value")

    TEST_ASSERT_EQ(token_string.value.string_val.data,NULL,"the old string token should be empty")
    TEST_ASSERT_EQ(token_var.value.var_data.var_name.data,NULL,"the old var token shoulc be empty")

    token_free(&token_int);
    token_free(&token_string);
    token_free(&token_var);

    token_free(&token_int_empty);
    token_free(&token_string_empty);
    token_free(&token_var_empty);

    TEST_SUCCES
}
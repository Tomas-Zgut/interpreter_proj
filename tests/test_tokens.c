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

CREATE_TEST(token_tests,is_literal) {
    token_t int_token = {.type = TOKEN_INT, .value = {.int_val = 1}};
    token_t string_token = {.type = TOKEN_STRING};
    token_t bool_token = {.type = TOKEN_BOOL, .value = {.boolean_val = false}};
    token_t float_token = {.type = TOKEN_FLOAT, .value = {.double_val = 1.0}};
    token_t nil_token = {.type = TOKEN_NIL};
    token_t label_token = {.type = TOKEN_LABEL };
    STRING_FROM_LIT(string_token.value.string_val,"string")
    STRING_FROM_LIT(label_token.value.string_val,"label")

    TEST_ASSERT(token_is_literal(&int_token)," Int Should be a literal")
    TEST_ASSERT(token_is_literal(&string_token)," String Should be a literal")
    TEST_ASSERT(token_is_literal(&bool_token)," Bool Should be a literal")
    TEST_ASSERT(token_is_literal(&float_token)," Float Should be a literal")
    TEST_ASSERT(token_is_literal(&nil_token)," Nil Should be a literal")
    TEST_ASSERT(!token_is_literal(&label_token)," Label Should not be a literal")

    token_free(&int_token);
    token_free(&string_token);
    token_free(&bool_token);
    token_free(&float_token);
    token_free(&nil_token);
    token_free(&label_token);

    TEST_SUCCES
}

CREATE_TEST(token_tests,is_varaible) {
    token_t int_token = {.type = TOKEN_INT, .value = {.int_val = 1}};
    token_t bool_token = {.type = TOKEN_BOOL, .value = {.boolean_val = false}};
    token_t float_token = {.type = TOKEN_FLOAT, .value = {.double_val = 1.0}};
    token_t var_token = {.type = TOKEN_VARIABLE, .value = {.var_data = {.var_frame = GF}}};
    STRING_FROM_LIT(var_token.value.var_data.var_name,"var")

    TEST_ASSERT(!token_is_variable(&int_token),"Int should not be a varaible!")
    TEST_ASSERT(!token_is_variable(&bool_token),"Bool should not be a varaible!")
    TEST_ASSERT(!token_is_variable(&float_token),"Float should not be a varaible!")
    TEST_ASSERT(token_is_variable(&var_token),"Variable should be a varaible!")

    token_free(&int_token);
    token_free(&bool_token);
    token_free(&float_token);
    token_free(&var_token);

    TEST_SUCCES
}

CREATE_TEST(token_tests,get_frame_string) {
    variable_t global_var = {.var_frame = GF};
    variable_t temp_var = {.var_frame = TF};
    variable_t local_var = {.var_frame = LF};
    STRING_FROM_LIT(global_var.var_name,"Gvar")
    STRING_FROM_LIT(temp_var.var_name,"Tvar")
    STRING_FROM_LIT(local_var.var_name,"Lvar")

    const StringView expected_g_var_res = VIEW_FROM_LIT("global");
    const StringView expected_l_var_res = VIEW_FROM_LIT("local");
    const StringView expected_t_var_res = VIEW_FROM_LIT("temporary");

    const StringView g_var_res = token_get_frame_view(&global_var);
    const StringView l_var_res = token_get_frame_view(&local_var); 
    const StringView t_var_res = token_get_frame_view(&temp_var); 

    TEST_ASSERT_EQ(sb_cmp(&expected_g_var_res,&g_var_res),0,"Returned strings should equal!")
    TEST_ASSERT_EQ(sb_cmp(&expected_l_var_res,&l_var_res),0,"Returned strings should equal!")
    TEST_ASSERT_EQ(sb_cmp(&expected_t_var_res,&t_var_res),0,"Returned strings should equal!")

    sb_free(&global_var.var_name);
    sb_free(&temp_var.var_name);
    sb_free(&local_var.var_name);

    TEST_SUCCES
}
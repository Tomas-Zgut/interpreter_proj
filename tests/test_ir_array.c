#include"tests_common.h"
#include<headders/ir_array.h>
#include<string.h>
CREATE_TEST(ir_array_tests,ir_array_init) {
    ir_array arr;
    TEST_MEM_CHECK(ir_array_init,&arr)

    TEST_ASSERT_EQ(arr.capacity,IR_ARRAY_INITIAL_SIZE,"capacity should bethe same as the init capacity")
    TEST_ASSERT_EQ(arr.size,0,"Size should be 0!")
    TEST_ASSERT_NEQ(arr.elements,NULL,"elements should be valid!")

    ir_array_free(&arr);

    TEST_SUCCES
}

CREATE_TEST(ir_array_tests,ir_array_free) {
    ir_array arr;
    TEST_MEM_CHECK(ir_array_init,&arr)

    ir_array_free(&arr);

    TEST_SUCCES
}


CREATE_TEST(ir_array_tests,ir_array_free_double) {
    ir_array arr;
    TEST_MEM_CHECK(ir_array_init,&arr)

    ir_array_free(&arr);
    ir_array_free(&arr);

    TEST_SUCCES
}

CREATE_TEST(ir_array_tests,ir_array_add_elm) {
    ir_array arr;
    TEST_MEM_CHECK(ir_array_init,&arr)
    token_t label_token = {.type = TOKEN_LABEL};
    STRING_FROM_LIT(label_token.value.string_val,"label")
    token_t *token_arr[1] = {&label_token};
    
    TEST_MEM_CHECK(ir_array_add_elm,&arr,INS_LABEL,token_arr)

    TEST_ASSERT_EQ(arr.size,1,"size should be 1")
    TEST_ASSERT_EQ(arr.elements[0].opcode,INS_LABEL,"shoulbe be correct")

    ir_array_free(&arr);

    TEST_SUCCES   
}

CREATE_TEST(ir_array_tests,ir_array_add_elm_multiple) {
    ir_array arr;
    TEST_MEM_CHECK(ir_array_init,&arr)
    token_t label_token = {.type = TOKEN_LABEL};
    STRING_FROM_LIT(label_token.value.string_val,"label")
    token_t *token_arr[1] = {&label_token};
    
    TEST_MEM_CHECK(ir_array_add_elm,&arr,INS_LABEL,token_arr)
    token_t var_token = {.type = TOKEN_VARIABLE, .value.var_data.var_frame = GF};
    STRING_FROM_LIT(var_token.value.var_data.var_name,"var")
    token_t *token_arr2[1] = {&var_token};

    TEST_MEM_CHECK(ir_array_add_elm,&arr,INS_DEFVAR,token_arr2)
    TEST_ASSERT_EQ(arr.size,2,"size should be 2")
    TEST_ASSERT_EQ(arr.elements[0].opcode,INS_LABEL,"shoulbe be correct")
    TEST_ASSERT_EQ(arr.elements[1].opcode,INS_DEFVAR,"shoulbe be correct")
    ir_array_free(&arr);

    TEST_SUCCES   
}

CREATE_TEST(ir_array_tests, ir_array_add_elm_with_resize) {
    ir_array arr;
    TEST_MEM_CHECK(ir_array_init,&arr)

    for (int i = 0; i < IR_ARRAY_INITIAL_SIZE; i++) {
        token_t num = {.type = TOKEN_INT, .value.int_val = 6};
        token_t *args[1] = {&num};
        TEST_MEM_CHECK(ir_array_add_elm,&arr,INS_EXIT,args)
    }
    token_t a = {.type = TOKEN_STRING};
    STRING_FROM_LIT(a.value.string_val,"string literal")
    token_t *args[1] = {&a};
    TEST_MEM_CHECK(ir_array_add_elm,&arr,INS_EXIT,args)

    ir_array_free(&arr);

    TEST_SUCCES
}

CREATE_TEST(ir_array_tests, ir_array_size_empty) {
    ir_array arr;
    TEST_MEM_CHECK(ir_array_init,&arr)

    TEST_ASSERT_EQ(ir_array_size(&arr),0,"empty arr size should be 0!")

    ir_array_free(&arr);

    TEST_SUCCES
}

CREATE_TEST(ir_array_tests, ir_array_size) {
    ir_array arr;
    TEST_MEM_CHECK(ir_array_init,&arr)
    token_t label_token = {.type = TOKEN_LABEL};
    STRING_FROM_LIT(label_token.value.string_val,"label")
    token_t *token_arr[1] = {&label_token};
    
    TEST_MEM_CHECK(ir_array_add_elm,&arr,INS_LABEL,token_arr)

    TEST_ASSERT_EQ(ir_array_size(&arr),1,"size of array with one element should be 1")

    ir_array_free(&arr);

    TEST_SUCCES
}

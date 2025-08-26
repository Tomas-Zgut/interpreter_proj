#include"tests_common.h"
#include<headders/opcodes.h>

CREATE_TEST(opcode_tests,get_number_operrands) {

    TEST_ASSERT_EQ(get_number_opperands(INS_BREAK),0,"number of operands should be 0")
    TEST_ASSERT_EQ(get_number_opperands(INS_CALL),1,"number of operands should be 1")
    TEST_ASSERT_EQ(get_number_opperands(INS_INT2CHAR),2 ,"number of operands should be 2")
    TEST_ASSERT_EQ(get_number_opperands(INS_ADD),3,"number of operands should be 3")

    TEST_SUCCES
}

CREATE_TEST(opcode_tests,compare_operand_types) {

    
    TEST_ASSERT(compare_operand_types(OP_TYPE,OP_TYPE),"types are compatible")
    TEST_ASSERT(compare_operand_types(OP_LABEL,OP_LABEL),"labels are compatible")
    TEST_ASSERT(compare_operand_types(OP_VARIABLE,OP_VARIABLE),"variables are compatible")
    TEST_ASSERT(compare_operand_types(OP_SYMBOL,OP_SYMBOL),"symbols are compatible")
    TEST_ASSERT(compare_operand_types(OP_VARIABLE,OP_SYMBOL),"variable and symbol are compatible")

    TEST_ASSERT(!compare_operand_types(OP_SYMBOL,OP_VARIABLE),"symbol and variable are notcompatible")
    TEST_ASSERT(!compare_operand_types(OP_TYPE,OP_LABEL),"type and label are not compatible!") //sanity check

    TEST_SUCCES
}   

CREATE_TEST(opcode_tests,get_operand_type) {

    TEST_ASSERT_EQ(get_operand_type(INS_LABEL,1),OP_LABEL,"1st operand of ins label is a label")
    TEST_ASSERT_EQ(get_operand_type(INS_DEFVAR,1),OP_VARIABLE,"1st operand of ins defvar is a variable")
    TEST_ASSERT_EQ(get_operand_type(INS_READ,2),OP_TYPE,"2nd operad of ins read is a type")
    TEST_ASSERT_EQ(get_operand_type(INS_JUMPNEQ,3),OP_SYMBOL,"3rd operand of ins jumpneq is a symbol")

    TEST_SUCCES
}

CREATE_TEST(opcode_tests,has_label) {

    TEST_ASSERT(has_label(INS_LABEL),"label ins has a label operand")
    TEST_ASSERT(has_label(INS_JUMP),"jump instrucion has a label operand")
    TEST_ASSERT(has_label(INS_JUMPEQ),"conditional jump instrucion has a label")
    TEST_ASSERT(has_label(INS_JUMPNEQ),"conditional jump instruction has a label")
    TEST_ASSERT(has_label(INS_CALL),"call instruction has a label")
    TEST_ASSERT(!has_label(INS_RETURN),"return instruction does not have a label")

    TEST_SUCCES
}

#include<headders/type_cheks.h>

/**
 * @brief macro for cerating the result of type checking
 * 
 * @param res: result of the check
 * @param idx: index of the failed argument
 */
#define MAKE_TYPE_CHECK_RES(res,idx)    \
(arg_type_check_t) {                    \
    .result=res,                        \
    .failed_arg=idx,                    \
}

arg_type_check_t instruction_args_valid(opcode_type opcode, token_t **arg_tokens) {
    assert(arg_tokens != NULL);
    const int num_args = get_number_opperands(opcode);
    
    for (int i = 0; i <num_args;i++) {

        const operand_type_t operand = get_operand_type(opcode,i+1);
        const operand_type_t arg_type = token_to_opcode(arg_tokens[i]->type);

        if(!compare_operand_types(operand,arg_type)) {
            return MAKE_TYPE_CHECK_RES(false,i);
        }
    }

    return MAKE_TYPE_CHECK_RES(true,ALL_ARGS_VALID);
}

/**
 * @brief maping of token types to operand types
 */
static const operand_type_t token_to_opcode_map[] = {
    [TOKEN_INT]=OP_SYMBOL,
    [TOKEN_STRING]=OP_SYMBOL,
    [TOKEN_BOOL]=OP_SYMBOL,
    [TOKEN_NIL]=OP_SYMBOL,
    [TOKEN_VARIABLE]=OP_VARIABLE,
    [TOKEN_LABEL]=OP_LABEL,
    [TOKEN_TYPE]=OP_TYPE,
};


operand_type_t token_to_opcode(token_type type) {
    assert(type>TOKEN_INSTRUCTION && type<TOKEN_UNKNOWN); // must be token of instrucion argument type

    return token_to_opcode_map[type];
}
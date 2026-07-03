#include <headders/interpreter.h>

#define CHECK_COPY(res)     \
if (res == 0) {             \
    return INTERNAL_ERROR;  \
}


int_ret_code interpret_ir(ir* ir,Memory *mem, const jump_table_t* jump_table) {
    int code_pos = 0;
    int next_code_pos = 0;
    while (true)
    {
        ir_elm_t instruction = ir->elements[code_pos]; 
        next_code_pos = code_pos + 1;
        switch (ir->elements[code_pos].opcode) {

            case INS_MOVE:
                variable_t dest_var = instruction.operands[0].value.var_data;
                memory_access_res_mut_t dest_val_res = memory_get_variable_mut(mem,&dest_var);
                CHECK_MEMORY_ACCESS(dest_val_res.error,&dest_var)
                
                // move the value out of a token literal do not copy
                if (token_is_literal(instruction.operands+1)) {
                    memory_value_t src_val = token_literal_to_memory_value(instruction.operands+1);
                    (*dest_val_res.value) = memory_value_move(&src_val);
                } else { // copy the value of a variable
                    variable_t src = instruction.operands[1].value.var_data;
                    memory_access_res_t src_val_res = memory_get_variable(mem,&src);
                    CHECK_MEMORY_ACCESS(src_val_res.error,&src)
                    CHECK_COPY(memory_value_copy(dest_val_res.value,src_val_res.value));
                }   
                break;
            case INS_CREATE_F:
                break;
            case INS_PUSH_F:
                break;
            case INS_POP_F:
                break;
            case INS_DEFVAR:
                break;
            case INS_CALL:
                break;
            case INS_RETURN:
                break;
            case INS_PUSHS:
                break;
            case INS_POPS:
                break;
            case INS_ADD:
                break;
            case INS_SUB:
                break;
            case INS_MUL:
                break;
            case INS_IDIV:
                break;
            case INS_LT:
                break;
            case INS_GT:
                break;
            case INS_EQ:
                break;
            case INS_AND:
                break;
            case INS_OR:
                break;
            case INS_NOT:
                break;
            case INS_INT2CHAR:
                break;
            case INS_STR2INT:
                break;
            case INS_READ:
                break;
            case INS_WRITE:
                break;
            case INS_CONCAT:
                break;
            case INS_STRLEN:
                break;
            case INS_GETCHAR:
                break;
            case INS_SETCHAR:
                break;
            case INS_TYPE:
                break;
            case INS_LABEL:
                // does nothing ... should be skipped
                break;
            case INS_JUMP:
                const StringView label_veiw = sb_get_view(&instruction.operands[0].value.string_val,0);
                const jump_table_entry_t *entry =  jump_table_lookup(jump_table,&label_veiw);
                next_code_pos = entry->destination;
                break;
            case INS_JUMPEQ:
                break;
            case INS_JUMPNEQ:
                break;
            case INS_EXIT:
                return instruction.operands[0].value.int_val;
            case INS_DPRINT:
                break;
            case INS_BREAK:
                break;
        
        default:
            break;
        }
        code_pos = next_code_pos;
        
    }
    return PROGRAM_OK;
}

/**
 * @brief Macro for creating memory access results
 * 
 * @param val: value member
 * @param err: error member
 * 
 * @see memory_access_res_t
 * 
 * @returns memory acccess result
 */
#define MAKE_MA_RES_T(val,err)      \
(memory_access_res_t) {             \
    .value=val,                     \
    .error=(memory_access_res)err   \
}


/**
 * @brief Macro for creating mutable memory access results
 * 
 * @param val: value member
 * @param err: error member
 * 
 * @see memory_access_res_mut_t
 * 
 * @returns mutable memory acccess result
 */
#define MAKE_MA_RES_MUT_T(val,err)  \
(memory_access_res_mut_t) {         \
    .value=(memory_value_t *) val,  \
    .error=(memory_access_res)err   \
}

/**
 * @brief Macro used for checking the access to a given memory frame.
 * 
 * @par Macro assumes that a memory pointer called `mem` and string view 
 * called `var_name` already exist in a given scope.
 * 
 * @param func: function to call
 * @param RES_MACRO: macro to use to create the result
 */
#define _CHECK_FRAME_ACCESS(func,RES_MACRO)             \
do {                                                    \
    const memory_value_t* res = func(mem,&var_name);    \
    if (res == NULL) {                                  \
        return RES_MACRO(NULL,UNDEFINED_VAR);           \
    }                                                   \
    return RES_MACRO(res,MEM_ACCESS_OK);                \
}while(0);

memory_access_res_t memory_get_variable(const Memory* mem, const variable_t* var) {
    const StringView var_name = sb_get_view(&var->var_name,0);
    switch (var->var_frame)
    {
        case GF:
            _CHECK_FRAME_ACCESS(memory_global_frame_get,MAKE_MA_RES_T)
        case TF:
            if (!memory_temp_frame_valid(mem)) {
                return MAKE_MA_RES_T(NULL,NO_FRAME);
            }
            _CHECK_FRAME_ACCESS(memory_temp_frame_get,MAKE_MA_RES_T)
        case LF:
            if (!memory_local_frame_valid(mem)) {
                return MAKE_MA_RES_T(NULL,NO_FRAME);
            }
            _CHECK_FRAME_ACCESS(memory_local_frame_get,MAKE_MA_RES_T)
        default:
            assert(false);
            return MAKE_MA_RES_T(NULL,INTERNAL_ERROR);
    }
}

memory_access_res_mut_t memory_get_variable_mut(const Memory* mem, const variable_t* var) {
    const StringView var_name = sb_get_view(&var->var_name,0);
    switch (var->var_frame)
    {
        case GF:
            _CHECK_FRAME_ACCESS(memory_global_frame_get_mut,MAKE_MA_RES_MUT_T)
        case TF:
            if (!memory_temp_frame_valid(mem)) {
                return MAKE_MA_RES_MUT_T(NULL,NO_FRAME);
            }
            _CHECK_FRAME_ACCESS(memory_temp_frame_get_mut,MAKE_MA_RES_MUT_T)
        case LF:
            if (!memory_local_frame_valid(mem)) {
                return MAKE_MA_RES_MUT_T(NULL,NO_FRAME);
            }
            _CHECK_FRAME_ACCESS(memory_local_frame_get_mut,MAKE_MA_RES_MUT_T)
        default:
            assert(false);
            return MAKE_MA_RES_MUT_T(NULL,INTERNAL_ERROR);
    }
}


memory_access_res memory_create_varaible(Memory *mem, const variable_t *var, const memory_value_t *value) {
    const StringView var_name = sb_get_view(&var->var_name,0);
    switch (var->var_frame)
    {
        case GF:
            if (!memory_global_frame_insert(mem,&var_name,value)) {
                return VAR_REDECLARATION;
            }
            return MEM_ACCESS_OK;
        case TF:
            if (!memory_temp_frame_valid(mem)) {
                return NO_FRAME;
            }
            if (!memory_temp_frame_insert(mem,&var_name,value)) {
                return VAR_REDECLARATION;
            }
            return MEM_ACCESS_OK;

        case LF:
            if (!memory_local_frame_valid(mem)) {
                return NO_FRAME;
            }
            if (!memory_global_frame_insert(mem,&var_name,value)) {
                return VAR_REDECLARATION;
            }
            return MEM_ACCESS_OK;
        default:
            assert(false);
            return (memory_access_res)INTERNAL_ERROR;
    }

}

memory_value_t token_literal_to_memory_value(token_t *token) {
    assert(token_is_literal(token));

    switch (token->type)
    {
    case TOKEN_BOOL:
        return memory_value_create(&token->value.boolean_val);
    case TOKEN_INT:
        return memory_value_create(&token->value.int_val);
    case TOKEN_NIL:
        nill_t val;
        return memory_value_create(&val);
    case TOKEN_STRING:
        return memory_value_create(&token->value.string_val);
    case TOKEN_FLOAT:
        return memory_value_create(&token->value.double_val);
    default:
        assert(false); // should not happen
        return memory_value_create_default();
        break;
    }
}
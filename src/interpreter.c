#include <headders/interpreter.h>

int interpret_ir(const ir* ir,Memory *mem, const jump_table_t* jump_table) {
    int code_pos = 0;
    int code_ret_val = 0;
    int code_pos_move = 1;
    while (true)
    {
        ir_elm_t instruction = ir->elements[code_pos]; 

        switch (ir->elements[code_pos].opcode) {
            case INS_MOVE:
                variable_t var =  instruction.operands[0].value.var_data;
                variable_t var2 = instruction.operands[1].value.var_data;
                const StringView var_name = sb_get_view(&var.var_name,0);
                const StringView var2_name = sb_get_view(&var2.var_name,0);
                const memory_value_t *var2_value = memory_global_frame_get(mem,&var2_name);
                memory_value_t *var_value = memory_local_frame_get_mut(mem,&var_name);
                memory_value_copy(var_value,var2_value);
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
                break;
            case INS_JUMP:
                break;
            case INS_JUMPEQ:
                break;
            case INS_JUMPNEQ:
                break;
            case INS_EXIT:
                break;
            case INS_DPRINT:
                break;
            case INS_BREAK:
                break;
        
        default:
            code_ret_val;
            break;
        }
        
    }
    return 0;
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
#define MAKE_MA_RES_T(val,err)  \
(memory_access_res_t) {         \
    .value=val,                 \
    .error=err                  \
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
    .value=val,                     \
    .error=err                      \
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
#define _CHECK_FRAME_ACCESS(func,RES_MACRO)                             \
do {                                                                    \
    const memory_value_t* res = memory_global_frame_get(mem,&var_name); \
    if (res == NULL) {                                                  \
        return RES_MACRO(NULL,UNDEFINED_VAR);                           \
    }                                                                   \
    return RES_MACRO(res,MEM_ACCESS_OK);                                \
}while(0);

memory_access_res_t memory_get_variable(const Memory* mem, const variable_t* var) {
    const StringView var_name = sb_get_view(&var->var_name,0);
    memory_value_t *result = NULL;
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
    memory_value_t *result = NULL;
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
            return INTERNAL_ERROR;
    }

}
#include <headders/parser.h>
#include <headders/memory.h>
#include <headders/memory_value.h>
int interpret_ir(const ir* ir,Memory *mem) {
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


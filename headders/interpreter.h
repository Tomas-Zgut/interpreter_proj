#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__
#include "jump_table.h"
#include "memory.h"
#include "ir_array.h"
#include "ret_codes.h"

/**
 * @brief enum representing errors when doing a memory access
 */
typedef enum {
    MEM_ACCESS_OK = 0,
    UNDEFINED_VAR=UNDEFINED_VARIABLE_ACCESS,
    NO_STACK=MISSING_STACK_VALUE,
    NO_FRAME=NONEXISTING_MEMORY_FRAME,
    VAR_REDECLARATION=SEMANTIC_ERROR,
} memory_access_res;

/**
 * @brief struct representing the result of a memory read
 */
typedef struct {
    const memory_access_res error;  // result of the memory read
    const memory_value_t* value;    // value read from memory
} memory_access_res_t;

/**
 * @brief struct rerepsenting the result of a mutable memory read
 */
typedef struct {
    const memory_access_res error;  // result of the memory read
    memory_value_t* value;          // value read from memory
} memory_access_res_mut_t;

/**
 * @brief Function interprets the passed in ir
 * 
 * @param ir: pointer to the programs ir
 * @param mem: pointer to the interpreters memory
 * @param jump_table: pointer to the jump table
 * 
 * @returns the appropriate interpreter return code
 */
int_ret_code interpret_ir(ir* ir,Memory *mem, const jump_table_t* jump_table);

/**
 * @brief Function to crate a new variable in memory with a given value
 * 
 * @param mem: pointer to memory
 * @param var: pointer to a varaible
 * @param value: pointer to a value of the variable
 * 
 * @returns memory_access_res that holds the correct error code to return
 */
memory_access_res memory_create_variable(Memory*mem, const variable_t *var, const memory_value_t *value);

/**
 * @brief Function to crate a new variable in memory with a default value
 * 
 * @param mem: pointer to memory
 * @param var: pointer to a varaible
 * 
 * @returns memory_access_res that holds the correct error code to return
 */
static inline memory_access_res memory_create_variable_default(Memory*mem, const variable_t *var) {
    const memory_value_t val = memory_value_create_default();
    return memory_create_variable(mem,var,&val);
}

/**
 * @brief Function to retrieve a variable value from memory.
 * 
 * @par Function tries to retrieve a given variable from memory. On success the `value` member of the 
 * return value is a valid const pointer to the value stored in memory, and the `error` member is set to `MEM_ACCESS_OK`.
 * Otherwise the `error` member holds the type of error and the `value` member is undefined.
 * 
 * @param mem: poiner to memory
 * @param var: pointer to a variable
 * 
 * @returns memory_access_res_t with `value` and `erorr` members set.
 * 
 * @see memory_access_res_t
 */
memory_access_res_t memory_get_variable(const Memory* mem, const variable_t *var);

/**
 * @brief Function to retrieve a mutable variable value from memory.
 * 
 * @par Function tries to retrieve a given variable from memory. On success the `value` member of the 
 * return value is a valid pointer to the value stored in memory, and the `error` member is set to `MEM_ACCESS_OK`.
 * Otherwise the `error` member holds the type of error and the `value` member is undefined.
 * 
 * @param mem: poiner to memory
 * @param var: pointer to a variable
 * 
 * @returns memory_access_res_mut_t with `value` and `erorr` members set.
 * 
 * @see memory_access_res_mut_t
 */
memory_access_res_mut_t memory_get_variable_mut(const Memory* mem, const variable_t *var);

/**
 * @brief Function that converts a token literal into a memory value
 * 
 * 
 * @param token: pointer to a token
 * 
 * @returns `memory_value_t` of the stored value in the token
 * 
 * @warning There is an assertion if the passed in token is not a literal!
 * 
 * @note This function moves the ownership of data from the token to the returned value.
 */
memory_value_t token_literal_to_memory_value(token_t *token);

/**
 * @brief Macro for checking the results of a memory operation.
 * 
 * @par Macro checks the result of a memory operation, than based on the result
 * prints out an error message onto stdout and returns from the function.
 * 
 * @param value: the return code of a memory operation
 * @param var: pointer to a variable
 * 
 * @see variable_t
 * @see _check_ma_ret_val_impl
 * @see memory_access_res_t
 */
#define CHECK_MEMORY_ACCESS(value,var)                                  \
do {                                                                    \
    const memory_access_res err = _check_ma_ret_val_impl(value,var);    \
    if (err != MEM_ACCESS_OK) {                                         \
        return (int_ret_code)err;                                       \
    }                                                                   \
} while(0);


/**
 * @brief Function implementing the `CHECK_MEMORY_ACCESS` macro behavior
 * 
 * @param result: return code from a memory operation
 * @param var: pointer to a variable
 * 
 * @returns the value of @p result
 */
static inline memory_access_res _check_ma_ret_val_impl(memory_access_res result, const variable_t *var) {
    const StringView frame_view = token_get_frame_view(var);
    switch (result)
    {
        case MEM_ACCESS_OK:
            return MEM_ACCESS_OK;
        case UNDEFINED_VAR:
            fprintf(stderr, "Undefined variable: ");
            sb_eprint(&frame_view,"@");
            sb_eprint(&var->var_name,"!\n");
            return UNDEFINED_VAR;
        case NO_FRAME:
            fprintf(stderr, "No ");
            sb_eprint(&frame_view," frame is defined!\n");
            return NO_FRAME;
        case VAR_REDECLARATION:
            fprintf(stderr,"Varaible redaclaration: ");
            sb_eprint(&frame_view,"@");
            sb_eprint(&var->var_name,"!\n");
            return VAR_REDECLARATION;  
        case NO_STACK: // todo!!!!
            return NO_STACK;
        default:
            assert(false);
            return NO_FRAME;
    }
}

#endif
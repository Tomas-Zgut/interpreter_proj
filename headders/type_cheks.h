#ifndef __TYPE_CHECKS_H__
#define __TYPE_CHECKS_H__
#include "opcodes.h"
#include "tokens.h"

/**
 * @brief struct for returning from `instrucion_args_valid`
 * 
 * @see instrucion_args_valid
 */
typedef struct {
    const bool result;          // result of the check
    const int8_t failed_arg;    // which arg failed to pass
} arg_type_check_t;

#define ALL_ARGS_VALID -1 // all intruction arguments are valid

/**
 * @brief function to check if given argument list is valid for a given instruction
 * 
 * @param opcode: opcode of an instruction
 * @param arg_tokens: tokens of instruction args
 * 
 * @returns arg_type_chek_t.
 * if all aguments are valid .result is true and .failed arg is `ALL_ARGS_VALID`.
 * Othewise .result is false and .failed_arg is the index fo the wrong argument
 * 
 * @see arg_type_check_t
 */
arg_type_check_t instruction_args_valid(opcode_type opcode, token_t **arg_tokens);

/**
 * @brief function maps a token type of an instruction argument to its type
 * 
 * @param type: token type to convert
 * 
 * @returns instruction argument for a given type of token
 */
operand_type_t token_to_opcode(token_type type);
#endif
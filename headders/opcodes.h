#ifndef __OPCODES_H__
#define __OPCODES_H__
#include <stdbool.h>
#include<assert.h>

#define INS_NUMBER(x) (x << 24)
#define OPERAND_1(x) (x << 16)
#define OPERAND_2(x) (x << 8)
#define OPERAND_3(x) (x << 0)

#define OP_COUNT_MASK (3u << 30)
#define OP_TYPE_MASK 0xFF
/**
 * @brief enum represent the number of
 * operands an instruction can have
 */
typedef enum {
	OP_COUNT_0 = 0 << 30,
	OP_COUNT_1 = 1 << 30,
	OP_COUNT_2 = 2 << 30,
	OP_COUNT_3 = 3 << 30,
} operand_count_t;
/**
 * @brief enum represents the type of an operand
 * for a given instruction.
 */
typedef enum {
	OP_SYMBOL = 0b1,
	OP_VARIABLE = 0b11, // variable  is compatible with a symbol
	OP_LABEL = 0b100,
	OP_TYPE = 0b1000,
} operand_type_t;

/**
 * @brief enum for represeting instruction opcodes.
 *
 * @par Each opcode for an instruction is 4 bytes.
 * The 1st byte holds the number of opernads for a given instruction
 * and the instruction number. The other 3 bytes each hold information
 * about the type of each operand
 */
typedef enum {
	INS_UNKNOWN = 0,
	INS_MOVE = OP_COUNT_2 | INS_NUMBER(10) | OPERAND_1(OP_VARIABLE) |
			   OPERAND_2(OP_SYMBOL),
	INS_CREATE_F = OP_COUNT_0 | INS_NUMBER(35),
	INS_PUSH_F = OP_COUNT_0 | INS_NUMBER(32),
	INS_POP_F = OP_COUNT_0 | INS_NUMBER(33),
	INS_DEFVAR = OP_COUNT_1 | INS_NUMBER(22) | OPERAND_1(OP_VARIABLE),
	INS_CALL = OP_COUNT_1 | INS_NUMBER(11) | OPERAND_1(OP_LABEL),
	INS_RETURN = OP_COUNT_0 | INS_NUMBER(23),
	INS_PUSHS = OP_COUNT_1 | INS_NUMBER(18) | OPERAND_1(OP_SYMBOL),
	INS_POPS = OP_COUNT_0 | INS_NUMBER(12),
	INS_ADD = OP_COUNT_3 | INS_NUMBER(5) | OPERAND_1(OP_VARIABLE) |
			  OPERAND_2(OP_SYMBOL) | OPERAND_3(OP_SYMBOL),
	INS_SUB = OP_COUNT_3 | INS_NUMBER(6) | OPERAND_1(OP_VARIABLE) |
			  OPERAND_2(OP_SYMBOL) | OPERAND_3(OP_SYMBOL),

	INS_MUL = OP_COUNT_3 | INS_NUMBER(7) | OPERAND_1(OP_VARIABLE) |
			  OPERAND_2(OP_SYMBOL) | OPERAND_3(OP_SYMBOL),
	INS_IDIV = OP_COUNT_3 | INS_NUMBER(13) | OPERAND_1(OP_VARIABLE) |
			   OPERAND_2(OP_SYMBOL) | OPERAND_3(OP_SYMBOL),
	INS_LT = OP_COUNT_3 | INS_NUMBER(1) | OPERAND_1(OP_VARIABLE) |
			 OPERAND_2(OP_SYMBOL) | OPERAND_3(OP_SYMBOL),
	INS_GT = OP_COUNT_3 | INS_NUMBER(2) | OPERAND_1(OP_VARIABLE) |
			 OPERAND_2(OP_SYMBOL) | OPERAND_3(OP_SYMBOL),
	INS_EQ = OP_COUNT_3 | INS_NUMBER(3) | OPERAND_1(OP_VARIABLE) |
			 OPERAND_2(OP_SYMBOL) | OPERAND_3(OP_SYMBOL),
	INS_AND = OP_COUNT_3 | INS_NUMBER(8) | OPERAND_1(OP_VARIABLE) |
			  OPERAND_2(OP_SYMBOL) | OPERAND_3(OP_SYMBOL),
	INS_OR = OP_COUNT_3 | INS_NUMBER(4) | OPERAND_1(OP_VARIABLE) |
			 OPERAND_2(OP_SYMBOL) | OPERAND_3(OP_SYMBOL),
	INS_NOT = OP_COUNT_3 | INS_NUMBER(9) | OPERAND_1(OP_VARIABLE) |
			  OPERAND_2(OP_SYMBOL) | OPERAND_3(OP_SYMBOL),
	INS_INT2CHAR = OP_COUNT_2 | INS_NUMBER(30) | OPERAND_1(OP_VARIABLE) |
				   OPERAND_2(OP_SYMBOL),
	INS_STR2INT = OP_COUNT_3 | INS_NUMBER(27) | OPERAND_1(OP_VARIABLE) |
				  OPERAND_2(OP_SYMBOL) | OPERAND_3(OP_SYMBOL),
	INS_READ =
		OP_COUNT_2 | INS_NUMBER(14) | OPERAND_1(OP_SYMBOL) | OPERAND_2(OP_TYPE),
	INS_WRITE = OP_COUNT_1 | INS_NUMBER(19) | OPERAND_1(OP_SYMBOL),
	INS_CONCAT = OP_COUNT_3 | INS_NUMBER(24) | OPERAND_1(OP_VARIABLE) |
				 OPERAND_2(OP_SYMBOL) | OPERAND_3(OP_SYMBOL),
	INS_STRLEN = OP_COUNT_2 | INS_NUMBER(25) | OPERAND_1(OP_VARIABLE) |
				 OPERAND_2(OP_SYMBOL),
	INS_GETCHAR = OP_COUNT_3 | INS_NUMBER(28) | OPERAND_1(OP_VARIABLE) |
				  OPERAND_2(OP_SYMBOL) | OPERAND_3(OP_SYMBOL),
	INS_SETCHAR = OP_COUNT_3 | INS_NUMBER(29) | OPERAND_1(OP_VARIABLE) |
				  OPERAND_2(OP_SYMBOL) | OPERAND_3(OP_SYMBOL),
	INS_TYPE = OP_COUNT_2 | INS_NUMBER(15) | OPERAND_1(OP_VARIABLE) |
			   OPERAND_2(OP_SYMBOL),
	INS_LABEL = OP_COUNT_1 | INS_NUMBER(20) | OPERAND_1(OP_LABEL),
	INS_JUMP = OP_COUNT_1 | INS_NUMBER(16) | OPERAND_1(OP_LABEL),
	INS_JUMPEQ = OP_COUNT_3 | INS_NUMBER(31) | OPERAND_1(OP_LABEL) |
				 OPERAND_2(OP_SYMBOL) | OPERAND_3(OP_SYMBOL),
	INS_JUMPNEQ = OP_COUNT_3 | INS_NUMBER(34) | OPERAND_1(OP_LABEL) |
				  OPERAND_2(OP_SYMBOL) | OPERAND_3(OP_SYMBOL),
	INS_EXIT = OP_COUNT_1 | INS_NUMBER(17) | OPERAND_1(OP_SYMBOL),
	INS_DPRINT = OP_COUNT_1 | INS_NUMBER(26) | OPERAND_1(OP_SYMBOL),
	INS_BREAK = OP_COUNT_0 | INS_NUMBER(21),
} opcode_type;

/**
 * @brief function to extract the number of operands from an opcode
 *
 * @param opcode: instruction opcode
 *
 * @returns number of operands for an intruction
 */
static inline int get_number_opperands(opcode_type opcode) {
	return (opcode & OP_COUNT_MASK) >> 30;
}

/**
 * @brief function to extract the type of an operand from an opcode
 *
 * @param opcode: instruction opcode
 * @param operand: operand numer 
 *
 * @returns the type of a operand for an instruction
 */
static inline operand_type_t get_operand_type(opcode_type opcode, int operand) {
	assert(operand > 0 || operand < 4);

	return opcode >> ((3 - operand) * 8) & OP_TYPE_MASK;
}

/**
 * @brief function to compare 2 operand types
 *
 * @param opt1: operand type from the instrucion
 * @param opt2: operand type from the token
 *
 * @returns true if operands are compatible, false otherwise
 */
static inline bool compare_operand_types(operand_type_t opt1,
										 operand_type_t opt2) {
	return (opt1 & opt2) == opt2;
}

/**
 * @brief function to check if an instruction has a label operand
 * 
 * @param opcode: instruction opcode
 * 
 * @returns true if instrucion has label operand, false otherwise
 */
static inline bool has_label(opcode_type opcode) {
	return (opcode & OPERAND_1(OP_LABEL)) == OPERAND_1(OP_LABEL);
}
#endif
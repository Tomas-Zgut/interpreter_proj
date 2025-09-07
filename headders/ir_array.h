#ifndef __PARSER_IR_H__
#define __PARSER_IR_H__
#include "opcodes.h"
#include "tokens.h"

#define MAX_OP_COUNT 3 // maximum munber of operands per insturcion
#define IR_ARRAY_INITIAL_SIZE 32 // initial size of the growable ir array

/**
 * @brief struct representing an element
 * of the intermediate representaion of the program
 */
typedef struct
{
	opcode_type opcode; 			// opcode of the insturcion
	token_t operands[MAX_OP_COUNT];	// intstucion opperands
} ir_elm_t;

/**
 * @brief strcut for the growable array of that is
 * used to build up the ir of the program.
 */
typedef struct
{
	ir_elm_t *elements; // array of ir elements
	uint64_t size;		// size of the array
	uint64_t capacity;	// capacity of the array
} ir_array;

/**
 * @brief struct for the ir of the program the
 * interpreter will use
 */
typedef struct
{
	ir_elm_t *elements;		// array of ir elements
	const uint64_t size;	// size of the array
} ir;

/**
 * @brief function to initialize an ir_array
 * 
 * @param array[out]: pointer to an array to be initialized
 * 
 * @returns true if the initialization was successfull, false otherwise
 */
bool ir_array_init(ir_array *array);

/**
 * @brief function for adding a new element to the array
 * 
 * @par Function returns a pointer to memory where the next element of the
 * array will be. I needs to be initalized by the user. If the array is too small
 * this function triggres its growth. If the growth fails function returns a NULL pointer.
 * 
 * @param array: pointer to an array to insert a new element into
 * @param op_code: op code of an instrucion
 * @param args_array: array tokens that hold the arguments of the isntrucion
 * 
 * @returns true if the operation is successful, false otherwise.
 */
bool ir_array_add_elm(ir_array *array,opcode_type op_code, token_t **args_array);

/**
 * @brief function to get the current size of the array
 * 
 * @param array: pointer to an array
 * 
 * @returns the size of the specified array
 */
uint64_t ir_array_size(const ir_array *array);

/**
 * @brief functio to free the array
 * 
 * @param array:pointer to an array
 */
void ir_array_free(ir_array *array);

/**
 * @brief function to provide read only access to a given element in the ir
 * 
 * @param array: pointer to the ir of the program
 * @param index: index of an element to access
 * 
 * @returns a pointer to the ir_elm_t at the given index
 */
const ir_elm_t *ir_index_elm(const ir *array, uint64_t index);

/**
 * @brief function to free the programs ir
 * 
 * @param ir: pointer to the programs ir
 */
void ir_free(ir *ir);

#endif
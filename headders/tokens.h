#ifndef __TOKENS_H__
#define __TOKENS_H__

#include "opcodes.h"
#include "string_buffer.h"
#include <stdint.h>
/**
 * @brief enum for types of tokens
 */
typedef enum {
	TOKEN_INSTRUCTION,	// token for instructions
	TOKEN_INT,			// token for integer literals
	TOKEN_STRING,		// token for string literals
	TOKEN_BOOL,			// token for boolean literals
	TOKEN_NIL,			// token for nill literals
	TOKEN_VARIABLE,		// token for varaible names
	TOKEN_LABEL,		// token for labels
	TOKEN_TYPE,			// token for the data type
	TOKEN_UNKNOWN,		// unknown token
	TOKEN_EOF,			// singaling end of file
} token_type;

/**
 * @brief enum for frames where varaibles are located
 */
typedef enum {
	GF,	// global frame
	LF,	// local frame
	TF,	// temoporary frame
} frame_type;

/**
 * @brief struct storing data for a variable name
 */
typedef struct {
	frame_type var_frame;	// frame where varaible is from	
	String var_name;		// name of the varaible
} variable_t;

/**
 * @brief union for storing value of a token
 */
typedef union {
	String string_val;		// string literal
	uint64_t int_val;		// integer literal
	bool boolean_val;		// boolean literal
	double double_val;		// doube literal
	variable_t var_data;	// data for a variable
	opcode_type ins_opcode;	// opcode of an intruction
} token_value_t;

/**
 * @brief struct representing a token
 */
typedef struct {
	token_type type;		// type of a token
	token_value_t value;	// value of a token
} token_t;

/**
 * @brief funcion to free a token's memory
 * 
 * @param token: pointer to a token
 */
void token_free(token_t *token);

/**
 * @brief function to initialize a token by transfering ownership from another token.
 * 
 * @par Funcion is used to trnasfer token data without
 * the need to copy strings. Only use this when the original
 * token data will not be used since this transfers the ownership
 * from @p src_token to @p dest_token and leaves the @p src_token in
 * uninitialized state. The data owned by @p dest_token
 * 
 * @warning Calling this function more than once on a given src_token triggers an assertion
 * 
 * @param[in] src_token: source token that transfers data ownership
 * @param[out] dest_token: destination token that receives the data ownership
 */ 
void token_move(token_t * src_token, token_t* dest_token);
#endif
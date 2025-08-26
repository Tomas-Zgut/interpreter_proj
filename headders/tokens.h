#ifndef __TOKENS_H__
#define __TOKENS_H__

#include "opcodes.h"
#include "string_buffer.h"
#include <stdint.h>
typedef enum {
	TOKEN_INSTRUCTION,
	TOKEN_INT,
	TOKEN_STRING,
	TOKEN_BOOL,
	TOKEN_NIL,
	TOKEN_VARIABLE,
	TOKEN_LABEL,
	TOKEN_TYPE,
	TOKEN_UNKNOWN,
	TOKEN_EOF,
} token_type;

typedef enum {
	GF,
	LF,
	TF,
} frame_type;

typedef struct {
	String var_name;
	frame_type var_frame;
} variable_t;

typedef union {
	String string_val;
	uint64_t int_val;
	variable_t var_data;
	opcode_type ins_opcode;
} token_value_t;

typedef struct {
	token_type type;
	token_value_t value;
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
 * @param src_token[in]: source token that transfers data ownership
 * @param dest_token[out]: destination token that receives the data ownership
 */ 
void token_move(token_t * src_token, token_t* dest_token);
#endif
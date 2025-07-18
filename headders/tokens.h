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
	TOKEN_UNKNOWN,
	TOKEN_EOF,
} token_type;

typedef enum {
	GF,
	LF,
	TF,
} frame_type;

typedef StringData string_t;

typedef uint64_t integer_t;

typedef struct {
	char *name;
	uint64_t name_len;
	frame_type var_frame;
} variable_t;

typedef union {
	const StringData *string_val;
	integer_t int_val;
	variable_t var_data;
	opcode_type ins_opcode;
} token_value_t;

typedef struct {
	token_type type;
	token_value_t value;
} token_t;
#endif
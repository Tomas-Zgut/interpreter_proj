#include <ctype.h>
#include <headders/lexer.h>
#include <headders/tokens.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT_PREFIX "int"
#define NIL_PREFIX "nil"
#define BOOL_PREFIX "bool"
#define STRING_PREFIX "string"

#define INT_LIT_SEP_OFFSET 3
#define STRING_LIT_SEP_OFFSET 6
#define BOOL_LIT_SEP_OFFSET 4
#define NIL_LIT_SEP_OFFSET 3
#define VAR_FRAME_SEP_OFFSET 2

typedef struct {
	opcode_type opcode;
	char ins_name[12];
} ins_data_t;

typedef struct {
	int8_t start;
	int8_t end;
} ins_data_indexes;

static ins_data_t instruction_map[] = {
	{INS_LT, "LT"},
	{INS_GT, "GT"},
	{INS_EQ, "EQ"},
	{INS_OR, "OR"},
	{INS_ADD, "ADD"},
	{INS_SUB, "SUB"},
	{INS_MUL, "MUL"},
	{INS_AND, "AND"},
	{INS_NOT, "NOT"},
	{INS_MOVE, "MOVE"},
	{INS_CALL, "CALL"},
	{INS_POPS, "POPS"},
	{INS_IDIV, "IDIV"},
	{INS_READ, "READ"},
	{INS_TYPE, "TYPE"},
	{INS_JUMP, "JUMP"},
	{INS_EXIT, "EXIT"},
	{INS_PUSHS, "PUSHS"},
	{INS_WRITE, "WRITE"},
	{INS_LABEL, "LABEL"},
	{INS_BREAK, "BREAK"},
	{INS_DEFVAR, "DEFVAR"},
	{INS_RETURN, "RETURN"},
	{INS_CONCAT, "CONCAT"},
	{INS_STRLEN, "STRLEN"},
	{INS_DPRINT, "DPRINT"},
	{INS_STR2INT, "STR2INT"},
	{INS_GETCHAR, "GETCHAR"},
	{INS_SETCHAR, "SETCHAR"},
	{INS_INT2CHAR, "INT2CHAR"},
	{INS_JUMPEQ, "JUMPIFEQ"},
	{INS_PUSH_F, "PUSHFRAME"},
	{INS_POP_F, "POPFFRAME"},
	{INS_JUMPNEQ, "JUMPIFNEQ"},
	{INS_CREATE_F, "CREATEFRAME"},
};

static ins_data_indexes ins_index_map[] = {
	{-1, -1}, {-1, -1}, {0, 4},	  {4, 9},	{9, 17},  {17, 21},
	{21, 26}, {26, 29}, {29, 31}, {31, 34}, {-1, -1}, {35, 36},
};

bool is_comment(const StringView *buff) { return buff->data[0] == '#'; }

/**
 * @brief function to find a separator in a string view
 *
 * @param buff: string view
 * @param sep: separator to find
 *
 * @returns: the index of a given separator if found -1 otherwise
 */
int find_sep(const StringView *buff, char sep) {
	const char *buff_data = buff->data;
	int buff_idx = 0;
	for (; (size_t)buff_idx < buff->length; buff_idx++) {
		if (buff_data[buff_idx] != sep) {
			return buff_idx;
		}
	}
	return -1;
}

token_type lex_lit_prefix(const StringView *buff) {
	if (buff->length == INT_LIT_SEP_OFFSET &&
		!memcmp(buff->data, INT_PREFIX, buff->length)) {
		return TOKEN_INT;
	}

	if (buff->length == NIL_LIT_SEP_OFFSET &&
		!memcmp(buff->data, NIL_PREFIX, buff->length)) {
		return TOKEN_NIL;
	}

	if (buff->length == BOOL_LIT_SEP_OFFSET &&
		!memcmp(buff->data, BOOL_PREFIX, buff->length)) {
		return TOKEN_BOOL;
	}

	if (buff->length == STRING_LIT_SEP_OFFSET &&
		!memcmp(buff->data, STRING_PREFIX, buff->length)) {
		return TOKEN_STRING;
	}

	return TOKEN_UNKNOWN;
}

token_type lex_var_prefix(const StringView *buff) {
	if (buff->length != 2) {
		return TOKEN_UNKNOWN;
	}

	if (buff->data[1] != 'F') {
		return TOKEN_UNKNOWN;
	}

	switch (buff->data[0]) {
	case 'G':
	case 'T':
	case 'L':
		return TOKEN_VARIABLE;
	default:
		return TOKEN_UNKNOWN;
	}
}

bool is_allowed_char(char input) {
	switch (input) {
	case '_':
	case '-':
	case '$':
	case '&':
	case '%':
	case '*':
	case '!':
	case '?':
		return true;

	default:
		return false;
	}
}

bool is_identifier(const StringView *buff) {
	const char *buff_data = buff->data;
	if (!isalpha(buff_data[0]) || !is_allowed_char(buff_data[0])) {
		return false;
	}

	for (size_t buff_idx = 1; buff_idx < buff->length; buff_idx++) {
		if (!isalnum(buff_data[buff_idx]) ||
			!is_allowed_char(buff_data[buff_idx])) {
			return false;
		}
	}
	return true;
}

bool is_literal(const StringView *buff) {
	int sep_idx = find_sep(buff, '@');

	if (sep_idx == -1 || sep_idx < INT_LIT_SEP_OFFSET ||
		sep_idx > STRING_LIT_SEP_OFFSET) {
		return false;
	}

	const StringView lit_prefix = sb_get_substring(buff, 0, sep_idx - 1);
	return lex_lit_prefix(&lit_prefix) != TOKEN_UNKNOWN;
}

bool is_variable(const StringView *buff) {
	if (buff->data[VAR_FRAME_SEP_OFFSET] != '@') {
		return false;
	}

	const StringView prefix = sb_get_substring(buff, 0, 2);

	if (lex_var_prefix(&prefix) == TOKEN_UNKNOWN) {
		return false;
	}

	const StringView postfix = sb_get_view(buff, VAR_FRAME_SEP_OFFSET);

	return is_identifier(&postfix);
}

bool is_label(const StringView *buff) { return is_identifier(buff); }

ins_data_indexes get_instruction_indexes(const StringView *buff) {
	if (buff->length > 11) {
		return (ins_data_indexes){-1, -1};
	}
	return ins_index_map[buff->length];
}

opcode_type lookup_insturction(const StringView *buff) {
	ins_data_indexes ins_idx = get_instruction_indexes(buff);

	if (ins_idx.start == -1) {
		return INS_UNKNOWN;
	}

	for (int8_t ins_index = ins_idx.start; ins_index < ins_idx.end;
		 ins_index++) {
		ins_data_t ins_data = instruction_map[ins_index];
		if (!memcmp(buff->data, &ins_data.ins_name, buff->length)) {
			return ins_data.opcode;
		}
	}
	return INS_UNKNOWN;
}

bool is_instruction(const StringView *buff) {
	return lookup_insturction(buff) != INS_UNKNOWN;
}

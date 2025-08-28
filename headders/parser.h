#ifndef __PARSER_H__
#define __PARSER_H__
#include "lexer.h"
#include "jump_table.h"
#include "ir_array.h"
typedef struct
{
	LexerContext lex_ctx;	 // context for lexer
	jump_table_t jump_table; // table of jumps
	ir_array ir;			 // array containing the ir of the program
} Parser;

typedef enum
{
	PARSER_MEM_ERR,
	PARSER_SUCCESS,
	PARSER_INVALID_TOKEN,
	PARSER_INVALID_INS_ARGS,
	PARSER_PROGRAM_END,
	PARSER_DUPLICATE_LABEL,
} parser_ret;

/**
 * @brief function creates a parser
 * 
 * @param input_file: file to parse
 * 
 * @returns pointer to heap alloced parser on success, NULL otherwise
 */
Parser *parser_create(FILE *input_file);

/**
 * @brief function to parse a line from a file
 * 
 * @param parser: pointer to the parser 
 * 
 * @returns 
 * - `PARSER_SUCCESS` if the parsgin was succesfull
 * - `PARSER_INVALID_TOKEN` if an invalid token was found
 * - `PARSER_INVALID_INS_ARGS` if an invalid instruction argument is encountered
 * - `PARSER_PROGRAM_END` if EOF is reached 
 * - `PARSER_MEM_ERR` if there is an issue with memory allocation
 */
parser_ret parser_parse_line(Parser *parser);

/**
 * @brief function to parse a file
 * 
 * @param parser: pointer to the parser 
 * 
 * @returns 
 * - `PARSER_SUCCESS` if the parsing was succesfull
 * - `PARSER_INVALID_TOKEN` if an invalid token was found
 * - `PARSER_INVALID_INS_ARGS` if an invalid instruction argument is encountered
 * - `PARSER_MEM_ERR` if there is an issue with memory allocation
 */
parser_ret parser_parse_file(Parser *parser);

/**
 * @brief function stores a lable in the jump table
 * 
 * @param parser: pointer to a parser
 * 
 * @returns 
 * - `PARSER_SUCCESS` if the store was successfull
 * - `PARSER_DUPLICATE_LABEL` if duplicate label is encountered
 * - `PARSER_MEM_ERR` if there is an issue with memory allocation
 */
parser_ret parser_store_label(Parser* parser, opcode_type opcode, token_t **operand_tokens);

/**
 * @brief function to retrieve the program's ir from the parser
 * 
 * @par Function transfers onwership of the ir from the parser to the user.
 * Function attempts to shrink to fit the ir array so it does not waste memory.
 * If this fails function just returns the original ir array.
 * 
 * @param parser: pointer to a parser
 * 
 * @returns the program's ir in an immutable array that takes the ownership of the data
 * from the parser.
 * 
 * @warning Calling this function more than once on a parser triggers an assertion!
 */
ir parser_get_ir(Parser *parser);

/**
 * @brief function to free a parser
 * 
 * @param parser: pointer to a parser
 */
void parser_free(Parser *parser);

#endif
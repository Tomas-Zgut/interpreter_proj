#ifndef __LEXER_H__
#define __LEXER_H__

#include "string_buffer.h"
#include "tokens.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct {
	StringBuffer current_line_buffer; // Dynamický buffer pro aktuální řádek
	size_t current_pos_in_line;		  // Aktuální pozice v current_line_bufferu
	bool line_is_exhausted; // zda je aktuální řádek zpracován a je potřeba
							// načíst nový

	uint32_t line_number; // Aktuální číslo řádku ve vstupním souboru
	FILE *input_stream;	  // Kdyby byla poteřeba na vstupní soubor
						  // Místo na další
} LexerContext;

/**
 * @brief Inicializuje lexer kontext.
 *
 * @param ctx Ukazatel na strukturu LexerContext.
 *
 * @param input Ukazatel na vstupní soubor (např. stdin).
 *
 * @return true při úspěchu, false při selhání (např. alokace paměti).
 */
bool lexer_init(LexerContext *ctx, FILE *input);

/**
 * @brief Uvolní paměť alokovanou lexer kontextem.
 *
 * @param ctx Ukazatel na strukturu LexerContext.
 */
void lexer_free(LexerContext *ctx);

/**
 * @brief function to get a new token
 *
 * @todo implement
 *
 * @returns a pointer to a new token.
 */
token_t *get_next_token(LexerContext *ctx);

/**
 * @brief function to check if the input contains a comment
 *
 * @param buff: immutable non owning buffer with data
 *
 * @returns true if the input is a comment, false otherwise.
 */
bool is_comment(const StringView *buff);

/**
 * @brief function to check if the input contains a variable
 *
 * @param buff: input string
 *
 * @returns true if the input is a  variable name false otherwise.
 */
bool is_variable(const StringView *buff);
/**
 * @brief function to check if the input contains an instruction
 *
 * @param buff: input string
 *
 * @returns true if the input is an instruction, false otherwise.
 */
bool is_instruction(const StringView *buff);

/**
 * @brief function to check if the input contains a label
 *
 * @param bufff: input string
 *
 * @returns true if the input is an label, false otherwise.
 */
bool is_label(const StringView *buff);

/**
 * @brief function to check if the input is a literal
 *
 * @param bufff: input string
 *
 * @returns true if the input is an literal, false otherwise.
 */
bool is_literal(const StringView *buff);

/**
 * @brief function  to create a vairable token
 *
 * @par Function attempts to creata a vairable token, if it suceeds
 * it modifies the token parameter and returns true. If not the token
 * parametr has undefined value and it returns false.
 *
 * @param buff[in]: string to containing a variable name
 * @param token[out]: token data
 *
 * @return true on success, false otherwise.
 *
 * @todo implement
 */
bool lex_variable(const StringView *buff, token_t *token);

/**
 * @brief function  to create a literal token
 *
 * @par Function attempts to creata a literal token, if it suceeds
 * it modifies the token parameter and returns true. If not the token
 * parametr has undefined value and it returns false.
 *
 * @param buff[in]: string to containing a literal
 * @param token[out]: token data
 *
 * @return true on success, false otherwise.
 *
 * @todo implement
 */
bool lex_literal(const StringView *buff, token_t *token);

/**
 * @brief function  to create an intruction token
 *
 * @par Function attempts to create an instruction token, if it suceeds
 * it modifies the token parameter and returns true. If not the token
 * parametr has undefined value and it returns false.
 *
 * @param buff[in]: string to containing an  instrunction
 * @param token[out]: token data
 *
 * @return true on success, false otherwise.
 *
 * @todo implement
 */
bool lex_instruction(const StringView *buff, token_t *token);

/**
 * @brief function  to create a label token
 *
 * @par Function attempts to creata a label token, if it suceeds
 * it modifies the token parameter and returns true. If not the token
 * parametr has undefined value and it returns false.
 *
 * @param buff[in]: string to containing a label
 * @param token[out]: token data
 *
 * @return true on success, false otherwise.
 *
 * @todo implement
 */
bool lex_label(const StringView *buff, token_t *token);

/**
 * @brief function to deal with tokens (might not be used)
 */
void lex_commnet(const StringView *buff);
#endif
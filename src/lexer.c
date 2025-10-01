#include <ctype.h>
#include <headders/lexer.h>
#include <headders/tokens.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT_PREFIX "int" 		// prefeix for int literals
#define NIL_PREFIX "nil"		// prefix for nill literals
#define BOOL_PREFIX "bool"		// prefix for bool lierals
#define STRING_PREFIX "string"	// prefix for string lierals

#define INT_LIT_SEP_OFFSET 3	// offset of separator in int literals
#define STRING_LIT_SEP_OFFSET 6	// offset of separator in string literals
#define BOOL_LIT_SEP_OFFSET 4	// offset of separator in bool literals
#define NIL_LIT_SEP_OFFSET 3	// offset of separator in nill literals
#define VAR_FRAME_SEP_OFFSET 2	// offset of separator in varaible names

/**
 * @brief struct mapping instruction opcode to its string representation
 */
typedef struct {
	const opcode_type opcode;
	const char ins_name[12];
} ins_data_t;
/**
 * @brief struct representing range of indexes
 */
typedef struct {
	const int8_t start;	// start of the index range
	const int8_t end;	// end of the index range
} ins_data_indexes;

/** 
 * @brief mapping of instruction opcodes to their strings
 * 
 * @see ins_data_t
*/
static const ins_data_t instruction_map[] = {
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

/**
 * @brief mapping of length of a string to indexes in `instruction_map`
 * 
 * @see ins_data_indexes
 */
static const ins_data_indexes ins_index_map[] = {
	{-1, -1}, {-1, -1}, {0, 4},	 {4, 9}, {9, 17}, {17, 21},
	{21, 26}, {26, 29}, {29, 31}, {31, 34}, {-1, -1}, {35, 36},
};

/**
 * @brief Interní funkce pro načtení dalšího řádku ze vstupního proudu do
 * bufferu lexeru.
 *
 * @param ctx Ukazatel na lexer kontext.
 *
 * @return true při úspěšném načtení řádku, false při konci souboru (EOF) nebo
 * chybě.
 */
bool lexer_read_next_line(LexerContext *ctx) {
	sb_reset(&ctx->current_line_buffer);

	int c;
	bool line_read = false;
	ctx->line_number++;

	while ((c = fgetc(ctx->input_stream)) != EOF) {
		if (c == '\n') {
			// Do bufferu přidáme '\n', pro zpracování dalších funkcí, ale pozice -> posune
			if (!sb_append_char(&ctx->current_line_buffer, (char)c)) {
				fprintf(stderr, "Chyba: Nedostatek paměti při čtení řádku %u.\n",
						ctx->line_number);
				return false;
			}
			line_read = true;
			break;
		}
		if (!sb_append_char(&ctx->current_line_buffer, (char)c)) {
			fprintf(stderr, "Chyba: Nedostatek paměti při čtení řádku %u.\n",
					ctx->line_number);
			return false;
		}
		line_read = true;
	}

	if (!line_read && c == EOF && ctx->current_line_buffer.length == 0) {
		return false;
	}

	ctx->current_pos_in_line = 0;
	ctx->line_is_exhausted = false;
	return true;
}

// Implementace funkcí deklarovaných v lexer.h, snad dostatečně dobře/alespoň
// decently (chtěl jsem pomoct XD)
bool lexer_init(LexerContext *ctx, FILE *input) {
	if (!sb_init(&ctx->current_line_buffer, 0)) { // Inicializace bufferu
		return false;
	}
	ctx->current_pos_in_line = 0;
	ctx->line_is_exhausted = true; // načíst první řádek
	ctx->line_number = 0;
	ctx->input_stream = input;
	return true;
}

void lexer_free(LexerContext *ctx) {
	sb_free(&ctx->current_line_buffer);
	// další alokace možná později?
}

/*
1. Zajištění, že máme co zpracovávat (načtený řádek)
2. Přeskakování bílých znaků a zpracování komentářů
3. Extrakce a rozpoznávání tokenu
*/

token_t *get_next_token(LexerContext *ctx) {
	if (ctx->line_is_exhausted ||
		ctx->current_pos_in_line >= ctx->current_line_buffer.length) {
		if (!lexer_read_next_line(ctx)) {
			token_t *eof_token = (token_t *)malloc(sizeof(token_t));
			if (eof_token == NULL) {
				perror("Chyba alokace token_t pro EOF");
				return NULL;
			}
			eof_token->type = TOKEN_EOF;
			return eof_token;
		}
	}

	while (ctx->current_pos_in_line < ctx->current_line_buffer.length) {
		char current_char =
			ctx->current_line_buffer.data[ctx->current_pos_in_line];

		if (isspace(current_char)) {
			ctx->current_pos_in_line++;
			continue;
		}

		if (current_char == '#') {
			ctx->line_is_exhausted = true;
			return get_next_token(ctx);
		}
		break;
	}

	if (ctx->current_pos_in_line >= ctx->current_line_buffer.length) {
		ctx->line_is_exhausted = true;
		return get_next_token(ctx);
	}

	token_t *new_token = (token_t *)malloc(sizeof(token_t));
	if (new_token == NULL) {
		perror("Chyba alokace token_t");
		return NULL;
	}
	new_token->type = TOKEN_UNKNOWN;
	// inicializace unionu pro bezpečnou manipulaci (doporučeno by ai xD)
	memset(&new_token->value, 0, sizeof(token_value_t));

	//určení délky
	size_t start_token_pos = ctx->current_pos_in_line;
	size_t current_token_len = 0;

	// smyčka pro nalezení konce tokenu (# nebo whitespace), zjednodušeně
	while (start_token_pos + current_token_len <
			   ctx->current_line_buffer.length &&
		   !isspace(ctx->current_line_buffer
						.data[start_token_pos + current_token_len]) &&
		   ctx->current_line_buffer.data[start_token_pos + current_token_len] !=
			   '#') {
		current_token_len++;
	}

	// StringView pro nalezený segment textu
	const StringView token_view = sb_get_substring(
		&ctx->current_line_buffer, start_token_pos, current_token_len);

	if (is_instruction(&token_view)) {
		if (lex_instruction(&token_view, new_token)) {
			new_token->type = TOKEN_INSTRUCTION;
		} else {
			fprintf(stderr, "Chyba: Nevalidní instrukce na řádku %u.\n",
					ctx->line_number);
			new_token->type = TOKEN_UNKNOWN;
		}
	} else if (is_variable(&token_view)) {
		if (lex_variable(&token_view, new_token)) {
			new_token->type = TOKEN_VARIABLE;
		} else {
			fprintf(stderr, "Chyba: Nevalidní proměnná na řádku %u.\n",
					ctx->line_number);
			new_token->type = TOKEN_UNKNOWN;
		}
	} else if (is_label(&token_view)) {
		if (lex_label(&token_view, new_token)) {
			new_token->type = TOKEN_LABEL;
		} else {
			fprintf(stderr, "Chyba: Nevalidní návěští na řádku %u.\n",
					ctx->line_number);
			new_token->type = TOKEN_UNKNOWN;
		}
	} else if (is_literal(&token_view)) {
		if (lex_literal(&token_view, new_token)) {
		} else {
			fprintf(stderr, "Chyba: Nevalidní literál na řádku %u.\n",
					ctx->line_number);
			new_token->type = TOKEN_UNKNOWN;
		}
	} else {
		fprintf(stderr, "Chyba: Neznámý token '%.*s' na řádku %u.\n",
				(int)token_view.length, token_view.data, ctx->line_number);
		new_token->type = TOKEN_UNKNOWN;
	}

	ctx->current_pos_in_line += current_token_len;
	return new_token;
}

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
		if (buff_data[buff_idx] == sep) {
			return buff_idx;
		}
	}
	return -1;
}

token_type lex_lit_prefix(const StringView *buff) {
	if (buff->length == strlen(INT_PREFIX) &&
		!memcmp(buff->data, INT_PREFIX, buff->length)) {
		return TOKEN_INT;
	}

	if (buff->length == strlen(NIL_PREFIX) &&
		!memcmp(buff->data, NIL_PREFIX, buff->length)) {
		return TOKEN_NIL;
	}

	if (buff->length == strlen(BOOL_PREFIX) &&
		!memcmp(buff->data, BOOL_PREFIX, buff->length)) {
		return TOKEN_BOOL;
	}

	if (buff->length == strlen(STRING_PREFIX) &&
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
	//indentifikator musí být povolené písmeno nebo znak
	if (!isalpha(buff_data[0]) && !is_allowed_char(buff_data[0])) {
		return false;
	}

	for (size_t buff_idx = 1; buff_idx < buff->length; buff_idx++) {
		//zbytek může být písmeno, číslo, znak
		if (!isalnum(buff_data[buff_idx]) &&
			!is_allowed_char(buff_data[buff_idx])) {
			return false;
		}
	}
	return true;
}

bool is_literal(const StringView *buff) {
	int sep_idx = find_sep(buff, '@');

	if (sep_idx <= 0) {
		return false;
	}
	
	const StringView lit_prefix = sb_get_substring(buff, 0, sep_idx);
	return lex_lit_prefix(&lit_prefix) != TOKEN_UNKNOWN;
}

bool is_variable(const StringView *buff) {
	int sep_idx = find_sep(buff, '@');

	if (sep_idx != 2) {
		return false;
	}
	
	const StringView prefix = sb_get_substring(buff, 0, 2);
	if (lex_var_prefix(&prefix) == TOKEN_UNKNOWN) {
		return false;
	}
	
	const StringView postfix = sb_get_view(buff, VAR_FRAME_SEP_OFFSET + 1);
	return is_identifier(&postfix);
}

bool is_label(const StringView *buff) {
	if (buff->length < 1 || buff->data[buff->length - 1] != ':') {
		return false;
	}
	
	const StringView label_name = sb_get_substring(buff, 0, buff->length - 1);
	return is_identifier(&label_name);
}

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

bool lex_variable(const StringView *buff, token_t *token) {
	assert(is_variable(buff)); // Předpokládáme, že funkce is_variable již
							   // proběhla
	
	// Nastavíme typ tokenu
	token->type = TOKEN_VARIABLE;
	
	// Extrakce rámce
	char frame_char = buff->data[0];
	switch (frame_char) {
		case 'G': token->value.var_data.var_frame = GF; break;
		case 'L': token->value.var_data.var_frame = LF; break;
		case 'T': token->value.var_data.var_frame = TF; break;
		default: return false; // Nemělo by nastat, ale pro jistotu
	}
	
	// Extrakce názvu proměnné
	const StringView var_name_view = sb_get_view(buff, VAR_FRAME_SEP_OFFSET + 1);
	
	// Kopírování názvu do tokenu
	if (!sb_copy(&token->value.var_data.var_name, &var_name_view)) {
		// handle malloc failure
		return false;
	}
	
	return true;
}

bool lex_literal(const StringView *buff, token_t *token) {
	assert(is_literal(buff)); // Předpokládáme, že funkce is_literal již proběhla

	int sep_idx = find_sep(buff, '@');
	const StringView lit_prefix = sb_get_substring(buff, 0, sep_idx);
	const StringView lit_value_view = sb_get_view(buff, sep_idx + 1);
	
	token->type = lex_lit_prefix(&lit_prefix);
	
	switch (token->type) {
		case TOKEN_INT:
			// Převedení na int
			if (lit_value_view.length > 0) {
				char* endptr;
				token->value.int_val = strtoull(lit_value_view.data, &endptr, 10);
				if (*endptr != '\0') { // Kontrola, zda se převedl celý řetězec
					token->type = TOKEN_UNKNOWN;
					return false;
				}
			} else {
				token->type = TOKEN_UNKNOWN;
				return false;
			}
			break;
		case TOKEN_STRING:
			if (!sb_copy(&token->value.string_val, &lit_value_view)) {
				token->type = TOKEN_UNKNOWN;
				return false;
			}
			break;
		case TOKEN_BOOL:
			if (lit_value_view.length == 4 && !memcmp(lit_value_view.data, "true", 4)) {
				token->value.int_val = 1;
			} else if (lit_value_view.length == 5 && !memcmp(lit_value_view.data, "false", 5)) {
				token->value.int_val = 0;
			} else {
				token->type = TOKEN_UNKNOWN;
				return false;
			}
			break;
		case TOKEN_NIL:
			token->value.int_val = 0; // nebo jiná reprezentace NIL
			break;
		default:
			return false;
	}
	
	return true;
}

bool lex_instruction(const StringView *buff, token_t *token) {
	assert(is_instruction(buff));
	
	token->value.ins_opcode = lookup_insturction(buff);
	token->type = TOKEN_INSTRUCTION;
	
	if (token->value.ins_opcode == INS_UNKNOWN) {
		return false;
	}
	
	return true;
}

bool lex_label(const StringView *buff, token_t *token) {
	assert(is_label(buff));
	
	token->type = TOKEN_LABEL;
	
	const StringView label_name = sb_get_substring(buff, 0, buff->length - 1);
	
	if (!sb_copy(&token->value.string_val, &label_name)) {
		return false;
	}
	
	return true;
}

/* @brief function to deal with tokens (might not be used) */
void lex_commnet(const StringView *buff) {
	// Tato funkce je v tuto chvíli zbytečná, protože komentáře se přeskočí.
	// Pokud by byla potřeba, mohla by se sem přidat nějaká logika
	// pro zpracování obsahu komentářů.
	(void)buff; // Potlačení warningu
}

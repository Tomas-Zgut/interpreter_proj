#include<headders/parser.h>
#include <headders/interpreter.h>
#include <headders/ret_codes.h>
#include <string.h>
/**
 * @brief Function that prints the help message of the program
 * 
 * @param program_name: name of the program
 */
void print_help(const char* program_name) {
	printf("This is an interpreter program\n");
	printf("Program interprets code in a provided [file].\n");
	printf("If no [file] is provided the stdin is used.\n");
	printf("Useage: %s [-h|--help] [file]\n",program_name);
	printf("Options:\n");
	printf("\t -h, --help:\tDisplays this message\n");
}

/**
 * @brief struct representing the command line configuration of the program
 */
typedef struct {
	FILE* file; // file to parse
} setup_t;

/**
 * @brief Function to parse the command line arguments of the program
 * 
 * @param config: pointer to the program config
 * @param argc: number of program cmd line arguments
 * @param argv: array of program arugments
 */
int parse_args(setup_t *config,int argc, const char *argv[]) {
	if (argc > 2) {
		fprintf(stderr,"Invalid number of parameters!\n");
		print_help(argv[0]);
		return -1;
	}

	if (argc == 1) {
		config->file = stdin;
		return 1;
	}

	if(!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")) {
		print_help(argv[0]);
		return 0;
	}

	if (argv[1][0] == '-' ) {
		fprintf(stderr,"Invalid option!\n");
		print_help(argv[0]);
		return -1;
	}

	config->file = fopen(argv[1],"r");
	return 1;
}
// TODO remove
// Funkce pro ukázkové zpracování tokenů
void process_tokens(LexerContext *ctx) {
	token_t *token;
	
	printf("Processing tokens...\n");
	
	while ((token = get_next_token(ctx)) != NULL) {
		if (token->type == TOKEN_EOF) {
			free(token);
			break;
		}

		printf("Line %u: ", ctx->line_number);
		switch (token->type) {
			case TOKEN_INSTRUCTION:
				printf("Instruction (Opcode: %u)\n", token->value.ins_opcode);
				break;
			case TOKEN_VARIABLE:
				printf("Variable (Frame: %u, Name: %.*s)\n", token->value.var_data.var_frame, (int)token->value.var_data.var_name.length, token->value.var_data.var_name.data);
				// Uvolnění String, který byl alokován uvnitř lex_variable
				sb_free(&token->value.var_data.var_name);
				break;
			case TOKEN_LABEL:
				printf("Label (Name: %.*s)\n", (int)token->value.string_val.length, token->value.string_val.data);
				sb_free(&token->value.string_val);
				break;
			case TOKEN_INT:
				printf("Literal (INT: %lu)\n", token->value.int_val);
				break;
			case TOKEN_STRING:
				printf("Literal (STRING: %.*s)\n", (int)token->value.string_val.length, token->value.string_val.data);
				sb_free(&token->value.string_val);
				break;
			case TOKEN_BOOL:
				printf("Literal (BOOL: %s)\n", token->value.int_val ? "true" : "false");
				break;
			case TOKEN_NIL:
				printf("Literal (NIL)\n");
				break;
			case TOKEN_UNKNOWN:
				printf("Unknown token\n");
				break;
			default:
				printf("Unhandled token type\n");
				break;
		}
		
		free(token);
	}
	
	printf("Finished processing tokens.\n");
}

/**
 * @brief main function of the program
 * 
 * @param argc: number cmd line arguments
 * @param argv: array of cmd line arguments
 * 
 * @returns exit code of the program
 */
int main(int argc, const char* argv[]) {
	setup_t config;
	int ret = parse_args(&config,argc,argv);

	if (ret <= 0) {
		return ret == 0 ? 0 : INVALID_ARG;
	}
	ret = 0;
	if (config.file == NULL) {
		fprintf(stderr,"Failed to open %s\n",argv[1]);
		return INPUT_FILE_OPEN_ERR;
	}

	Parser *parser = parser_create(config.file);
	if (parser == NULL) {
		fprintf(stderr,"Failed to initialize parser\n!");
		ret = INTERNAL_ERROR;
		goto parser_init_err;
	}

	const parser_ret parser_retval = parser_parse_file(parser);
	if (parser_retval != PARSER_SUCCESS) {
		ret = INTERNAL_ERROR;
		goto parse_fail;
	}

	if (!parser_check_jump_table(parser)) {
		ret=SEMANTIC_ERROR;
		goto parse_fail;
	}

	ir program_ir = parser_get_ir(parser);
	jump_table_t jump_table = parser_move_table(parser);
	parser_free(parser); // no longer necessary
	Memory memory;

	if (!memory_init(&memory)) {
		fprintf(stderr,"Failed to initialize interpreters memory!\n") ;
		ret=INTERNAL_ERROR;
		goto mem_init_fail;
	}

	ret = interpret_ir(&program_ir,&memory,&jump_table);
	ir_free(&program_ir);
	memory_free(&memory);
	jump_table_free(&jump_table);
	return ret;
	
mem_init_fail:
parse_fail:
	parser_free(parser);
parser_init_err:
	if(config.file != stdin) {
		fclose(config.file);
	}

	return ret;

}

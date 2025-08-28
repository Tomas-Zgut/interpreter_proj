#include<headders/parser.h>
#include<headders/type_cheks.h>
#include<headders/opcodes.h>

Parser * parser_create(FILE *input_file) {
    Parser *parser = malloc(sizeof(Parser));
    if (parser == NULL) {
        goto parser_alloc_err;
    }

    if (!lexer_init(&parser->lex_ctx,input_file)) {
        goto lexer_init_err;
    }

    if (!jump_table_init(&parser->jump_table)) {
        goto jump_table_init_err;
    }

    if (!ir_array_init(&parser->ir)) {
        goto ir_arra_init_err;
    }

    return parser;

ir_arra_init_err:
    jump_table_free(&parser->jump_table);
jump_table_init_err:
    lexer_free(&parser->lex_ctx);
lexer_init_err:
    free(parser);
parser_alloc_err:
    return NULL;
}

#define MAX_OPERAND_TOKENS 3
#define PARSER_MEM_CHECK(val,err)               \
    if((void *) (val) == NULL || (val) == 0) {  \
        retval = PARSER_MEM_ERR;                \
        goto err;                               \
    }

parser_ret parser_parse_line(Parser *parser) {
    token_t *operand_tokens[MAX_OPERAND_TOKENS] = {0};
    token_t *instruction = get_next_token(&parser->lex_ctx);
    parser_ret retval = PARSER_SUCCESS;
    PARSER_MEM_CHECK(instruction,ins_token_err)

    if(instruction->type == TOKEN_UNKNOWN) {
        retval = PARSER_INVALID_TOKEN;
        goto ins_token_err;
    } else if (instruction->type == TOKEN_EOF) {
        retval = PARSER_PROGRAM_END;
        goto ins_end;
    }

    const opcode_type opcode = instruction->value.ins_opcode;
    const int arg_count = get_number_opperands(opcode);
    int alloced_tokens = 0;

    for (int operand_idx = 0; operand_idx < arg_count; operand_idx++) {
        operand_tokens[operand_idx] = get_next_token(&parser->lex_ctx);
        PARSER_MEM_CHECK(operand_tokens[operand_idx],operand_token_err)

        alloced_tokens++;
        const token_type operand_type = operand_tokens[operand_idx]->type;
        if (operand_type == TOKEN_EOF || operand_type == TOKEN_INSTRUCTION) {
            fprintf(stderr,"Not enough arguments for instrucion on %d:%ld\n",parser->lex_ctx.line_number,parser->lex_ctx.current_pos_in_line);
            retval = PARSER_INVALID_INS_ARGS;
            goto operand_token_err;
        } else if (operand_type == TOKEN_UNKNOWN) { //tokenizer has already handled the error
            retval = PARSER_INVALID_INS_ARGS;
            goto operand_token_err;
        }
    }

    const arg_type_check_t result = instruction_args_valid(opcode,operand_tokens);

    if (result.result == false) {
        fprintf(stderr,"Argument number %d is not the correct type!\n",result.failed_arg + 1);
        retval = PARSER_INVALID_INS_ARGS;
        goto arg_type_err;
    }

    if (has_label(instruction->value.ins_opcode)) {
        const parser_ret ret = parser_store_label(parser,opcode,operand_tokens);
        PARSER_MEM_CHECK(ret,label_store_err)
        if (ret == PARSER_DUPLICATE_LABEL) {
            fprintf(stderr,"Duplicit label declaration! Duplicate found at %d:%ld\n",parser->lex_ctx.line_number,parser->lex_ctx.current_pos_in_line);
            retval = PARSER_INVALID_INS_ARGS;
            goto label_store_err;
        }
    }

    PARSER_MEM_CHECK(ir_array_add_elm(&parser->ir,instruction->value.ins_opcode,operand_tokens),ir_alloc_err)

label_store_err:
ir_alloc_err:
arg_type_err:
operand_token_err:
    for (int i = 0; i < alloced_tokens; i++) {
        token_free(operand_tokens[i]);
        free(operand_tokens[i]);
    }
    free(instruction);
ins_token_err:
ins_end:
    return retval;
}

parser_ret parser_store_label(Parser *parser, opcode_type opcode, token_t **operand_tokens) {
    assert(parser != NULL);
    assert(operand_tokens != NULL);

    jump_table_entry_t entry;
    jump_table_entry_t *entry_ptr = &entry;
    const StringView label = sb_get_view(&operand_tokens[0]->value.string_val,0);
    const jump_table_ret ret = jump_table_insert(&parser->jump_table,&label,&entry_ptr);

    if(ret == JUMP_TABLE_ALLOC_ERR) {
        return PARSER_MEM_ERR;
    }

    if (ret == JUMP_TABLE_DUPLICATE_FOUND && entry_ptr->destination_filled) {
        return PARSER_DUPLICATE_LABEL;
    } 

    if (opcode == INS_LABEL) {
        entry_ptr->destination = ir_array_size(&parser->ir);
        entry_ptr->destination_filled = true;
    } else {
        entry_ptr->destination_filled = false;
    }
    
    return PARSER_SUCCESS;
}


ir parser_get_ir(Parser *parser) {
    assert(parser != NULL);
    assert(parser->ir.elements != NULL); // make sure this is done with valid data
    
    ir_elm_t *old_elms = parser->ir.elements;
    const uint64_t size = parser->ir.size;
    ir_elm_t *new_elms = realloc(old_elms,parser->ir.size*sizeof(ir_elm_t)); //shirnk to fit
    parser->ir.elements = NULL; // loose the ownership of this data
    if (new_elms == NULL) {
        return (ir) {.elements = old_elms, .size = size }; //shrink failed (data is still valid)
    }
    return (ir) {.elements = new_elms, .size = size}; // successfull shrink

}

parser_ret parser_parse_file(Parser *parser) {
    while (true)
	{
		const parser_ret retval = parser_parse_line(parser);

		if (retval == PARSER_PROGRAM_END) {
			return PARSER_SUCCESS;
		}

		if (retval != PARSER_SUCCESS) {
			parser_free(parser);
            return retval;
		}
	}
}


void parser_free(Parser *parser) {
    assert(parser != NULL);
    ir_array_free(&parser->ir);
    jump_table_free(&parser->jump_table);
    lexer_free(&parser->lex_ctx);
    free(parser);
}
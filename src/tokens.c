#include<headders/tokens.h>

void token_free(token_t *token) {
	assert(token != NULL);

	switch (token->type)
	{
	case TOKEN_LABEL:
	case TOKEN_STRING:
	case TOKEN_TYPE:
		sb_free(&token->value.string_val);
		return;
	case TOKEN_VARIABLE:
		sb_free(&token->value.var_data.var_name);
		return;
	case TOKEN_BOOL:
	case TOKEN_EOF:
	case TOKEN_INT:
	case TOKEN_NIL:
	case TOKEN_FLOAT:
	case TOKEN_INSTRUCTION:
	case TOKEN_UNKNOWN:
		return;
	default:
		assert(false); //should never happen
	}
}


void token_move(token_t * src_token, token_t* dest_token) {
	assert(src_token != NULL);
	assert(dest_token != NULL);

	*dest_token = *src_token;
	switch (src_token->type)
	{
	case TOKEN_LABEL:
	case TOKEN_STRING:
	case TOKEN_TYPE:
		dest_token->value.string_val = sb_move_string(&src_token->value.string_val);
		return;
	case TOKEN_VARIABLE:
		dest_token->value.var_data.var_name = sb_move_string(&src_token->value.var_data.var_name);
		return;
	case TOKEN_BOOL:
	case TOKEN_EOF:
	case TOKEN_INT:
	case TOKEN_NIL:
	case TOKEN_FLOAT:
	case TOKEN_INSTRUCTION:
	case TOKEN_UNKNOWN:
		return;
	default:
		assert(false); //should never happen
	}
}

/**
 * @brief Macro for creating a StrigView from @p lit
 * 
 * @param lit: string literal to put into a StringView
 * 
 * @returns a new StringView holding @p lit
 * 
 * @see StringView
 */
#define MAKE_VIEW(lit)          \
    (StringView) {              \
        .data = lit,            \
        .length = sizeof(lit)-1 \
    }


static const StringView frame_lit_array[] = {
	[GF] = MAKE_VIEW("global"),
	[LF] = MAKE_VIEW("local"),
	[TF] = MAKE_VIEW("temporary")
};

StringView token_get_frame_view(const variable_t* var) {
	assert(var->var_frame <= TF && var->var_frame >= GF);
	return frame_lit_array[var->var_frame];
}
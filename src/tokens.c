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
	default:
		return; // nothing to free
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
		assert(src_token->value.string_val.data != NULL);
		src_token->value.string_val.data = NULL;
		return;
	case TOKEN_VARIABLE:
		assert(src_token->value.var_data.var_name.data != NULL);
		src_token->value.var_data.var_name.data = NULL;
		return;
	default:
		return;
	}
}
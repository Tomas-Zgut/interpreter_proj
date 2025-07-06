#ifndef __STRING_BUFFER_H__
#define __STRING_BUFFER_H__

#include "tokens.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
	char *data;
	size_t length;
	size_t capacity;
} StringBuffer;

typedef struct {
	char const *data;
	size_t length;
} StringView;

#define sb_get_substring(buff, offset, len)                                    \
	_Generic((buff),                                                           \
		const StringBuffer *: _sb_get_substring_impl,                          \
		StringBuffer *: _sb_get_substring_impl,                                \
		const StringView *: _sb_get_substring_impl)(buff->data, buff->length,  \
													offset, len)

#define sb_get_view(buff, offset)                                              \
	_Generic((buff),                                                           \
		const StringBuffer *: _sb_get_substring_impl,                          \
		StringBuffer *: _sb_get_substring_impl,                                \
		const StringView *: _sb_get_substring_impl)(buff->data, buff->length,  \
													offset, SIZE_MAX)

bool sb_init(StringBuffer *buf, size_t initial_capacity);
bool sb_append_char(StringBuffer *buf, char c);
void sb_reset(StringBuffer *buf);
bool sb_get_string_copy(const StringBuffer *buf, StringBuffer *out);
StringView _sb_get_substring_impl(const char *buff, size_t buff_len,
								  size_t offset, size_t len);
void sb_free(StringBuffer *buf);
#endif
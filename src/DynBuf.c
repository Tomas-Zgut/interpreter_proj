#include "headders/string_buffer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SB_DEFAULT_INITIAL_CAPACITY 16

bool sb_init(StringBuffer *buf, size_t initial_capacity) {
	if (initial_capacity == 0) {
		initial_capacity = SB_DEFAULT_INITIAL_CAPACITY;
	}
	buf->data = (char *)calloc(initial_capacity, sizeof(char));
	if (buf->data == NULL) {
		perror("Error: Allocation memory failed");
		buf->capacity = 0;
		buf->length = 0;
		return false;
	}

	buf->capacity = initial_capacity;
	buf->length = 0;
	return true;
}

bool sb_append_char(StringBuffer *buf, char c) {
	if (buf->length + 1 >= buf->capacity) {
		size_t new_capacity = buf->capacity * 2;
		if (new_capacity == 0) {
			new_capacity = SB_DEFAULT_INITIAL_CAPACITY;
		}
		char *new_data = (char *)realloc(buf->data, new_capacity);
		if (new_data == NULL) {
			perror("Error: Memory reallocation failure");
			return false;
		}
		buf->data = new_data;
		buf->capacity = new_capacity;
	}

	buf->data[buf->length] = c;
	buf->length++;
	return true;
}

void sb_reset(StringBuffer *buf) { buf->length = 0; }

bool __sb_copy_mut_impl(const char *buf, size_t buff_len, StringBuffer *out) {
	if (!sb_init(out, buff_len)) {
		return false;
	}

	memcpy(out->data, buf, buff_len);
	out->length = buff_len;
	return true;
}

StringView __sb_get_substring_impl(const char *buff, size_t buff_len,
								   size_t offset, size_t len) {
	assert(offset <= buff_len);
	assert(len <= buff_len - offset);
	assert(buff != NULL);

	if (len == 0) {
		len = buff_len;
	}

	return (StringView){buff + offset, len};
}

bool __sb_copy_impl(const char *buff, size_t buff_len, StringData *out) {
	assert(buff != NULL);
	assert(out != NULL);

	char *mem = malloc(buff_len);

	if (mem == NULL) {
		perror("Error: Memory reallocation failure");
		return false;
	}

	memcpy(mem, buff, buff_len);
	out->data = mem;
	out->length = buff_len;

	return true;
}

void __sb_free(StringBuffer *buf) {
	free(buf->data);
	buf->data = NULL;
	buf->length = 0;
	buf->capacity = 0;
}
void __sd_free(StringData *buff) {
	free((void *)buff->data);
	buff->data = NULL;
	buff->length = 0;
}

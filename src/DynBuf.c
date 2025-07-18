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
	return (StringView){buff + offset, len};
}

const StringData *__sb_copy_impl(const char *buff, size_t buff_len) {
	if (buff_len > SIZE_MAX - sizeof(StringData)) {
		perror("Error: Memory reallocation failure");
		return NULL;
	}
	size_t alloc_size = sizeof(StringData) + buff_len;
	void *mem = malloc(alloc_size);

	if (mem == NULL) {
		perror("Error: Memory reallocation failure");
		return NULL;
	}

	StringData *data = (StringData *)mem;
	uintptr_t data_addr = (uintptr_t)mem + sizeof(StringData);
	data->length = buff_len;
	data->data = (const char *)data_addr;
	memcpy((void *)data->data, buff, buff_len);

	return (const StringData *)data;
}

void __sb_free(StringBuffer *buf) {
	free(buf->data);
	buf->data = NULL;
	buf->length = 0;
	buf->capacity = 0;
}
void __sd_free(const StringData *buff) { free((void *)buff); }

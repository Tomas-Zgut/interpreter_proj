#ifndef __STRING_BUFFER_H__
#define __STRING_BUFFER_H__

#include "tokens.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
/**
 * @brief Struct represents a mutable String type
 */
typedef struct {
	char *data;		 // pointer to data (owns)
	size_t length;	 // lenght of the string
	size_t capacity; // capacity for string
} StringBuffer;

/**
 * @brief Struct represents a immutable non owning view into a string type
 */
typedef struct {
	const char *data;	 // pointer to data
	const size_t length; // legnth of the view
} StringView;

/**
 * @brief Struct represents an immutable onwing string type
 */
typedef struct {
	const char *data; // pointer to data (owing)
	size_t length;	  // length of the string
} StringData;

/**
 * @brief Overloaded function for returing a substring
 *
 * @par Function creats a substring from a buffer.
 * Supported buffer types:
 * - StringBuffer  -> mutable string type @see StringBuffer
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - StringData -> owning immutable string type @see StringData
 *
 * @param buff - buffer to extract substring from (type dependent)
 * @param offset - offset in the buffer where the substring starts
 * @param len - length of the substring (when 0 the length to the end of the
 * buffer is used)
 *
 * @returns a non owing immtable view into the buffer @see StringView
 *
 * @see __sb_get_substring_impl
 */
#define sb_get_substring(buff, offset, len)                                    \
	_Generic((buff),                                                           \
		const StringBuffer *: __sb_get_substring_impl,                         \
		const StringData *: __sb_get_substring_impl,                           \
		const StringView *: __sb_get_substring_impl)(buff->data, buff->length, \
													 offset, len)
/**
 * @brief Overloaded function for returing a substring
 *
 * @par Function creats a substring from a buffer.
 * Supported buffer types:
 * - StringBuffer  -> mutable string type @see StringBuffer
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - StringData -> owning immutable string type @see StringData
 * The lenght of the substring returned is till the end of the buffer
 *
 * @param buff - buffer to extract substring from (type dependent)
 * @param offset - offset in the buffer where the substring starts
 *
 * @returns a non owing immtable view into the buffer @see StringView
 *
 * @see __sb_get_substring_impl
 */
#define sb_get_view(buff, offset)                                              \
	_Generic((buff),                                                           \
		const StringBuffer *: __sb_get_substring_impl,                         \
		const StringData *: __sb_get_substring_impl,                           \
		const StringView *: __sb_get_substring_impl)(buff->data, buff->length, \
													 offset, buff->length)
/**
 * @brief Overloaded function for creating a  copy of a string
 *
 * @par Function creats an immutable copy of a string type
 * Supported buffer types:
 * - StringBuffer  -> mutable string type @see StringBuffer
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - StringData -> owning immutable string type @see StringData
 *
 * @param buff - buffer to copy (type dependent)
 *
 * @returns an owing immtable copy of the buffer if copying was succesfull, NULL
 * otherwise @see StringData
 *
 * @see __sb_copy_impl
 */
#define sb_copy(buff)                                                          \
	_Generic((buff),                                                           \
		StringBuffer *: __sb_copy_impl,                                        \
		const StringBuffer *: __sb_copy_impl,                                  \
		const StringData *: __sb_copy_impl,                                    \
		const StringView *: __sb_copy_impl)(buff->data, buff->length)

/**
 * @brief Overloaded function for creating a  copy of a string
 *
 * @par Function creats a mutable copy of a string type
 * Supported buffer types:
 * - StringBuffer  -> mutable string type @see StringBuffer
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - StringData -> owning immutable string type @see StringData
 *
 * @param buff[in] - buffer to copy (type dependent)
 * @param out[out] - buffer where to store the copy
 *
 * @returns true if copying was succesfull, false otherwise
 *
 * @see __sb_copy_mut_impl
 */
#define sb_copy_mut(buff, out)                                                 \
	_Generic((buff),                                                           \
		StringBuffer *: __sb_copy_mut_impl,                                    \
		const StringBuffer *: __sb_copy_mut_impl,                              \
		const StringData *: __sb_copy_mut_impl,                                \
		const StringView *: __sb_copy_mut_impl)(buff->data, buff->length, out)

#define sb_free(buff)                                                          \
	_Generic((buff)StringBuffer *: __sb_free,                                  \
		const StringData *: _sd_free)(buff)

bool sb_init(StringBuffer *buff, size_t initial_capacity);
bool sb_append_char(StringBuffer *buf, char c);
void sb_reset(StringBuffer *buf);
/**
 * @brief Funcion implements creating substrings from a string type
 *
 * @param buff: pointer to character data
 * @param buff_len: lenght of the character data
 * @param offest: offset into the buffer data where the substring starts
 * @param len: lenght of the substring, if 0 the lenght is till the end of the
 * buffer
 *
 * @note This function should not be used @see sb_get_substring
 *
 * @returns a non owing immtable view into the buffer @see StringView
 */
StringView __sb_get_substring_impl(const char *buff, size_t buff_len,
								   size_t offset, size_t len);
/**
 * @brief Funcion implements freeing a string type
 *
 * @param buff: buffer to free
 *
 * @note This function should not be used @see sb_free
 */
void __sb_free(StringBuffer *buf);
/**
 * @brief Funcion implements freeing a string type
 *
 * @param buff: buffer to free
 *
 * @note This function should not be used @see sb_free
 */
void __sd_free(const StringData *buff);

/**
 * @brief This function implements creating an immutable copy of a string
 *
 * @param buff: pointer to character data
 * @param buff_len: length of the character data
 *
 * @note This function should not be used @see sb_copy
 */
const StringData *__sb_copy_impl(const char *buff, size_t buff_len);

/**
 * @brief This function imlpements creating a mutable copy of a string
 *
 * @param buff[in]: pointer to character data
 * @param buff_len[in]: length of the character data
 * @param out[out]: destination buffer for the copy
 *
 * @note This funstion should not be used @see sb_copy_mut
 */
bool __sb_copy_mut_impl(const char *buff, size_t buff_len, StringBuffer *out);
#endif
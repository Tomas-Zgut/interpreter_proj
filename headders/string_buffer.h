#ifndef __STRING_BUFFER_H__
#define __STRING_BUFFER_H__

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
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
		StringBuffer *: __sb_get_substring_impl,                               \
		const StringBuffer *: __sb_get_substring_impl,                         \
		StringData *: __sb_get_substring_impl,                           	   \
		const StringData *: __sb_get_substring_impl,                           \
		const StringView *: __sb_get_substring_impl)(                          \
		(buff)->data, (buff)->length, offset, len)
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
		StringBuffer *: __sb_get_substring_impl,                               \
		const StringBuffer *: __sb_get_substring_impl,                         \
		StringData *: __sb_get_substring_impl,                           	   \
		const StringData *: __sb_get_substring_impl,                           \
		const StringView *: __sb_get_substring_impl)(                          \
		(buff)->data, (buff)->length, offset, (buff)->length)
/**
 * @brief Overloaded function for creating a  copy of a string
 *
 * @par Function creats an immutable copy of a string type
 * Supported buffer types:
 * - StringBuffer  -> mutable string type @see StringBuffer
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - StringData -> owning immutable string type @see StringData
 *
 * @param buff[in] - buffer to copy (type dependent)
 * @param out[out] - destination for the copy
 *
 * @returns true if copying was succesfull, false otherwise
 *
 * @see __sb_copy_impl
 */
#define sb_copy(buff, out)                                                     \
	_Generic((buff),                                                           \
		StringBuffer *: __sb_copy_impl,                                        \
		const StringBuffer *: __sb_copy_impl,                                  \
		StringData *: __sb_copy_impl,                                    	   \
		const StringData *: __sb_copy_impl,                                    \
		const StringView *: __sb_copy_impl)((buff)->data, (buff)->length, (out))

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
 * @param out[out] - destination for the copy
 *
 * @returns true if copying was succesfull, false otherwise
 *
 * @see __sb_copy_mut_impl
 */
#define sb_copy_mut(buff, out)                                                 \
	_Generic((buff),                                                           \
		StringBuffer *: __sb_copy_mut_impl,                                    \
		const StringBuffer *: __sb_copy_mut_impl,                              \
		StringData *: __sb_copy_mut_impl,                                	   \
		const StringData *: __sb_copy_mut_impl,                                \
		const StringView *: __sb_copy_mut_impl)((buff)->data, (buff)->length,  \
												(out))

/**
 * @brief Overloaded function to free a string type
 * 
 * @par Function frees and deinitializes a string type.
 * Supported buffer types:
 * - StringBuffer  -> mutable string type @see StringBuffer
 * - StringData -> owning immutable string type @see StringData
 * 
 * @param buff: string to free (type dependent)
 * 
 * @see__sb_free
 * @see _sd_free
 */
#define sb_free(buff)                                                          \
	_Generic((buff), StringBuffer *: __sb_free, StringData *: __sd_free)((buff))


/**
 * @brief private macro for accesing fields of a string type
 * 
 * @par Macro used in the implenetation of a few functions that
 * reuire generic acces to a specific field
 * 
 * @see sb_length
 * @see sb_concat
 * @see sb_concat_mut
 */
#define __FIELD_ACCES(buff,field)				\
	_Generic((buff),							\
		StringBuffer *: (buff)->field, 			\
		const StringBuffer *: (buff)->field,	\
		StringData *: (buff)->field, 			\
		const StringData *: (buff)->field, 		\
		const StringView *:(buff)->field 		\
)

/**
 * @brief Overloaded function that returns the length of a srting type
 * 
 * @par function returns the length of a string type
 * Supported buffer types:
 * - StringBuffer  -> mutable string type @see StringBuffer
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - StringData -> owning immutable string type @see StringData
 * 
 * @param buff: string type (type dependent)
 * 
 * @returns length of the buffer as a size_t
 */
#define sb_length(buff) __FIELD_ACCES(buff,length)

/**
 * @brief Overloaded function that indexes into a string type
 * 
 * @par Function indexes into a string type and returns the char at
 * the given index.
 * Supported buffer types:
 * - StringBuffer  -> mutable string type @see StringBuffer
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - StringData -> owning immutable string type @see StringData
 * 
 * @param buff: target buffer (type dependent)
 * @param index: inex of the buffer
 * 
 * @returns char at the specified indesx of a raw string
 * 
 * @warning No bounds check, use carefuly!
 * @see __sb_get_char_at_impl
 */
#define sb_get_char_at(buff, index) 					\
	_Generic((buff), 									\
		StringBuffer *: __sb_get_char_at_impl, 			\
		const StringBuffer *: __sb_get_char_at_impl,	\
		StringData *: __sb_get_char_at_impl, 			\
		const StringData *: __sb_get_char_at_impl, 		\
		const StringView *:__sb_get_char_at_impl 		\
)((buff)->data, index)


/**
 * @brief Overloaded function that appends a string to mutable srting
 * 
 * @par Function appends the value of a string type to a mutable string
 * Supported buffer types:
 * - StringBuffer  -> mutable string type @see StringBuffer
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - StringData -> owning immutable string type @see StringData
 * 
 * @param[out] out: mutable string output
 * @param[in] buff: string to append (type dependent)
 * 
 * @returns true if the append was succesful, false otherwise
 * 
 * @see __sb_append_string_impl
 */
#define sb_append_string(out,buff) 						\
	_Generic((buff), 									\
		StringBuffer *: __sb_append_string_impl, 		\
		const StringBuffer *: __sb_append_string_impl,	\
		StringData *:__sb_append_string_impl,			\
		const StringData *:__sb_append_string_impl,		\
		const StringView *:__sb_append_string_impl 		\
)((out),(buff)->data, (buff)->length)	

/**
 * @brief Ovlerloaded function concatenates 2 strings and puts them
 * into an immutable buffer destination.
 *
 * @par Funcion concatenates data genericaly between any 2 string types and
 * saves the result into an immutable string @see StringData.
 * Supported buffer types:
 * - StringBuffer  -> mutable string type @see StringBuffer
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - StringData -> owning immutable string type @see StringData
 * 
 * @param out[out]: immutable srting @see StringData
 * @param buff1[in]: string to concat (type dependent)
 * @param buff2[in]: string to concat (type dependent)
 * 
 * @returns true if concatenations was succesful, false otherwise.
 * 
 * @see sb_concat_impl
 */
#define sb_concat(out,buff1, buff2) 	\
	__sb_concat_impl(					\
		(out),							\
		__FIELD_ACCES(buff1,data), 		\
		__FIELD_ACCES(buff1,length),	\
		__FIELD_ACCES(buff2,data),		\
		__FIELD_ACCES(buff2,length))


/**
 * @brief Overloaded function concatenates 2 strings and puts them
 * into a mutable buffer destination.
 * 
 * @par Funcion concatenates data genericaly between any 2 string types and
 * saves the reuslt into a mutable string @see StringBuffer.
 * Supported buffer types:
 * - StringBuffer  -> mutable string type @see StringBuffer
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - StringData -> owning immutable string type @see StringData
 * @param out[out]: mutable srting @see StringBuffer
 * @param buff1[in]: string to concat (type dependent)
 * @param buff2[in]: string to concat (type dependent)
 * 
 * @returns true if concatenations was succesful, false otherwise.
 * 
 * @see __sb_concat_mut_impl
 */
#define sb_concat_mut(out,buff1, buff2) 	\
	__sb_concat_mut_impl(					\
		(out),								\
		__FIELD_ACCES(buff1,data), 			\
		__FIELD_ACCES(buff1,length),		\
		__FIELD_ACCES(buff2,data),			\
		__FIELD_ACCES(buff2,length))



bool sb_init(StringBuffer *buff, size_t initial_capacity);
bool sb_append_char(StringBuffer *buf, char c);
void sb_reset(StringBuffer *buf);
/**
 * @brief Funcion implements creating substrings from raw string
 *
 * @param buff: raw string
 * @param buff_len: lenght of the raw string
 * @param offest: offset into the raw string where the substring starts
 * @param len: lenght of the substring
 *
 * @note This function should not be used @see sb_get_substring
 *
 * @returns a non owing immtable view into the buffer @see StringView
 */
StringView __sb_get_substring_impl(const char *buff, size_t buff_len,
								   size_t offset, size_t len);
/**
 * @brief Funcion frees a string type
 *
 * @param buff: buffer to free
 *
 * @note This function should not be used @see sb_free
 */
void __sb_free(StringBuffer *buf);
/**
 * @brief Funcion frees a string type
 *
 * @param buff: buffer to free
 *
 * @note This function should not be used @see sb_free
 */
void __sd_free(StringData *buff);

/**
 * @brief Function implements creating an immutable copy of a raw string
 *
 * @param buff[in]: raw string
 * @param buff_len[in]: length of raw string
 * @param out[out]: destination for the copy
 *
 * @note This function should not be used @see sb_copy
 *
 * @returns true if copying was succesfull, false otherwise
 */
bool __sb_copy_impl(const char *buff, size_t buff_len, StringData *out);

/**
 * @brief This function imlpements creating a mutable copy of a raw string
 *
 * @param buff[in]: raw string
 * @param buff_len[in]: length of raw string
 * @param out[out]: destination for the copy
 *
 * @note This funstion should not be used @see sb_copy_mut
 *
 * @returns true if copying was succesfull, false otherwise
 */
bool __sb_copy_mut_impl(const char *buff, size_t buff_len, StringBuffer *out);

/**
 * @brief Function that indexes into a raw string
 * 
 * @param buff: raw string
 * @param index: inex of the raw string
 * 
 * @warning No bounds check, use carefuly!
 * 
 * @note This function should not be used @see sb_get_char_at_impl
 * 
 * @returns char at the specified index of a given buffer
 */
static inline char __sb_get_char_at_impl(const char *buff, size_t index) {
	return buff[index];
}

/**
 * @brief Function appends a raw string to a mutable string
 * 
 * @param out[out]: mutable srting @see StringBuffer
 * @param buff[in]: raw string  to append
 * @param buff_len[in]: length of raw string 
 * 
 * @note Function should not be used @see sb_append_string
 * 
 * @returns true if append was succesful, false otherwise.
 */
bool __sb_append_string_impl(StringBuffer *out, const char *buff, size_t buff_len);

/**
 * @brief Function concatenates 2 string into an immutable string
 * 
 * @param out[out]: immutable srting @see StringData
 * @param buff1[in]: raw string to concatenate
 * @param buff_len1[in]: length of 1st raw string
 * @param buff2[in]: raw string to concatenate
 * @param buff_len2[in]: lenth of 2nd raw string
 * 
 * @note Function should not be used @see sb_concat
 * 
 * @returns true if concatenation was succesful, false otherwise.
 */
bool __sb_concat_impl(StringData *out, const char *buff1, size_t buff_len1, const char *buff2, size_t buff_len2);


 /**
 * @brief Function concatenates 2 strings into a mutable string
 * 
 * @param out[out]: mutable srting @see StringBuffer
 * @param buff1[in]: raw string to concatenate
 * @param buff_len1[in]: length of 1st raw string
 * @param buff2[in]: raw string to concatenate
 * @param buff_len2[in]: lenth of 2nd raw string
 * 
 * @note Function should not be used @see sb_concat_mut
 * 
 * @returns true if concatenation was succesful, false otherwise.
 */
bool __sb_concat_mut_impl(StringBuffer *out, const char *buff1, size_t buff_len1, const char *buff2, size_t buff_len2);
#endif
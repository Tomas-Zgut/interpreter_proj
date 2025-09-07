#ifndef __STRING_BUFFER_H__
#define __STRING_BUFFER_H__

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include<string.h>


#include"optionals.h"
/**
 * @brief Struct represents a mutable string type
 */
typedef struct {
    char *data;      // pointer to data (owns)
    size_t length;   // lenght of the string
    size_t capacity; // capacity for string
} StringMut;

/**
 * @brief Struct represents a immutable non owning view into a string type
 */
typedef struct {
    const char *data;    // pointer to data
    const size_t length; // legnth of the view
} StringView;

/**
 * @brief Struct represents an immutable onwing string type
 */
typedef struct {
    const char *data; // pointer to data (owing)
    size_t length;    // length of the string
} String;

/**
 * @brief Overloaded function for returing a substring
 *
 * @par Function creats a substring from a buffer.
 * Supported buffer types:
 * - StringMut   -> mutable string type @see StringMut
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - String -> owning immutable string type @see String
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
#define sb_get_substring(buff, offset, len)        \
    _Generic((buff),                 \
        StringMut *: __sb_get_substring_impl,      \
        const StringMut *: __sb_get_substring_impl, \
        String *: __sb_get_substring_impl,        \
        const String *: __sb_get_substring_impl,   \
        const StringView *: __sb_get_substring_impl \
)((buff)->data, (buff)->length, offset, len)

/**
 * @brief Overloaded function for returing a substring
 *
 * @par Function creats a substring from a buffer.
 * Supported buffer types:
 * - StringMut   -> mutable string type @see StringMut
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - String -> owning immutable string type @see String
 * The lenght of the substring returned is till the end of the buffer
 *
 * @param buff - buffer to extract substring from (type dependent)
 * @param offset - offset in the buffer where the substring starts
 *
 * @returns a non owing immtable view into the buffer @see StringView
 *
 * @see __sb_get_substring_impl
 */
#define sb_get_view(buff, offset)            \
    _Generic((buff),                 \
        StringMut *: __sb_get_substring_impl,      \
        const StringMut *: __sb_get_substring_impl, \
        String *: __sb_get_substring_impl,        \
        const String *: __sb_get_substring_impl,   \
        const StringView *: __sb_get_substring_impl \
)((buff)->data, (buff)->length, offset, (buff)->length - (offset))

/**
 * @brief Overloaded function for creating a  copy of a string
 *
 * @par Function creats an immutable copy of a string type
 * Supported buffer types:
 * - StringMut   -> mutable string type @see StringMut
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - String -> owning immutable string type @see String
 *
 * @param out[out] - destination for the copy
 * @param buff[in] - buffer to copy (type dependent)
 *
 * @returns true if copying was succesfull, false otherwise
 *
 * @see __sb_copy_impl
 */
#define sb_copy(out,buff)              \
    _Generic((buff),                 \
        StringMut *: __sb_copy_impl,      \
        const StringMut *: __sb_copy_impl, \
        String *: __sb_copy_impl,        \
        const String *: __sb_copy_impl,   \
        const StringView *: __sb_copy_impl \
)((out),(buff)->data, (buff)->length)

/**
 * @brief Overloaded function for creating a  copy of a string
 *
 * @par Function creats a mutable copy of a string type
 * Supported buffer types:
 * - StringMut   -> mutable string type @see StringMut
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - String -> owning immutable string type @see String
 *
 * @param out[out] - destination for the copy
 * @param buff[in] - buffer to copy (type dependent)
 *
 * @returns true if copying was succesfull, false otherwise
 *
 * @see __sb_copy_mut_impl
 */
#define sb_copy_mut(out,buff)              \
    _Generic((buff),                 \
        StringMut *: __sb_copy_mut_impl,      \
        const StringMut *: __sb_copy_mut_impl, \
        String *: __sb_copy_mut_impl,        \
        const String *: __sb_copy_mut_impl,   \
        const StringView *: __sb_copy_mut_impl \
)((out), (buff)->data, (buff)->length)

/**
 * @brief Overloaded function to free a string type
 *
 * @par Function frees and deinitializes a string type.
 * Supported buffer types:
 * - StringMut   -> mutable string type @see StringMut
 * - String -> owning immutable string type @see String
 *
 * @param buff: string to free (type dependent)
 *
 * @see__sb_free
 * @see _sd_free
 */
#define sb_free(buff)      \
    _Generic((buff),      \
    StringMut *: __sb_free, \
    String *: __sd_free     \
)((buff))

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
#define __FIELD_ACCES(buff,field)          \
    _Generic((buff),                 \
        StringMut *: (buff)->field,      \
        const StringMut *: (buff)->field,  \
        String *: (buff)->field,        \
        const String *: (buff)->field,   \
        const StringView *:(buff)->field    \
)

/**
 * @brief Overloaded function that returns the length of a srting type
 *
 * @par function returns the length of a string type
 * Supported buffer types:
 * - StringMut   -> mutable string type @see StringMut
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - String -> owning immutable string type @see String
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
 * - StringMut   -> mutable string type @see StringMut
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - String -> owning immutable string type @see String
 *
 * @param buff: target buffer (type dependent)
 * @param index: inex of the buffer
 *
 * @returns char at the specified indesx of a raw string
 *
 * @warning No bounds check, use carefuly!
 * @see __sb_get_char_at_impl
 */
#define sb_get_char_at(buff, index)              \
    _Generic((buff),                 \
        StringMut *: __sb_get_char_at_impl,      \
        const StringMut *: __sb_get_char_at_impl, \
        String *: __sb_get_char_at_impl,        \
        const String *: __sb_get_char_at_impl,   \
        const StringView *:__sb_get_char_at_impl    \
)((buff)->data, index)

/**
 * @brief Overloaded function that appends a string to mutable srting
 *
 * @par Function appends the value of a string type to a mutable string
 * Supported buffer types:
 * - StringMut   -> mutable string type @see StringMut
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - String -> owning immutable string type @see String
 *
 * @param[out] out: mutable string output
 * @param[in] buff: string to append (type dependent)
 *
 * @returns true if the append was succesful, false otherwise
 *
 * @see __sb_append_string_impl
 */
#define sb_append_string(out,buff)             \
    _Generic((buff),                 \
        StringMut *: __sb_append_string_impl,      \
        const StringMut *: __sb_append_string_impl, \
        String *:__sb_append_string_impl,         \
        const String *:__sb_append_string_impl,     \
        const StringView *:__sb_append_string_impl  \
)((out),(buff)->data, (buff)->length)  

/**
 * @brief Ovlerloaded function concatenates 2 strings and puts them
 * into an immutable buffer destination.
 *
 * @par Funcion concatenates data genericaly between any 2 string types and
 * saves the result into an immutable string @see String.
 * Supported buffer types:
 * - StringMut   -> mutable string type @see StringMut
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - String -> owning immutable string type @see String
 *
 * @param out[out]: immutable srting @see String
 * @param buff1[in]: string to concat (type dependent)
 * @param buff2[in]: string to concat (type dependent)
 *
 * @returns true if concatenations was succesful, false otherwise.
 *
 * @see sb_concat_impl
 */
#define sb_concat(out,buff1, buff2)      \
    __sb_concat_impl(              \
        (out),                 \
        __FIELD_ACCES(buff1,data),      \
        __FIELD_ACCES(buff1,length),    \
        __FIELD_ACCES(buff2,data),      \
        __FIELD_ACCES(buff2,length)     \
)


/**
 * @brief Overloaded function concatenates 2 strings and puts them
 * into a mutable buffer destination.
 *
 * @par Funcion concatenates data genericaly between any 2 string types and
 * saves the reuslt into a mutable string @see StringMut.
 * Supported buffer types:
 * - StringMut   -> mutable string type @see StringMut
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - String -> owning immutable string type @see String
 * @param out[out]: mutable srting @see StringMut
 * @param buff1[in]: string to concat (type dependent)
 * @param buff2[in]: string to concat (type dependent)
 *
 * @returns true if concatenations was succesful, false otherwise.
 *
 * @see __sb_concat_mut_impl
 */
#define sb_concat_mut(out,buff1, buff2) \
    __sb_concat_mut_impl(         \
        (out),                  \
        __FIELD_ACCES(buff1,data),      \
        __FIELD_ACCES(buff1,length),    \
        __FIELD_ACCES(buff2,data),      \
        __FIELD_ACCES(buff2,length)     \
)

/**
 * @brief Overloaded function for printing strings to a file
 *
 * @par Function prints a string type to a specified file.
 * Supported buffer types:
 * - StringMut   -> mutable string type @see StringMut
 * - StringView -> non owning immutable view into a buffer @see StringView
 * - String -> owning immutable string type @see String
 *
 * @warning Function only supports printing strings whose length is < INT32_MAX
 *
 * @see __sb_print_impl
 */
#define sb_print_file(file, buff)            \
    _Generic((buff),                 \
    StringMut *: __sb_print_impl,      \
    const StringMut *: __sb_print_impl, \
    String *: __sb_print_impl,        \
    const String *: __sb_print_impl,   \
    const StringView *: __sb_print_impl \
)((file), (buff)->data, (buff)->length)

/**
 * @brief Overloaded function for printing strings to stdout
 *
 * @par Function prints a string type to the standard output.
 * Supported buffer types:
 * - StringMut   -> mutable string type @see StringMut
 * - StringView -> non owning immutable view into a buffer @see StringView
 * - String -> owning immutable string type @see String
 *
 * @warning Function only supports printing strings whose length is < INT32_MAX
 *
 * @see __sb_print_impl
 */
#define sb_print(buff) sb_print_file(stdout, buff)

/**
 * @brief Overloaded function for comapring strings
 *
 * @par Function compares 2 strings, does not gurantee total ordering.
 * If both strings are the same 0 is returned. If they do not have the same lenght
 * the difference is of lengths is returned. If they are the same length the return value
 * is the same as memcmp.
 * Supported buffer types:
 * - StringMut   -> mutable string type @see StringMut
 * - StringVeiw -> non owning immutable view into a buffer @see StringView
 * - String -> owning immutable string type @see String
 *
 * @param buff1: string to compare (type dependend)
 * @param buff2: string to compare (type dependend)
 *
 * @returns 0 if both strings equal, non zero if strings are different @see memcmp
 *
 * @see __sb_cmp_impl
 */
#define sb_cmp(buff1,buff2)          \
    __sb_cmp_impl(                 \
        __FIELD_ACCES(buff1,data),      \
        __FIELD_ACCES(buff1,length),    \
        __FIELD_ACCES(buff2,data),      \
        __FIELD_ACCES(buff2,length)     \
    )  

bool sb_init(StringMut *buff, size_t initial_capacity);
bool sb_append_char(StringMut *buf, char c);
void sb_reset(StringMut *buf);

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
StringView __sb_get_substring_impl(const char *buff, size_t buff_len, size_t offset, size_t len);

/**
 * @brief Funcion frees a string type
 *
 * @param buff: buffer to free
 *
 * @note This function should not be used @see sb_free
 */
void __sb_free(StringMut *buf);

/**
 * @brief Funcion frees a string type
 *
 * @param buff: buffer to free
 *
 * @note This function should not be used @see sb_free
 */
void __sd_free(String *buff);

/**
 * @brief Function implements creating an immutable copy of a raw string
 *
 * @param out[out]: destination for the copy
 * @param buff[in]: raw string
 * @param buff_len[in]: length of raw string
 *
 * @note This function should not be used @see sb_copy
 *
 * @returns true if copying was succesfull, false otherwise
 */
bool __sb_copy_impl(String *out, const char *buff, size_t buff_len);

/**
 * @brief This function imlpements creating a mutable copy of a raw string
 *
 * @param out[out]: destination for the copy
 * @param buff[in]: raw string
 * @param buff_len[in]: length of raw string
 *
 * @note This funstion should not be used @see sb_copy_mut
 *
 * @returns true if copying was succesfull, false otherwise
 */
bool __sb_copy_mut_impl(StringMut *out, const char *buff, size_t buff_len);

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
    assert(buff != NULL);
    return buff[index];
}

/**
 * @brief Function appends a raw string to a mutable string
 *
 * @param out[out]: mutable srting @see StringMut
 * @param buff[in]: raw string  to append
 * @param buff_len[in]: length of raw string
 *
 * @note Function should not be used @see sb_append_string
 *
 * @returns true if append was succesful, false otherwise.
 */
bool __sb_append_string_impl(StringMut *out, const char *buff, size_t buff_len);

/**
 * @brief Function concatenates 2 raw strings into an immutable string
 *
 * @param out[out]: immutable srting @see String
 * @param buff1[in]: raw string to concatenate
 * @param buff_len1[in]: length of 1st raw string
 * @param buff2[in]: raw string to concatenate
 * @param buff_len2[in]: lenth of 2nd raw string
 *
 * @note Function should not be used @see sb_concat
 *
 * @returns true if concatenation was succesful, false otherwise.
 */
bool __sb_concat_impl(String *out, const char *buff1, size_t buff_len1, const char *buff2, size_t buff_len2);

 /**
 * @brief Function concatenates 2 raw strings into a mutable string
 *
 * @param out[out]: mutable srting @see StringMut
 * @param buff1[in]: raw string to concatenate
 * @param buff_len1[in]: length of 1st raw string
 * @param buff2[in]: raw string to concatenate
 * @param buff_len2[in]: lenth of 2nd raw string
 *
 * @note Function should not be used @see sb_concat_mut
 *
 * @returns true if concatenation was succesful, false otherwise.
 */
bool __sb_concat_mut_impl(StringMut *out, const char *buff1, size_t buff_len1, const char *buff2, size_t buff_len2);

/**
 * @brief Funcion prints a raw string to a file
 *
 * @param file: file to print to
 * @param buff: raw string to print
 * @param buff_len: length of raw string
 *
 * @note Function should not be used @see sb_print
 *
 * @warning Function only supports printing strings whose length is < INT32_MAX
 */
static inline void __sb_print_impl(FILE *file, const char *buff, size_t buff_len) {
    assert(buff_len < INT32_MAX);
    fprintf(file, "%.*s\n",(int32_t)buff_len, buff);
}

/**
 * @brief Function compares 2 raw strings
 *
 * @param buff1: raw string to compare
 * @param buff1_len: length of a raw string
 * @param buff2: raw string to compare
 * @param buff2_len: length of a raw string
 *
 * @par Function compares raw 2 strings, does not gurantee total ordering.
 * If both strings are the same 0 is returned. If they do not have the same lenght
 * the difference is of lengths is returned. If they are the same length the return value
 * is the same as memcmp.
 *
 * @note Function should not be used @see sb_cmp
 *
 * @returns 0 if both strings equal, non zero if strings are different @see memcmp
 */
int __sb_cmp_impl(const char *buff1, size_t buff1_len, const char *buff2, size_t buff2_len);

/**
 * @brief Function transfers the ownership from the passed in String
 *
 * @par Function is used to semanticaly transfer ownership from one string into another.
 * It creates a shallow copy of the source string.
 * 
 * @param string pointer to a string
 * 
 * @warning the input string should not be referenced again!!
 * 
 * @returns a new string with the ownership of the data
 */
static inline  String sb_move_string(String *string) {
	assert(string != NULL);
	assert(string->data != NULL);

	const char *tmp_data = string->data;
	const size_t tmp_lenght = string->length;
	memset(string,0,sizeof(String));
	return (String) {.data = tmp_data, .length = tmp_lenght};
}

/**
 * @brief Function transfers the ownership from the passed in StringMut
 * 
 * @par Function is used to semanticaly transfer ownership from one string into another.
 * It creates a shallow copy of the source string.
 * 
 * @param string pointer to a string
 *
 * @warning the input string should not be referenced again!!
 *
 * @returns a new string with the ownership of the data
 */
static inline  StringMut sb_move_string_mut(StringMut *string) {
	assert(string != NULL);
	assert(string->data != NULL);

	char *tmp_data = string->data;
	const size_t tmp_lenght = string->length;
	const size_t tmp_cap = string->capacity;
	memset(string,0,sizeof(StringMut));
	return (StringMut) {.data = tmp_data, .length = tmp_lenght,.capacity = tmp_cap};
}

/**
 * @brief funtion to transform an imuutable string into a mutable one
 * 
 * @par Function moves ownership of data from an immutable srting to a mutable one.
 * The original srting is left unititialized, it is safe to call sb_free on.
 * This function cannot be called more than once with the same sting
 * 
 * @param string: pointer to an owning immutable srting
 * 
 * @warning If this function is called more than once on a give string there is an assertion
 * 
 * @returns a mutable string with ownership of the original strings data
 */
static inline StringMut sb_to_mutable(String *string) {
	assert(string != NULL);
	assert(string->data != NULL);

	StringMut new_string = (StringMut){
		.data = (char *) string->data,
		.length = string->length,
		.capacity = string->length
	};
	memset(string,0,sizeof(String));
	return new_string;
}
DEFINE_OPTIONAL(String,str)
DEFINE_OPTIONAL(StringView,str_view)
DEFINE_OPTIONAL(StringMut,str_mut)

#endif
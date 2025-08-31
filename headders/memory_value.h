#ifndef __MEMORY_VALUE_H__
#define __MEMORY_VALUE_H__
#include "string_buffer.h"
typedef struct
{
	const char _unused;
} nill_t;

typedef union
{
	StringMut str_value;
	String str_lit_value;
	bool bool_value;
	int64_t int_value;
	double decimal_value;
	nill_t nill_value;
} value_t;

typedef enum
{
	VALUE_INT,
	VALUE_BOOL,
	VALUE_STRING_MUT,
	VALUE_STRING_LIT,
	VALUE_DOUBLE,
	VALUE_NILL,

} value_type;

typedef struct
{
	value_t value;
	value_type type;
} memory_value_t;

#define __MOVE_VALUE(val) \
	_Generic((val),\
		String: __sb_move_string(&val),\
		StringMut: __sb_move_string_mut(&val),\
		default: val,\
	)


#define __MAKE_MEM_VALUE(val,type)	\
	(memory_value_t) {				\
		.value = __MOVE_VALUE(val),	\
		.type = type				\
	}

/**
 * @brief Overloaded function that creates a memory_value
 * 
 * @par Function creates a value to be insterted into memory
 */
#define memory_value_create(val) 			\
	__MAKE_MEM_VALUE(val,					\
		_Generic((val),						\
			int64_t: VALUE_INT,				\
			bool: VALUE_BOOL,				\
			StringMut: VALUE_STRING_MUT,	\
			String: VALUE_STRING_LIT,		\
			double: VALUE_DOUBLE,			\
			nill_t: VALUE_NILL				\
	)										\
)

StringView memory_value_get_type_str(const memory_value_t *value);

static inline value_type memory_value_get_type(const memory_value_t *value)
{
	return value->type;
}

void memory_value_free(memory_value_t *value);

#endif
#ifndef __MEMORY_VALUE_H__
#define __MEMORY_VALUE_H__
#include "string_buffer.h"
typedef struct
{
	char _unused;
} nill_t;

DEFINE_OPTIONAL(nill_t,nill)
typedef struct 
{
	uint8_t type_str_idx;
}type_string;

typedef struct 
{
	char _unused;
}undefined_t;


typedef union
{
	StringMut str_value;
	String str_lit_value;
	type_string type_str_value;
	bool bool_value;
	int64_t int_value;
	double decimal_value;
	nill_t nill_value;
	undefined_t undef_value;
} value_t;

typedef enum
{
	VALUE_UNDEFINED,			// undefnied value
	VALUE_INT = 0b1,			//integer is stored
	VALUE_BOOL = 0b10,			// bool is stored
	VALUE_DOUBLE = 0b100,		// dobule is strored
	VALUE_STRING = 0b1000,		// string is stored
	__VALUE_STRING_MUT = 0b1001,	// internal do not use
	__VALUE_STRING_LIT = 0b1010,	// internal do no use
	VALUE_TYPE_STRING = 0b1100, // special string is stored
	VALUE_NILL = 0b10000,		// nil is stored
} value_type;

typedef struct
{
	value_t value;
	value_type type;
} memory_value_t;


/**
 * @brief Macro that moves string types or just copies non string types
 * 
 * @par Macro calls the appropriate move function for a given string type,
 * if not a string type is passed in, it is dereferenced. The casts are there
 * to ensure that if a non string pointer is the @p value the compiler correctly 
 * picks the default branch and just dereferences the passed in @p value. 
 * 
 * @param value: pointer to a value
 * @param cast_t: type to cast the @p value
 */
#define __MOVE_VALUE(value,cast_t)								\
	_Generic(((cast_t *)(value)),								\
		String *: sb_move_string((String *)(value)),			\
		StringMut *: sb_move_string_mut((StringMut *)(value)),	\
		default: *((cast_t *) (value))							\
	)

/**
 * @brief Macro that creates a memory value
 * 
 * @param val: pointer to a value
 * @param TYPE: the type to store in the tagged union
 * @param member: what member of the union to assign to
 * @param cast_t: the unqualified type of @p val
 */
#define __MAKE_MEM_VALUE(val,TYPE,member,cast_t)		\
	(memory_value_t) {									\
		.value = (value_t) {							\
			.member = __MOVE_VALUE(val,cast_t)			\
		},												\
		.type = TYPE,									\
	}

/**
 * @brief Macro that stores new @p val into @p mem_val
 * 
 * @param mem_val: pointer to a memory value
 * @param val: value to store
 * @param TYPE: type descriptor for memory value with the type of @p val
 * @param member: which member to assign to
 * @param cast_t: type to cast @p val to
 */
#define __STORE_MEM_VALUE(mem_val,val,TYPE, member,cast_t) 	\
	((mem_val)->value.member = __MOVE_VALUE(val,cast_t),	\
	(mem_val)->TYPE = type)									\


/**
 * @brief Overloaded function that creates a memory_value from the value it should store
 * 
 * @par Function creates a value to be insterted into memory.
 * * Supported types:
 * - int64_t
 * - bool
 * - StringMut
 * - String
 * - double
 * - type_string
 * - nill_t
 * 
 * @param val: pointer to a value to store in the union [type dependent]
 * 
 * @returns new memory value created from the passed in value
 */
#define memory_value_create(val)															\
	_Generic((val),																			\
		int64_t *: __MAKE_MEM_VALUE(val,VALUE_INT,int_value,int64_t),						\
		bool *: __MAKE_MEM_VALUE(val,VALUE_BOOL,bool_value,bool),							\
		StringMut *: __MAKE_MEM_VALUE(val,__VALUE_STRING_MUT,str_value,StringMut),			\
		String *: __MAKE_MEM_VALUE(val,__VALUE_STRING_LIT,str_lit_value,String),			\
		double *: __MAKE_MEM_VALUE(val,VALUE_DOUBLE,decimal_value,double),					\
		type_string *: __MAKE_MEM_VALUE(val,VALUE_TYPE_STRING,type_str_value,type_string),	\
		nill_t *: __MAKE_MEM_VALUE(val,VALUE_NILL,nill_value,nill_t)						\
)

/**
 * @brief Overloaded function to stores a new value into a provided memory value
 * 
 * @par Function updates the passed in @p mem_val in place with it's new @p val.
 * Supported types:
 * - int64_t
 * - bool
 * - StringMut
 * - String
 * - double
 * - type_string
 * - nill_t
 * 
 * @param mem_val: pointer to a memory value
 * @param val: value to store (type dependet)
 * 
 */
#define memory_value_store(mem_val,val)																\
(memory_value_free(mem_val),																		\
	_Generic((val),																					\
		int64_t *: __STORE_MEM_VALUE(mem_val,val,VALUE_INT,int_value,int64_t),						\
		bool *: __STORE_MEM_VALUE(mem_val,val,VALUE_BOOL,bool_value,bool),							\
		StringMut *: __STORE_MEM_VALUE(mem_val,val,__VALUE_STRING_MUT,str_value,StringMut),			\
		String *: __STORE_MEM_VALUE(mem_val,val,__VALUE_STRING_LIT,str_lit_value,String),			\
		double *: __STORE_MEM_VALUE(mem_val,val,VALUE_DOUBLE,decimal_value,double),					\
		type_string *: __STORE_MEM_VALUE(mem_val,val,VALUE_TYPE_STRING,type_str_value,type_string),	\
		nill_t *: __STORE_MEM_VALUE(mem_val,val,VALUE_NILL,nill_value,nill_t)						\
	)																								\
)

/**
 * @brief function that default initializes a new memory_value
 * 
 * @returns New default initialized memory_value
 */
static inline memory_value_t memory_value_create_default() {
	static const undefined_t undef = {0}; //
	return __MAKE_MEM_VALUE(&undef,VALUE_UNDEFINED,undef_value,undefined_t);
}

/**
 * @brief function retuns a StringView of the type string of a given memory value
 * 
 * @param value: value to get the type string from
 * 
 * @returns String of the type string
 */
StringView memory_value_get_type_view(const memory_value_t *value);

/**
 * @brief function returns the type string of a given memory value
 * 
 * @param value: pointer to a memory value
 * 
 * @returns type string of the value
 * 
 * @see type_string
 */
type_string memory_value_get_type_string(const memory_value_t *value);

/**
 * @brief fucntion that returns the type of a given memory value
 * 
 * @param value: pointer to a value in memory
 * 
 * @returns value_type of the passed in value
 * 
 * @see value_type
 */
static inline value_type memory_value_get_type(const memory_value_t *value)
{
	assert(value != NULL);

	return (value->type & VALUE_STRING)? VALUE_STRING : value->type;
}
/**
 * @brief function to free a given memory value
 * 
 * @par After thi sfunction call the given memory value is set to UNDEFINED
 * 
 * @param value: pointer to value to free
 */
void memory_value_free(memory_value_t *value);

/**
 * @brief function that checks if a value has the specifeid type
 * 
 * @param value: pointer to a value
 * @param type: expected type of a value
 * 
 * @returns true if the value has a specified type, false otherwise
 */
static inline bool memory_value_is_type(const memory_value_t *value, value_type type) {
	assert(value != NULL);
	return (value->type & type) == type;
}

/**
 * @brief function to get an  view of the string stored in the memory value
 * 
 * @par If the stored value is a string type, a view of it is returned, if not there is an assertion
 * 
 * @param value: pointer to a memory value
 * 
 * @warning If the stored value is not a string type there is an assertion
 * 
 * @returns string view of the string in @p value
 */
StringView memory_value_get_string(const memory_value_t *value);

/**
 * @brief function to get an mutalbe string from @p value
 * 
 * @par Function transfers ownsership of string data into @p out
 * if possible. If the stored data is a type string there is a copy,
 * otherwise the data is move into @p out.
 * 
 * @param value: pointer to a memory value
 * 
 * @warning If the stored value is not a string type there is an assertion
 * 
 * @returns  the mutable string in @p value
 */
bool memory_value_get_str_mut(StringMut *out, memory_value_t *value);

/**
 * @brief function to get bool value stored in the memory value
 * 
 * @par If the stored value if a bool the returned option contains it.
 * If the sotred value is not a bool an empty optional is returned
 * 
 * @returns optional bool value from @p value
 */
bool memory_value_get_bool(const memory_value_t *value);

/**
 * @brief function to get int value stored in the memory value
 * 
 * @par If the stored value if an int the returned option contains it.
 * If the sotred value is not an int an empty optional is returned
 * 
 * @returns optional int value from @p value
 */
int64_t memory_value_get_int(const memory_value_t *value);

/**
 * @brief function to get double value stored in the memory value
 * 
 * @par If the stored value if a double the returned option contains it.
 * If the sotred value is not a double an empty optional is returned
 * 
 * @returns and optional double value from @p value
 */
double memory_value_get_double(const memory_value_t *value);

/**
 * @brief function to get nill value stored in the memory value
 * 
 * @par If the stored value if a nill the returned option contains it.
 * If the sotred value is not a nill an empty optional is returned
 * 
 * @returns and optional nill value from @p value
 */
nill_t memory_value_get_nill(const memory_value_t *value);


/**
 * @brief function that moves ownership from @p mem_val to the return value
 * 
 * @param mem_val: pointer to memory value to move ouf of
 * 
 * @returns new memory value that owns the data of @p mem_val
 */
static inline memory_value_t memory_value_move(memory_value_t *mem_val) {
	assert(mem_val != NULL);

	memory_value_t new_val;
	memcpy(&new_val,mem_val,sizeof(memory_value_t));
	memset(mem_val,0,sizeof(memory_value_t));
	return new_val;
}

/**
 * @brief function to copy the data from @p val to @p out
 * 
 * @param out[out]: pointer to the destinaion of the copy
 * @param val[in]: source fro the copy
 * 
 * @returns true if the copy was successful, false otherwise
 */
bool memory_value_copy(memory_value_t *out, const memory_value_t *val);

#endif
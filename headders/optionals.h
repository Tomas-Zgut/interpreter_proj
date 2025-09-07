#ifndef __OPTIONALS_H__
#define __OPTIONALS_H__
#include <stdint.h>
/**
 * @brief Macro defines an optional type
 * 
 * @param TYPE: type the optional will sotre
 * @param NAME: name of the optional type
 */
#define DEFINE_OPTIONAL(TYPE, NAME) \
	typedef struct                  \
	{                               \
		const bool valid;           \
		TYPE data;                  \
	} opt_##NAME##_t;

/**
 * @brief Macro creates an empty optional of a given @p NAME
 * 
 * @param NAME: name of the optional to create
 */
#define MAKE_OPT_EMTPY(NAME) 	\
	(opt_##NAME##_t) { 0 }

/**
 * @brief Macro creates an optional that holds @p value of a given @p NAME
 * 
 * @param NAME: name of the optional to create
 * @param value: value that the optional holds
 */
#define MAKE_OPT_VALUE(NAME, value) \
	(opt_##NAME##_t)                \
	{                               \
		.valid = true,              \
		.data = (value)             \
	}

/**
 * @brief Macro that checks if a given optional value if valid
 * 
 * @param value: optional to check
 * 
 * @returns true if the optional is valid, false othewise
 */
#define IS_OPT_VALID(value) \
	(value.valid == true)
/**
 * @brief Macro that returns the value stored in the optional
 * 
 * @param value: optional to lift the value out of
 * 
 * @warning If the optional is not valid there is an assertion
 * 
 * @returns value stored in the optional
 */
#define OPT_GET_DATA(value) \
	(*(assert(value.valid == true),&value.data))


	
DEFINE_OPTIONAL(int64_t, int)
DEFINE_OPTIONAL(bool, bool)
DEFINE_OPTIONAL(double, double)

#endif
#ifndef __STACKS_H__
#define __STACKS_H__

#include <stdint.h>
#include <stdlib.h>
#include<string.h>
/**
 * @brief Macro that defines a given stack
 *
 * @param type: type of value the stack will hold
 * @param name: name for the stack type
 */
#define __DEFINE_STACK_STRUCT(TYPE, NAME) \
	typedef struct                        \
	{                                     \
		TYPE *data;                       \
		uint32_t size;                    \
		uint32_t capacity;                \
	} NAME##_stack_t;

#define __DEFINE_STACK_NOOP(TYPE,NAME) \
static inline void __##NAME##_stack_noop(TYPE *x) {(void)x;} 

/**
 * @brief Macro that defines an init function for a stack type
 *
 * @param type: type of value the stack will hold
 * @param name: name for the stack type
 */
#define __DEFINE_STACK_INIT(TYPE, NAME)                                  		\
	static inline bool NAME##_stack_init(NAME##_stack_t *s, uint32_t capacity) 	\
	{                                                                    		\
		s->data = malloc(sizeof(TYPE) * capacity);                       		\
		if (!s->data)                                                    		\
		{                                                                		\
			return false;                                                		\
		}                                                                		\
		memset(s->data,0,capacity*sizeof(TYPE));								\
		s->size = 0;                                                     		\
		s->capacity = capacity;                                          		\
		return true;                                                     		\
	}
/**
 * @brief Macro that defines a cleanup function for a stack type
 *
 * @param type: type of value the stack will hold
 * @param name: name for the stack type
 * @param cleanup: function to call on each element on the stack as it is freed
 */
#define __DEFINE_STACK_FREE(TYPE, NAME, CLEANUP)				\
	static inline void NAME##_stack_free(NAME##_stack_t *s)		\
	{															\
		if (s->data == NULL) {									\
			return;												\
		}														\
		for (size_t i = 0; i < s->size; i++)      				\
		{   													\
			CLEANUP(s->data + i);								\
		}                                         				\
		free(s->data);                                			\
		s->data = NULL;                               			\
		s->size = s->capacity = 0;                    			\
	}
/**
 * @brief Macro that defines a push function for a stack type
 *
 * @param type: type of value the stack will hold
 * @param name: name for the stack type
 */
#define __DEFINE_STACK_PUSH(TYPE, NAME, PTR, REF)                               \
	static inline bool NAME##_stack_push(NAME##_stack_t *s, TYPE PTR value) 	\
	{                                                                      		\
		if (s->size == s->capacity)                                        		\
		{                                                                  		\
			s->capacity = s->capacity ? s->capacity * 2 : 1;               		\
			TYPE *new_data = realloc(s->data, sizeof(TYPE) * s->capacity); 		\
			if (!new_data)                                                 		\
			{                                                              		\
				return false;                                              		\
			}                                                              		\
			s->data = new_data;													\
			memset(s->data + s->size,0,sizeof(TYPE)*(s->capacity - s->size));	\
		}																		\
		memcpy(s->data+s->size,REF value,sizeof(TYPE));							\
		s->size++;																\
		return true;                                                       		\
	}

/**
 * @brief Macro that defines a top function for a stack type
 *
 * @param type: type of value the stack will hold
 * @param name: name for the stack type
 */
#define __DEFINE_STACK_TOP(TYPE, NAME)                            			\
	static inline const TYPE *NAME##_stack_top(const NAME##_stack_t *s)		\
	{                                                             			\
		if (s->size == 0)                                         			\
		{                                                         			\
			return NULL;                                          			\
		}                                                         			\
		return (const TYPE *) s->data +s->size;								\
	}

/**
 * @brief Macro that defines a top function for a stack type
 *
 * @param type: type of value the stack will hold
 * @param name: name for the stack type
 */	
#define __DEFINE_STACK_TOP_MUT(TYPE, NAME)								\
	static inline TYPE *NAME##_stack_top_mut(const NAME##_stack_t *s)	\
	{                                                             		\
		if (s->size == 0)                                         		\
		{                                                         		\
			return NULL;                                          		\
		}                                                         		\
		return  s->data +s->size;										\
	}


/**
 * @brief Macro that defines a pop function for a stack type
 *
 * @param type: type of value the stack will hold
 * @param name: name for the stack type
 */
#define __DEFINE_STACK_POP(TYPE, NAME)                		\
	static inline TYPE *NAME##_stack_pop(NAME##_stack_t *s)	\
	{                                                 		\
		if (s->size == 0)                             		\
		{                                             		\
			return NULL;                              		\
		}                                             		\
		const uint32_t top = s->size;                 		\
		s->size--;                                    		\
		return s->data + top;                         		\
	}

/**
 * @brief Macro that defines an empty function for a stack type
 *
 * @param type: type of value the stack will hold
 * @param name: name for the stack type
 */
#define __DEFINE_STACK_EMPTY(TYPE, NAME)                     		\
	static inline bool NAME##_stack_empty(const NAME##_stack_t *s) 	\
	{                                                        		\
		return s->size == 0;                                 		\
	}

/**
 * @brief Macro that is used to define a generic stack
 *
 * @param type: type of value the stack will hold
 * @param name: name for the stack type
 * @param cleanup: function to call on each element on stack cleanup
 */
#define __DEFINE_STACK_IMPL(TYPE, NAME, CLEANUP, PTR, REF)	\
	__DEFINE_STACK_STRUCT(TYPE, NAME)            			\
	__DEFINE_STACK_INIT(TYPE, NAME)              			\
	__DEFINE_STACK_PUSH(TYPE, NAME, PTR, REF)           	\
	__DEFINE_STACK_TOP(TYPE, NAME)               			\
	__DEFINE_STACK_TOP_MUT(TYPE,NAME)						\
	__DEFINE_STACK_POP(TYPE, NAME)               			\
	__DEFINE_STACK_EMPTY(TYPE, NAME)             			\
	__DEFINE_STACK_NOOP(TYPE, NAME)							\
	__DEFINE_STACK_FREE(TYPE, NAME, CLEANUP)

#define __GET_NOOP(NAME) \
	__##NAME##_stack_noop

/**
 * @brief Macro that defines a generic stack with no clean up
 * function for its elements
 *
 * @param type: type of value the stack will hold
 * @param name: name for the stack type
 */
#define DEFINE_STACK(TYPE, NAME) \
	__DEFINE_STACK_IMPL(TYPE, NAME, __GET_NOOP(NAME), ,&)

/**
 * @brief Macro that defines a generic stack with a clean up
 * function for its elements
 *
 * @param type: type of value the stack will hold
 * @param name: name for the stack type
 * @param cleanup: function that should be called to clean up en element
 */
#define DEFINE_STACK_CLEANUP(TYPE, NAME, CLEANUP) \
	__DEFINE_STACK_IMPL(TYPE, NAME, CLEANUP, ,&)

#define DEFINE_STACK_PTR(TYPE,NAME) \
	__DEFINE_STACK_IMPL(TYPE,NAME,__GET_NOOP(NAME), *, )

#define DEFINE_STACK_PTR_CLEANUP(TYPE, NAME, CLEANUP) \
	__DEFINE_STACK_IMPL(TYPE, NAME, CLEANUP, *, )
#endif
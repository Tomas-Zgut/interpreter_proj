#ifndef __MEMORY_FRAME_H__
#define __MEMORY_FRAME_H__
#include "robin_hood_table.h"
#include "memory_value.h"
typedef struct
{
	rh_table_t _table;
} memory_frame_t;

#define MEMORY_FRAME_INIT_SIZE 16

static inline bool memory_frame_init(memory_frame_t *frame)
{
	return rh_table_init_deleter(&frame->_table, sizeof(memory_value_t), MEMORY_FRAME_INIT_SIZE, memory_value_free) == RH_TABLE_SUCCESS;
}

static inline const memory_value_t *memory_frame_get_val(const memory_frame_t *frame, const StringView *key)
{
	memory_value_t *out_val = NULL;
	rh_table_look_up(&frame->_table, key, (void **)&out_val);
	return (const memory_value_t *)out_val;
}

static inline memory_value_t *memory_frame_get_val_mut(const memory_frame_t *frame, const StringView *key)
{
	memory_value_t *out_val = NULL;
	rh_table_look_up(&frame->_table, key, (void **)&out_val);
	return out_val;
}

static inline bool memory_frame_insert_new_val(memory_frame_t *frame, const StringView *key, const memory_value_t *val)
{
	void **out_val = (void **)&val;
	rh_table_ret ret = rh_table_insert(&frame->_table, key, out_val);
	if (ret == RH_TABLE_TABLE_FULL)
	{
		ret = rh_table_resize(&frame->_table);
		if (ret == RH_TABLE_ALLOC_FAIL)
		{
			fprintf(stderr, "Memory error! Intepreter ran out of memory!\n");
		}
		ret = rh_table_insert(&frame->_table, key, out_val);
	}

	if (ret == RH_TABLE_KEY_FOUND)
	{
		fprintf(stderr, "Variable redeclaration attempt!\n");
		return false;
	}

	return true;
}

static inline void memory_frame_free(memory_frame_t *frame)
{
	rh_table_free(&frame->_table);
}

#endif

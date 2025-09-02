#ifndef __MEMORY_FRAME_H__
#define __MEMORY_FRAME_H__
#include "robin_hood_table.h"
#include "memory_value.h"
/**
 * @brief struct representin a memory frame
 */
typedef struct
{
	rh_table_t _table; //underlying hash table
} memory_frame_t;

#define MEMORY_FRAME_INIT_SIZE 16 // default initialized size of the memory frame

/**
 * @brief function to free the memory of the memory frame
 * 
 * @param input: pointer to data to clean up
 */
static inline void memory_frame_data_cleanup(void *input) {
	assert(input != NULL);
	memory_value_free((memory_value_t *)input);
}

/**
 * @brief function to initialize a new memory frame
 * 
 * @param frame[out]: pointer to a memory frame to initialize
 * 
 * @returns true if the initialization was successful, false otherwise
 */
static inline bool memory_frame_init(memory_frame_t *frame)
{
	assert(frame != NULL);
	return rh_table_init_deleter(&frame->_table, sizeof(memory_value_t), MEMORY_FRAME_INIT_SIZE, memory_frame_data_cleanup) == RH_TABLE_SUCCESS;
}

/**
 * @brief funtion to get an immutable reference to a value stored in memory
 * 
 * @param frame: pointer to a memoy frame
 * @param key: name of the variable to lookup
 * 
 * @returns const pointer to the stored data if the varaible exists, NULL otherwise
 */
static inline const memory_value_t *memory_frame_get_val(const memory_frame_t *frame, const StringView *key)
{
	assert(frame != NULL);
	assert(key != NULL);

	memory_value_t *out_val = NULL;
	rh_table_look_up(&frame->_table, key, (void **)&out_val);
	return (const memory_value_t *)out_val;
}

/**
 * @brief funtion to get an mutable reference to a value stored in memory
 * 
 * @param frame: pointer to a memoy frame
 * @param key: name of the variable to lookup
 * 
 * @returns pointer to the stored data if the varaible exists, NULL otherwise
 */
static inline memory_value_t *memory_frame_get_val_mut(const memory_frame_t *frame, const StringView *key)
{
	assert(frame != NULL);
	assert(key != NULL);

	memory_value_t *out_val = NULL;
	rh_table_look_up(&frame->_table, key, (void **)&out_val);
	return out_val;
}

/**
 * @brief funtion to insert a new value into a memory frame
 * 
 * @par Funtion tires to insert a new key into the memory frame, if the key exists function fails.
 * 
 * @param frame: pointer to a memoy frame
 * @param key: name of the variable to lookup
 * @param val: pointer to the value to insert
 * 
 * @returns true if the insert was successful, false otherwise
 */
static inline bool memory_frame_insert_new_val(memory_frame_t *frame, const StringView *key, const memory_value_t *val)
{
	assert(frame != NULL);
	assert(key != NULL);
	assert(val != NULL);

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

/**
 * @brief function to move a memory_frame
 * 
 * @par Fuiton returns a new memory frame with the ownership of data of te old one.
 * This function cannot be called more than once on a given memory frame. If it safe to
 * free the old memory  frame
 * 
 * @param frame: pointer to a frame to move out of
 * 
 * @returns new memory frame with ownership of frame's data
 */
static inline memory_frame_t memory_frame_move(memory_frame_t *frame) {
	assert(frame != NULL);

	memory_frame_t new_frame = (memory_frame_t) {
		._table = rh_table_move(&frame->_table)
	};
	return new_frame;
}


/**
 * @brief function that checks if a memory frame has been initialized
 * 
 * @param frame: pointer to a memory frame
 * 
 * @returns true if the memory_frame has been initialized, false otherwise
 */
static inline bool memory_frame_initialized(const memory_frame_t *frame) {
	assert(frame != NULL);

	return rh_table_initialized(&frame->_table);
}

/**
 * @brief function to clean up a memory frame
 * 
 * @param frame: pointer to a memory frame
 */
static inline void memory_frame_free(memory_frame_t *frame)
{	
	assert(frame != NULL);

	rh_table_free(&frame->_table);
}

#endif

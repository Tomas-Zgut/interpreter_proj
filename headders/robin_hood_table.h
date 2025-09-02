#ifndef __ROBIN_HOOD_TABLE_H__
#define __ROBIN_HOOD_TABLE_H__
#include "string_buffer.h"
#include <string.h>
/**
 * @brief structure of a table entry
 */
typedef struct
{
	uint32_t data_index;	// index to the data
	uint16_t distance;		// distance from an ideal locaion 
	uint16_t finger_print;	// 1st bit falg, 15bits top 15bits of hash
} rh_table_entry_t;

/**
 * @brief pointer to users cleanup function
 * 
 * @param elm_ptr: pointer to the data that needs to be deleted
 * 
 * @note the elm_ptr belongs to the table and it is only valid during the
 * execution of this funcion
 */
typedef void (*cleanup_func_t)(void *elm_ptr);

/**
 * @brief structure of a robin hood hasing table
 * 
 * @par Table is a robin hood table so that means 
 * that the data that is stored inside must be trivialy 
 * relocatable => can use memcpy to move it around.
 * Table supports iteration.
 */
typedef struct
{	
	rh_table_entry_t *entries; 	// array of table metadata
	String *entry_keys;			// array for entry keys
	uint8_t *entry_data;		// array for entry data
	cleanup_func_t cleanup;		// poitner to a cleanup function
	uint64_t entry_data_size;	// size of entry data
	uint32_t size;				// size of a table
	uint32_t capacity;			// capacity of a table
} rh_table_t;


/**
 * @brief struct for the table's iterator
 * 
 * @par The iterator is not stable, if any changes happen to the table
 * a new iterator should be created or the old one reinitialized.
 */
typedef struct
{
	const rh_table_t *table;	// pointer to a table
	uint32_t table_line;		// index of the current entry in the table
} rh_table_iter_t;

/**
 * @brief enum representing return values of
 * table functions
 */
typedef enum {
	RH_TABLE_ALLOC_FAIL,
	RH_TABLE_SUCCESS,
	RH_TABLE_TABLE_FULL,
	RH_TABLE_TABLE_EMPTY,
	RH_TABLE_KEY_FOUND,
	RH_TABLE_KEY_NOT_FOUND,
	RH_TABLE_INVALID_ARGS
} rh_table_ret;


/**
 * @brief enum representing different states of iteration 
 * over the table
 */
typedef enum
{
	RH_TABLE_ITER_NEXT,
	RH_TABLE_ITER_END,
} rh_table_iter_ret;

/**
 * @brief function to initialize the table with a deleter function
 *
 * @param[out] table: table to initialize
 * @param[in] entry_size: the size of user's data
 * @param[in] size: the initial capaciy of the table
 * @param[in] deleter: user provided function to proprerly delete their data
 * 
 * @returns
 * - `RH_TABLE_ALLOC_FAIL` - in case the allocation fails
 * 
 * - `RH_TABLE_SUCCESS` - in case the function succeeds
 * 
 * @see rh_table_ret
 */
rh_table_ret rh_table_init_deleter(rh_table_t *table, uint32_t entry_size, uint32_t size,cleanup_func_t deleter);

/**
 * @brief function to initialize the table, without a deleter fucntion
 *
 * @param[out] table: table to initialize
 * @param[in] entry_size: the size of user's data
 * @param[in] size: the initial capaciy of the table
 * 
 * @returns
 * - `RH_TABLE_ALLOC_FAIL` - in case the allocation fails
 * 
 * - `RH_TABLE_SUCCESS` - in case the function succeeds
 * 
 * @see rh_table_ret
 * @rh_table_init_deleter
 */
static inline rh_table_ret rh_table_init(rh_table_t *table, uint32_t entry_size,uint32_t size) {
	return rh_table_init_deleter(table, entry_size, size,NULL);
}


/**
 * @brief function to look up data in the table with a key
 * 
 * @par Function on success sets `data_out` to the pointer of the found data. On failure
 * is the value of that parameter undefined
 * 
 * @param[in] table: pointer to a table
 * @param[in] key: key to look up
 * @param[out] data_out: where to store the pointer to the found data
 * 
 * @returns
 * - `RH_TABLE_KEY_NOT_FOUND` - in case the key is not found

 * - `RH_TABLE_SUCCESS` - in case the function succeeds
 * 
 * @see rh_table_ret
 */
rh_table_ret rh_table_look_up(const rh_table_t *table, const StringView *key, void **data_out);

/**
 * @brief function to insert a new key value pair to the table
 * 
 * @par Function inserts the key value pair into the table if the key is not found.
 * If the key is found than function has update on insert semantics. On success the
 * `data_out` points to the inserted data. On failure `data_out` is undefined
 * 
 * @param[in] table: pointer to a table
 * @param[in] key: key to insert
 * @param[in,out] data_out: data to insert, after succes pointer to the inserted data
 * 
 * @returns
 * - `RH_TABLE_TABLE_FULL` - in case the table is full
 * 
 * - `RH_TABLE_KEY_FOUND` - in case key is found and to be updated
 * 
 * - `RH_TABLE_SUCCESS` - in case key is inserted into the table
 * 
 * - `RH_TABLE_ALLOC_FAIL` - in case there is a failure when allocating the key
 * 
 * @see rh_table_ret
 */
rh_table_ret rh_table_insert(rh_table_t *table, const StringView *key, void **data_out);

/**
 * @brief function to delete a table entry
 * 
 * @par If the user has provided a custom deleter it will be called for the deleted element
 * 
 * @param table: pointer to a table
 * @param key: key to delete
 * 
 * @returns
 * - `RH_TABLE_TABLE_EMPTY` - in case the table is emty
 * 
 * - `RH_TABLE_KEY_NOT_FOUND` - in case the specified key is in the table
 * 
 * - `RH_TABLE_SUCCESS` - in case the funcion succeeds
 * 
 * @see rh_table_ret
 */
rh_table_ret rh_table_delete(rh_table_t *table, const StringView *key);


/**
 * @brief function to resize the table
 * 
 * @par Function resizes the table by doubling its capacity,
 * this requires a rehash of all the keys. I the resize fails
 * the original data table is not affected
 * 
 * @param table: pointer to a table
 * 
 * @returns
 * - `RH_TABLE_ALLOC_FAIL` - in case the allocation fails
 * 
 * - `RH_TABLE_SUCCESS` - in case the function succeeds
 * 
 * @see rh_table_res
 */
rh_table_ret rh_table_resize(rh_table_t *table);

/**
 * @brief function to clear out a table
 * 
 * @param table: pointer to a table
 *
 * @note User data is not clean up, user needs to iterate over 
 * the table to clean up the data.
 */
void rh_table_clear(rh_table_t *table);

/**
 * @brief function to clean up a table
 * 
 * @par If the user has provided a custom deleter it will be called for each elemet
 * 
 * @param table: pointer to a table
 */
void rh_table_free(rh_table_t *table);

/**
 * @brief gettern for the table's size
 * 
 * @par If the user has provided a custom deleter it will be called for each elemet
 * 
 * @param table: pointer to a table
 * 
 * @returns number of entries in the table
 */
static inline uint32_t rh_table_size(const rh_table_t *table) {
	assert(table != NULL);
	
	return table->size;
}

/**
 * @brief getter for the table's capacity
 * 
 * @param table: pointer to a table
 * 
 * @returns capacity of a table
 */
static inline uint32_t rh_table_capacity(const rh_table_t *table) {
	assert(table != NULL);
	
	return table->capacity;
}

/**
 * @brief fucntion to create an iterator for a table
 * 
 * @par Function creates an iterator to a table and sets it
 * for the 1st key value pair in the table
 * 
 * @param table: pointer to a table
 * 
 * @returns an iterator for a given table
 * 
 * @see rh_table_iter_t
 */
rh_table_iter_t rh_table_iter_init(const rh_table_t *table);

/**
 * @brief function to move the iterator to the next key value
 * pair in the table
 * 
 * @param iter: pointer to a table iterator
 * 
 * @returns
 * - `RH_TABLE_ITER_NEXT` - iteration can continue
 * 
 * - `RH_TABLE_ITER_END` - iteration ends
 * 
 * @see rh_table_iter_ret
 */
rh_table_iter_ret rh_table_iter_next(rh_table_iter_t *iter);

/**
 * @brief function checks if the iterator should continue iterating
 * 
 * @param iter: pointer to a table iterator
 * 
 * @returns true if iteration should continue, false otherwise
 */
static inline bool rh_table_iter_valid(const rh_table_iter_t *iter) {
	assert(iter != NULL);

	return iter->table_line < iter->table->capacity;
}

/**
 * @brief function retrieves key from the table throug the iterator
 * 
 * @param iter: pointer to an iterator
 * 
 * @returns view of the key stored in the table at the current position
 */
static inline StringView rh_table_iter_get_key(const rh_table_iter_t *iter) {
	assert(iter != NULL);

	const rh_table_t *table = iter->table;
	return sb_get_view(table->entry_keys + table->entries[iter->table_line].data_index,0);
	
}
/**
 * @brief function retrieves data from the table through the iterator
 * 
 * @param iter: pointer to an iterator
 * 
 * @returns pointer to data stored in the table at the current position
 */
static inline void *rh_table_iter_get_data(const rh_table_iter_t *iter) {
	assert(iter != NULL);

	const rh_table_t *table = iter->table;
	return table->entry_data + table->entries[iter->table_line].data_index*table->entry_data_size;
}


/**
 * @brief Macro used for implementing itaration over the table.
 * 
 * @par Macro is useful for types that contain the table to create iterators over those types (not just the table)
 */
#define __RH_TABLE_ITER_IMPL(iter_name,table,iter_type,MAKE_ITER,ACCESSS_TABLE,ACCESS_ITER)		\
	for (iter_type iter_name = MAKE_ITER(rh_table_iter_init(ACCESSS_TABLE(table)));				\
		rh_table_iter_valid(&(ACCESS_ITER(iter_name)));											\
		rh_table_iter_next(&(ACCESS_ITER(iter_name)))											\
	)

#define __EMTPY(expr) expr
/**
 * @brief Macro for easier iteration over the table. Creates a new iterator.
 * 
 * @param iter_name: name of the iterator variable
 * @param table: pointer to a table
 */
#define RH_TABLE_ITER(iter_name,table) __RH_TABLE_ITER_IMPL(iter_name,table,rh_table_iter_t,__EMTPY,__EMTPY,__EMTPY)


/**
 * @brief function to move a soruce table to a new table.
 * 
 * @par Function transfers ownership of data from @p src to a new table.
 * This leaves @p src in an uninitialized state, and can only be freed afterwards (or reinitalized).
 * This function can be called only one on an initialized table, otherwise there is an assertion 
 * 
 * @param src: table to move from
 * 
 * @returns a new table that has all the data from the @p src table
 * 
 * @warning If this function is called more than once on a given table there s an assertion
 */
static inline rh_table_t rh_table_move(rh_table_t *src) {
	assert(src != NULL);
	assert(src->entries != NULL);
	assert(src->entry_data != NULL);
	assert(src->entry_keys != NULL);

	rh_table_t new_table;
	memcpy(&new_table,src,sizeof(rh_table_t));
	memset(src,0,sizeof(rh_table_t));
	return new_table;
}
#endif
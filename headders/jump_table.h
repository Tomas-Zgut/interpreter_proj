#ifndef __JUMP_TABLE_H__
#define __JUMP_TABLE_H__
#include "robin_hood_table.h"

/**
 * @brief jump table type just wraps the hash table
 */
typedef struct
{
	rh_table_t _table; // internal implementation of a table
} jump_table_t;

typedef struct
{
	uint64_t destination;	 // destination for the label
	bool destination_filled; // flag .... true if destination is valid
} jump_table_entry_t;

typedef struct
{
	rh_table_iter_t _iter; // internal implenetation of the iterator
} jump_table_iter_t;

/**
 * @brief enum for jump tabel fucntion return type
 *
 * @par enums values are compatible with the underlying hashtable used
 */
typedef enum
{
	JUMP_TABLE_ALLOC_ERR = RH_TABLE_ALLOC_FAIL,
	JUMP_TABLE_SUCCESS = RH_TABLE_SUCCESS,
	JUMP_TABLE_NOT_FOUND = RH_TABLE_KEY_NOT_FOUND,
	JUMP_TABLE_DUPLICATE_FOUND = RH_TABLE_KEY_FOUND,
} jump_table_ret;

#define JUMP_TABLE_INIT_SIZE 32 // default starting table size

/**
 * @brief funciotn to initialize a jump table
 *
 * @param[out] table: pointer to a jump table
 *
 * @returns
 *  - `JUMP_TABLE_SUCCESS` - initialization was succesfull
 *
 *  - `JUMP_TABLE_ALLOC_ERR` - initalization failed due to a failed allocation
 */
static inline jump_table_ret jump_table_init(jump_table_t *table)
{
	assert(table != NULL);

	return (jump_table_ret)rh_table_init(&table->_table, sizeof(jump_table_entry_t), JUMP_TABLE_INIT_SIZE);
}

/**
 * @brief funciotn to find a jump_destiantion in a table
 *
 * @param table: pointer to a jump table
 * @param jump_label: label to where the jump points to
 *
 * @returns on success pointer to a jump table entry, on fail NULL
 *
 */
static inline jump_table_entry_t *jump_table_lookup(const jump_table_t *table, const StringView *jump_label)
{
	assert(table != NULL);
	assert(jump_label != NULL);

	jump_table_entry_t *result = NULL;
	rh_table_look_up(&table->_table, jump_label, (void **)&result);
	return result;
}

/**
 * @brief function to insert a new destiantion unde a given label
 *
 * @par Function tries to insert into a jump table.If the table is too small it tries to resize and than
 * reinserts the jump destination. If function succeeds the `out` parameter points to inserted data
 *
 * @param[in] table: pointer to a table
 * @param[in] jump_label: lable where to jump
 * @param[in,out] out: data to insert, points to inserted data on success
 *
 * @returns
 *  - `jUMP_TABLE_SUCCESS` - insert was successful, out points to inserted data
 *  - `JUMP_TABLE_DUPLICATE_FOUND` - attempted to insert an existing label
 *  - `JUMP_TABLE_ALLOC_ERR` - alocation failed during resizing
 */
static inline jump_table_ret jump_table_insert(jump_table_t *table, const StringView *jump_label, jump_table_entry_t **out)
{
	assert(table != NULL);
	assert(jump_label != NULL);
	assert(out != NULL);

	rh_table_ret ret = rh_table_insert(&table->_table, jump_label, (void **)out);

	if (ret == RH_TABLE_TABLE_FULL)
	{
		if (rh_table_resize(&table->_table) == RH_TABLE_ALLOC_FAIL)
		{
			return JUMP_TABLE_ALLOC_ERR;
		}
		ret = rh_table_insert(&table->_table, jump_label, (void **)out);
		assert(ret == RH_TABLE_SUCCESS);
	}

	return (jump_table_ret)ret;
}

/**
 * @brief function for cleaning up after a jump table
 * 
 * @param table: pointer to a jump table
 */
static inline void jump_table_free(jump_table_t *table) {
	assert(table != NULL);

	rh_table_free(&table->_table);
}

/**
 * @brief iterator getter for keys in jump table
 *
 * @param iter: pointer to a jump table iterator
 *
 * @returns key sotred in a jump table at iterator's position
 */
static inline StringView jump_table_iter_get_key(const jump_table_iter_t *iter)
{
	assert(iter != NULL);

	return rh_table_iter_get_key(&iter->_iter);
}

/**
 * @brief iterator getter for entries in jump table. Data can be mutated
 *
 * @param iter: pointer to a jump table iterator
 *
 * @returns mutable pointer to data in a jump table at iterator's position
 */
static inline jump_table_entry_t *jump_table_iter_get_entry_mut(const jump_table_iter_t *iter)
{	
	assert(iter != NULL);

	return (jump_table_entry_t *)rh_table_iter_get_data(&iter->_iter);
}

/**
 * @brief iterator getter for entries in jump table. Data cannot be mutated
 *
 * @param iter: pointer to a jump table iterator
 *
 * @returns immutable pointer to data in a jump table at iterator's position
 */
static inline const jump_table_entry_t *jump_table_iter_get_entry(const jump_table_iter_t *iter)
{
	assert(iter != NULL);

	return (const jump_table_entry_t *)rh_table_iter_get_data(&iter->_iter);
}


#define __JUMP_TABLE_MAKE_ITER(expr) {._iter = expr}
#define __JUMP_TABLE_ACCESS_TABLE(t) &table->_table
#define __JUMP_TABLE_ACCESS_ITER(iter_name) iter_name._iter

/**
 * @brief Macro for easy iteration over the jump table
 *
 * @par Macro is meant to be used before the block for the loop.
 * inside this block the `iter_name` variable is created and it is accesible.
 * To get data from the iterator use getter functions
 *
 * @param iter_name: name to give to the iterator
 * @param table: pointer to a table, from which to make the iterator
 *
 * @see jump_table_iter_get_entry
 * @see jump_table_iter_get_entry_mut
 * @see jump_table_iter_get_key
 */
#define JUMP_TABLE_ITER(iter_name, table) \
	__RH_TABLE_ITER_IMPL(iter_name, table, jump_table_iter_t, __JUMP_TABLE_MAKE_ITER, __JUMP_TABLE_ACCESS_TABLE, __JUMP_TABLE_ACCESS_ITER)

/**
 * @brief function checks is all entris in the table are valid.
 *
 * @param table: pointer to a jump table
 *
 * @returns true if all the entries have a valid destination, false otherwise
 */
static inline bool jump_table_check_entries(const jump_table_t *table)
{
	assert(table != NULL);

	JUMP_TABLE_ITER(iter, table)
	{
		const jump_table_entry_t *entry = jump_table_iter_get_entry(&iter);
		if (!entry->destination_filled)
		{
			return false;
		}
	}

	return true;
}

#endif
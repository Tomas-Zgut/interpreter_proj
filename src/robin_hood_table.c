#include<headders/robin_hood_table.h>
#include<headders/hashes.h>
#include<string.h>
#include<headders/memswap.h>
typedef enum {
    RH_TABLE_SLOT_FOUND_EMPTY,
    RH_TABLE_SLOT_FOUND_KEY,
    RH_TABLE_SLOT_FOUND_STEAL
} rh_table_find_ret;

typedef struct {
  const uint32_t pos;
  const uint32_t distance;
  const rh_table_find_ret result;
} rh_table_pos_t;


/*
*************************************************************************
*                  DECLERATIONS OF PRIVATE FUNCTIONS                    *  
*************************************************************************
*/

/**
 * @brief privte function for shifting back
 * entries for deletions
 * 
 * @param table: pointer to a table
 * @param deletion_index: index where the deletion happened
 */
static inline void __rh_table_shift_back(rh_table_t *table, uint32_t deletion_index);

/**
 * @brief private function for sawpping 2 key_value pairs in the table.
 * 
 * @par function swaps 2 key value pairs from a given table
 * 
 * @param table: pointer to a table
 * @param idx1: table index for 1st key value pair
 * @param idx2: table index for 2nd key value pair
 */
static inline void __rh_table_swap(rh_table_t *table, uint32_t idx1,uint32_t idx2);

/**
 * @brief private function for realocating the table
 * 
 * @par Funcion handles the realoc part of table resizing.
 * If function fails, it has no side effects.
 * 
 * @param table: pointer to a table
 * 
 * @returns
 * - `RH_TABLE_ALLOC_FAIL` - in case the allocation fails
 * 
 * - `RH_TABLE_SUCCESS` - in case the function succeeds
 * 
 * @see rh_table_ret
 */
static inline rh_table_ret __rh_table_realloc(rh_table_t *table);

/**
 * @brief pirvate function for finding a slot in a table
 * 
 * @par Funcion serves as a helper for finding a slot in the table for 
 * a given key (and hash). Funcion traverses one proble sequence lenght over the
 * table. 
 * Function returns a `rh_table_pos_t` struct that holds the following info:
 * - index into the table for a given key
 * - distance of the key from its ideal position
 * - result of the lookup
 * 
 * @param table: pointer to a table
 * @param hah: hash of the passed in key
 * @param key: key to look up in the table
 * 
 * @returns the result of the lookup in the rh_table_pos_t struct. possible values:
 * 
 * - `RH_TABLE_SLOT_FOUND_KEY` - found a slot with the matching key
 * 
 * - `RH_TABLE_SLOT_FOUND_EMPTY` - found an empty slot
 * 
 * - `RH_TABLE_SLOT_FOUND_STEAL` - found a slot where insert requries a robbin hood walk
 * 
 * @see rh_table_pos_t
 * @see rh_table_find_ret
 */
static inline rh_table_pos_t __rh_table_find_slot(const rh_table_t *table, uint32_t hash, const StringView *key);

/**
 * @brief private fucntion that does the robbin hood table wlak
 * 
 * @par Function walk over the table swaping entries and setting their distances back correctly
 * until it finds an empty slot.
 * 
 * @param table: pointer to a table
 * @param slot: where to start the walk
 */
static inline void __rh_table_steal_slot(rh_table_t *table, const rh_table_pos_t* slot);

/**
 * @brief private funcion that inserts an already created key value pair into the table
 * 
 * @par Function is used during resizing of the table to move data from the old table into the new table
 * Funcion only moves data around, it does not create any other allocations
 * 
 * @param table: pointer to a bigger table
 * @param entry: entry to insert into the new table
 */
static inline void __rh_table_insert_entry(rh_table_t *table, rh_table_entry_t *entry, const void *data);
/*
*************************************************************************
*                    PUBLIC TABLE FUNCTIONS                             *  
*************************************************************************
*/


rh_table_ret rh_table_init(rh_table_t *table, int32_t data_size, int32_t size) {
    assert(table != NULL);

    if(data_size <= 0 || size <= 0) {
        return RH_TABLE_INVALID_ARGS;
    }

    uint64_t alloc_size = data_size * (uint32_t) size;
    if (alloc_size > UINT32_MAX) {
        goto err; //overflow occured!
    }

    rh_table_entry_t *entries = (rh_table_entry_t *) calloc(size,sizeof(rh_table_entry_t));

    if (entries == NULL) {
        goto entry_alloc_err;
    }

    uint8_t *entry_data = (uint8_t *) malloc(alloc_size);

    if (entry_data == NULL) {
        goto entry_data_alloc_err;
    }

    table->entry_data_size = (uint32_t) data_size;
    table->entries = entries;
    table->entry_data = entry_data;
    table->capacity = (uint32_t) size;
    table->size = 0;
    return RH_TABLE_SUCCESS;

entry_data_alloc_err:
    free(entries);
entry_alloc_err:
err:
    return RH_TABLE_ALLOC_FAIL;
}


rh_table_ret rh_table_look_up(const rh_table_t *table,const StringView *key, void **data_out) {
    assert(table != NULL);
    assert(key != NULL);
    assert(data_out != NULL);

    const uint32_t hash = get_hash(key);
    const rh_table_pos_t slot = __rh_table_find_slot(table,hash,key);

    if (slot.result == RH_TABLE_SLOT_FOUND_KEY) {
        *data_out = table->entry_data + slot.pos*table->entry_data_size;
        return RH_TABLE_SUCCESS;
    }

    return RH_TABLE_KEY_NOT_FOUND;
}

rh_table_ret rh_table_insert(rh_table_t *table,const StringView *key, void **data_out) {
    assert(table != NULL);
    assert(key != NULL);
    assert(data_out != NULL);
    
    if (table->size == table->capacity) {
        return RH_TABLE_TABLE_FULL;
    }

    const uint32_t hash = get_hash(key);
    rh_table_pos_t slot = __rh_table_find_slot(table,hash,key);
    void *orig_data = *data_out;
    *data_out = table->entry_data + slot.pos*table->entry_data_size;
    
    if (slot.result == RH_TABLE_SLOT_FOUND_KEY) {
        return RH_TABLE_KEY_FOUND;
    }

    rh_table_entry_t new_entry = (rh_table_entry_t) {
        .distance = slot.distance,
        .hash = hash,
        .ocupied = true,
    };

    if (!sb_copy(&new_entry.key,key)) {
        return RH_TABLE_ALLOC_FAIL;
    }

    if (slot.result == RH_TABLE_SLOT_FOUND_STEAL) {
        __rh_table_steal_slot(table,&slot);    
    }

    table->entries[slot.pos] = new_entry;
    memcpy(table->entry_data + slot.pos*table->entry_data_size,orig_data,table->entry_data_size);
    table->size++;
    return RH_TABLE_SUCCESS;
}

rh_table_ret rh_table_delete(rh_table_t *table,const StringView *key) {
    assert(table != NULL);
    assert(key != NULL);

    if (table->size == 0) {
        return RH_TABLE_TABLE_EMPTY;
    }

    const uint32_t hash = get_hash(key);
    const rh_table_pos_t slot = __rh_table_find_slot(table,hash,key);

    if (slot.result != RH_TABLE_SLOT_FOUND_KEY) {
        return RH_TABLE_KEY_NOT_FOUND;
    }

    table->size--;
    __rh_table_shift_back(table,slot.pos);
    return RH_TABLE_SUCCESS;

}


rh_table_ret rh_table_delete_custom(rh_table_t *table, const StringView *key, const cleanup_func_t callback) {
    assert(table != NULL);
    
    if (table->size == 0) {
        return RH_TABLE_TABLE_EMPTY;
    }

    const uint32_t hash = get_hash(key);
    const rh_table_pos_t slot = __rh_table_find_slot(table,hash,key);

    if (slot.result != RH_TABLE_SLOT_FOUND_KEY) {
        return RH_TABLE_SUCCESS;
    }

    table->size--;
    callback(table->entry_data + slot.pos*table->entry_data_size);

    __rh_table_shift_back(table,slot.pos);
    return RH_TABLE_SUCCESS;
}



rh_table_ret rh_table_resize(rh_table_t *table) {
    assert(table != NULL);

    // save the original table arrays, the get over ridden in realoc
    rh_table_entry_t *old_entries = table->entries;
    uint8_t *old_entry_data = table->entry_data;

    rh_table_ret ret_val = __rh_table_realloc(table);

    if (ret_val != RH_TABLE_SUCCESS) {
        return ret_val;
    }
    
    for (uint32_t entry_index = 0; entry_index < table->size; entry_index++) {

        rh_table_entry_t *old_entry = old_entries + entry_index;
        void *old_data = old_entry_data + entry_index*table->entry_data_size;
        if(!old_entry->ocupied) {
            continue;
        }

        __rh_table_insert_entry(table,old_entry,old_data);
    }

    free(old_entries);
    free(old_entry_data);

    return RH_TABLE_SUCCESS;
}


void rh_table_clear(rh_table_t *table) {
    assert(table != NULL);
    
    for (uint32_t entry_idx = 0; entry_idx < table->size; entry_idx++) {
        rh_table_entry_t entry = table->entries[entry_idx];
        if (!entry.ocupied) {
            sb_free(&entry.key);
        }
    }

    table->size = 0;
}


void rh_table_clear_custom(rh_table_t *table, cleanup_func_t callback) {
    assert(table != NULL);
    
    for (uint32_t entry_idx = 0; entry_idx < table->size; entry_idx++) {
        rh_table_entry_t entry = table->entries[entry_idx];
        if (!entry.ocupied) {
            sb_free(&entry.key);
            callback(table->entry_data + entry_idx*table->entry_data_size);
        }
    }

    table->size = 0;
}

void rh_table_free(rh_table_t *table) {
    assert(table != NULL);

    if (table->entries == NULL || table->entry_data == NULL) {
        return;
    }

    rh_table_clear(table);

    free(table->entries);
    free(table->entry_data);

    table->entry_data = NULL;
    table->entries = NULL;
}


void rh_table_free_custom(rh_table_t *table, cleanup_func_t callback) {
    assert(table != NULL);

    if (table->entries == NULL || table->entry_data == NULL) {
        return;
    }

    rh_table_clear_custom(table,callback);

    free(table->entries);
    free(table->entry_data);

    table->entry_data = NULL;
    table->entries = NULL;
}

/*
*************************************************************************
*                   PUBLIC ITERATOR FUNCTIONS                           *  
*************************************************************************
*/

/**
 * @brief Macor to create table iterator struct
 */
#define ITERATOR(tableptr, index)   \
    (rh_table_iter_t) {             \
            .table = tableptr,      \
            .table_line = index,    \
    }

rh_table_iter_t rh_table_iter_init(const rh_table_t *table) {
    assert(table != NULL);

    // find the first ocuppied table entry
    for (uint32_t entry_idx = 0; entry_idx < table->size; entry_idx++) {
        const rh_table_entry_t entry = table->entries[entry_idx];

        if (!entry.ocupied) {
            continue;
        }

        return ITERATOR(table,entry_idx);
        
    }
    
    // return an empty ierator on empty table
    return ITERATOR(NULL,0);
}

rh_table_iter_ret rh_table_iter_next(rh_table_iter_t *iter) {
    assert(iter != NULL);

    const rh_table_t *table = iter->table;

    // end iteration on empty table
    if (table == NULL) {
        return RH_TABLE_ITER_END;
    }

    //find the next ocuppied enrty
    for (uint32_t entry_idx = iter->table_line; entry_idx < table->size; entry_idx++) {
        const rh_table_entry_t entry = table->entries[entry_idx];

        if (!entry.ocupied) {
            continue;
        }
        
        iter->table_line = entry_idx;
        return RH_TABLE_ITER_NEXT;
    }
    // end interation
    return RH_TABLE_ITER_END;
}

/*
*************************************************************************
*                       PRIVATE FUNCTIONS                               *  
*************************************************************************
*/

static inline void __rh_table_shift_back(rh_table_t *table, uint32_t deletion_index) {
    assert(table != NULL);

    uint32_t table_capacity = table->capacity;
    uint32_t current_idx = (deletion_index + 1) % table_capacity;
    uint32_t prev_idx = deletion_index;

    while (table->entries[current_idx].ocupied) 
    {

        if (table->entries[current_idx].distance == 0) {
            break;
        }

        table->entries[prev_idx] = table->entries[current_idx];
        table->entries[prev_idx].distance--;
        memcpy(
            table->entry_data + prev_idx * table->entry_data_size,
            table->entry_data + current_idx * table->entry_data_size,
            table->entry_data_size
        );

        prev_idx = current_idx;
        current_idx = (current_idx + 1) % table_capacity;
    }
    table->entries[prev_idx].ocupied = false;

}

/**
 * @brief Macro to creata a sarch result
 * 
 * @note Macro requires pos and distance varailbes to be defined and uint32_t
 */
#define SEARCH_RESULT(res)      \
    (rh_table_pos_t) {          \
        .pos = pos,             \
        .distance = distance,   \
        .result=res             \
    }

static inline rh_table_pos_t __rh_table_find_slot(const rh_table_t *table, uint32_t hash, const StringView *key) {
    uint32_t pos = hash % table->capacity;
    uint32_t distance = 0;

    while (true)
    {
        rh_table_entry_t *current_entry = table->entries + pos;

        if (!current_entry->ocupied) {
            return SEARCH_RESULT(RH_TABLE_SLOT_FOUND_EMPTY);
        }

        if (current_entry->hash == hash && sb_cmp(&current_entry->key,key)) {
            return SEARCH_RESULT(RH_TABLE_SLOT_FOUND_KEY);
        }
        if (current_entry->distance < distance) {
            return SEARCH_RESULT(RH_TABLE_SLOT_FOUND_STEAL);
        }

        pos = (pos + 1) % table->capacity;
        distance++;
    }
    
}

static inline rh_table_ret __rh_table_realloc(rh_table_t *table) {
    assert(table != NULL);

    const size_t old_capacity = table->capacity;  
    rh_table_ret ret_val = RH_TABLE_SUCCESS;

    // overflow number 1
    if (old_capacity > UINT32_MAX/2) {
        ret_val = RH_TABLE_ALLOC_FAIL;
        goto err_overflow;
    }

    //overflow number 2
    if (old_capacity * table->entry_data_size > INT64_MAX / 2) {
        ret_val  = RH_TABLE_ALLOC_FAIL;
        goto err_overflow;
    }

    size_t new_capacity = old_capacity * 2;

    rh_table_entry_t *new_entries = (rh_table_entry_t *) calloc(new_capacity,sizeof(rh_table_entry_t));
    if (new_entries == NULL) {
        ret_val =  RH_TABLE_ALLOC_FAIL;
        goto err_entries_alloc;
    }

    uint8_t *new_entry_data = (uint8_t *) malloc(new_capacity * table->entry_data_size);
    if (new_entry_data == NULL) {
        ret_val = RH_TABLE_ALLOC_FAIL;
        goto err_entry_data_alloc;
    }



    table->entry_data = new_entry_data;
    table->entries = new_entries;
    table->capacity = new_capacity;

    return ret_val;

err_entry_data_alloc:
    free(new_entries);
err_entries_alloc:
err_overflow:
    return ret_val;
}

static inline void __rh_table_steal_slot(rh_table_t *table, const rh_table_pos_t* slot) {
    assert(table != NULL);
    assert(slot != NULL);

    uint32_t prev_index = slot->pos;
    uint32_t current_index = (prev_index + 1) % table->capacity;
    uint32_t distance = slot->distance;
    
    
    while (true)
    {
        if (!table->entries[current_index].ocupied) {

            memcpy(
                table->entries + current_index,
                table->entries + prev_index,
                sizeof(rh_table_entry_t)
            );

            memcpy(
                table->entry_data + current_index*table->entry_data_size,
                table->entry_data + prev_index* table->entry_data_size,
                table->entry_data_size
            );
            table->entries[current_index].distance = distance;
        }

    
        if (table->entries[current_index].distance < distance) {
            __rh_table_swap(table,prev_index,current_index);
        }

        distance++;
        prev_index = current_index;
        current_index = (current_index + 1) % table->capacity;
    }
}

static inline void __rh_table_insert_entry(rh_table_t *table, rh_table_entry_t *entry, const void *data) {
    assert(table != NULL);
    assert(entry != NULL);
    assert(data != NULL);

    const StringView key = sb_get_view(&entry->key,0);
    const rh_table_pos_t slot = __rh_table_find_slot(table,entry->hash,&key);

    assert(slot.result != RH_TABLE_SLOT_FOUND_KEY);

    if (slot.result == RH_TABLE_SLOT_FOUND_STEAL) {
        __rh_table_steal_slot(table,&slot);
    }

    entry->distance = slot.distance;
    table->entries[slot.result] = *entry;
    memcpy(table->entry_data + slot.pos*table->entry_data_size,data,table->entry_data_size);
}

static inline void __rh_table_swap(rh_table_t *table, uint32_t idx1, uint32_t idx2) {
    assert(idx1 != idx2);

    rh_table_entry_t tmp_entry;
    rh_table_entry_t *entry_a = table->entries + idx1;
    rh_table_entry_t *entry_b = table->entries + idx2;

    tmp_entry = *entry_a;
    *entry_a = *entry_b;
    *entry_b = tmp_entry;

    uint8_t * data_a = table->entry_data + idx1*table->entry_data_size;
    uint8_t *data_b = table->entry_data + idx2*table->entry_data_size;

    memswap(data_a,data_b,table->entry_data_size);
}
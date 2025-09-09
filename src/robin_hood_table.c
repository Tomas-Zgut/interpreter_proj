#include<headders/robin_hood_table.h>
#include<headders/hashes.h>
#include<string.h>
#include<stddef.h>
#include<stdalign.h>
#include<stdbit.h>
typedef enum {
    RH_TABLE_SLOT_FOUND_EMPTY,
    RH_TABLE_SLOT_FOUND_KEY,
    RH_TABLE_SLOT_FOUND_STEAL
} rh_table_find_ret;

typedef struct {
  const uint32_t pos;
  const rh_table_find_ret result;
} rh_table_pos_t;

#define CACHE_LINE_SIZE 64
#define ALIGN_ROUND_UP(value,align) ((((value) + ((align)-1)) / align) * align)

#define OCCUPIED_MASK 0x8000

#define SLOT_EMPTY(slot) (((slot) & OCCUPIED_MASK) == 0)

#define SET_SLOT_EMPTY(slot) slot &= ~OCCUPIED_MASK;

#define HASH_MASK 0x7FFF
#define MASKED_HASH(x) ((x >> 49) & HASH_MASK)

#define IS_POWER_OF_2(value) (((value) & ((value)-1)) == 0)
/*
*************************************************************************
*                  DECLERATIONS OF PRIVATE FUNCTIONS                    *  
*************************************************************************
*/

/**
 * @brief private function for allocating a table
 * 
 * @par Function alocates data for a table with a given size and
 * initialize the table entries. This function only uses 1 allocation.
 * 
 * @param table: pointer to a table
 * @param size: size of table to alloc
 * @param dat_size: size of the user data in the table
 * 
 * @returns
 * - `RH_TABLE_ALLOC_FAIL` - in case the allocation fails
 * 
 * - `RH_TABLE_SUCCESS` - in case the function succeeds
 * 
 * @see rh_table_ret
 */
static inline rh_table_ret __rh_table_alloc(rh_table_t *table, size_t size, size_t data_size);

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
static inline rh_table_pos_t __rh_table_find_slot(const rh_table_t *table, uint64_t hash, const StringView *key);

/**
 * @brief private funcion that inserts an already created key value pair into the table
 * 
 * @par Function is used during resizing of the table to move data from the old table into the new table
 * Funcion only moves data around, it does not create any other allocations
 * 
 * @param table: pointer to a bigger table
 * @param key: key to insert
 * @param data: data to insert
 * 
 */
static inline void __rh_table_rehash_insert(rh_table_t *table, const StringView *key, const void *data);

static inline void __rh_table_shift_back(rh_table_t *table, uint32_t position);
/*
*************************************************************************
*                    PUBLIC TABLE FUNCTIONS                             *  
*************************************************************************
*/


rh_table_ret rh_table_init_deleter(rh_table_t *table, uint32_t data_size, uint32_t size,cleanup_func_t deleter) {
    assert(table != NULL);

    if(!IS_POWER_OF_2(size)) {
        return RH_TABLE_INVALID_ARGS;
    }

    if (__rh_table_alloc(table,size,data_size)!= RH_TABLE_SUCCESS) {
        return RH_TABLE_ALLOC_FAIL;
    }

    table->entry_data_size = data_size;
    table->capacity = size;
    table->cleanup = deleter;
    table->size = 0;
    return RH_TABLE_SUCCESS;
}


rh_table_ret rh_table_look_up(const rh_table_t *table,const StringView *key, void **data_out) {
    assert(table != NULL);
    assert(key != NULL);
    assert(data_out != NULL);
    assert(rh_table_initialized(table));

    const uint64_t hash = get_hash(key);
    const rh_table_pos_t slot = __rh_table_find_slot(table,hash,key);

    if (slot.result == RH_TABLE_SLOT_FOUND_KEY) {
        *data_out = table->entry_data + table->entries[slot.pos].data_index*table->entry_data_size;
        return RH_TABLE_SUCCESS;
    }

    return RH_TABLE_KEY_NOT_FOUND;
}

rh_table_ret rh_table_insert(rh_table_t *table,const StringView *key, void **data_out) {
    assert(table != NULL);
    assert(key != NULL);
    assert(data_out != NULL);
    assert(rh_table_initialized(table));

    if (table->size + 1 >= table->capacity * (15.0f/16) ) {
        return RH_TABLE_TABLE_FULL;
    }
    
    const uint64_t hash = get_hash(key);
    const uint32_t mask = table->capacity - 1;
    uint32_t pos = hash & mask;
    uint16_t distance = 0;
    bool is_orig_entry = true;
    uint32_t orig_entry_pos;
    uint32_t kicked_out_idx;

    rh_table_entry_t new_entry = (rh_table_entry_t) {
        .distance = 0,
        .finger_print = MASKED_HASH(hash) | OCCUPIED_MASK,
        .data_index = 0,
    };

    while (true)
    {
        rh_table_entry_t *current_entry_ptr = table->entries + pos;
        rh_table_entry_t current_entry = *current_entry_ptr;

        if (SLOT_EMPTY(current_entry.finger_print)) {
            new_entry.distance = distance;
            kicked_out_idx = current_entry.data_index;
            *current_entry_ptr = new_entry;
            orig_entry_pos = is_orig_entry ? pos : orig_entry_pos;
            break;
        }

        if((current_entry.finger_print & HASH_MASK) == MASKED_HASH(hash) && 
            sb_cmp(table->entry_keys + current_entry.data_index,key) == 0) {
                *data_out = table->entry_data + current_entry.data_index*table->entry_data_size;
            return RH_TABLE_KEY_FOUND;
        }

        if (current_entry.distance < distance) {
            new_entry.distance = distance;
            //entry swap
            rh_table_entry_t temp = current_entry;
            *current_entry_ptr = new_entry;
            new_entry = temp;

            distance = new_entry.distance + 1;

            orig_entry_pos = is_orig_entry ? pos : orig_entry_pos;
            is_orig_entry = is_orig_entry ? false: is_orig_entry;
        } else {
            distance++;
        }

        pos = (pos + 1) & mask;
    }
    
    
    //insert
    table->entries[orig_entry_pos].data_index = kicked_out_idx;
    if (!sb_copy(table->entry_keys + kicked_out_idx,key)) {
        return RH_TABLE_ALLOC_FAIL;
    }
    
    memcpy(table->entry_data + kicked_out_idx*table->entry_data_size,*data_out,table->entry_data_size);
    *data_out = table->entry_data + kicked_out_idx*table->entry_data_size;
    table->size++;

    return RH_TABLE_SUCCESS;
}

rh_table_ret rh_table_delete(rh_table_t *table,const StringView *key) {
    assert(table != NULL);
    assert(key != NULL);
    assert(rh_table_initialized(table));

    if (table->size == 0) {
        return RH_TABLE_TABLE_EMPTY;
    }

    const uint64_t hash = get_hash(key);
    const rh_table_pos_t slot = __rh_table_find_slot(table,hash,key);

    if (slot.result != RH_TABLE_SLOT_FOUND_KEY) {
        return RH_TABLE_KEY_NOT_FOUND;
    }

    table->size--;
    const uint32_t data_idx = table->entries[slot.pos].data_index;
    sb_free(table->entry_keys + data_idx);
    if (table->cleanup) {
        table->cleanup(table->entry_data + data_idx*table->entry_data_size);
    }
    __rh_table_shift_back(table,slot.pos);
    return RH_TABLE_SUCCESS;
}

rh_table_ret rh_table_resize(rh_table_t *table) {
    assert(table != NULL);
    assert(rh_table_initialized(table));

    // save the original table arrays, they get overridden in realoc
    rh_table_entry_t *old_entries = table->entries;
    uint8_t *old_entry_data = table->entry_data;
    String *old_keys = table->entry_keys;
    const uint32_t old_capacity = table->capacity;

    const rh_table_ret ret_val = __rh_table_alloc(table,old_capacity * 2,table->entry_data_size);

    if (ret_val != RH_TABLE_SUCCESS) {
        return ret_val;
    }

    
    for (uint32_t entry_index = 0; entry_index < old_capacity; entry_index++) {

        const rh_table_entry_t *old_entry = old_entries + entry_index;
        const uint32_t data_index = old_entry->data_index;
        if(SLOT_EMPTY(old_entry->finger_print)) {
            continue;
        }
        const StringView key = sb_get_view(old_keys + data_index,0);
        const void *data = old_entry_data + data_index*table->entry_data_size;
        __rh_table_rehash_insert(table,&key,data);

    }     


    // free the old table arrays
    free(old_entries);
    return RH_TABLE_SUCCESS;
}


void rh_table_clear(rh_table_t *table) {
    assert(table != NULL);
    assert(rh_table_initialized(table));

    for (uint32_t entry_idx = 0; entry_idx < table->capacity; entry_idx++) {
        rh_table_entry_t *entry = table->entries + entry_idx;
        const uint32_t pos = entry->data_index;
        if (SLOT_EMPTY(entry->finger_print)) {
            continue;
        }
        SET_SLOT_EMPTY(entry->finger_print);
        sb_free(table->entry_keys + pos);
        if (table->cleanup) {
            table->cleanup(table->entry_data + pos*table->entry_data_size);
        }
    }

    table->size = 0;
}

void rh_table_free(rh_table_t *table) {
    assert(table != NULL);

    if (!rh_table_initialized(table)) {
        return;
    }

    rh_table_clear(table);

    free(table->entries);

    table->entry_data = NULL;
    table->entries = NULL;
    table->entry_keys = NULL;
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
    assert(rh_table_initialized(table));

    // find the first ocuppied table entry
    for (uint32_t entry_idx = 0; entry_idx < table->capacity; entry_idx++) {
        const rh_table_entry_t entry = table->entries[entry_idx];

        if (SLOT_EMPTY(entry.finger_print)) {
            continue;
        }

        return ITERATOR(table,entry_idx);
        
    }
    
    // return an empty ierator on empty table
    return ITERATOR(table,table->capacity);
}

rh_table_iter_ret rh_table_iter_next(rh_table_iter_t *iter) {
    assert(iter != NULL);

    const rh_table_t *table = iter->table;

    //find the next ocuppied enrty
    for (uint64_t entry_idx = iter->table_line + 1; entry_idx < table->capacity; entry_idx++) {
        const rh_table_entry_t entry = table->entries[entry_idx];

        if (SLOT_EMPTY(entry.finger_print)) {
            continue;
        }
        
        iter->table_line = (uint32_t)entry_idx;
        return RH_TABLE_ITER_NEXT;
    }
    // end interation
    iter->table_line = table->capacity;
    return RH_TABLE_ITER_END;
}

/*
*************************************************************************
*                       PRIVATE FUNCTIONS                               *  
*************************************************************************
*/

/**
 * @brief Macro to creata a sarch result
 * 
 * @note Macro requires pos and distance varailbes to be defined and uint32_t
 */
#define SEARCH_RESULT(res)      \
    (rh_table_pos_t) {          \
        .pos = pos,             \
        .result=res             \
    }

static inline rh_table_pos_t __rh_table_find_slot(const rh_table_t *table, uint64_t hash, const StringView *key) {
    assert(table != NULL);
    assert(key != NULL);
    assert(rh_table_initialized(table));

    const uint32_t wrap_mask = table->capacity - 1;
    uint32_t pos = hash & wrap_mask;
    uint16_t distance = 0;

    while (true)
    {   
        rh_table_entry_t *current_entry_ptr = table->entries + pos;
        rh_table_entry_t current_entry = *current_entry_ptr;
        if (SLOT_EMPTY(current_entry.finger_print)) {
            return SEARCH_RESULT(RH_TABLE_SLOT_FOUND_EMPTY);
        }

        if ((current_entry.finger_print & HASH_MASK) == MASKED_HASH(hash) &&
            sb_cmp(table->entry_keys + current_entry.data_index,key) == 0) {
            return SEARCH_RESULT(RH_TABLE_SLOT_FOUND_KEY);
        }

        if (current_entry.distance < distance) {
            return SEARCH_RESULT(RH_TABLE_SLOT_FOUND_STEAL);
        }

        pos = (pos + 1) & wrap_mask;
        distance++;
    }
}

static inline rh_table_ret __rh_table_alloc(rh_table_t *table, size_t size, size_t data_size) {
    assert(table != NULL);

    const size_t entry_alloc_size = size *sizeof(rh_table_entry_t); // already 8 byte aligned
    const size_t entry_key_alloc_size = ALIGN_ROUND_UP(size * sizeof(String),alignof(max_align_t));
    const size_t entry_data_alloc_size = size * data_size; //aligment unknown
    const size_t table_alloc_size = ALIGN_ROUND_UP(entry_alloc_size + entry_key_alloc_size+entry_data_alloc_size,CACHE_LINE_SIZE);

    uint8_t *alloced_mem = aligned_alloc(CACHE_LINE_SIZE,table_alloc_size);
    if (alloced_mem == NULL) {
        return RH_TABLE_ALLOC_FAIL;
    }
    const uintptr_t alloced_address = (uintptr_t) alloced_mem;
    const uintptr_t entry_address = alloced_address;
    rh_table_entry_t *entries = (rh_table_entry_t *) entry_address;
    const uintptr_t key_address = entry_address + entry_alloc_size;
    String * entry_keys = (String *)key_address;
    const uintptr_t data_address = key_address + entry_key_alloc_size;
    uint8_t *entry_data = (uint8_t *)data_address;

    // initialize the  table entries
    for (uint32_t entry_idx = 0; entry_idx < size; entry_idx++) {
        entries[entry_idx].data_index = entry_idx;
        entries[entry_idx].finger_print = 0;
    }

    table->entry_data = entry_data;
    table->entries = entries;
    table->entry_keys = entry_keys;
    table->capacity = size;
    return RH_TABLE_SUCCESS;
}

static inline void __rh_table_rehash_insert(rh_table_t *table, const StringView *key, const void *data) {
    assert(table != NULL);
    assert(key != NULL);
    assert(data != NULL);
    assert(rh_table_initialized(table));

    const uint64_t hash = get_hash(key);
    const uint32_t mask = table->capacity - 1;

    uint32_t pos = hash & mask;
    uint16_t distance = 0;
    bool is_orig_entry = true;
    uint32_t orig_entry_pos;
    uint32_t kicked_out_idx;

    rh_table_entry_t new_entry = (rh_table_entry_t) {
        .distance = 0,
        .finger_print = MASKED_HASH(hash) | OCCUPIED_MASK,
        .data_index = 0,
    };
    
    while (true)
    {
        rh_table_entry_t *current_entry_ptr = table->entries + pos;
        rh_table_entry_t current_entry = *current_entry_ptr;
        if (SLOT_EMPTY(current_entry.finger_print)) {
            new_entry.distance = distance;
            kicked_out_idx = current_entry.data_index;
            *current_entry_ptr = new_entry;
            orig_entry_pos = is_orig_entry ? pos : orig_entry_pos;
            break;
        }

        if (current_entry.distance < distance) {
            new_entry.distance = distance;
            rh_table_entry_t temp = current_entry;
            *current_entry_ptr = new_entry;
            new_entry = temp;

            distance = new_entry.distance + 1;

            orig_entry_pos = is_orig_entry ? pos : orig_entry_pos;
            is_orig_entry = is_orig_entry ? false: is_orig_entry;
        } else {
            distance++;
        }

        pos = (pos + 1) & mask;
    }

    //insert
    table->entries[orig_entry_pos].data_index = kicked_out_idx;
    memcpy(table->entry_keys + kicked_out_idx,key,sizeof(String));
    memcpy(table->entry_data + kicked_out_idx*table->entry_data_size,data,table->entry_data_size);
}

static inline void __rh_table_shift_back(rh_table_t *table,uint32_t stating_position) {
    assert(table != NULL);
    assert(rh_table_initialized(table));

    const uint32_t mask = table->capacity - 1;
    uint32_t position = (stating_position + 1) & mask;
    uint32_t prev_position = stating_position;
    rh_table_entry_t displaced_entry = table->entries[stating_position];
    SET_SLOT_EMPTY(displaced_entry.finger_print);

    while (true)
    {
        rh_table_entry_t *current_entry = table->entries + position;
        rh_table_entry_t *prev_entry = table->entries + prev_position;

        if (SLOT_EMPTY(current_entry->finger_print)) {
            *prev_entry = displaced_entry;
            return;
        }

        if (current_entry->distance == 0) {
            *prev_entry = displaced_entry;
            return;
        }

        if (current_entry->distance > 0) {
            current_entry->distance--;
            *prev_entry = *current_entry;
        }
        prev_position = position;
        position = (position + 1) & mask;
    }
}

#ifndef __MEMORY_H__
#define __MEMORY_H__
#include "stacks.h"
#include "string_buffer.h"
#include "memory_frame.h"
#include "memory_value.h"


DEFINE_STACK(uint32_t,call)
DEFINE_STACK_PTR_CLEANUP(memory_frame_t,frame,memory_frame_free)
DEFINE_STACK_PTR_CLEANUP(memory_value_t,value,memory_value_free)

/**
 * @brief structure representing the interpreter's memory
 */
typedef struct 
{
    call_stack_t call_stack;        //interpreter's call stack
    frame_stack_t frame_stack;      // interpreter's stack of local frames
    value_stack_t value_stack;      // interpreter's stack data stack
    memory_frame_t global_frame;    // interpreter's global memory frame
    memory_frame_t temp_frame;      // interpreter's temporary frame
} Memory;

/**
 * @brief function to initialize the interpreter's memory
 * 
 * @param[out] memory: pointer to interpreters memory
 * 
 * @returns true if the initialization succeeds, false otherwise
 */
bool memory_init(Memory *memory);

/**
 * @brief function to push a value to the interpreter's data stack
 * 
 * @param memory: pointer to interpreter's memory
 * @param value: pointer to the value to push
 * 
 * @returns true if the operation was successful, false otherwise
 */
bool memory_value_stack_push(Memory *memory,memory_value_t *value);

/**
 * @brief function to push an address to the interpreter's call stack
 * 
 * @param memory: pointer to interpreter's memory
 * @param return_address: address to return to after a call
 * 
 * @returns true if the operation was successful, false otherwise
 */
bool memory_call_stack_push(Memory *memory, uint32_t return_address);

/**
 * @brief function to pop an address from the call_stack
 * 
 * @param memory: pointer to interpreter's memory
 * 
 * @returns a pointer to the top address on success, NULL otherwise
 */
uint32_t *memory_call_stack_pop(Memory *memory);

/**
 * @brief fucntion to get a const reference to a variable from the temporary frame
 * 
 * @par Function returns a NULL pointer it the variable is not in the temp frame 
 *  
 * @param memory: pointer to the interpreter's memory
 * @param variable_name: variable to look up
 * 
 * @returns const pointer to data in memory on success, false otherwise
 * 
 * @warning there is an assertion if the temp frame is not valid @see memry_is_temp_frame_valid
 */
const memory_value_t *memory_temp_frame_get(const Memory *memory,const StringView *variable_name);
/**
 * @brief fucntion to get a const reference to a variable from the global frame
 * 
 * @par Function returns a NULL pointer it the variable is not in the global memory frame 
 * 
 * @param memory: pointer to the interpreter's memory
 * @param variable_name: variable to look up
 * 
 * @returns const pointer to data in memory on success, false otherwise
 */
const memory_value_t *memory_global_frame_get(const Memory *memory,const StringView *variable_name);

/**
 * @brief fucntion to get a const reference to a variable from the top local frame
 * 
 * @par Function returns a NULL pointer it the variable is not in the top local frame 
 *  
 * @param memory: pointer to the interpreter's memory
 * @param variable_name: variable to look up
 * 
 * @returns const pointer to data in memory on success, false otherwise
 * 
 * @warning there is an assertion if the top local frame is not valid @see memry_is_local_frame_valid
 */
const memory_value_t *memory_local_frame_get(const Memory *memory, const StringView *variable_name);

/**
 * @brief fucntion to get a mutable reference to a variable from the temporary frame
 * 
 * @par Function returns a NULL pointer it the variable is not in the temp frame 
 *  
 * @param memory: pointer to the interpreter's memory
 * @param variable_name: variable to look up
 * 
 * @returns pointer to data in memory on success, false otherwise
 * 
 * @warning there is an assertion if the temp frame is not valid @see memry_is_temp_frame_valid
 */
memory_value_t *memory_temp_frame_get_mut(const Memory *memory, const StringView *variable_name);

/**
 * @brief fucntion to get a mutable reference to a variable from the global frame
 * 
 * @par Function returns a NULL pointer it the variable is not in the global frame 
 *  
 * @param memory: pointer to the interpreter's memory
 * @param variable_name: variable to look up
 * 
 * @returns pointer to data in memory on success, false otherwise
 */
memory_value_t *memory_global_frame_get_mut(const Memory *memory, const StringView *variable_name);

/**
 * @brief fucntion to get a mutable reference to a variable from the top local frame
 * 
 * @par Function returns a NULL pointer it the variable is not in the top local frame 
 *  
 * @param memory: pointer to the interpreter's memory
 * @param variable_name: variable to look up
 * 
 * @returns const pointer to data in memory on success, false otherwise
 * 
 * @warning there is an assertion if the top local frame is not valid @see memry_is_local_frame_valid
 */
memory_value_t *memory_local_frame_get_mut(const Memory *memory, const StringView *variable_name);

/**
 * @brief function to check if the temporary frame is valid
 * 
 * @param memory: pointer to the interpreter's memory
 * 
 * @returns true if the temp frame is valid, fasle otherwise
 */
bool memory_temp_frame_valid(const Memory *memory);

/**
 * @brief function to check if the top local frame is valid
 * 
 * @param memory: pointer to the interpreter's memory
 * 
 * @returns true if the top local frame is valid, fasle otherwise
 */
bool memory_local_frame_valid(const Memory *memory);

/**
 * @brief function to add a new varaible into the temp memory frame
 * 
 * @par Fucntion requires that the temp frame is valid, if it is a vairable insert is attempted.
 * It can fail under 2 conditions: 
 * - if the variable already exists
 * - if there is no more memory for the insert
 * 
 * @param memory: pointer to the interpreter's memory
 * @param variable_name: new vairable to insert
 * @param value: value of the new variable to insert (maybe unused)
 * 
 * @returns true on success, false otherwise
 * 
 * @warning There is an assertion in case the temp frame is not valid @see memory_temp_frame_valid
 */
bool memory_temp_frame_insert(Memory *memory, const StringView *variable_name,const memory_value_t *value);

/**
 * @brief function to add a new varaible into the global memory frame
 * 
 * @par Fucntion attempts to  insert a variable, there are  2 failure conditions: 
 * - if the variable already exists
 * - if there is no more memory for the insert
 * 
 * @param memory: pointer to the interpreter's memory
 * @param variable_name: new vairable to insert
 * @param value: value of the new variable to insert (maybe unused)
 * 
 * @returns true on success, false otherwise
 */
bool memory_global_frame_insert(Memory *memory,const StringView *variable_name,const memory_value_t *value);

/**
 * @brief function to add a new varaible into the top local memory frame
 * 
 * @par Fucntion requires that the temp frame is valid, if it is a vairable insert is attempted.
 * It can fail under 2 conditions: 
 * - if the variable already exists
 * - if there is no more memory for the insert
 * 
 * @param memory: pointer to the interpreter's memory
 * @param variable_name: new vairable to insert
 * @param value: value of the new variable to insert (maybe unused)
 * 
 * @returns true on success, false otherwise
 * 
 * @warning There is an assertion in case the temp frame is not valid @see memory_local_frame_valid
 */
bool memory_local_frame_insert(Memory *memory, const StringView *variable_name,const memory_value_t *value);

/**
 * @brief function creates a temp frame (the old one is deicarded)
 * 
 * @param memory: pointer to interpreter's memory
 * 
 * @returns true on success, false otherwise
 */
bool memory_temp_frame_create(Memory *memory);

/**
 * @brief fuction adds the temporary frame to the top of the local frame stack and invalidates the temporary frame
 * 
 * @param memory: pointer to the interpreter's memory
 * 
 * @returns true on success, false otherwise
 * 
 * @warning if the temporay frame is not valid there is an assertion @see memory_local_frame_valid
 */
bool memory_local_frame_create(Memory *memory);
/**
 * @brief fuction moves the top local frame into the temporary frame (old temporary frame is discarded)
 * 
 * @param memory: pointer to the interpretr's memory
 * 
 * @warning if there are no local frames available there is an assertion @see memory_local_frame_valid
 */
void memory_local_frame_drop(Memory *memory);

/**
 * @brief function to free the interpretr's memory
 * 
 * @param memory: pointer to the interpreter's memory
 */
void memory_free(Memory *memory);
#endif
#include<headders/memory.h>


#define STACK_CAPACITY 16
bool memory_init(Memory *memory) {
    
    if (!call_stack_init(&memory->call_stack,STACK_CAPACITY)) {
        goto call_stack_init_err;
    }

    if (!frame_stack_init(&memory->frame_stack,STACK_CAPACITY)) {
        goto frame_stack_init_err;
    }

    if (!value_stack_init(&memory->value_stack,STACK_CAPACITY)) {
        goto value_stack_init_err;
    }

    if (!memory_frame_init(&memory->global_frame)) {
        goto global_frame_init_err;
    }

    memset(&memory->temp_frame,0,sizeof(memory_frame_t));
    return true;

global_frame_init_err:
    value_stack_free(&memory->value_stack);
value_stack_init_err:    
    frame_stack_free(&memory->frame_stack);
frame_stack_init_err:
    call_stack_free(&memory->call_stack);    
call_stack_init_err:
    return false;
}

bool memory_value_stack_push(Memory *memory, memory_value_t *value) {
    assert(memory != NULL);
    assert(value != NULL);

    return value_stack_push(&memory->value_stack,value);
}

bool memory_call_stack_push(Memory *memory, uint32_t return_address) {
    assert(memory != NULL);
    return call_stack_push(&memory->call_stack,return_address);
}

uint32_t *memory_call_stack_pop(Memory *memory) {
    assert(memory != NULL);
    return call_stack_pop(&memory->call_stack);
}

const memory_value_t *memory_temp_frame_get(const Memory *memory,const StringView *varaible_name) {
    assert(memory != NULL);
    assert(varaible_name != NULL);
    assert(memory_temp_frame_valid(memory));

    return memory_frame_get_val(&memory->temp_frame,varaible_name);
}

const memory_value_t *memory_global_frame_get(const Memory *memory,const StringView *varaible_name) {
    assert(memory != NULL);
    assert(varaible_name != NULL);

    return memory_frame_get_val(&memory->global_frame,varaible_name);
}

const memory_value_t *memory_local_frame_get(const Memory *memory,const StringView *varaible_name) {
    assert(memory != NULL);
    assert(varaible_name != NULL);
    assert(memory_local_frame_valid(memory));

    const memory_frame_t *local_frame = frame_stack_top(&memory->frame_stack);

    return memory_frame_get_val(local_frame,varaible_name);
}

memory_value_t *memory_temp_frame_get_mut(const Memory *memory,const StringView *varaible_name) {
    assert(memory != NULL);
    assert(varaible_name != NULL);
    assert(memory_temp_frame_valid(memory));

    return memory_frame_get_val_mut(&memory->temp_frame,varaible_name);
}


memory_value_t *memory_global_frame_get_mut(const Memory *memory,const StringView *varaible_name) {
    assert(memory != NULL);
    assert(varaible_name != NULL);

    return memory_frame_get_val_mut(&memory->global_frame,varaible_name);
}

memory_value_t *memory_local_frame_get_mut(const Memory *memory,const StringView *varaible_name) {
    assert(memory != NULL);
    assert(varaible_name != NULL);
    assert(memory_local_frame_valid(memory));

    const memory_frame_t *local_frame = frame_stack_top(&memory->frame_stack);

    return memory_frame_get_val_mut(local_frame,varaible_name);
}

bool memory_temp_frame_valid(const Memory *memory) {
    assert(memory != NULL);
    
    return memory_frame_initialized(&memory->temp_frame);
}

bool memory_local_frame_valid(const Memory *memory) {
    assert(memory != NULL);

    return frame_stack_top(&memory->frame_stack) != NULL;
}

bool memory_temp_frame_insert(Memory *memory,const StringView *varaible_name,const memory_value_t *value) {
    assert(memory != NULL);
    assert(memory_temp_frame_valid(memory));

    return memory_frame_insert_new_val(&memory->temp_frame,varaible_name,value);
}

bool memory_global_frame_insert(Memory *memory, const StringView *variable_name, const memory_value_t *value) {
    assert(memory != NULL);
    return memory_frame_insert_new_val(&memory->global_frame,variable_name,value);    
}

bool memory_local_frame_insert(Memory *memory, const StringView *variable_name, const memory_value_t *value) {
    assert(memory != NULL);
    assert(memory_local_frame_valid(memory));

    memory_frame_t *local_frame = frame_stack_top_mut(&memory->frame_stack);
    return memory_frame_insert_new_val(local_frame,variable_name,value);
}

bool memory_temp_frame_create(Memory *memory) {
    assert(memory != NULL);
    memory_frame_free(&memory->temp_frame);
    return memory_frame_init(&memory->temp_frame);
}

bool memory_local_frame_create(Memory *memory) {
    assert(memory != NULL);
    assert(memory_temp_frame_valid(memory));

    memory_frame_t new_local_frame = memory_frame_move(&memory->temp_frame);
    return frame_stack_push(&memory->frame_stack,&new_local_frame);
}

void memory_local_frame_drop(Memory *memory) {
    assert(memory != NULL);
    assert(memory_local_frame_valid(memory));
    memory_frame_free(&memory->temp_frame);
    
    memory_frame_t *old_local_frame = frame_stack_pop(&memory->frame_stack);
    memory->temp_frame = memory_frame_move(old_local_frame);
}



void memory_free(Memory *memory) {
    assert(memory != NULL);

    memory_frame_free(&memory->temp_frame);
    memory_frame_free(&memory->global_frame);
    value_stack_free(&memory->value_stack);
    frame_stack_free(&memory->frame_stack);
    call_stack_free(&memory->call_stack);   
}
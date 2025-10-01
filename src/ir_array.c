#include<headders/ir_array.h>

/*
*****************************************************************
|				Private function declarations					|
*****************************************************************
*/

/**
 * @brief private helper function to facilitate array's allocations
 * 
 * @par Function uses realloc to allocate a new elements array, or
 * grow the array up to the specified size. If the allocation fails,
 * the orignal data is not lost, and will need to be freed.
 * 
 * @param[out] array: pointer to an array
 * @param[in] size: new size of the array
 * 
 * @returns true if the allocation was successful, false otherwise
 */
bool __ir_array_alloc(ir_array *array, uint64_t size);

void __elm_array_clear(ir_elm_t *elemets,uint64_t size);

/*
*****************************************************************
|					Public ir_array functions					|
*****************************************************************
*/


bool ir_array_init(ir_array *array)
{
	assert(array != NULL);
	array->elements = NULL;
	if (!__ir_array_alloc(array, IR_ARRAY_INITIAL_SIZE))
	{
		return false;
	}
	array->size = 0;
	return true;
}

bool ir_array_add_elm(ir_array *array,opcode_type op_code, token_t **arg_array)
{
	assert(array != NULL);

	if (array->size == UINT64_MAX)
	{
		return false;
	}
	if (array->size == array->capacity)
	{
		if (!__ir_array_alloc(array, array->capacity * 2))
		{
			return false;
		}
	}
	
	const int operand_count = get_number_opperands(op_code);
	ir_elm_t new_ir_elm;
	new_ir_elm.opcode = op_code;
	for (int i = 0; i < operand_count; i++) {
		token_move(arg_array[i],new_ir_elm.operands + i);
	}
	array->elements[array->size++] = new_ir_elm;
	return true;
}

uint64_t ir_array_size(const ir_array *array) {
	assert(array != NULL);
	return array->size;
}

void ir_array_free(ir_array *array) {
	assert(array !=NULL);

    if (array->elements == NULL) {
        return;
    }
	__elm_array_clear(array->elements,array->size);
    free(array->elements);
	
    array->elements = NULL;
    array->size = 0;
    array->capacity = 0;
}

/*
*****************************************************************
|					Private  functions							|
*****************************************************************
*/

bool __ir_array_alloc(ir_array *array, uint64_t size)
{
	assert(array != NULL);
	
	ir_elm_t *new_elems = (ir_elm_t *)realloc(array->elements, size*sizeof(ir_elm_t));
	if (new_elems == NULL)
	{
		return false;
	}
	
	array->elements = new_elems;
	array->capacity = size;
	return true;
}

void __elm_array_clear(ir_elm_t *elements, uint64_t size) {
	assert(elements != NULL);

	for (uint64_t elm_idx = 0; elm_idx < size; elm_idx++) {
		ir_elm_t elm = elements[elm_idx];
		const int op_num = get_number_opperands(elm.opcode);
		for (int op_idx = 0; op_idx < op_num; op_idx++) {
			token_free(elm.operands);
		}
	}
}

/*
*****************************************************************
|					Public ir functions							|
*****************************************************************
*/

const ir_elm_t *ir_index_elm(const ir *array, uint64_t index)
{
	assert(array != NULL);
	assert(index < array->size);

	return array->elements + index;
}

void ir_free(ir *ir) {
    assert(ir != NULL);

    if (ir->elements == NULL) {
        return;
    }

	__elm_array_clear(ir->elements,ir->size);

    free(ir->elements);
	ir->elements = NULL;
}
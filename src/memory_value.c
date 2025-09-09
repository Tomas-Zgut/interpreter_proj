#include<headders/memory_value.h>
#include<assert.h>

#define MAKE_VIEW(lit)          \
    (StringView) {              \
        .data = lit,            \
        .length = sizeof(lit)-1 \
    }

const StringView type_strings[] = {
        MAKE_VIEW("bool"),
        MAKE_VIEW("float"),
        MAKE_VIEW("int"),
        MAKE_VIEW("string"),
        MAKE_VIEW("nill"),
        MAKE_VIEW("undefined")
    };

#define MAKE_TYPE_STRING(idx)   \
    (type_string) {             \
        .type_str_idx = idx     \
    }    

typedef enum {
    BOOL_IDX,
    DOUBLE_IDX,
    INT_IDX,
    STRING_IDX,
    NILL_IDX,
    UNDEFINED_IDX,
} type_string_idxs;


StringView memory_value_get_type_view(const memory_value_t *value) {
    assert(value != NULL);

    switch (value->type)
    {
    case VALUE_BOOL:
        return type_strings[BOOL_IDX];
    case VALUE_DOUBLE:
        return type_strings[DOUBLE_IDX];
    case VALUE_INT:
        return type_strings[INT_IDX];
    case __VALUE_STRING_MUT:
    case __VALUE_STRING_LIT:
    case VALUE_TYPE_STRING:
        return type_strings[STRING_IDX];
    case VALUE_NILL:
        return type_strings[NILL_IDX];
    case VALUE_UNDEFINED:
        return type_strings[UNDEFINED_IDX];
    default:
        assert(false); //should never happen
        return type_strings[UNDEFINED_IDX];
    }
}

type_string memory_value_get_type_string(const memory_value_t *value) {

    switch (value->type)
    {
        case VALUE_BOOL:
            return MAKE_TYPE_STRING(BOOL_IDX);
        case VALUE_DOUBLE:
            return MAKE_TYPE_STRING(DOUBLE_IDX);
        case VALUE_INT:
            return MAKE_TYPE_STRING(INT_IDX);
        case __VALUE_STRING_MUT:
        case __VALUE_STRING_LIT:
        case VALUE_TYPE_STRING:
            return MAKE_TYPE_STRING(STRING_IDX);
        case VALUE_NILL:
            return MAKE_TYPE_STRING(NILL_IDX);
        case VALUE_UNDEFINED:
            return MAKE_TYPE_STRING(UNDEFINED_IDX);
        default:
            assert(false); //should never happen
            return MAKE_TYPE_STRING(UNDEFINED_IDX);
    }
}


void memory_value_free(memory_value_t *value) {
    assert(value != NULL);

    static const undefined_t undef = {0};
    switch (value->type)
    {
    case VALUE_BOOL:
    case VALUE_DOUBLE:
    case VALUE_INT:
    case VALUE_NILL:
    case VALUE_TYPE_STRING:
    case VALUE_UNDEFINED:
        return; //mothing to free
    case __VALUE_STRING_MUT:
        sb_free(&value->value.str_value);
        return;
    case __VALUE_STRING_LIT:
        sb_free(&value->value.str_lit_value);
        return;
    default:
        assert(false); // should never happen
    }

    value->type = VALUE_UNDEFINED;
    value->value.undef_value = undef;
}

StringView memory_value_get_string(const memory_value_t *value) {
    assert(value != NULL);
    assert(memory_value_is_type(value,VALUE_STRING));

    switch (value->type)
    {
    case __VALUE_STRING_MUT:
        return sb_get_view(&value->value.str_value,0);
    case __VALUE_STRING_LIT:
        return sb_get_view(&value->value.str_lit_value,0);
    case VALUE_TYPE_STRING:
        return type_strings[STRING_IDX];
    default:
        assert(false); //should not happen
        return type_strings[UNDEFINED_IDX];
    }
}

bool memory_value_get_str_mut(StringMut *out, memory_value_t *value) {
    assert(value != NULL);
    assert(out != NULL);
    assert(memory_value_is_type(value,VALUE_STRING));

    switch (value->type)
    {
    case __VALUE_STRING_MUT:
        *out = sb_move_string_mut(&value->value.str_value);
        return true;
    case __VALUE_STRING_LIT:
        return sb_copy_mut(out,&value->value.str_lit_value);
    case VALUE_TYPE_STRING:
        return sb_copy_mut(out,type_strings + value->value.type_str_value.type_str_idx);
    default:
        assert(false); //should never happen
        return false;
    }
    
    
}

bool memory_value_get_bool(const memory_value_t *value) {
    assert(value != NULL);
    assert(memory_value_is_type(value,VALUE_BOOL));

    return value->value.bool_value;
}

int64_t memory_value_get_int(const memory_value_t *value) {
    assert(value != NULL);
    assert(memory_value_is_type(value,VALUE_INT));
    
    return value->value.int_value;
}

double memory_value_get_double(const memory_value_t *value) {
    assert(value != NULL);
    assert(memory_value_is_type(value,VALUE_DOUBLE));

    return value->value.decimal_value;
}

nill_t memory_value_get_nill(const memory_value_t *value) {
    assert(value != NULL);
    assert(memory_value_is_type(value,VALUE_NILL));

    return value->value.nill_value;
}


bool memory_value_copy(memory_value_t *out, const memory_value_t *value) {
    assert(out != NULL);
    assert(value != NULL);

    switch (value->type)
    {
    case VALUE_BOOL:
    case VALUE_INT:
    case VALUE_NILL:
    case VALUE_TYPE_STRING:
    case VALUE_UNDEFINED:
        *out=*value;
        return true;
    case __VALUE_STRING_LIT:
        return sb_copy(&out->value.str_lit_value,&value->value.str_lit_value);
    case __VALUE_STRING_MUT:
        return sb_copy_mut(&out->value.str_value,&value->value.str_value);
    default:
        assert(false); // should never happen
        return false;
    }
}
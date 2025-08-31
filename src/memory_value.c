#include<headders/memory_value.h>
#include<assert.h>
#define MAKE_VIEW(lit) \
    (StringView) {\
        .data = lit,\
        .length = sizeof(lit)-1\
    }\

StringView memory_value_get_type_str(const memory_value_t *value) {
    switch (value->type)
    {
    case VALUE_BOOL:
        return MAKE_VIEW("bool");
    case VALUE_DOUBLE:
        return MAKE_VIEW("float");
    case VALUE_INT:
        return MAKE_VIEW("int");
    case VALUE_STRING_MUT:
    case VALUE_STRING_LIT:
        return MAKE_VIEW("string");
    case VALUE_NILL:
        return MAKE_VIEW("nill");
    default:
        assert(false); // should never happen
    }
}

void memory_value_free(memory_value_t *value) {
    switch (value->type)
    {
    case VALUE_BOOL:
    case VALUE_DOUBLE:
    case VALUE_INT:
    case VALUE_NILL:
        return;
    case VALUE_STRING_MUT:
        sb_free(&value->value.str_value);
        return;
    case VALUE_STRING_LIT:
        sb_free(&value->value.str_lit_value);
        return;
    default:
        assert(false); // should never happen
    }
}
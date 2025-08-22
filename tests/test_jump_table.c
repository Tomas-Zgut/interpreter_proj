#include"tests_common.h"
#include<headders/jump_table.h>
#include<string.h>

CREATE_TEST(jump_table_tests,test_insert) {
    jump_table_t table;
    TEST_MEM_CHECK(jump_table_init,&table)
    
    const StringView key = VIEW_FROM_LIT("key to insert");
    jump_table_entry_t out;
    jump_table_entry_t *out_ptr = &out;    
    TEST_ASSERT_EQ(jump_table_insert(&table,&key,&out_ptr),JUMP_TABLE_SUCCESS,"insert should succeed")

    jump_table_free(&table);

    TEST_SUCCES
}


CREATE_TEST(jump_table_tests,test_insert_repeat) {
    jump_table_t table;
    TEST_MEM_CHECK(jump_table_init,&table)

    const StringView key = VIEW_FROM_LIT("key to insert");
    jump_table_entry_t out;
    jump_table_entry_t *out_ptr = &out;    
    TEST_ASSERT_EQ(jump_table_insert(&table,&key,&out_ptr),JUMP_TABLE_SUCCESS,"insert should succeed")

    TEST_ASSERT_EQ(jump_table_insert(&table,&key,&out_ptr),JUMP_TABLE_DUPLICATE_FOUND,"insert should fail")

    jump_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(jump_table_tests, test_lookup_existing_key) {
    jump_table_t table;
    TEST_MEM_CHECK(jump_table_init,&table)

    const StringView key = VIEW_FROM_LIT("key to insert");
    jump_table_entry_t out;
    jump_table_entry_t *out_ptr = &out;   

    TEST_ASSERT_EQ(jump_table_insert(&table,&key,&out_ptr),JUMP_TABLE_SUCCESS,"insert should succeed")

    TEST_ASSERT_NEQ(jump_table_lookup(&table,&key),NULL,"key should be found")

    jump_table_free(&table);

    TEST_SUCCES
}


CREATE_TEST(jump_table_tests, test_lookup_empty) {
    jump_table_t table;
    TEST_MEM_CHECK(jump_table_init,&table)

    const StringView key = VIEW_FROM_LIT("key to find");

    TEST_ASSERT_EQ(jump_table_lookup(&table,&key),NULL,"key should not be found")

    jump_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(jump_table_tests, test_lookup_nonexisting_key) {
    jump_table_t table;
    TEST_MEM_CHECK(jump_table_init,&table)

    const StringView key = VIEW_FROM_LIT("key to insert");
    jump_table_entry_t out;
    jump_table_entry_t *out_ptr = &out;   

    TEST_ASSERT_EQ(jump_table_insert(&table,&key,&out_ptr),JUMP_TABLE_SUCCESS,"insert should succeed")

    const StringView key2 = VIEW_FROM_LIT("key to lookup");

    TEST_ASSERT_EQ(jump_table_lookup(&table,&key2),NULL,"key should not be found")

    jump_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(jump_table_tests,test_insert_full) {
    jump_table_t table;
    TEST_MEM_CHECK(jump_table_init,&table)
    jump_table_entry_t out;
    jump_table_entry_t *out_ptr = &out;
    const char base[] = "insert key ";
    const int length = LIT_LENGTH(base) + LIT_LENGTH("00") + 1;
    const int max_size = JUMP_TABLE_INIT_SIZE*0.9f;
    // fiil up table
    for (int i = 0; i < max_size;i++) {
        char data[length];
        snprintf(data,sizeof(data),"%s%02d",base,i);
        const StringView key = VIEW_FROM_LIT(data);
        TEST_ASSERT_EQ(jump_table_insert(&table,&key,&out_ptr),JUMP_TABLE_SUCCESS,"should work");
    }
    // insert with rehash
    const StringView key_final = VIEW_FROM_LIT("final key insert");
    out_ptr = &out;
    TEST_ASSERT_EQ(jump_table_insert(&table,&key_final,&out_ptr),JUMP_TABLE_SUCCESS,"should work");

    jump_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(jump_table_tests,test_check_entries_empty) {
    jump_table_t table;
    TEST_MEM_CHECK(jump_table_init,&table)
    TEST_ASSERT_EQ(jump_table_check_entries(&table),true,"empty table contains only correct entries")

    jump_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(jump_table_tests,test_check_entries_empty_valid) {
    jump_table_t table;
    TEST_MEM_CHECK(jump_table_init,&table)

    jump_table_entry_t entries[] = {
        {.destination = 5, .destination_filled = true},
        {.destination = 7, .destination_filled = true},
        {.destination = 8, .destination_filled = true}
    };

    const StringView keys[] = {
        VIEW_FROM_LIT("key1"),
        VIEW_FROM_LIT("key2"),
        VIEW_FROM_LIT("key3")
    };

    const int num_iters = sizeof(entries) / sizeof(entries[0]);

    for (int i = 0; i < num_iters; i++) {
        jump_table_entry_t *entry = entries + i;
        TEST_ASSERT_EQ(jump_table_insert(&table,keys + i,&entry),JUMP_TABLE_SUCCESS,"insert should work")
    }

    TEST_ASSERT_EQ(jump_table_check_entries(&table),true," table contains only correct entries")

    jump_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(jump_table_tests,test_check_entries_empty_invalid) {
    jump_table_t table;
    TEST_MEM_CHECK(jump_table_init,&table)

    jump_table_entry_t entries[] = {
        {.destination = 5, .destination_filled = true},
        {.destination = 7, .destination_filled = false},
        {.destination = 8, .destination_filled = true}
    };

    const StringView keys[] = {
        VIEW_FROM_LIT("key1"),
        VIEW_FROM_LIT("key2"),
        VIEW_FROM_LIT("key3")
    };

    const int num_iters = sizeof(entries) / sizeof(entries[0]);

    for (int i = 0; i < num_iters; i++) {
        jump_table_entry_t *entry = entries + i;
        TEST_ASSERT_EQ(jump_table_insert(&table,keys + i,&entry),JUMP_TABLE_SUCCESS,"insert should work")
    }


    TEST_ASSERT_EQ(jump_table_check_entries(&table),false,"table contains invalid entries")

    jump_table_free(&table);

    TEST_SUCCES
}





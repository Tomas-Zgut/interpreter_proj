#include "tests_common.h"
#include<string.h>
#include<headders/robin_hood_table.h>
CREATE_TEST(table_init_tests,table_init_invalid_size) {
    rh_table_t table;
    rh_table_ret ret = rh_table_init(&table,6,12);

    TEST_ASSERT_EQ(ret, RH_TABLE_INVALID_ARGS,"should not create the table!")

    TEST_SUCCES
}

CREATE_TEST(table_init_tests,table_init_valid) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)

    TEST_ASSERT_EQ(table.capacity,16,"capacity should be 16!")
    TEST_ASSERT_EQ(table.entry_data_size,6,"the size of the data should be 6!")
    TEST_ASSERT_EQ(table.size, 0,"table shoulb be empty!")
    TEST_ASSERT_NEQ(table.entries,NULL,"should be valid!")
    TEST_ASSERT_NEQ(table.entry_data,NULL,"should be valid!")

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_misc_tests,table_size) {
    TODO("implement!")
    
}

CREATE_TEST(table_misc_tests,table_capacity) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)

    TEST_ASSERT_EQ(rh_table_capacity(&table),16,"capacity should be 16!")

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_look_up_tests,look_up_empty) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)
    
    const StringView key = VIEW_FROM_LIT("key to lookup");
    char data[6];
    char *ptr = data;
    void **data_ptr = (void **)&ptr;
    TEST_ASSERT_EQ(rh_table_look_up(&table,&key,data_ptr),RH_TABLE_KEY_NOT_FOUND,"key should not be in the table!\n");

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_insert_tests,table_insert_basic) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)

    const StringView key = VIEW_FROM_LIT("key to insert");
    char *data = "123456";
    void **data_ptr = (void **)&data;

    rh_table_ret retval = rh_table_insert(&table,&key,data_ptr);
    TEST_ASSERT_EQ(retval,RH_TABLE_SUCCESS,"should succeed");
    TEST_ASSERT_EQ(memcmp(*data_ptr,data,6),0,"data should match")
    TEST_ASSERT_EQ(table.size,1,"size should be 1")
    TEST_ASSERT_EQ(rh_table_look_up(&table,&key,(void**)data_ptr),RH_TABLE_SUCCESS,"key should be in the table")
    TEST_ASSERT_EQ(memcmp(*data_ptr,data,6),0,"data should match")

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_insert_tests,table_insert_duplicate) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)

    const StringView key = VIEW_FROM_LIT("key to insert");
    char *data = "123456";
    void **data_ptr = (void **)&data;

    rh_table_ret retval = rh_table_insert(&table,&key,data_ptr);
    TEST_ASSERT_EQ(retval,RH_TABLE_SUCCESS,"should succeed");
    TEST_ASSERT_EQ(memcmp(*data_ptr,data,6),0,"data should match")
    TEST_ASSERT_EQ(table.size,1,"size should be 1")
    TEST_ASSERT_EQ(rh_table_look_up(&table,&key,(void**)data_ptr),RH_TABLE_SUCCESS,"key should be in the table")
    TEST_ASSERT_EQ(memcmp(*data_ptr,data,6),0,"data should match")

    TEST_ASSERT_EQ(rh_table_insert(&table,&key,data_ptr),RH_TABLE_KEY_FOUND,"should find the key!")
    TEST_ASSERT_EQ(table.size,1,"size should be 1")
    TEST_ASSERT_EQ(memcmp(*data_ptr,data,6),0,"data should match")

    rh_table_free(&table);

    TEST_SUCCES
}
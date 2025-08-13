#include "tests_common.h"
#include<headders/robin_hood_table.h>
CREATE_TEST(table_init_tests,table_init_invalid_args1) {
    rh_table_t table;
    rh_table_ret ret = rh_table_init(&table,-5,-6);

    TEST_ASSERT_EQ(ret, RH_TABLE_INVALID_ARGS,"should not create the table!")

    TEST_SUCCES
}

CREATE_TEST(table_init_tests,table_init_invalid_args2) {
    rh_table_t table;
    rh_table_ret ret = rh_table_init(&table,-5,0);

    TEST_ASSERT_EQ(ret, RH_TABLE_INVALID_ARGS,"should not create the table!")

    TEST_SUCCES
}

CREATE_TEST(table_init_tests,table_init_invalid_args3) {
    rh_table_t table;
    rh_table_ret ret = rh_table_init(&table,0,-6);

    TEST_ASSERT_EQ(ret, RH_TABLE_INVALID_ARGS,"should not create the table!")

    TEST_SUCCES
}

CREATE_TEST(table_init_tests,table_init_overflow) {
    rh_table_t table;
    rh_table_ret ret = rh_table_init(&table,INT32_MAX,INT32_MAX);

    TEST_ASSERT_EQ(ret, RH_TABLE_ALLOC_FAIL,"should not create the table!")

    TEST_SUCCES
}

CREATE_TEST(table_init_tests,table_init_valid) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,12)

    TEST_ASSERT_EQ(table.capacity,12,"capacity should be 12!")
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
    TEST_MEM_CHECK(rh_table_init,&table,6,12)

    TEST_ASSERT_EQ(rh_table_capacity(&table),12,"capacity should be 12!")

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_look_up_tests,look_up_empty) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,12)
    

    rh_table_free(&table);

    TEST_SUCCES
}



CREATE_TEST(table_delete_tests,delete_empty) {

    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,12)
    const StringView key = VIEW_FROM_LIT("hash map key");

    TEST_ASSERT_EQ(rh_table_delete(&table,&key),RH_TABLE_TABLE_EMPTY,"delete should fail!")

    rh_table_free(&table);

    TEST_SUCCES
}


CREATE_TEST(table_delete_tests,delete_missing) {

    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,12)
    const StringView key = VIEW_FROM_LIT("hash map key");

    TEST_ASSERT_EQ(rh_table_delete(&table,&key),RH_TABLE_TABLE_EMPTY,"delete should fail!")

    rh_table_free(&table);

    TEST_SUCCES
}


CREATE_TEST(table_delete_tests,delete_key) {

    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,12)
    const StringView key = VIEW_FROM_LIT("hash map key");

    TEST_ASSERT_EQ(rh_table_delete(&table,&key),RH_TABLE_TABLE_EMPTY,"delete should fail!")

    rh_table_free(&table);

    TEST_SUCCES
}


CREATE_TEST(table_delete_tests,delete_multiple_keys) {

    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,12)
    const StringView key = VIEW_FROM_LIT("hash map key");
    TEST_ASSERT_EQ(rh_table_delete(&table,&key),RH_TABLE_TABLE_EMPTY,"delete should fail!")

    rh_table_free(&table);

    TEST_SUCCES
}
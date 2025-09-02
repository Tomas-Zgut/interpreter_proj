#include "tests_common.h"
#include<string.h>
#include<headders/robin_hood_table.h>
CREATE_TEST(table_init_tests,init_invalid_size) {
    rh_table_t table;
    rh_table_ret ret = rh_table_init(&table,6,12);

    TEST_ASSERT_EQ(ret, RH_TABLE_INVALID_ARGS,"should not create the table!")

    TEST_SUCCES
}

CREATE_TEST(table_init_tests,init_valid) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)

    TEST_ASSERT_EQ(table.capacity,16,"capacity should be 16!")
    TEST_ASSERT_EQ(table.entry_data_size,6,"the size of the data should be 6!")
    TEST_ASSERT_EQ(table.size, 0,"table shoulb be empty!")
    TEST_ASSERT_NEQ(table.entries,NULL,"should be valid!")
    TEST_ASSERT_NEQ(table.entry_data,NULL,"should be valid!")
    TEST_ASSERT_EQ(table.cleanup,NULL,"no deleter should be set")

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_init_tests,init_deleter) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init_deleter,&table,6,16,free)

    TEST_ASSERT_EQ(table.capacity,16,"capacity should be 16!")
    TEST_ASSERT_EQ(table.entry_data_size,6,"the size of the data should be 6!")
    TEST_ASSERT_EQ(table.size, 0,"table shoulb be empty!")
    TEST_ASSERT_NEQ(table.entries,NULL,"should be valid!")
    TEST_ASSERT_NEQ(table.entry_data,NULL,"should be valid!")
    TEST_ASSERT_EQ(table.cleanup,free,"deleter should be set")
    
    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_misc_tests,table_size_empty) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)

    TEST_ASSERT_EQ(rh_table_size(&table),0,"size should be 0!")

    rh_table_free(&table);

    TEST_SUCCES    
}


CREATE_TEST(table_misc_tests,table_size_insert) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)
    const StringView key = VIEW_FROM_LIT("aaaaaa");
    char data[6];
    void *data_prt = (void*)data;
    TEST_ASSERT_EQ(rh_table_size(&table),0,"size should be 0!")

    TEST_MEM_CHECK(rh_table_insert,&table,&key,&data_prt)
    TEST_ASSERT_EQ(rh_table_size(&table),1,"size should be 1!")
    rh_table_free(&table);

    TEST_SUCCES    
}

CREATE_TEST(table_misc_tests,table_size_lookup) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)
    const StringView key = VIEW_FROM_LIT("aaaaaa");
    char data[6];
    void *data_prt = (void*)data;
    TEST_ASSERT_EQ(rh_table_size(&table),0,"size should be 0!")

    TEST_MEM_CHECK(rh_table_insert,&table,&key,&data_prt)
    TEST_ASSERT_EQ(rh_table_look_up(&table,&key,&data_prt),RH_TABLE_SUCCESS,"should succeed!")
    TEST_ASSERT_EQ(rh_table_size(&table),1,"size should be 1!")
    rh_table_free(&table);

    TEST_SUCCES    
}


CREATE_TEST(table_misc_tests,table_size_delete) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)
    const StringView key = VIEW_FROM_LIT("aaaaaa");
    char data[6];
    void *data_prt = (void*)data;
    TEST_ASSERT_EQ(rh_table_size(&table),0,"size should be 0!")

    TEST_MEM_CHECK(rh_table_insert,&table,&key,&data_prt)
    TEST_ASSERT_EQ(rh_table_size(&table),1,"size should be 1!")
    TEST_ASSERT_EQ(rh_table_delete(&table,&key),RH_TABLE_SUCCESS,"should succeed!")
    TEST_ASSERT_EQ(rh_table_size(&table),0,"shoudl be 0")
    rh_table_free(&table);

    TEST_SUCCES    
}

CREATE_TEST(table_misc_tests,table_capacity) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)

    TEST_ASSERT_EQ(rh_table_capacity(&table),16,"capacity should be 16!")

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_misc_tests,table_move) {

    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)

    const StringView key = VIEW_FROM_LIT("key to lookup");
    char data[6] = "abcdef";
    char *ptr = data;
    void **data_ptr = (void **)&ptr;
    TEST_MEM_CHECK(rh_table_insert,&table,&key,data_ptr);

    rh_table_t new_table = rh_table_move(&table);

    TEST_ASSERT_EQ(table.entries,NULL,"old table should not have entries")
    TEST_ASSERT_EQ(table.entry_data,NULL,"old table should not have entry data")
    TEST_ASSERT_EQ(table.entry_keys,NULL,"old table should not have keys")
    TEST_ASSERT_EQ(table.size,0,"old table should have 0 entries")
    TEST_ASSERT_EQ(table.capacity,0,"old table should not have any capacity")

    TEST_ASSERT_NEQ(new_table.entries,NULL,"new table should  have entries")
    TEST_ASSERT_NEQ(new_table.entry_data,NULL,"new table should  have entry data")
    TEST_ASSERT_NEQ(new_table.entry_keys,NULL,"new table should  have keys")
    TEST_ASSERT_EQ(new_table.capacity,16,"new table should have 16 capacity")
    TEST_ASSERT_EQ(new_table.size,1,"there should be 1 entry in the new table")
    TEST_ASSERT_EQ(rh_table_look_up(&new_table,&key,data_ptr),RH_TABLE_SUCCESS,"key should be in new table")
    TEST_ASSERT_EQ(memcmp(*data_ptr,data,6),0,"data should be equal in the new table")

    rh_table_free(&table);
    rh_table_free(&new_table);

    TEST_SUCCES
}   

CREATE_TEST(table_look_up_tests,look_up_empty) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)
    
    const StringView key = VIEW_FROM_LIT("key to lookup");
    char data[6];
    char *ptr = data;
    void **data_ptr = (void **)&ptr;
    TEST_ASSERT_EQ(rh_table_look_up(&table,&key,data_ptr),RH_TABLE_KEY_NOT_FOUND,"key should not be in the table!\n")

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_look_up_tests,look_up_key_exists) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)
    const StringView key = VIEW_FROM_LIT("key to lookup");
    char data[6] = "abcdef";
    char *ptr = data;
    void **data_ptr = (void **)&ptr;
    TEST_MEM_CHECK(rh_table_insert,&table,&key,data_ptr);
    TEST_ASSERT_EQ(rh_table_look_up(&table,&key,data_ptr),RH_TABLE_SUCCESS,"key should be in the table!\n")
    TEST_ASSERT_EQ(memcmp(*data_ptr,data,6),0,"memory should be the same!")

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_look_up_tests,look_up_key_not_exists) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)
    const StringView key = VIEW_FROM_LIT("key to lookup");
    const StringView key2 = VIEW_FROM_LIT("key to lookup2");
    char data[6];
    char *ptr = data;
    void **data_ptr = (void **)&ptr;

    TEST_MEM_CHECK(rh_table_insert,&table,&key,data_ptr);
    TEST_ASSERT_EQ(rh_table_look_up(&table,&key2,data_ptr),RH_TABLE_KEY_NOT_FOUND,"key should not be in the table!\n")

    rh_table_free(&table);

    TEST_SUCCES
}


CREATE_TEST(table_insert_tests,insert_basic) {
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

CREATE_TEST(table_insert_tests,insert_duplicate) {
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

CREATE_TEST(table_insert_tests,insert_until_full) {
    rh_table_t table;
    const char lit[] = "key to insert ";
    const size_t lit_size = LIT_LENGTH(lit) + LIT_LENGTH("xx") + 1;
    TEST_MEM_CHECK(rh_table_init,&table,lit_size-1,16)
    const int max_table_size = 0.9f*16;
    for (int i = 0; i < max_table_size; i++) {
        char data[lit_size];
        char *data_ptr = data;
        snprintf(data,lit_size,"%s%02d",lit,i);
        const StringView key = VIEW_FROM_LIT(data);
        TEST_ASSERT_EQ(rh_table_insert(&table,&key,(void **)&data_ptr),RH_TABLE_SUCCESS,"should succeed")
        TEST_ASSERT_EQ(memcmp(data_ptr,data,lit_size-1),0,"data should match")
        TEST_ASSERT_EQ(table.size,(uint32_t)i+1,"size should match")
    }

    char data[lit_size];
    char *data_ptr = data;
    snprintf(data,lit_size,"%s%02d",lit,max_table_size+1);
    const StringView key = VIEW_FROM_LIT(data);
    TEST_ASSERT_EQ(rh_table_insert(&table,&key,(void **)&data_ptr),RH_TABLE_TABLE_FULL,"should fail")
    TEST_ASSERT_EQ(table.size,(uint32_t)max_table_size,"size should match")

    rh_table_free(&table);

    TEST_SUCCES

}


CREATE_TEST(table_delete_tests,delete_empty) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)
    const char lit[] = "key to delete";
    const StringView key = VIEW_FROM_LIT(lit);
    TEST_ASSERT_EQ(rh_table_delete(&table,&key),RH_TABLE_TABLE_EMPTY,"should not delete!")

    rh_table_free(&table);

    TEST_SUCCES;
}

CREATE_TEST(table_delete_tests,delete_empty_with_deleter) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init_deleter,&table,6,16,free)
    const char lit[] = "key to delete";
    const StringView key = VIEW_FROM_LIT(lit);
    TEST_ASSERT_EQ(rh_table_delete(&table,&key),RH_TABLE_TABLE_EMPTY,"should not delete!")

    rh_table_free(&table);

    TEST_SUCCES;
}

CREATE_TEST(table_delete_tests,delete) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)
    const char lit[] = "key to delete";
    const StringView key = VIEW_FROM_LIT(lit);
    char data[6] = "abcde";
    char *data_ptr = data;
    TEST_MEM_CHECK(rh_table_insert,&table,&key,(void**)&data_ptr)

    TEST_ASSERT_EQ(rh_table_delete(&table,&key),RH_TABLE_SUCCESS,"should  delete!")
    TEST_ASSERT_EQ(table.size,0,"size should match")

    rh_table_free(&table);

    TEST_SUCCES;
}

void some_free(void *data) {
    sb_free((String*)data);
}


CREATE_TEST(table_delete_tests,delete_with_deleter) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init_deleter,&table,sizeof(String),16,some_free)
    const char lit[] = "key to delete";
    const StringView key = VIEW_FROM_LIT(lit);
    String out;
    TEST_MEM_CHECK(sb_copy,&out,&key)
    void *data_ptr = &out;

    TEST_MEM_CHECK(rh_table_insert,&table,&key,&data_ptr)

    TEST_ASSERT_EQ(rh_table_delete(&table,&key),RH_TABLE_SUCCESS,"should delete!")
    TEST_ASSERT_EQ(table.size,0,"size should match")

    rh_table_free(&table);

    TEST_SUCCES;
}

CREATE_TEST(table_delete_tests,delete_wrong_key) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,6,16)
    const char lit[] = "key to delete";
    const char lit2[] = "key_to insert";
    const StringView key = VIEW_FROM_LIT(lit);
    const StringView key2 = VIEW_FROM_LIT(lit2);
    char data[6] = "abcde";
    char *data_ptr = data;
    TEST_MEM_CHECK(rh_table_insert,&table,&key2,(void**)&data_ptr)

    TEST_ASSERT_EQ(rh_table_delete(&table,&key),RH_TABLE_KEY_NOT_FOUND,"should not delete!")
    TEST_ASSERT_EQ(table.size,1,"size should match")

    rh_table_free(&table);
    
    TEST_SUCCES;
}

CREATE_TEST(table_delete_tests,delete_wrong_key_with_deleter) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init_deleter,&table,sizeof(String),16,some_free)
    const char lit[] = "key to delete";
    const char lit2[] ="key to insert";
    const StringView key = VIEW_FROM_LIT(lit);
    const StringView key2 = VIEW_FROM_LIT(lit2);
    String out;
    TEST_MEM_CHECK(sb_copy,&out,&key)
    void *data_ptr = &out;
    
    TEST_MEM_CHECK(rh_table_insert,&table,&key2,&data_ptr)

    TEST_ASSERT_EQ(rh_table_delete(&table,&key),RH_TABLE_KEY_NOT_FOUND,"should not delete!")
    TEST_ASSERT_EQ(table.size,1,"size should match")
    rh_table_free(&table);

    TEST_SUCCES;
}


CREATE_TEST(table_delete_tests,delete_full) {
    rh_table_t table;
    const char lit[] = "key to delete ";
    const size_t lit_size = LIT_LENGTH(lit) + LIT_LENGTH("xx") + 1;
    TEST_MEM_CHECK(rh_table_init,&table,lit_size-1,16)
    const int max_table_size = 0.9f*16;
    for (int i = 0; i < max_table_size; i++) {
        char data[lit_size];
        char *data_ptr = data;
        snprintf(data,lit_size,"%s%02d",lit,i);
        const StringView key = VIEW_FROM_LIT(data);

        TEST_ASSERT_EQ(rh_table_insert(&table,&key,(void **)&data_ptr),RH_TABLE_SUCCESS,"insert should succeed")
    }

    for (int i = 0; i < max_table_size;i++) {
        char data[lit_size];
        snprintf(data,lit_size,"%s%02d",lit,i);
        const StringView key = VIEW_FROM_LIT(data);
        TEST_ASSERT_EQ(rh_table_delete(&table,&key),RH_TABLE_SUCCESS,"delete should succeed")
    }

    rh_table_free(&table);

    TEST_SUCCES
       
}

CREATE_TEST(table_delete_tests,delete_full_with_deleter) {
    rh_table_t table;
    const char lit[] = "key to delete ";
    const size_t lit_size = LIT_LENGTH(lit) + LIT_LENGTH("xx") + 1;
    TEST_MEM_CHECK(rh_table_init_deleter,&table,sizeof(String),16,some_free)
    const int max_table_size = 0.9f*16;
    for (int i = 0; i < max_table_size; i++) {
        char data[lit_size];
        snprintf(data,lit_size,"%s%02d",lit,i);
        const StringView key = VIEW_FROM_LIT(data);
        String out;
        TEST_MEM_CHECK(sb_copy,&out,&key);
        void *data_ptr = (void *) &out;
        TEST_ASSERT_EQ(rh_table_insert(&table,&key,&data_ptr),RH_TABLE_SUCCESS,"insert should succeed")
    }

    for (int i = 0; i < max_table_size;i++) {
        char data[lit_size];
        snprintf(data,lit_size,"%s%02d",lit,i);
        const StringView key = VIEW_FROM_LIT(data);
        TEST_ASSERT_EQ(rh_table_delete(&table,&key),RH_TABLE_SUCCESS,"delete should succeed")
    }

    rh_table_free(&table);

    TEST_SUCCES       
}


CREATE_TEST(table_clear_tests,clear_empty) {
    rh_table_t table;
    const char lit[] = "key to delete ";
    const size_t lit_size = LIT_LENGTH(lit) + LIT_LENGTH("xx") + 1;
    TEST_MEM_CHECK(rh_table_init,&table,lit_size-1,16)

    rh_table_clear(&table);

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_clear_tests,clear_empty_with_deleter) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init_deleter,&table,sizeof(String),16,some_free)

    rh_table_clear(&table);

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_clear_tests,clear_full) {
    rh_table_t table;
    const char lit[] = "key to delete ";
    const size_t lit_size = LIT_LENGTH(lit) + LIT_LENGTH("xx") + 1;
    TEST_MEM_CHECK(rh_table_init,&table,lit_size-1,16)

    //fill up table
    const int max_table_size = 0.9f*16;
    for (int i = 0; i < max_table_size; i++) {
        char data[lit_size];
        char *data_ptr = data;
        snprintf(data,lit_size,"%s%02d",lit,i);
        const StringView key = VIEW_FROM_LIT(data);

        TEST_ASSERT_EQ(rh_table_insert(&table,&key,(void **)&data_ptr),RH_TABLE_SUCCESS,"insert should succeed")
    }

    rh_table_clear(&table);

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_clear_tests,clear_full_with_deleter) {
    rh_table_t table;
    const char lit[] = "key to delete ";
    const size_t lit_size = LIT_LENGTH(lit) + LIT_LENGTH("xx") + 1;
    TEST_MEM_CHECK(rh_table_init_deleter,&table,sizeof(String),16,some_free)

    //fill up table
    const int max_table_size = 0.9f*16;
    for (int i = 0; i < max_table_size; i++) {
        char data[lit_size];
        snprintf(data,lit_size,"%s%02d",lit,i);
        const StringView key = VIEW_FROM_LIT(data);
        String out;
        TEST_MEM_CHECK(sb_copy,&out,&key);
        void *data_ptr = (void *) &out;
        TEST_ASSERT_EQ(rh_table_insert(&table,&key,&data_ptr),RH_TABLE_SUCCESS,"insert should succeed")
    }

    rh_table_clear(&table);

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_free_tests,free_empty) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,20,16)

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_free_tests,free_empty_with_deleter) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init_deleter,&table,20,16,some_free)

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_free_tests,free) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,20,16)
    const StringView key = VIEW_FROM_LIT("key");
    char data[20];
    void *data_ptr = (void *)data;
    TEST_ASSERT_EQ(rh_table_insert(&table,&key,&data_ptr),RH_TABLE_SUCCESS,"should succeed")

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_free_tests,free_with_deleter) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init_deleter,&table,sizeof(String),16,some_free)
    const StringView key = VIEW_FROM_LIT("key");
    String out;
    TEST_MEM_CHECK(sb_copy,&out,&key)
    void *data_ptr = (void *)&out;
    TEST_ASSERT_EQ(rh_table_insert(&table,&key,&data_ptr),RH_TABLE_SUCCESS,"should succeed")

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_free_tests,free_double) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,20,16)
    const StringView key = VIEW_FROM_LIT("key");
    char data[20];
    void *data_ptr = (void *)data;
    TEST_ASSERT_EQ(rh_table_insert(&table,&key,&data_ptr),RH_TABLE_SUCCESS,"should succeed")

    rh_table_free(&table);
    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_free_tests,free_double_with_deleter) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init_deleter,&table,sizeof(String),16,some_free)
    const StringView key = VIEW_FROM_LIT("key");
    String out;
    TEST_MEM_CHECK(sb_copy,&out,&key)
    void *data_ptr = (void *)&out;
    TEST_ASSERT_EQ(rh_table_insert(&table,&key,&data_ptr),RH_TABLE_SUCCESS,"should succeed")
    
    rh_table_free(&table);
    rh_table_free(&table);

    TEST_SUCCES
}


CREATE_TEST(table_resize_tests,resize_empty) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,20,16)
    TEST_MEM_CHECK(rh_table_resize,&table)
    TEST_ASSERT_EQ(table.size,0,"shoudl be 0")
    TEST_ASSERT_EQ(table.capacity,32,"should be 32")

    rh_table_free(&table);

    TEST_SUCCES
}


CREATE_TEST(table_resize_tests,resize_full) {
    rh_table_t table;
    const char lit[] = "key to insert ";
    const size_t lit_size = LIT_LENGTH(lit) + LIT_LENGTH("xx") + 1;
    TEST_MEM_CHECK(rh_table_init,&table,lit_size-1,16)

    //fill up table
    const int max_table_size = 0.9f*16;
    for (int i = 0; i < max_table_size; i++) {
        char data[lit_size];
        char *data_ptr = data;
        snprintf(data,lit_size,"%s%02d",lit,i);
        const StringView key = VIEW_FROM_LIT(data);
        TEST_ASSERT_EQ(rh_table_insert(&table,&key,(void **)&data_ptr),RH_TABLE_SUCCESS,"insert should succeed")
    }
    const StringView key = VIEW_FROM_LIT("aaaaaa");
    char data[lit_size];
    void *data_ptr = (void* )data;
    TEST_ASSERT_EQ(rh_table_insert(&table,&key,&data_ptr),RH_TABLE_TABLE_FULL,"insert not succeed now!")
    TEST_MEM_CHECK(rh_table_resize,&table)
    TEST_ASSERT_EQ(table.capacity,32,"should be 32")
    
    for (int i = 0; i < max_table_size; i++) {
        char data[lit_size];
        char *data_ptr = data;
        snprintf(data,lit_size,"%s%02d",lit,i);
        const StringView key = VIEW_FROM_LIT(data);
        TEST_ASSERT_EQ(rh_table_look_up(&table,&key,(void **)&data_ptr),RH_TABLE_SUCCESS,"lookup should succeed")
        TEST_ASSERT_EQ(memcmp(data,data_ptr,lit_size-1),0,"data should match!")
    }

    TEST_ASSERT_EQ(rh_table_insert(&table,&key,&data_ptr),RH_TABLE_SUCCESS,"insert should work now!")

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_resize_tests,resize_full_with_deleter) {
    rh_table_t table;
    const char lit[] = "key to insert ";
    const size_t lit_size = LIT_LENGTH(lit) + LIT_LENGTH("xx") + 1;
    TEST_MEM_CHECK(rh_table_init_deleter,&table,sizeof(String),16,some_free)

    //fill up table
    const int max_table_size = 0.9f*16;
    for (int i = 0; i < max_table_size; i++) {
        char data[lit_size];
        snprintf(data,lit_size,"%s%02d",lit,i);
        const StringView key = VIEW_FROM_LIT(data);
        String out;
        TEST_MEM_CHECK(sb_copy,&out,&key);
        void *data_ptr = (void *)&out;
        TEST_ASSERT_EQ(rh_table_insert(&table,&key,&data_ptr),RH_TABLE_SUCCESS,"insert should succeed")
    }

    const StringView key = VIEW_FROM_LIT("aaaaaa");
    String out;
    TEST_MEM_CHECK(sb_copy,&out,&key)
    void *data_ptr = (void* )&out;
    TEST_ASSERT_EQ(rh_table_insert(&table,&key,&data_ptr),RH_TABLE_TABLE_FULL,"insert not succeed now!")
    TEST_MEM_CHECK(rh_table_resize,&table)
    TEST_ASSERT_EQ(table.capacity,32,"should be 32")

    for (int i = 0; i < max_table_size; i++) {
        char data[lit_size];
        snprintf(data,lit_size,"%s%02d",lit,i);
        const StringView key = VIEW_FROM_LIT(data);
        String *out;
        TEST_ASSERT_EQ(rh_table_look_up(&table,&key,(void **)&out),RH_TABLE_SUCCESS,"lookup should succeed")
        TEST_ASSERT_EQ(sb_cmp(out,&key),0,"data should match")
    }
    TEST_ASSERT_EQ(rh_table_insert(&table,&key,&data_ptr),RH_TABLE_SUCCESS,"insert should work now!")

    rh_table_free(&table);

    TEST_SUCCES
}


CREATE_TEST(table_iter_tests,iter_init_empty) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,8,16)
    rh_table_iter_t iter = rh_table_iter_init(&table);
    
    TEST_ASSERT_EQ(iter.table,&table,"table should match")
    TEST_ASSERT_EQ(iter.table_line,table.capacity,"iterator should be at the end!")
    TEST_ASSERT_EQ(rh_table_iter_valid(&iter),false,"iterator should not be valid")
    TEST_ASSERT_EQ(rh_table_iter_next(&iter),RH_TABLE_ITER_END,"iterator shouldd be at the end")

    rh_table_free(&table);

    TEST_SUCCES
}


CREATE_TEST(table_iter_tests,iter_init) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,8,16)
    char data[8] = "abcdefg";
    char *data_ptr = data;
    const StringView key = VIEW_FROM_LIT(data);
    TEST_MEM_CHECK(rh_table_insert,&table,&key,(void**)&data_ptr)
    
    rh_table_iter_t iter = rh_table_iter_init(&table);
    TEST_ASSERT_EQ(iter.table,&table,"table should match")
    TEST_ASSERT_NEQ(iter.table_line,table.capacity,"iterator not be at the end!")
    TEST_ASSERT_EQ(rh_table_iter_valid(&iter),true,"iterator should be valid")
    
    const StringView iter_key = rh_table_iter_get_key(&iter);
    char *iter_data = rh_table_iter_get_data(&iter);
    TEST_ASSERT_EQ(sb_cmp(&iter_key,&key),0,"keys should match")
    TEST_ASSERT_EQ(memcmp(iter_data,data,LIT_LENGTH(data)),0,"data should match")
    
    TEST_ASSERT_EQ(rh_table_iter_next(&iter),RH_TABLE_ITER_END,"next should be the end")
    TEST_ASSERT_EQ(rh_table_iter_valid(&iter),false,"iterator should not be valid")

    rh_table_free(&table);

    TEST_SUCCES
}


CREATE_TEST(table_iter_tests,iter_over_table) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,8,16)

    char data[8] = "abcdefg";
    char *data_ptr = data;
    const StringView key = VIEW_FROM_LIT(data);
    TEST_MEM_CHECK(rh_table_insert,&table,&key,(void**)&data_ptr)
    const StringView key2 = VIEW_FROM_LIT("key2");
    TEST_MEM_CHECK(rh_table_insert,&table,&key2,(void**)&data_ptr)
    const StringView key3 = VIEW_FROM_LIT("key3");
    TEST_MEM_CHECK(rh_table_insert,&table,&key3,(void**)&data_ptr)
    uint32_t count = 0;
    RH_TABLE_ITER(iter,&table) {
        count ++;
    }
    TEST_ASSERT_EQ(count,3,"Should iterate 3 times")

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_iter_tests,iter_over_table_rehash) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,8,16)

    char data[8] = "abcdefg";
    char *data_ptr = data;
    const StringView key = VIEW_FROM_LIT(data);
    TEST_MEM_CHECK(rh_table_insert,&table,&key,(void**)&data_ptr)
    const StringView key2 = VIEW_FROM_LIT("key2");
    TEST_MEM_CHECK(rh_table_insert,&table,&key2,(void**)&data_ptr)
    const StringView key3 = VIEW_FROM_LIT("key3");
    TEST_MEM_CHECK(rh_table_insert,&table,&key3,(void**)&data_ptr)
    TEST_MEM_CHECK(rh_table_resize,&table)
    uint32_t count = 0;
    RH_TABLE_ITER(iter,&table) {
        count ++;
    }
    TEST_ASSERT_EQ(count,3,"Should iterate 3 times")

    rh_table_free(&table);

    TEST_SUCCES
}

CREATE_TEST(table_iter_tests,iter_over_table_delete) {
    rh_table_t table;
    TEST_MEM_CHECK(rh_table_init,&table,8,16)

    char data[8] = "abcdefg";
    char *data_ptr = data;
    const StringView key = VIEW_FROM_LIT(data);
    TEST_MEM_CHECK(rh_table_insert,&table,&key,(void**)&data_ptr)
    const StringView key2 = VIEW_FROM_LIT("key2");
    TEST_MEM_CHECK(rh_table_insert,&table,&key2,(void**)&data_ptr)
    const StringView key3 = VIEW_FROM_LIT("key3");
    TEST_MEM_CHECK(rh_table_insert,&table,&key3,(void**)&data_ptr)
    TEST_ASSERT_EQ(rh_table_delete(&table,&key),RH_TABLE_SUCCESS,"should succeed")
    uint32_t count = 0;
    RH_TABLE_ITER(iter,&table) {
        count ++;
    }
    TEST_ASSERT_EQ(count,2,"Should iterate 2 times")

    rh_table_free(&table);

    TEST_SUCCES
}
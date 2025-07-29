#include "tests_common.h"
#include<headders/string_buffer.h>

CREATE_TEST(string_view_tests,view_from_empty_string1) {
    StringBuffer buff;
    TEST_MEM_CHECK(sb_init,&buff,0);

    const StringView view = sb_get_view(&buff,0);

    TEST_ASSERT_EQ(sb_length(&view),0,"Length of an empty view should be 0!")
    TEST_ASSERT_EQ(memcmp(view.data,buff.data,sb_length(&view)),0,"The data in empty strings should be the same!")

    TEST_SUCCES
}

CREATE_TEST(string_view_tests,view_from_empty_string2) {
    StringBuffer buff;
    TEST_MEM_CHECK(sb_init,&buff,0);

    StringData dataBuff;
    TEST_MEM_CHECK(sb_copy,&buff,&dataBuff)

    const StringView view = sb_get_view(&dataBuff,0);

    TEST_ASSERT_EQ(sb_length(&view),0,"Length of an empty view should be 0!")

    TEST_ASSERT_EQ(memcmp(view.data,dataBuff.data,sb_length(&view)),0,"The data in empty strings should be the same!")

    TEST_SUCCES
}

CREATE_TEST(string_view_tests,view_from_empty_string3) {
    StringBuffer buff;
    TEST_MEM_CHECK(sb_init,&buff,0);

    const StringView test_view = sb_get_view(&buff,0);
    const StringView view = sb_get_view(&buff,0);

    TEST_ASSERT_EQ(sb_length(&view),0,"Length of an empty view should be 0!")
    TEST_ASSERT_EQ(memcmp(view.data,buff.data,sb_length(&view)),0,"The data in empty strings should be the same!")

    TEST_SUCCES
}

CREATE_TEST(string_view_tests, view_from_string1) {
    StringBuffer buff;
    const char str_lit[] = "this is a string!";
    TEST_MEM_CHECK(sb_init,&buff,LIT_LENGTH(str_lit))
    buff.length = LIT_LENGTH(str_lit);
    memcpy(buff.data,str_lit,LIT_LENGTH(str_lit));
    // string start
    const StringView view_this = sb_get_substring(&buff,0,4);
    TEST_ASSERT_EQ(sb_length(&view_this), 4,"length of a substring should be correct!")
    TEST_ASSERT_EQ(memcmp(view_this.data,"this",sb_length(&view_this)),0,"Substring should equal the source!")
    // srting middle
    const StringView view_is = sb_get_substring(&buff,5,2);
    TEST_ASSERT_EQ(sb_length(&view_is), 2,"length of a substring should be correct!")
    TEST_ASSERT_EQ(memcmp(view_is.data,"is",sb_length(&view_is)),0,"Substring should equal the source!")

    //string end
    const StringView view_string = sb_get_substring(&buff,10,6);
    TEST_ASSERT_EQ(sb_length(&view_string), 6,"length of a substring should be correct!")
    TEST_ASSERT_EQ(memcmp(view_string.data,"string",sb_length(&view_string)),0,"Substring should equal the source!")

    TEST_SUCCES   
}



CREATE_TEST(string_view_tests, view_from_string2) {
    StringBuffer buff;
    const char str_lit[] = "this is a string!";
    TEST_MEM_CHECK(sb_init,&buff,LIT_LENGTH(str_lit))
    buff.length = LIT_LENGTH(str_lit);
    memcpy(buff.data,str_lit,LIT_LENGTH(str_lit));

    // string start
    const StringView view_this = sb_get_substring(&buff,0,LIT_LENGTH("this"));
    TEST_ASSERT_EQ(sb_length(&view_this), LIT_LENGTH("this"),"length of a substring should be correct!")
    TEST_ASSERT_EQ(memcmp(view_this.data,"this",sb_length(&view_this)),0,"Substring should equal the source!")

    // srting middle
    const StringView view_is = sb_get_substring(&buff,5,LIT_LENGTH("is"));
    TEST_ASSERT_EQ(sb_length(&view_is), LIT_LENGTH("is"),"length of a substring should be correct!")
    TEST_ASSERT_EQ(memcmp(view_is.data,"is",sb_length(&view_is)),0,"Substring should equal the source!")

    //string end
    const StringView view_string = sb_get_substring(&buff,10,LIT_LENGTH("string"));
    TEST_ASSERT_EQ(sb_length(&view_string), LIT_LENGTH("string"),"length of a substring should be correct!")
    TEST_ASSERT_EQ(memcmp(view_string.data,"string",sb_length(&view_string)),0,"Substring should equal the source!")

    TEST_SUCCES   
}
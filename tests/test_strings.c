#include "tests_common.h"
#include<headders/string_buffer.h>
#include<string.h>

CREATE_TEST(string_view_tests,view_from_empty_string1) {
    StringMut buff;
    TEST_MEM_CHECK(sb_init,&buff,0);

    const StringView view = sb_get_view(&buff,0);

    TEST_ASSERT_EQ(sb_length(&view),0,"Length of an empty view should be 0!")
    TEST_ASSERT_EQ(memcmp(view.data,buff.data,sb_length(&view)),0,"The data in empty strings should be the same!")

    TEST_SUCCES
}

CREATE_TEST(string_view_tests,view_from_empty_string2) {
    StringMut buff;
    TEST_MEM_CHECK(sb_init,&buff,0);

    String dataBuff;
    TEST_MEM_CHECK(sb_copy,&dataBuff,&buff)

    const StringView view = sb_get_view(&dataBuff,0);

    TEST_ASSERT_EQ(sb_length(&view),0,"Length of an empty view should be 0!")

    TEST_ASSERT_EQ(memcmp(view.data,dataBuff.data,sb_length(&view)),0,"The data in empty strings should be the same!")

    TEST_SUCCES
}

CREATE_TEST(string_view_tests,view_from_empty_string3) {
    StringMut buff;
    TEST_MEM_CHECK(sb_init,&buff,0);

    const StringView test_view = sb_get_view(&buff,0);
    const StringView view = sb_get_view(&buff,0);

    TEST_ASSERT_EQ(sb_length(&view),0,"Length of an empty view should be 0!")
    TEST_ASSERT_EQ(memcmp(view.data,buff.data,sb_length(&view)),0,"The data in empty strings should be the same!")

    TEST_SUCCES
}

CREATE_TEST(string_view_tests, view_from_string1) {
    StringMut buff;
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
    StringMut buff;
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



CREATE_TEST(string_append_tests,append_char_empty) {
    StringMut buff;
    TEST_MEM_CHECK(sb_init,&buff,0)
    TEST_MEM_CHECK(sb_append_char,&buff,'a')

    TEST_ASSERT_EQ(sb_length(&buff),1,"length of buffer should be 1!")
    TEST_ASSERT_EQ(memcmp(buff.data,"a",1),0,"appended buffer should match!")
    TEST_SUCCES
}

#define ASSERT_ARRAY(lit) _Static_assert(!__builtin_types_compatible_p(__typeof__(lit), __typeof__(&(lit)[0])),"Must be an array");
#define ASSERT_CHAR_ARRAY(lit) _Static_assert(__builtin_types_compatible_p(__typeof__(lit[0]), const char ),"Must be char!"); 

CREATE_TEST(string_append_tests,append_string_empty) {
    StringMut buff;
    TEST_MEM_CHECK(sb_init,&buff,0)
    
    StringMut append_buff;
    const char lit[] = "append string";
    TEST_MEM_CHECK(sb_init,&append_buff,LIT_LENGTH(lit))
    memcpy(append_buff.data,lit,LIT_LENGTH(lit));
    append_buff.length = LIT_LENGTH(lit);

    TEST_MEM_CHECK(sb_append_string,&buff,&append_buff)

    TEST_ASSERT_EQ(sb_length(&buff),LIT_LENGTH(lit),"lengths should match after appending!")
    TEST_ASSERT_EQ(memcmp(buff.data,append_buff.data,sb_length(&append_buff)),0,"appended buffer should match!")

    TEST_SUCCES
}
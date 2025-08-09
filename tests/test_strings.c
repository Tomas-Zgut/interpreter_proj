#include "tests_common.h"
#include<headders/string_buffer.h>
#include<string.h>

CREATE_TEST(string_view_tests,view_from_empty_string1) {
    StringMut buff;
    TEST_MEM_CHECK(sb_init,&buff,0);

    const StringView view = sb_get_view(&buff,0);

    TEST_ASSERT_EQ(sb_length(&view),0,"Length of an empty view should be 0!")
    TEST_ASSERT_EQ(memcmp(view.data,buff.data,sb_length(&view)),0,"The data in empty strings should be the same!")

    sb_free(&buff);

    TEST_SUCCES
}

CREATE_TEST(string_view_tests,view_from_empty_string2) {
    String buff;
    STRING_FROM_LIT(buff,"")

    const StringView view = sb_get_view(&buff,0);

    TEST_ASSERT_EQ(sb_length(&view),0,"Length of an empty view should be 0!")
    TEST_ASSERT_EQ(memcmp(view.data,buff.data,sb_length(&view)),0,"The data in empty strings should be the same!")

    sb_free(&buff);

    TEST_SUCCES
}

CREATE_TEST(string_view_tests,view_from_empty_string3) {
    const StringView test_view = VIEW_FROM_LIT("");
    const StringView view = sb_get_view(&test_view,0);

    TEST_ASSERT_EQ(sb_length(&view),0,"Length of an empty view should be 0!")
    TEST_ASSERT_EQ(memcmp(view.data,test_view.data,sb_length(&view)),0,"The data in empty strings should be the same!")

    TEST_SUCCES
}

CREATE_TEST(string_view_tests, view_from_string1) {
    StringMut buff;
    const char str_lit[] = "this is a string!";
    STRING_MUT_FROM_LIT(buff,str_lit)
    
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

    sb_free(&buff);

    TEST_SUCCES   
}



CREATE_TEST(string_view_tests, view_from_string2) {
    String buff;
    const char str_lit[] = "this is a string!";
    STRING_FROM_LIT(buff,str_lit)

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

    sb_free(&buff);

    TEST_SUCCES   
}



CREATE_TEST(string_view_tests, view_from_string3) {
    const char str_lit[] = "this is a string!";
    const StringView view = VIEW_FROM_LIT(str_lit);

    // string start
    const StringView view_this = sb_get_substring(&view,0,LIT_LENGTH("this"));
    TEST_ASSERT_EQ(sb_length(&view_this), LIT_LENGTH("this"),"length of a substring should be correct!")
    TEST_ASSERT_EQ(memcmp(view_this.data,"this",sb_length(&view_this)),0,"Substring should equal the source!")

    // srting middle
    const StringView view_is = sb_get_substring(&view,5,LIT_LENGTH("is"));
    TEST_ASSERT_EQ(sb_length(&view_is), LIT_LENGTH("is"),"length of a substring should be correct!")
    TEST_ASSERT_EQ(memcmp(view_is.data,"is",sb_length(&view_is)),0,"Substring should equal the source!")

    //string end
    const StringView view_string = sb_get_substring(&view,10,LIT_LENGTH("string"));
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

    sb_free(&buff);

    TEST_SUCCES
}

CREATE_TEST(string_append_tests,append_string_empty1) {
    StringMut buff;
    TEST_MEM_CHECK(sb_init,&buff,0)
    
    StringMut append_buff;
    const char lit[] = "append string";
    STRING_MUT_FROM_LIT(append_buff,lit)

    TEST_MEM_CHECK(sb_append_string,&buff,&append_buff)

    TEST_ASSERT_EQ(sb_length(&buff),LIT_LENGTH(lit),"lengths should match after appending!")
    TEST_ASSERT_EQ(memcmp(buff.data,append_buff.data,sb_length(&append_buff)),0,"appended buffer should match!")

    sb_free(&buff);
    sb_free(&append_buff);

    TEST_SUCCES
}



CREATE_TEST(string_append_tests,append_string_empty2) {
    StringMut buff;
    TEST_MEM_CHECK(sb_init,&buff,0)
    
    String append_buff;
    const char lit[] = "append string";
    STRING_FROM_LIT(append_buff,lit)

    TEST_MEM_CHECK(sb_append_string,&buff,&append_buff)

    TEST_ASSERT_EQ(sb_length(&buff),LIT_LENGTH(lit),"lengths should match after appending!")
    TEST_ASSERT_EQ(memcmp(buff.data,append_buff.data,sb_length(&append_buff)),0,"appended buffer should match!")

    sb_free(&buff);
    sb_free(&append_buff);

    TEST_SUCCES
}

CREATE_TEST(string_append_tests,append_string_empty3) {
    StringMut buff;
    TEST_MEM_CHECK(sb_init,&buff,0)
    
    const char lit[] = "append string";
    const StringView append_buff = VIEW_FROM_LIT(lit);

    TEST_MEM_CHECK(sb_append_string,&buff,&append_buff)

    TEST_ASSERT_EQ(sb_length(&buff),LIT_LENGTH(lit),"lengths should match after appending!")
    TEST_ASSERT_EQ(memcmp(buff.data,append_buff.data,sb_length(&append_buff)),0,"appended buffer should match!")
    
    sb_free(&buff);
    TEST_SUCCES
}

CREATE_TEST(string_append_tests,append_char) {
    StringMut buff;
    TEST_MEM_CHECK(sb_init,&buff,0)
    TEST_MEM_CHECK(sb_append_char,&buff,'a')
    TEST_MEM_CHECK(sb_append_char,&buff,'a')
    TEST_MEM_CHECK(sb_append_char,&buff,'a')

    TEST_ASSERT_EQ(sb_length(&buff),3,"length of buffer should be 1!")
    TEST_ASSERT_EQ(memcmp(buff.data,"aaa",3),0,"appended buffer should match!")

    sb_free(&buff);

    TEST_SUCCES
}

CREATE_TEST(string_append_tests,append_string1) {
    StringMut buff;
    const char basic_lit[] = "also ";
    STRING_MUT_FROM_LIT(buff,basic_lit)

    StringMut append_buff;
    const char lit[] = "append string";
    STRING_MUT_FROM_LIT(append_buff,lit)

    TEST_MEM_CHECK(sb_append_string,&buff,&append_buff)

    TEST_ASSERT_EQ(sb_length(&buff),LIT_LENGTH(lit) + LIT_LENGTH(basic_lit),"lengths should match after appending!")
    TEST_ASSERT_EQ(memcmp(buff.data,"also append string",sb_length(&buff)),0,"appended buffer should match!")

    sb_free(&buff);
    sb_free(&append_buff);

    TEST_SUCCES
}



CREATE_TEST(string_append_tests,append_string2) {
    StringMut buff;
    const char basic_lit[] = "also ";
    STRING_MUT_FROM_LIT(buff,basic_lit)

    String append_buff;
    const char lit[] = "append string";
    STRING_FROM_LIT(append_buff,lit)

    TEST_MEM_CHECK(sb_append_string,&buff,&append_buff)

    TEST_ASSERT_EQ(sb_length(&buff),LIT_LENGTH(lit) + LIT_LENGTH(basic_lit),"lengths should match after appending!")
    TEST_ASSERT_EQ(memcmp(buff.data,"also append string",sb_length(&buff)),0,"appended buffer should match!")

    sb_free(&buff);
    sb_free(&append_buff);

    TEST_SUCCES
}

CREATE_TEST(string_append_tests,append_string3) {
    StringMut buff;
    const char basic_lit[] = "also ";
    STRING_MUT_FROM_LIT(buff,basic_lit)
    
    const char lit[] = "append string";
    const StringView append_buff = VIEW_FROM_LIT(lit);

    TEST_MEM_CHECK(sb_append_string,&buff,&append_buff)

    TEST_ASSERT_EQ(sb_length(&buff),LIT_LENGTH(lit) + LIT_LENGTH(basic_lit),"lengths should match after appending!")
    TEST_ASSERT_EQ(memcmp(buff.data,"also append string",sb_length(&buff)),0,"appended buffer should match!")
    
    sb_free(&buff);
    TEST_SUCCES
}

CREATE_TEST(string_append_tests,apend_char_full) {
    StringMut buff;
    STRING_MUT_FROM_LIT(buff,"aaaa")
    TEST_MEM_CHECK(sb_append_char,&buff,'a')
    TEST_MEM_CHECK(sb_append_char,&buff,'a')

    TEST_ASSERT_EQ(sb_length(&buff),6,"length should match after appending!")
    TEST_ASSERT_EQ(memcmp(buff.data,"aaaaaa",6),0,"append buffer should match!")
    TEST_ASSERT_EQ(buff.capacity,8,"Capacity should match!")

    sb_free(&buff);
    TEST_SUCCES
}



CREATE_TEST(string_concat_tests,concat_mut1) {
    StringMut out;
    StringMut buff1;
    StringMut buff2;
    const char lit1[] = "concat ";
    const char lit2[] = "strings";
    
    STRING_MUT_FROM_LIT(buff1,lit1)
    STRING_MUT_FROM_LIT(buff2,lit2)

    TEST_MEM_CHECK(sb_concat_mut,&out,&buff1,&buff2)

    TEST_ASSERT_EQ(sb_length(&out),LIT_LENGTH(lit1)+LIT_LENGTH(lit2),"lengths should equal after concatenation")
    TEST_ASSERT_EQ(memcmp(out.data,"concat strings",sb_length(&out)),0,"concatenation output should be correct!")
    TEST_ASSERT(sb_length(&out) == out.capacity,"capacity should be the same as length!")

    sb_free(&out);
    sb_free(&buff1);
    sb_free(&buff2);

    TEST_SUCCES
}

CREATE_TEST(string_concat_tests,concat_mut2) {
    StringMut out;
    String buff1;
    String buff2;
    const char lit1[] = "concat ";
    const char lit2[] = "strings";

    STRING_FROM_LIT(buff1,lit1)
    STRING_FROM_LIT(buff2,lit2)

    TEST_MEM_CHECK(sb_concat_mut,&out,&buff1,&buff2)

    TEST_ASSERT_EQ(sb_length(&out),LIT_LENGTH(lit1)+LIT_LENGTH(lit2),"lengths should equal after concatenation")
    TEST_ASSERT_EQ(memcmp(out.data,"concat strings",sb_length(&out)),0,"concatenation output should be correct!")
    TEST_ASSERT(sb_length(&out) == out.capacity,"capacity should be the same as length!")

    sb_free(&out);
    sb_free(&buff1);
    sb_free(&buff2);

    TEST_SUCCES
}

CREATE_TEST(string_concat_tests,concat_mut3) {
    StringMut out;
    const char lit1[] = "concat ";
    const char lit2[] = "strings";
    const StringView buff1 = VIEW_FROM_LIT(lit1);
    const StringView buff2 = VIEW_FROM_LIT(lit2);

    TEST_MEM_CHECK(sb_concat_mut,&out,&buff1,&buff2)

    TEST_ASSERT_EQ(sb_length(&out),LIT_LENGTH(lit1)+LIT_LENGTH(lit2),"lengths should equal after concatenation")
    TEST_ASSERT_EQ(memcmp(out.data,"concat strings",sb_length(&out)),0,"concatenation output should be correct!")
    TEST_ASSERT(sb_length(&out) == out.capacity,"capacity should be the same as length!")

    sb_free(&out);

    TEST_SUCCES
}

CREATE_TEST(string_concat_tests,concat1) {
    String out;
    StringMut buff1;
    StringMut buff2;
    const char lit1[] = "concat ";
    const char lit2[] = "strings";

    STRING_MUT_FROM_LIT(buff1,lit1)
    STRING_MUT_FROM_LIT(buff2,lit2)

    TEST_MEM_CHECK(sb_concat,&out,&buff1,&buff2)

    TEST_ASSERT_EQ(sb_length(&out),LIT_LENGTH(lit1)+LIT_LENGTH(lit2),"lengths should equal after concatenation")
    TEST_ASSERT_EQ(memcmp(out.data,"concat strings",sb_length(&out)),0,"concatenation output should be correct!")


    sb_free(&out);
    sb_free(&buff1);
    sb_free(&buff2);

    TEST_SUCCES
}

CREATE_TEST(string_concat_tests,concat2) {
    String out;
    String buff1;
    String buff2;
    const char lit1[] = "concat ";
    const char lit2[] = "strings";

    STRING_FROM_LIT(buff1,lit1)
    STRING_FROM_LIT(buff2,lit2)

    TEST_MEM_CHECK(sb_concat,&out,&buff1,&buff2)

    TEST_ASSERT_EQ(sb_length(&out),LIT_LENGTH(lit1)+LIT_LENGTH(lit2),"lengths should equal after concatenation")
    TEST_ASSERT_EQ(memcmp(out.data,"concat strings",sb_length(&out)),0,"concatenation output should be correct!")


    sb_free(&out);
    sb_free(&buff1);
    sb_free(&buff2);

    TEST_SUCCES
}

CREATE_TEST(string_concat_tests,concat3) {
    String out;
    const char lit1[] = "concat ";
    const char lit2[] = "strings";
    const StringView buff1 = VIEW_FROM_LIT(lit1);
    const StringView buff2 = VIEW_FROM_LIT(lit2);

    TEST_MEM_CHECK(sb_concat,&out,&buff1,&buff2)

    TEST_ASSERT_EQ(sb_length(&out),LIT_LENGTH(lit1)+LIT_LENGTH(lit2),"lengths should equal after concatenation")
    TEST_ASSERT_EQ(memcmp(out.data,"concat strings",sb_length(&out)),0,"concatenation output should be correct!")


    sb_free(&out);

    TEST_SUCCES
}


CREATE_TEST(string_copy_tests,copy_mut1) {
    StringMut out;
    StringMut buff;
    const char lit[] = "string to copy";
    STRING_MUT_FROM_LIT(buff,lit)

    TEST_MEM_CHECK(sb_copy_mut,&out,&buff)

    TEST_ASSERT_EQ(sb_length(&out),sb_length(&buff),"lengths after copy should match")
    TEST_ASSERT_EQ(memcmp(out.data,buff.data,sb_length(&out)),0,"datashould match")
    TEST_ASSERT(sb_length(&out) == out.capacity,"capacity should be the same as length!")

    sb_free(&out);
    sb_free(&buff);

    TEST_SUCCES
}


CREATE_TEST(string_copy_tests,copy_mut2) {
    StringMut out;
    String buff;
    const char lit[] = "string to copy";
    STRING_FROM_LIT(buff,lit)

    TEST_MEM_CHECK(sb_copy_mut,&out,&buff)

    TEST_ASSERT_EQ(sb_length(&out),sb_length(&buff),"lengths after copy should match")
    TEST_ASSERT_EQ(memcmp(out.data,buff.data,sb_length(&out)),0,"datashould match")
    TEST_ASSERT(sb_length(&out) == out.capacity,"capacity should be the same as length!")

    sb_free(&out);
    sb_free(&buff);

    TEST_SUCCES
}


CREATE_TEST(string_copy_tests,copy_mut3) {
    StringMut out;
    const char lit[] = "string to copy";
    const StringView buff = VIEW_FROM_LIT(lit);

    TEST_MEM_CHECK(sb_copy_mut,&out,&buff)

    TEST_ASSERT_EQ(sb_length(&out),sb_length(&buff),"lengths after copy should match")
    TEST_ASSERT_EQ(memcmp(out.data,buff.data,sb_length(&out)),0,"datashould match")
    TEST_ASSERT(sb_length(&out) == out.capacity,"capacity should be the same as length!")

    sb_free(&out);

    TEST_SUCCES
}


CREATE_TEST(string_copy_tests,copy1) {
    String out;
    StringMut buff;
    const char lit[] = "string to copy";
    STRING_MUT_FROM_LIT(buff,lit)

    TEST_MEM_CHECK(sb_copy,&out,&buff)

    TEST_ASSERT_EQ(sb_length(&out),sb_length(&buff),"lengths after copy should match")
    TEST_ASSERT_EQ(memcmp(out.data,buff.data,sb_length(&out)),0,"datashould match")

    sb_free(&out);
    sb_free(&buff);

    TEST_SUCCES
}


CREATE_TEST(string_copy_tests,copy2) {
    String out;
    String buff;
    const char lit[] = "string to copy";
    STRING_FROM_LIT(buff,lit)

    TEST_MEM_CHECK(sb_copy,&out,&buff)

    TEST_ASSERT_EQ(sb_length(&out),sb_length(&buff),"lengths after copy should match")
    TEST_ASSERT_EQ(memcmp(out.data,buff.data,sb_length(&out)),0,"datashould match")


    sb_free(&out);
    sb_free(&buff);

    TEST_SUCCES
}


CREATE_TEST(string_copy_tests,copy3) {
    String out;
    const char lit[] = "string to copy";
    const StringView buff = VIEW_FROM_LIT(lit);

    TEST_MEM_CHECK(sb_copy,&out,&buff)

    TEST_ASSERT_EQ(sb_length(&out),sb_length(&buff),"lengths after copy should match")
    TEST_ASSERT_EQ(memcmp(out.data,buff.data,sb_length(&out)),0,"datashould match")

    sb_free(&out);

    TEST_SUCCES
}


CREATE_TEST(string_reset_tests,reset_empty) {
    StringMut buff;
    TEST_MEM_CHECK(sb_init,&buff,5)
    
    sb_reset(&buff);
    
    TEST_ASSERT_EQ(sb_length(&buff),0,"length should be 0!")
    TEST_ASSERT_EQ(memcmp(buff.data,"",sb_length(&buff)),0,"data should match!")
    TEST_ASSERT_EQ(buff.capacity,5,"capacity should equal!")

    sb_free(&buff);

    TEST_SUCCES
}

CREATE_TEST(string_reset_tests,reset_full) {
    StringMut buff;
    const char lit[] = "lit";
    STRING_MUT_FROM_LIT(buff,lit)

    sb_reset(&buff);

    TEST_ASSERT_EQ(sb_length(&buff),0,"length should be 0!")
    TEST_ASSERT_EQ(memcmp(buff.data,"",sb_length(&buff)),0,"data should match!")
    TEST_ASSERT_EQ(buff.capacity,LIT_LENGTH(lit),"capacity should equal!")

    sb_free(&buff);

    TEST_SUCCES
}



CREATE_TEST(string_edgecase_tests,sb_init_fail) {
    StringMut buff;
    bool ret = sb_init(&buff,INT64_MAX); // too much memory

    TEST_ASSERT_EQ(ret,false,"should fail to alloc!")
    TEST_ASSERT_EQ(sb_length(&buff),0,"length should be 0!")
    TEST_ASSERT_EQ(buff.capacity,0,"capacity should be 0!")
    TEST_ASSERT_EQ(buff.data,NULL,"data should be NULL!")

    TEST_SUCCES
}



CREATE_TEST(string_edgecase_tests,concat_overflow_mut) {
    StringMut out;
    const char lit1[] = "won't be concatenated!";
    const char lit2[] = "neither will this!";
    bool ret = __sb_concat_mut_impl(&out,lit1,SIZE_MAX - 55,lit2,70);

    TEST_ASSERT_EQ(ret,false,"Shouled be false!")
    
    TEST_SUCCES
}

CREATE_TEST(string_edgecase_tests,concat_alloc_fail_mut) {
    StringMut out;
    const char lit1[] = "won't be concatenated!";
    const char lit2[] = "neither will this!";
    bool ret = __sb_concat_mut_impl(&out,lit1,INT64_MAX-70,lit2,70);

    TEST_ASSERT_EQ(ret,false,"Shouled be false!")
    
    TEST_SUCCES
}


CREATE_TEST(string_edgecase_tests,concat_overflow) {
    String out;
    const char lit1[] = "won't be concatenated!";
    const char lit2[] = "neither will this!";
    bool ret = __sb_concat_impl(&out,lit1,SIZE_MAX - 55,lit2,70);

    TEST_ASSERT_EQ(ret,false,"Shouled be false!")

    TEST_SUCCES
}


CREATE_TEST(string_edgecase_tests,concat_alloc_fail) {
    String out;
    const char lit1[] = "won't be concatenated!";
    const char lit2[] = "neither will this!";
    bool ret = __sb_concat_impl(&out,lit1,INT64_MAX-70,lit2,70);

    TEST_ASSERT_EQ(ret,false,"Shouled be false!")

    TEST_SUCCES
}

CREATE_TEST(string_edgecase_tests,append_char_alloc_fail) {
    StringMut buff;
    const char lit[] = "contents of a string";
    STRING_MUT_FROM_LIT(buff,lit)
    buff.capacity = INT64_MAX / 4;
    buff.length = INT64_MAX / 4;

    bool ret = sb_append_char(&buff,'a');

    TEST_ASSERT_EQ(ret, false, "this should fail!")
    TEST_ASSERT_EQ(memcmp(buff.data,lit,LIT_LENGTH(lit)),0,"the data should be still the same!")

    sb_free(&buff);

    TEST_SUCCES
}


CREATE_TEST(string_edgecase_tests,append_string_overflow) {
    StringMut out;
    const char lit1[] = "Basic string data";
    const char lit2[] = "won't be appended";
    STRING_MUT_FROM_LIT(out,lit1)

    bool ret = __sb_append_string_impl(&out,lit2,SIZE_MAX - 10);
    size_t old_cap = out.capacity;

    TEST_ASSERT_EQ(ret,false,"Shouled be false!")
    TEST_ASSERT_EQ(memcmp(out.data,lit1,sb_length(&out)),0,"length should match the old length")    
    TEST_ASSERT_EQ(sb_length(&out),LIT_LENGTH(lit1),"data should match the old data")
    TEST_ASSERT_EQ(out.capacity,old_cap,"capacity should not be changed")

    sb_free(&out);

    TEST_SUCCES
}

CREATE_TEST(string_edgecase_tests,append_string_overflow2) {
    StringMut out;
    const char lit1[] = "Basic string data";
    const char lit2[] = "won't be appended";
    STRING_MUT_FROM_LIT(out,lit1)

    size_t bad_length = (UINT64_MAX / 3) * 2;
    bool ret = __sb_append_string_impl(&out,lit2,bad_length);
    size_t old_cap = out.capacity;
    TEST_ASSERT_EQ(ret,false,"Shouled be false!")
    TEST_ASSERT_EQ(memcmp(out.data,lit1,sb_length(&out)),0,"length should match the old length")    
    TEST_ASSERT_EQ(sb_length(&out),LIT_LENGTH(lit1),"data should match the old data")
    TEST_ASSERT_EQ(out.capacity,old_cap,"capacity should not be changed")

    sb_free(&out);

    TEST_SUCCES
}

CREATE_TEST(string_edgecase_tests,append_string_alloc_fail) {
    StringMut out;
    const char lit1[] = "Basic string data";
    const char lit2[] = "won't be appended";
    STRING_MUT_FROM_LIT(out,lit1)

    size_t bad_length = INT64_MAX/4;
    bool ret = __sb_append_string_impl(&out,lit2,bad_length);
    size_t old_cap = out.capacity;
    TEST_ASSERT_EQ(ret,false,"Shouled be false!")
    TEST_ASSERT_EQ(memcmp(out.data,lit1,sb_length(&out)),0,"length should match the old length")    
    TEST_ASSERT_EQ(sb_length(&out),LIT_LENGTH(lit1),"data should match the old data")
    TEST_ASSERT_EQ(out.capacity,old_cap,"capacity should not be changed")

    sb_free(&out);

    TEST_SUCCES
}


CREATE_TEST(string_edgecase_tests,copy_alloc_fail) {
    String out;
    const char lit[] = "This won't be copied!";
    bool ret = __sb_copy_impl(&out,lit,INT64_MAX);

    TEST_ASSERT_EQ(ret,false,"should return false!")

    TEST_SUCCES
}


CREATE_TEST(string_edgecase_tests,copy_alloc_fail_mut) {
    StringMut out;
    const char lit[] = "This won't be copied!";
    bool ret = __sb_copy_mut_impl(&out,lit,INT64_MAX);

    TEST_ASSERT_EQ(ret,false,"should return false!")

    TEST_SUCCES
}

CREATE_TEST(string_misc_tests,string_length) {
    StringMut buff1;
    String buff2;
    const char lit[] = "aaaa";
    const StringView view = VIEW_FROM_LIT(lit);
    STRING_MUT_FROM_LIT(buff1,lit)
    STRING_FROM_LIT(buff2,lit)

    TEST_ASSERT_EQ(sb_length(&buff1),LIT_LENGTH(lit),"lenghts should match!")
    TEST_ASSERT_EQ(sb_length(&buff2),LIT_LENGTH(lit),"lenghts should match!")
    TEST_ASSERT_EQ(sb_length(&view),LIT_LENGTH(lit),"lenghts should match!")

    sb_free(&buff1);
    sb_free(&buff2);
    
    TEST_SUCCES
}

CREATE_TEST(string_misc_tests,string_get_char_at) {
    StringMut buff1;
    String buff2;
    const char lit[] = "aaaa";
    const StringView view = VIEW_FROM_LIT(lit);
    STRING_MUT_FROM_LIT(buff1,lit)
    STRING_FROM_LIT(buff2,lit)

    TEST_ASSERT_EQ(sb_get_char_at(&buff1,1),lit[1],"characters should match!")
    TEST_ASSERT_EQ(sb_get_char_at(&buff2,1),lit[1],"characters should match!")
    TEST_ASSERT_EQ(sb_get_char_at(&view,1),lit[1],"characters should match!")

    sb_free(&buff1);
    sb_free(&buff2);
    
    TEST_SUCCES
}


CREATE_TEST(string_misc_tests,string_free1) {
    String buff;
    const char lit[] = "this is a string";
    STRING_FROM_LIT(buff,lit)

    sb_free(&buff);

    TEST_ASSERT_EQ(sb_length(&buff),0,"length should be 0!")
    TEST_ASSERT_EQ(buff.data,NULL,"data should be NULL!")

    TEST_SUCCES
}

CREATE_TEST(string_misc_tests,string_free2) {
    StringMut buff;
    const char lit[] = "this is a string";
    STRING_MUT_FROM_LIT(buff,lit)

    sb_free(&buff);

    TEST_ASSERT_EQ(sb_length(&buff),0,"length should be 0!")
    TEST_ASSERT_EQ(buff.data,NULL,"data should be NULL!")
    TEST_ASSERT_EQ(buff.capacity,0,"data should be NULL!")

    TEST_SUCCES
}
#!/usr/bin/python3
import enum
import os
import re
from collections import defaultdict
from typing import Generator

tests_type = defaultdict[str,list[str]]
pattern = re.compile(r"\bCREATE_TEST\(\s*(\w+)\s*,\s*(\w+)\s*\)\s*\{")

def get_test_names(tests_dic:tests_type) ->list[str]:
    output:list[str] = []
    for suit_name, tests in tests_dic.items():
        for test in tests:
            output.append(f"test_suit_{suit_name}_test_case_{test}")
    return output


def test_struct(test_func:str, test_name:str,depth:int)->str:
    return f"""{"\t"*depth}{"{"}{test_func}, "{test_name}"{"}"},\n"""

def gen_test_runner_file(test_dict:tests_type)->str:
    output =  """/*
**************************************
This file is generated, do not modify!
**************************************
*/
#include<stddef.h>
typedef int (*test_func_t)(void);

typedef struct {
	const test_func_t test_func;
	const char *test_name;
} test_t;\n\n
"""
    test_names = get_test_names(test_dict)
    for name in test_names:
        output += f"""int {name}(void);\n"""
    test_suit_count = len(test_dict.keys())
    output += """\n\n"""
    arr_size = max(len(val) for val in test_dict.values()) +1
    output += f"""static const test_t test_funcs[{test_suit_count}][{arr_size}] = {"{"}\n"""
    for suit_name, tests in test_dict.items():
        output += f"""\t{"{"} \n {test_struct("NULL",suit_name,1)}"""
        for test in tests:
            name = f"test_suit_{suit_name}_test_case_{test}"
            output += test_struct(name,name,2)
        output += f"""\t{"}"},\n"""
    output+= """};\n"""
    return output

def process_file(filepath:str,tests_dct:tests_type) -> None:
    with open(filepath,"r") as file:
        file_contents = file.read()
    test_funcs = pattern.findall(file_contents)
    for test_suit,test_case in test_funcs:
        tests_dct[test_suit].append(test_case)  

def main()->None:
    test_suites:tests_type = defaultdict(list)
    for dir,_,files in os.walk(os.path.abspath("./tests")):
        for file in [x for x in files if x.endswith(".c") and x != "test_runner.c"]:
            filepath = os.path.join(dir,file)
            process_file(filepath,test_suites)
    with open (os.path.abspath("./tests/test_runner.h"),"w") as f:
        f.writelines(gen_test_runner_file(test_suites))

if __name__ == '__main__':
    main()
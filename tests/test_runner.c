#include<stdio.h>
#include"test_runner.h"

const int num_suites = sizeof(test_funcs)/sizeof(test_funcs[0]);
const int max_test_count = sizeof(test_funcs[0])/sizeof(test_funcs[0][0]) -1;

static inline int is_test_index_valid(int test_suit_index, int test_index) {
    return (
    test_index <= max_test_count &&
    test_funcs[test_suit_index][test_index].test_name != NULL &&
    test_funcs[test_suit_index][test_index].test_func != NULL
);
}

int main() {

    int tests_pass_global = 0;
    int tests_fail_global = 0;
    int tests_err_global = 0;
    float num_tests_global = 0.0f;

    for (int test_suit_index = 0; test_suit_index < num_suites; test_suit_index++) {
        test_t test_suit = test_funcs[test_suit_index][0];
        printf("Statring test suite: %s\n",test_suit.test_name);
        int tests_pass = 0;
        int tests_fail = 0;
        int tests_err = 0;
        float num_tests = 0.0f;
        for (int test_index = 1;is_test_index_valid(test_suit_index,test_index) ;test_index++) {
            test_t test = test_funcs[test_suit_index][test_index];
            printf("Running test: %s\n",test.test_name);
            int ret = test.test_func();
            if (ret == 0 || ret == 3) {
                tests_pass++;
                printf("\033[1;32mPASSED\033[0m\n");
            } else if (ret == 2) {
                tests_err++;
                printf("\033[1;34mERROR\033[0m\n");
            } else {
                tests_fail++;
                printf("\033[1;31mFAILED\033[0m\n");
            }
            num_tests++;
        }
        tests_pass_global += tests_pass;
        tests_fail_global += tests_fail;
        tests_err_global += tests_err;
        num_tests_global += num_tests;
        printf("Finished running test suite: %s\n",test_suit.test_name);
        printf("Number of tests ran: %d\n",(int)num_tests);
        printf("Results: passed: %d, failed: %d, err: %d, pass rate: %.2f\n", tests_pass ,tests_fail, tests_err, tests_pass / num_tests);
        printf("Overall: %s\n", tests_fail == 0 ? "\033[1;32mPASSED\033[0m": "\033[1;31mFAILED\033[0m");
    }

    printf("\n\nFinished running all tests\n");
    printf("Number of suits ran: %d, number of tests ran: %d\n",num_suites,(int)num_tests_global);
    printf("Results: passed: %d, failed: %d, err: %d, pass rate: %.2f\n", tests_pass_global ,tests_fail_global, tests_err_global, tests_pass_global / num_tests_global);
    printf("Overall: %s\n", tests_fail_global == 0 ? "\033[1;32mPASSED\033[0m": "\033[1;31mFAILED\033[0m");
    return tests_fail_global;
}
/** @file test_common.h
    @brief Common testing framework for Substance Connector
    @author Galen Helfter - Allegorithmic
    @date 20190504
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_TESTING_TEST_COMMON_H
#define _SUBSTANCE_CONNECTOR_TESTING_TEST_COMMON_H

#include <stdlib.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

typedef unsigned int (*_connector_test_fp)(void);

void _print_error(const char *name, const char *error);

void _print_success(const char *name);

/* Main function, a few things must be defined first. For each file,
 * TEST_COUNT should be defined as the number of tests run in. There
 * should be tables _connector_test_names, _connector_test_errors and
 * _connector_test_functions containing the information for main to iterate
 * over them. Each should be of size TEST_COUNT. */
#define _CONNECTOR_TEST_MAIN \
    int main(int argc, char **argv)\
    {\
        int retcode = EXIT_SUCCESS;\
        unsigned int test_result = 0u;\
        (void) argc;\
        (void) argv;\
        \
        for (unsigned int i = 0u; i < TEST_COUNT; ++i)\
        {\
            test_result = _connector_test_functions[i]();\
            \
            if (test_result != 0u)\
            {\
                _print_error(_connector_test_names[i],\
                             _connector_test_errors[i][test_result - 1u]);\
                retcode = EXIT_FAILURE;\
            }\
            else\
            {\
                _print_success(_connector_test_names[i]);\
            }\
        }\
        \
        return retcode;\
    }

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_TESTING_TEST_COMMON_H */

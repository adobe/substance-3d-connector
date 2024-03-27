/** @file test_common.c
    @brief Common testing framework for Substance Connector
    @author Galen Helfter - Allegorithmic
    @date 20190510
    @copyright Allegorithmic. All rights reserved.
*/

#include <stdio.h>

#include <common/test_common.h>

void _print_error(const char *name, const char *error)
{
    fprintf(stderr, "Test %s failed with error: %s\n", name, error);
}

void _print_success(const char *name)
{
    fprintf(stderr, "Test %s succeeded.\n", name);
}

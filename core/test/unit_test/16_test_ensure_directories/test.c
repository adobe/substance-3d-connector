/** @file test.c
    @brief Testing that ensuring connection directories succeeds or fails
    @author Galen Helfter - Adobe
    @date 20190719
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/system/connectiondirectory.h>
#include <substance/connector/details/system/fileutils.h>
#include <substance/connector/details/system/pathstringdetails.h>
#include <substance/connector/details/system/pathutils.h>

#include <common/test_common.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TEST_COUNT 3u

/* begin connector_test_ensure_unix_directory block */

static const char * _connector_test_ensure_unix_directory_errors[] =
{
#if defined(SUBSTANCE_CONNECTOR_WIN32)
    "Default directory should be null on Windows systems",
    "Ensure unix directory should always fail on Windows"
#elif defined(SUBSTANCE_CONNECTOR_POSIX)
    "Default directory should not be null on Posix systems",
    "Ensure unix directory should ensure the directory exists"
#endif
};

static unsigned int _connector_test_ensure_unix_directory()
{
    unsigned int result = 0u;

#if defined(SUBSTANCE_CONNECTOR_WIN32)
    if (connector_get_default_unix_directory() != NULL)
    {
        result = 1u;
    }
    else if (connector_ensure_default_unix_directory() == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 2u;
    }
#elif defined(SUBSTANCE_CONNECTOR_POSIX)
    if (connector_get_default_unix_directory() == NULL)
    {
        result = 1u;
    }
    else if (connector_ensure_default_unix_directory() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 2u;
    }
#endif

    return result;
}

/* end connector_test_ensure_unix_directory block */

/* begin connector_test_ensure_tcp_directory block */

static const char * _connector_test_ensure_tcp_directory_errors[] =
{
    "Initializing the default directory failed",
    "Default directory should not be null",
    "Ensure tcp directory should ensure the directory exists",
    "Cleaning up the default directory should succeed"
};

static unsigned int _connector_test_ensure_tcp_directory()
{
    unsigned int result = 0u;

    if (connector_setup_default_tcp_directory() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 1u;
    }
    else if (connector_get_default_tcp_directory() == NULL)
    {
        result = 2u;
    }
    else if (connector_ensure_default_tcp_directory() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 3u;
    }
    else if (connector_cleanup_default_tcp_directory() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 4u;
    }

    return result;
}

/* end connector_test_ensure_tcp_directory block */

/* begin test_open_tcp_port_file block */

static unsigned int _connector_test_find_port_file(const connector_pathchar_t *port_string,
                                              const connector_pathchar_t *directory)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    connector_pathchar_t **elements = NULL;
    connector_pathchar_t **iterator = NULL;

    connector_list_directory(directory, NULL, NULL, &elements);

    if (elements != NULL)
    {
        iterator = elements;

        while (*iterator != NULL)
        {
            /* Iterate over the returned files to ensure everything is there */

            if (CONNECTOR_PATH_STRCMP(*iterator, port_string) == 0)
            {
                retcode = SUBSTANCE_CONNECTOR_SUCCESS;
                break;
            }

            iterator += 1u;
        }

        connector_clear_directory_list(&elements);

        /* Delete the file */
        const connector_pathchar_t *parts[] =
        {
            directory,
            port_string,
            NULL
        };
        connector_pathchar_t *path = NULL;
        connector_path_join((const connector_pathchar_t**) &parts, &path);

        connector_remove_file(path);

        connector_free(path);
    }

    return retcode;
}

static const char * _connector_test_open_tcp_port_file_errors[] =
{
    "Failed to initialize default tcp directory",
    "Ensure tcp directory should ensure the directory exists",
    "Creating the tcp port file should succeed",
    "Failed to find the port file in the default tcp directory",
    "Failed to clear default tcp directory",
};

static unsigned int _connector_test_open_tcp_port_file()
{
    unsigned int result = 0u;

    const unsigned int test_port = 40021u;
    const connector_pathchar_t * const test_port_string = CONNECTOR_PATH_STRING("40021");

    if (connector_setup_default_tcp_directory() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 1u;
    }
    else if (connector_ensure_default_tcp_directory() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 2u;
    }
    else if (connector_commit_open_tcp_port(test_port) != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 3u;
    }
    else if (_connector_test_find_port_file(test_port_string,
             connector_get_default_tcp_directory()) != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 4u;
    }
    else if (connector_cleanup_default_tcp_directory() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 5u;
    }

    return result;
}

/* end test_open_tcp_port_file block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_ensure_unix_directory",
    "test_ensure_tcp_directory",
    "test_open_tcp_port_file",
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_ensure_unix_directory_errors,
    _connector_test_ensure_tcp_directory_errors,
    _connector_test_open_tcp_port_file_errors,
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_ensure_unix_directory,
    _connector_test_ensure_tcp_directory,
    _connector_test_open_tcp_port_file,
};

/* Test main function */
_CONNECTOR_TEST_MAIN

/** @file test.c
    @brief Testing system file utilities
    @author Galen Helfter - Adobe
    @date 20190719
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/system/fileutils.h>
#include <substance/connector/details/system/pathutils.h>
#include <substance/connector/details/system/pathstringdetails.h>

#include <common/test_common.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TEST_COUNT 3u

/* begin connector_test_path_string block */

static const char * _connector_test_path_string_errors[] =
{
    "Size of the literal should be that of connector_pathchar_t",
};

static unsigned int _connector_test_path_string()
{
    unsigned int result = 0u;

    const connector_pathchar_t * const _path = CONNECTOR_PATH_STRING("test");

    if (sizeof(_path[0]) != sizeof(connector_pathchar_t))
    {
        result = 1u;
    }

    return result;
}

/* end connector_test_path_string block */

/* begin connector_test_create_directory block */

static const char * _connector_test_create_directory_errors[] =
{
    "Null path should return an error",
    "Failed to create a directory",
    "Created directory does not exist",
    "Failed to remove created directory",
    "Removed directory should now not exist"
};

static unsigned int _connector_test_create_directory()
{
    unsigned int result = 0u;

    const connector_pathchar_t * const _test_path =
        CONNECTOR_PATH_STRING("./substance_connector_test_dir");

    if (connector_make_directory(NULL) == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 1u;
    }
    else if (connector_make_directory(_test_path) != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 2u;
    }
    else if (connector_is_directory(_test_path) != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 3u;
    }
    else if (connector_remove_directory(_test_path) != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 4u;
    }
    else if (connector_is_directory(_test_path) == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 5u;
    }

    return result;
}

/* end connector_test_create_directory block */

/* begin connector_test_list_directory block */

#define _CONNECTOR_CREATE_FILES_BUFFER_SIZE 4096

static unsigned int _connector_test_create_files(const connector_pathchar_t *directory,
                                            const connector_pathchar_t **filenames)
{
    unsigned int result = SUBSTANCE_CONNECTOR_SUCCESS;

    const connector_pathchar_t **iterator = filenames;

    while (*iterator != NULL)
    {
        connector_pathchar_t *path = NULL;
        const connector_pathchar_t* parts[] =
        {
            directory,
            *iterator,
            NULL
        };

        unsigned int retcode = connector_path_join(parts, &path);

        if (retcode != SUBSTANCE_CONNECTOR_SUCCESS)
        {
            result = SUBSTANCE_CONNECTOR_ERROR;
            break;
        }

        /* Open and close the file to create it */
        FILE *fp = CONNECTOR_PATH_FOPEN(path, "w");

        if (fp == NULL)
        {
            result = SUBSTANCE_CONNECTOR_ERROR;
            break;
        }

        fclose(fp);
        fp = NULL;

        connector_free(path);
        path = NULL;

        /* Walk the pointer iterator forward */
        iterator += 1u;
    }

    return result;
}

static unsigned int _connector_test_remove_files(const connector_pathchar_t *directory,
                                            const connector_pathchar_t **filenames)
{
    unsigned int result = SUBSTANCE_CONNECTOR_SUCCESS;

    const connector_pathchar_t **iterator = filenames;

    while (*iterator != NULL)
    {
        connector_pathchar_t *path = NULL;
        const connector_pathchar_t* parts[] =
        {
            directory,
            *iterator,
            NULL
        };

        unsigned int retcode = connector_path_join(parts, &path);
        if (retcode != SUBSTANCE_CONNECTOR_SUCCESS)
        {
            result = SUBSTANCE_CONNECTOR_ERROR;
            break;
        }

        if (connector_remove_file(path) != SUBSTANCE_CONNECTOR_SUCCESS)
        {
            result = SUBSTANCE_CONNECTOR_ERROR;
            break;
        }

        connector_free(path);
        path = NULL;

        /* Walk the pointer iterator forward */
        iterator += 1u;
    }

    return result;
}

static unsigned int _connector_test_compare_files(connector_pathchar_t **returned_files,
                                             const connector_pathchar_t **originals)
{
    unsigned int result = SUBSTANCE_CONNECTOR_SUCCESS;

    const connector_pathchar_t **iterator = originals;

    while (*iterator != NULL)
    {
        /* Iterate over the returned files to ensure everything is there */
        connector_pathchar_t **returned_iterator = returned_files;
        unsigned int working_result = SUBSTANCE_CONNECTOR_ERROR;
        while (*returned_iterator != NULL)
        {
            if (CONNECTOR_PATH_STRCMP(*returned_iterator, *iterator) == 0)
            {
                working_result = SUBSTANCE_CONNECTOR_SUCCESS;
                break;
            }

            returned_iterator += 1u;
        }

        if (working_result != SUBSTANCE_CONNECTOR_SUCCESS)
        {
            result = SUBSTANCE_CONNECTOR_ERROR;
            break;
        }

        iterator += 1u;
    }

    return result;
}

static const char * _connector_test_list_directory_errors[] =
{
    "Failed to create directory",
    "Created directory does not exist",
    "Could not create files in created directory",
    "Failed to list the created directory",
    "Files returned are not what they should be",
    "Failed to remove all files in the directory",
    "Failed to remove the directoery"
};

static unsigned int _connector_test_list_directory()
{
    unsigned int result = 0u;

    const connector_pathchar_t * const _test_path =
        CONNECTOR_PATH_STRING("./substance_connector_test_dir");

    const connector_pathchar_t * _filenames[] =
    {
        CONNECTOR_PATH_STRING("file0.txt"),
        CONNECTOR_PATH_STRING("file1.txt"),
        CONNECTOR_PATH_STRING("file2.txt"),
        CONNECTOR_PATH_STRING("file3.txt"),
        NULL /* Sentinal to denote the end */
    };

    connector_pathchar_t **elements = NULL;

    if (connector_make_directory(_test_path) != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 1u;
    }
    else if (connector_is_directory(_test_path) != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 2u;
    }
    else if (_connector_test_create_files(_test_path, _filenames)
             != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 3u;
    }
    else if (connector_list_directory(_test_path, NULL, NULL, &elements)
             != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 4u;
    }
    else if (_connector_test_compare_files(elements, _filenames)
             != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 5u;
    }
    else if (_connector_test_remove_files(_test_path, _filenames)
             != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 6u;
    }
    else if (connector_remove_directory(_test_path) != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 7u;
    }

    connector_clear_directory_list(&elements);

    return result;
}

/* end connector_test_list_directory block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_path_string",
    "test_create_directory",
    "test_list_directory",
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_path_string_errors,
    _connector_test_create_directory_errors,
    _connector_test_list_directory_errors,
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_path_string,
    _connector_test_create_directory,
    _connector_test_list_directory,
};

/* Test main function */
_CONNECTOR_TEST_MAIN

/** @file fileutils.c
    @brief Contains shared file and directory operations
    @author Galen Helfter - Adobe
    @date 20190719
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/details/system/fileutils.h>
#include <substance/connector/common.h>

#include <string.h>
#include <stdio.h>

/* Both Posix and Windows have both of these headers available */
#if defined(SUBSTANCE_CONNECTOR_POSIX)
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif

/* Needs to be included on Linux and possibly other systems before sys/stat.h
 * in order to set the internal system file type constants */
#include <fcntl.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/system/pathstringdetails.h>
#include <substance/connector/details/system/pathutils.h>

#ifndef CONNECTOR_DEFAULT_FILE_LIST_SIZE
#define CONNECTOR_DEFAULT_FILE_LIST_SIZE 16
#endif

#if defined(SUBSTANCE_CONNECTOR_POSIX)
#include <dirent.h>
#include <unistd.h>
#define CONNECTOR_STAT_IFDIR S_IFDIR
#define CONNECTOR_STAT_IFREG S_IFREG
#define CONNECTOR_STAT stat
#define CONNECTOR_UNLINK unlink
#define CONNECTOR_RMDIR rmdir
typedef struct stat connector_stat_t;
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#include <windows.h>
#include <direct.h>
#define CONNECTOR_STAT_IFDIR _S_IFDIR
#define CONNECTOR_STAT_IFREG _S_IFREG
#define CONNECTOR_STAT _wstat
#define CONNECTOR_UNLINK _wunlink
#define CONNECTOR_RMDIR _wrmdir
typedef struct _stat connector_stat_t;
#endif

static unsigned int default_filter(connector_pathchar_t *path, const void *data)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    (void)data;

    if (path != NULL)
    {
        /* Filter out hidden files */
        if (path[0] != '.')
        {
            retcode = SUBSTANCE_CONNECTOR_SUCCESS;
        }
    }

    return retcode;
}

static unsigned int test_file_type(const connector_pathchar_t *path, int flag)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    connector_stat_t stat_buffer;

    if (path != NULL)
    {
        if (CONNECTOR_STAT(path, &stat_buffer) == 0)
        {
            if (stat_buffer.st_mode & flag)
            {
                retcode = SUBSTANCE_CONNECTOR_SUCCESS;
            }
        }
    }

    return retcode;
}

static size_t append_file_list(const connector_pathchar_t *path,
                               connector_pathchar_t ***elements,
                               size_t *capacity,
                               size_t size)
{
    connector_pathchar_t **array = NULL;

    if (*elements == NULL)
    {
        *capacity = CONNECTOR_DEFAULT_FILE_LIST_SIZE;

        *elements = connector_allocate(sizeof(**elements) * *capacity);

        memset(*elements, 0x00, sizeof(**elements) * *capacity);
    }
    else if ((size + 2u) == *capacity)
    {
        /* Resize the array */
        array = connector_allocate(sizeof(*array) * *capacity * 2u);

        memset(array, 0x00, sizeof(*array) * *capacity);

        memcpy(array, *elements, sizeof(*array) * *capacity);

        connector_free(*elements);
        *elements = array;

        *capacity *= 2u;
    }

    (*elements)[size] = connector_path_strdup(path);

    return size + 1u;
}

unsigned int connector_list_directory(const connector_pathchar_t *path,
                                 connector_list_filter_fp filter_op,
                                 const void *data,
                                 connector_pathchar_t ***elements)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    size_t element_count = 0u;
    size_t capacity = 0u;

#if defined(SUBSTANCE_CONNECTOR_POSIX)
    DIR *directory = NULL;
    struct dirent *entry = NULL;
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
    connector_pathchar_t *path_wildcard = NULL;
    size_t path_length = 0u;
    HANDLE file_handle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW entry;
#endif

    /* Use a trivial filter if the given one is NULL. */
    if (filter_op == NULL)
    {
        filter_op = default_filter;
    }

    if (path != NULL && elements != NULL)
    {
#if defined(SUBSTANCE_CONNECTOR_POSIX)
        directory = opendir(path);

        if (directory != NULL)
        {
            while ((entry = readdir(directory)) != NULL)
            {
                if (filter_op(entry->d_name, data) == SUBSTANCE_CONNECTOR_SUCCESS)
                {
                    /* Element is accepted, append to the list */
                    element_count = append_file_list(entry->d_name, elements,
                                                     &capacity, element_count);
                }
            }

            retcode = SUBSTANCE_CONNECTOR_SUCCESS;
        }
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
        path_length = CONNECTOR_PATH_STRLEN(path) + 3u;
        path_wildcard = connector_allocate(sizeof(path[0]) * (path_length));
        CONNECTOR_PATH_SNPRINTF(path_wildcard, path_length,
                           CONNECTOR_PATH_STRING("%s\\*"), path);
        file_handle = FindFirstFileW(path_wildcard, &entry);

        if (file_handle != INVALID_HANDLE_VALUE)
        {
            /* Use a do-while loop, as the first entry was acquired using
             * the FindFirstFile function to establish the directory walk */
            do
            {
                if (filter_op(entry.cFileName, data) == SUBSTANCE_CONNECTOR_SUCCESS)
                {
                    /* Element is accepted, append to the list */
                    element_count = append_file_list(entry.cFileName, elements,
                                                     &capacity, element_count);
                }


            } while (FindNextFileW(file_handle, &entry) != 0);

            FindClose(file_handle);

            retcode = SUBSTANCE_CONNECTOR_SUCCESS;
        }

        connector_free(path_wildcard);
#endif
    }

    return retcode;
}

void connector_clear_directory_list(connector_pathchar_t ***elements)
{
    connector_pathchar_t **iterator = NULL;

    if (elements != NULL)
    {
        iterator = *elements;

        if (iterator != NULL)
        {
            while (*iterator != NULL)
            {
                connector_free(*iterator);

                *iterator = NULL;

                iterator += 1u;
            }

            connector_free(*elements);

            /* Clear the pointer */
            *elements = NULL;
        }
    }
}

unsigned int connector_is_directory(const connector_pathchar_t *path)
{
    return test_file_type(path, CONNECTOR_STAT_IFDIR);
}

unsigned int connector_is_unix_socket(const connector_pathchar_t *path)
{
#if defined(SUBSTANCE_CONNECTOR_WIN32)
    return SUBSTANCE_CONNECTOR_ERROR;
#elif defined(SUBSTANCE_CONNECTOR_POSIX)
    return test_file_type(path, S_IFSOCK);
#endif
}

unsigned int connector_is_file(const connector_pathchar_t *path)
{
    return test_file_type(path, CONNECTOR_STAT_IFREG);
}

unsigned int connector_make_directory(const connector_pathchar_t *path)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    int result = 0;

    if (path != NULL)
    {
#if defined(SUBSTANCE_CONNECTOR_WIN32)
        result = _wmkdir(path);
#elif defined(SUBSTANCE_CONNECTOR_POSIX)
        result = mkdir(path, S_IRWXO);

        result = result || chmod(path, 0777);
#endif

        if (result == 0)
        {
            retcode = SUBSTANCE_CONNECTOR_SUCCESS;
        }
    }

    return retcode;
}

unsigned int connector_remove_directory(const connector_pathchar_t *path)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (CONNECTOR_RMDIR(path) == 0)
    {
        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}

unsigned int connector_remove_file(const connector_pathchar_t *path)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (CONNECTOR_UNLINK(path) == 0)
    {
        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}

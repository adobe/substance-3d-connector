/** @file string_utils.c
    @brief Contains internal string utility functions
    @author Galen Helfter - Allegorithmic
    @date 20190107
    @copyright Allegorithmic. All rights reserved.
*/

#include <stddef.h>
#include <string.h>

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/string_utils.h>

void* connector_memrchr(const void *buffer, int c, size_t length)
{
    void *result = NULL;
    const char *index = NULL;
    const char *beginning = buffer;

    for (index = beginning + length - 1; index >= beginning; --index)
    {
        if (*index == c)
        {
            result = (void*) index;
            break;
        }
    }

    return result;
}

char* connector_strdup(const char *s)
{
    char *result = NULL;
    size_t length = 0u;

    if (s != NULL)
    {
        length = strlen(s) + 1;
        result = connector_allocate(length);
        memcpy(result, s, length);
    }

    return result;
}

char* connector_strndup(const char *s, size_t n)
{
    char *result = NULL;
    size_t length = 0u;

    if (s != NULL)
    {
        length = strlen(s) + 1;

        if (n <= length)
        {
            result = connector_allocate(n);
            memcpy(result, s, n);
        }
        else
        {
            /* Ensure the string is null terminated */
            result = connector_allocate(n + 1);
            memcpy(result, s, n);
            *(result + n) = '\0';
        }
    }

    return result;
}

size_t connector_chomp(char *s, char delim)
{
    long i = ((long) strlen(s)) - 1;
    size_t amount = 0u;

    while (i >= 0 && s[i] == delim)
    {
        s[i] = '\0';
        amount += 1u;
        i -= 1;
    }

    return amount;
}

unsigned int connector_untrusted_strlen(const char *buffer, size_t buffer_length,
                                   size_t *length)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    const char *found = connector_memrchr(buffer, '\0', buffer_length);

    if (found != NULL && length != NULL)
    {
        *length = (found - buffer);
        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}

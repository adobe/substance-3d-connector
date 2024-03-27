/** @file pathutils.c
    @brief Contains shared file and directory operations
    @author Galen Helfter - Adobe
    @date 20190722
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/details/system/pathutils.h>

#include <string.h>
#include <stdio.h>

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/string_utils.h>
#include <substance/connector/details/system/pathstringdetails.h>

unsigned int connector_path_join(const connector_pathchar_t **parts,
                            connector_pathchar_t **result)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    const connector_pathchar_t **iterator = parts;
    connector_pathchar_t *location = NULL;

    /* Store the needed size for the new string as well as the number of
     * entries found in the parts array */
    size_t size = 0u;
    size_t count = 0u;
    size_t i = 0u;
    size_t length = 0u;

    if (parts != NULL && result != NULL)
    {
        while (*iterator != NULL)
        {
            size += CONNECTOR_PATH_STRLEN(*iterator);

            /* Walk the iterator pointer forward until a NULL pointer is hit. */
            count += 1u;
            iterator += 1u;
        }

        /* At least one valid string must be joined for success */
        if (count > 0)
        {
            /* Add space for the path delimeters */
            size += (count - 1u);

            *result = connector_allocate(sizeof(connector_pathchar_t) * (size + 1u));

            location = *result;

            for (i = 0u; i < count; ++i)
            {
                length = CONNECTOR_PATH_STRLEN(parts[i]);
                memcpy(location, parts[i], sizeof(connector_pathchar_t) * length);
                location += length;

                if ((i + 1u) == count)
                {
                    *location = '\0'; /* Add the NULL terminator at the end */
                }
                else
                {
                    *location = CONNECTOR_PATH_DELIM; /* Add path separator */
                }

                location += 1u;
            }

            retcode = SUBSTANCE_CONNECTOR_SUCCESS;
        }
    }

    return retcode;
}

connector_pathchar_t* connector_path_strdup(const connector_pathchar_t *str)
{
#if defined(SUBSTANCE_CONNECTOR_WIN32)
    connector_pathchar_t *result = NULL;
    size_t length = 0u;

    if (str != NULL)
    {
        length = CONNECTOR_PATH_STRLEN(str) + 1;
        result = connector_allocate(sizeof(connector_pathchar_t) * length);
        memcpy(result, str, sizeof(connector_pathchar_t) * length);
    }

    return result;
#elif defined(SUBSTANCE_CONNECTOR_POSIX)
    return connector_strdup(str);
#endif
}


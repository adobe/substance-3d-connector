/** @file autoconnect.c
    @brief Contains functions for automatically connecting to other instances
    @author Galen Helfter - Adobe
    @date 20190718
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/details/network/autoconnect.h>

#include <stddef.h>
#include <sys/types.h>

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/context_queue.h>
#include <substance/connector/details/context_struct.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/system/connectiondirectory.h>
#include <substance/connector/details/system/fileutils.h>
#include <substance/connector/details/system/pathstringdetails.h>
#include <substance/connector/details/system/pathutils.h>

#if defined(SUBSTANCE_CONNECTOR_POSIX)
#include <sys/socket.h>
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#include <winsock2.h>
#endif

typedef const connector_pathchar_t* (*get_default_path_fp)();
typedef unsigned int (*test_file_type_fp)(const connector_pathchar_t*);
typedef unsigned int (*connect_path_fp)(const connector_pathchar_t *path,
                                        unsigned int *context);

static unsigned int broadcast_connect_impl(get_default_path_fp get_default_path,
                                           test_file_type_fp test_file_type,
                                           connect_path_fp connect_path)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    connector_pathchar_t **elements = NULL;
    connector_pathchar_t *filepath = NULL;
    connector_pathchar_t **iterator = NULL;

    const connector_pathchar_t * parts[3];

    const connector_pathchar_t *socket_dir = get_default_path();
    unsigned int context = 0u;

    if (connector_is_directory(socket_dir) == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        /* Acquire the list of paths in the expected directory */
        if (connector_list_directory(socket_dir, NULL, NULL, &elements)
            == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            if (elements != NULL)
            {
                iterator = elements;

                while (*iterator != NULL)
                {
                    parts[0] = socket_dir;
                    parts[1] = *iterator;
                    parts[2] = NULL;       /* Add sentinel to denote length */

                    connector_path_join((const connector_pathchar_t **) &parts,
                                   &filepath);

                    /* Now that the list is there, combine with the directory
                     * and proceed to attempt to connect to it */
                    if (test_file_type(filepath) == SUBSTANCE_CONNECTOR_SUCCESS)
                    {
                        retcode = connect_path(filepath, &context);

                        if (retcode != SUBSTANCE_CONNECTOR_SUCCESS)
                        {
                            /* Remove the file  */
                            connector_remove_file(filepath);
                        }
                    }

                    connector_free(filepath);

                    iterator += 1u;
                }


                /* Set the return code to true, as any previous errors were
                 * successfully handled. */
                retcode = SUBSTANCE_CONNECTOR_SUCCESS;

                connector_clear_directory_list(&elements);
            }
        }
    }

    return retcode;
}

static unsigned int test_tcp_validity(unsigned int port)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    unsigned int i = 0u;

    if (port <= 65535u)
    {
        retcode = SUBSTANCE_CONNECTOR_SUCCESS;

        for (i = 0u; i < SUBSTANCE_CONNECTOR_CONTEXT_COUNT; ++i)
        {

            if ((connector_context_state(i) != SUBSTANCE_CONNECTOR_CONN_CLOSED)
                && connector_context_type(i) == SUBSTANCE_CONNECTOR_COMM_TCP)
            {
                if (port == connector_context_port(i))
                {
                    retcode = SUBSTANCE_CONNECTOR_ERROR;
                    break;
                }
            }
        }
    }

    return retcode;
}

static unsigned int connect_tcp_path_impl(const connector_pathchar_t *path,
                                          unsigned int *context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    int port = 0;

    /* Pull the port number out of the string */
    const connector_pathchar_t *filename = CONNECTOR_PATH_STRRCHR(path, CONNECTOR_PATH_DELIM);

    if (*filename != '\0')
    {
        filename += 1u;
    }

    port = CONNECTOR_PATH_ATOI(filename);

    if (test_tcp_validity((unsigned int) port) == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        /* Test whether that port has already been connected to */
        retcode = connector_context_connect_tcp((unsigned int) port, context);
    }
    else
    {
        /* Filtered out ports should be ignored, but are not erroneous */
        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}

unsigned int connector_broadcast_connect_unix(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_UNSUPPORTED;

#if defined(SUBSTANCE_CONNECTOR_POSIX)
    /* Unix sockets are only currently implemented for POSIX-compliant
     * systems */
    retcode = broadcast_connect_impl(connector_get_default_unix_directory,
                                     connector_is_unix_socket,
                                     connector_context_connect_unix);
#endif

    return retcode;
}

unsigned int connector_broadcast_connect_tcp(void)
{
    return broadcast_connect_impl(connector_get_default_tcp_directory,
                                  connector_is_file,
                                  connect_tcp_path_impl);
}

unsigned int connector_broadcast_connect_default(void)
{
#if defined(SUBSTANCE_CONNECTOR_POSIX)
    return connector_broadcast_connect_unix();
#else
    return connector_broadcast_connect_tcp();
#endif
}

unsigned int connector_open_default_tcp(unsigned int *context)
{
    return connector_context_open_tcp(0u, context);
}

unsigned int connector_open_default_unix(unsigned int *context)
{
    return connector_context_open_unix(NULL, context);
}

unsigned int connector_open_default_connection(unsigned int *context)
{
#if defined(SUBSTANCE_CONNECTOR_POSIX)
    return connector_open_default_unix(context);
#else
    return connector_open_default_tcp(context);
#endif
}

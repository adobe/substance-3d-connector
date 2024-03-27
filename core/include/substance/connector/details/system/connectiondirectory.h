/** @file connectiondirectory.h
    @brief Contains the default directories for the Connector sockets
    @author Galen Helfter - Adobe
    @date 20190718
    @copyright Adobe. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_DETAILS_SYSTEM_CONNECTIONDIRECTORY_H
#define _SUBSTANCE_CONNECTOR_DETAILS_SYSTEM_CONNECTIONDIRECTORY_H

#include <substance/connector/details/system/pathtype.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* Performs any initialization steps needed to acquire the default tcp
 * directory. Returns an errorcode representing success or failure. */
unsigned int connector_setup_default_tcp_directory(void);

/* Performs any cleanup necessary to clear the memory for the default
 * tcp directory. Returns an errorcode representing success or failure. */
unsigned int connector_cleanup_default_tcp_directory(void);

/* Returns a constant string representing the default directory to search for
 * unix sockets. */
const connector_pathchar_t* connector_get_default_unix_directory(void);

/* Returns a constant string representing the default directory to search for
 * tcp socket file proxies. */
const connector_pathchar_t* connector_get_default_tcp_directory(void);

/* Ensures that the default unix directory exists, by attempting to create
 * it and then returning the result of whether the directory exists. */
unsigned int connector_ensure_default_unix_directory(void);

/* Ensures that the default tcp directory exists, by attempting to create
 * it and then returning the result of whether the directory exists. */
unsigned int connector_ensure_default_tcp_directory(void);

/* Commits the newly created tcp port to the default tcp directory. Returns
 * an errorcode denoting the success of this. */
unsigned int connector_commit_open_tcp_port(unsigned int port);

/* Removes the file with the open tcp port. Returns an errorcode denoting
 * the success of this. */
unsigned int connector_remove_open_tcp_port(unsigned int port);

/* Builds a default Unix socket path, using the default Unix socket directory.
 * Returns a dynamically allocated pointer to this string, which can be freed
 * using connector_free found in memory.h */
char* connector_build_default_unix_path(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_DETAILS_SYSTEM_CONNECTIONDIRECTORY_H */

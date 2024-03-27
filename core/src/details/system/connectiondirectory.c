/** @file connectiondirectory.c
    @brief Contains the default directories for the Connector sockets
    @author Galen Helfter - Adobe
    @date 20190718
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/details/system/connectiondirectory.h>

#include <stddef.h>

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/system/fileutils.h>
#include <substance/connector/details/system/pathtype.h>
#include <substance/connector/details/system/pathutils.h>
#include <substance/connector/details/system/pathstringdetails.h>

#ifndef CONNECTOR_DEFAULT_UNIX_DIRECTORY_BASE
#define CONNECTOR_DEFAULT_UNIX_DIRECTORY_BASE "/tmp"
#endif

#ifndef CONNECTOR_DEFAULT_UNIX_SUBDIRECTORY
#define CONNECTOR_DEFAULT_UNIX_SUBDIRECTORY "substanceconnectoropenunix"
#endif

#ifndef CONNECTOR_DEFAULT_TCP_DIRECTORY_BASE
#define CONNECTOR_DEFAULT_TCP_DIRECTORY_BASE CONNECTOR_PATH_STRING("/tmp")
#endif

#ifndef CONNECTOR_DEFAULT_TCP_SUBDIRECTORY
#define CONNECTOR_DEFAULT_TCP_SUBDIRECTORY CONNECTOR_PATH_STRING("substanceconnectoropentcp")
#endif

/* C will concatenate adjacent string literals into a single string */
#define CONNECTOR_DEFAULT_UNIX_DIRECTORY CONNECTOR_DEFAULT_UNIX_DIRECTORY_BASE "/"\
                                    CONNECTOR_DEFAULT_UNIX_SUBDIRECTORY

#define CONNECTOR_DEFAULT_TCP_DIRECTORY CONNECTOR_DEFAULT_TCP_DIRECTORY_BASE "/"\
                                   CONNECTOR_DEFAULT_TCP_SUBDIRECTORY

#define CONNECTOR_PORT_SPRINTF_SIZE 16u

#if defined(SUBSTANCE_CONNECTOR_WIN32)
#include <shlobj.h>
static connector_pathchar_t *default_tcp_directory = NULL;
#endif

static connector_pathchar_t* create_tcp_filepath(unsigned int port)
{
    connector_pathchar_t *result = NULL;

    /* If ports are encoded in host byte order, then the maximum size is
     * 65535, and this buffer size will always be large enough. */
    connector_pathchar_t buffer[CONNECTOR_PORT_SPRINTF_SIZE];
    const connector_pathchar_t* parts[3];

    CONNECTOR_PATH_SNPRINTF(buffer, CONNECTOR_PORT_SPRINTF_SIZE,
                       CONNECTOR_PATH_STRING("%u"), port);

    parts[0] = connector_get_default_tcp_directory();
    parts[1] = buffer;
    parts[2] = NULL;

    connector_path_join(parts, &result);

    return result;
}

unsigned int connector_setup_default_tcp_directory(void)
{
#if defined(SUBSTANCE_CONNECTOR_WIN32)
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
	connector_pathchar_t *appdata_folder = NULL;
    connector_pathchar_t *parts[3];
	HRESULT result = S_OK;

    memset(parts, 0x00, sizeof(parts[0]) * 3u);

    /* Acquire the directory for the AppData user folder */
    result = SHGetKnownFolderPath(&FOLDERID_RoamingAppData, KF_FLAG_DEFAULT,
                                  NULL, &appdata_folder);

    if (result == S_OK && appdata_folder != NULL)
    {
        parts[0] = appdata_folder;
        parts[1] = CONNECTOR_DEFAULT_TCP_SUBDIRECTORY;
        parts[2] = NULL;

        /* Concatenate the paths */
        retcode = connector_path_join(parts, &default_tcp_directory);

        /* Free the folder path allocated by windows */
        CoTaskMemFree(appdata_folder);
        appdata_folder = NULL;
    }

    return retcode;
#elif defined(SUBSTANCE_CONNECTOR_POSIX)
    return SUBSTANCE_CONNECTOR_SUCCESS;
#endif
}

unsigned int connector_cleanup_default_tcp_directory(void)
{
#if defined(SUBSTANCE_CONNECTOR_WIN32)
    connector_free(default_tcp_directory);
    default_tcp_directory = NULL;
#endif
    return SUBSTANCE_CONNECTOR_SUCCESS;
}

const connector_pathchar_t* connector_get_default_unix_directory(void)
{
#if defined(SUBSTANCE_CONNECTOR_POSIX)
    return CONNECTOR_DEFAULT_UNIX_DIRECTORY;
#else
    return NULL;
#endif
}

const connector_pathchar_t* connector_get_default_tcp_directory(void)
{
#if defined(SUBSTANCE_CONNECTOR_POSIX)
    return CONNECTOR_DEFAULT_TCP_DIRECTORY;
#else
    return default_tcp_directory;
#endif
}

unsigned int connector_ensure_default_unix_directory(void)
{
    const connector_pathchar_t *path = connector_get_default_unix_directory();

    connector_make_directory(path);

    return connector_is_directory(path);
}

unsigned int connector_ensure_default_tcp_directory(void)
{
    const connector_pathchar_t *path = connector_get_default_tcp_directory();

    connector_make_directory(path);

    return connector_is_directory(path);
}

unsigned int connector_commit_open_tcp_port(unsigned int port)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    FILE *fp = NULL;

    connector_pathchar_t *location = create_tcp_filepath(port);

    fp = CONNECTOR_PATH_FOPEN(location, CONNECTOR_PATH_STRING("w"));

    if (fp != NULL)
    {
        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    fclose(fp);

    connector_free(location);

    return retcode;
}

unsigned int connector_remove_open_tcp_port(unsigned int port)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    /* Acquire the proper path to the TCP port */
    connector_pathchar_t *location = create_tcp_filepath(port);

    /* Delete the file if it exists */
    if (connector_is_file(location) == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        retcode = connector_remove_file(location);
    }

    connector_free(location);

    return retcode;
}

char* connector_build_default_unix_path(void)
{
    return NULL;
}

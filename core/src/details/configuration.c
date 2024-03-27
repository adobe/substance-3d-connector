/** @file configuration.c
    @brief Contains the functions for reading a configuration and the
           structures to store them.
    @author Galen Helfter - Allegorithmic
    @date 20181019
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/details/configuration.h>

#include <stdio.h>

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/string_utils.h>

static char *application_name = NULL;

unsigned int connector_parse_configuration_file(const char *filepath)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    FILE *file = NULL;
    long length = 0l;
    char *buffer = NULL;

    if (filepath != NULL)
    {
        /* Open the filepath and read into the buffer */
        file = fopen(filepath, "r");

        if (file != NULL)
        {
            fseek(file, 0, SEEK_END);
            length = ftell(file);
            fseek(file, 0, SEEK_SET);

            buffer = connector_allocate((length + 1l) * sizeof(char));
            fread(buffer, sizeof(char), length, file);

            if (buffer != NULL)
            {
                retcode = connector_parse_configuration_buffer(buffer);
                connector_free(buffer);
                buffer = NULL;
            }
        }
    }

    return retcode;
}

unsigned int connector_parse_configuration_buffer(const char *buffer)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (buffer != NULL)
    {
        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}

unsigned int connector_set_application_name(const char *name)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    connector_free(application_name);
    application_name = connector_strdup(name);

    if (application_name != NULL)
    {
        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}

const char* connector_get_application_name(void)
{
    return application_name;
}

void connector_clear_application_name(void)
{
    connector_free(application_name);

    application_name = NULL;
}

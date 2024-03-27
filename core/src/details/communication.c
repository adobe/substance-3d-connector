/** @file communication.c
    @brief Contains communication thread functionality
    @author Galen Helfter - Allegorithmic
    @date 20181217
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/details/communication.h>

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/available_queue.h>
#include <substance/connector/details/threadimpl/read_threads.h>
#include <substance/connector/details/threadimpl/write_threads.h>

static unsigned int connector_terminate_comm(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    unsigned int read_retcode = SUBSTANCE_CONNECTOR_ERROR;

    retcode = connector_shutdown_write_threads();

    read_retcode = connector_shutdown_read_threads();

    if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        retcode = read_retcode;
    }

    return retcode;
}

unsigned int connector_flag_read(void)
{
    return connector_flag_read_impl();
}

unsigned int connector_flag_write(void)
{
    return connector_flag_write_impl();
}

unsigned int connector_init_comm_subsystem(void)
{
    /* Initialize available connection queue */
    connector_available_queue_init();

    connector_init_write_threads();

    connector_init_read_threads();

    return SUBSTANCE_CONNECTOR_SUCCESS;
}

unsigned int connector_shutdown_comm_subsystem(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    retcode = connector_terminate_comm();

    /* Destroy available connection queue */
    retcode = connector_available_queue_shutdown();

    return retcode;
}

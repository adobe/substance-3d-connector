/** @file available_queue.c
    @brief Contains all unassigned connections that can be acquired
           by the read threads.
    @author Galen Helfter - Allegorithmic
    @date 20190108
    @copyright Allegorithmic. All rights reserved.
*/

#include <stddef.h>

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/context_queue.h> /* Defines context count */
#include <substance/connector/details/uint_queue.h>

#ifndef SUBSTANCE_CONNECTOR_AVAILABLE_RETRIES
#define SUBSTANCE_CONNECTOR_AVAILABLE_RETRIES 5u
#endif /* SUBSTANCE_CONNECTOR_AVAILABLE_RETRIES */

static connector_uint_queue_t *connection_queue = NULL;

unsigned int connector_available_queue_init(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (connection_queue == NULL)
    {
        connection_queue = connector_uint_queue_create(SUBSTANCE_CONNECTOR_CONTEXT_COUNT);

        retcode = (connection_queue == NULL) ? retcode : SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}

unsigned int connector_available_queue_shutdown(void)
{
    unsigned int retcode = connector_uint_queue_destroy(connection_queue);

    if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        connection_queue = NULL;
    }

    return retcode;
}

unsigned int connector_available_queue_push(unsigned int context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (connection_queue != NULL)
    {
        retcode = connector_uint_queue_push(connection_queue, context);
    }

    return retcode;
}

unsigned int connector_available_queue_pop(unsigned int *context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    unsigned int internal_context = 0u;
    unsigned int i = 0u;

    if (connection_queue != NULL && context != NULL)
    {
        /* Spin for the given amount of retries, attempting to acquire a
         * context. */
        for (i = 0u; i < SUBSTANCE_CONNECTOR_AVAILABLE_RETRIES; ++i)
        {
            retcode = connector_uint_queue_pop(connection_queue, &internal_context);
            if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
            {
                *context = internal_context;
                break;
            }
        }
    }

    return retcode;
}

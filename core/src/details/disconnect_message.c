/** @file disconnect_message.c
    @brief Handles error disconnects by propagating a disconnect message up
    @author Galen Helfter - Adobe
    @date 20200806
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/details/disconnect_message.h>

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/internal_uuids.h>
#include <substance/connector/details/callbacks.h>
#include <substance/connector/details/context_queue.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/string_utils.h>

unsigned int connector_handle_error_disconnect(unsigned int context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    /* Duplicate the application name to ensure it persists through
     * the callback */
    char* name = connector_strdup(connector_context_get_application_name(context));

    if (name != NULL)
    {
        /* Dispatch directly from read thread - special message */
        connector_notify_trampolines(context, &connector_internal_connection_closed_uuid,
                                name);

        /* Free contents of application name after finished */
        connector_free(name);
        name = NULL;

        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}


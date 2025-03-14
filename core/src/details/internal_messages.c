/** @file internal_messages.c
    @brief Contains internal message operations
    @author Galen Helfter - Allegorithmic
    @date 20190206
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/errorcodes.h>
#include <substance/connector/types.h>
#include <substance/connector/details/callbacks.h>
#include <substance/connector/details/communication.h>
#include <substance/connector/details/configuration.h>
#include <substance/connector/details/context_queue.h>
#include <substance/connector/details/internal_messages.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/message.h>
#include <substance/connector/details/message_queue.h>
#include <substance/connector/details/uuid_utils.h>
#include <substance/connector/details/internal_uuids.h>

#include <stdlib.h>
#include <string.h>

/* Inside this file, the trampoline function signature is used as an
 * actual callback. */
typedef substance_connector_trampoline_fp connector_callback_fp;

/* Declare internal callbacks */
static void internal_handshake_callback(unsigned int context,
                                        const substance_connector_uuid_t *type,
                                        const char *message);

/* The number of internal messages should be fixed at compile time, as it ends
 * up as a fixed-size array. */
#define CONNECTOR_MAX_INTERN_CALLBACKS 0x01u

typedef struct _intern_callback
{
    const substance_connector_uuid_t* const key;
    connector_callback_fp callback;
} intern_callback_t;

/* Internal callback array, in sorted order by the string key. If a new
 * callback is added, it must maintain the sorted array, to make it so that
 * the standard C bsearch call works properly. */
static const intern_callback_t internal_messages[CONNECTOR_MAX_INTERN_CALLBACKS] =
{
    {
        &connector_internal_handshake_uuid,
        internal_handshake_callback
    }
};

/* Internal function for accepting an internal handshake message. It will
 * create a new handshake_accept call and send that back to the other
 * application, which will be the one that this side made a connection to. It
 * will tell that application what the identifier name is for this application,
 * so it can properly display what is connected and handle that there is a new
 * connection in its userspace. */
static void internal_handshake_callback(unsigned int context,
                                        const substance_connector_uuid_t *type,
                                        const char *message)
{
    const char *application_name = NULL;
    connector_message_t *out_message = NULL;
    connector_message_t *in_message = NULL;

    SUBSTANCE_CONNECTOR_UNUSED(type);

    application_name = connector_get_application_name();
    out_message = connector_build_message(context, &connector_internal_handshake_uuid,
                                     application_name);
    in_message = connector_build_message(context, &connector_internal_handshake_uuid, message);

    if (out_message != NULL)
    {
        /* Enqueue the message and flag the write threads, so that they handle
         * the outbound message. */
        connector_enqueue_outbound_message(out_message);

        connector_flag_write();
    }

    /* Finish by registering the connected application's name on this end as
     * well by calling the callback with the inbound name */
    if (in_message != NULL)
    {
        /* Set the application name to the context */
        connector_context_set_application_name(context, message);

        connector_clear_message(in_message);
        connector_free(in_message);
    }
}

/* Compares the uuid key with the callback's identifier key, used for a
 * bsearch call from the C standard library */
static int compare_callback(const void *key, const void *callback)
{
    const substance_connector_uuid_t *uid = key;
    const intern_callback_t *callback_struct = callback;
    return connector_compare_uuid(callback_struct->key, uid);
}

static connector_callback_fp find_callback(const substance_connector_uuid_t *type)
{
    connector_callback_fp result = NULL;
    intern_callback_t *callback = NULL;

    /* Use a binary search to find the message type that matches the
     * one given */
    callback = bsearch(type, internal_messages,
                       CONNECTOR_MAX_INTERN_CALLBACKS, sizeof(intern_callback_t),
                       compare_callback);

    if (callback != NULL)
    {
        result = callback->callback;
    }

    return result;
}

unsigned int connector_call_internal_message(unsigned int context,
                                        const substance_connector_uuid_t *type,
                                        const char *message)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    connector_callback_fp callback = NULL;

    /* Search for the callback in the table */
    callback = find_callback(type);

    if (callback != NULL)
    {
        /* Call the internal callback if it was found */
        callback(context, type, message);
        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}

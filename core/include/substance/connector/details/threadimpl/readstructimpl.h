/** @file readstructimpl.h
    @brief Contains the read thread structure implementation, separated
           from the read thread routines
    @author Galen Helfter - Adobe
    @date 20190726
    @copyright Adobe. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_DETAILS_THREADIMPL_READSTRUCTIMPL_H
#define _SUBSTANCE_CONNECTOR_DETAILS_THREADIMPL_READSTRUCTIMPL_H

#include <substance/connector/details/connection_utils.h>
#include <substance/connector/details/context_queue.h>
#include <substance/connector/details/thread.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

#ifndef SUBSTANCE_CONNECTOR_INBOUND_COUNT
#define SUBSTANCE_CONNECTOR_INBOUND_COUNT 2u
#endif

#define SUBSTANCE_CONNECTOR_READ_CONTEXTS (SUBSTANCE_CONNECTOR_CONTEXT_COUNT/\
                                      SUBSTANCE_CONNECTOR_INBOUND_COUNT) + 1

/* Structure definition for the read threads. Contains all of the information
 * handling the state of the thread. */
typedef struct _connector_read_thread
{
    connector_thread_t thread;
    unsigned int id;
    connector_poll_t contexts[SUBSTANCE_CONNECTOR_READ_CONTEXTS];
    uint32_t context_ids[SUBSTANCE_CONNECTOR_READ_CONTEXTS];
    uint32_t assigned_contexts;
} connector_read_thread_t;

/* Removes a context at the given index from the read thread structure.
 * The state of the read thread will be updated with this removed
 * context. If there are other valid contexts, then the last valid context
 * will be swapped with the index provided. Returns an errorcode. */
unsigned int connector_read_thread_remove_context(connector_read_thread_t *thread,
                                             unsigned int index);

/* Checks whether the read thread can handle more load. Returns
 * SUBSTANCE_CONNECTOR_TRUE if the thread is able to handle another connection, */
unsigned int connector_read_thread_check_load(const connector_read_thread_t *thread);

/* Attempts to acquire a connection for the read threads, acquiring a
 * new one from the available queue. Returns an errorcode denoting
 * success. */
unsigned int connector_read_thread_try_acquire(connector_read_thread_t *thread);

/* Perform a cleanup of the current connections, removing any from the thread
 * state that are no longer valid. Performs a full shutdown of these contexts,
 * making them available for future reuse. Returns an errorcode denoting
 * success. */
unsigned int connector_read_thread_cleanup_connections(connector_read_thread_t *thread);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_DETAILS_THREADIMPL_READSTRUCTIMPL_H */

/** @file locked_queue.h
    @brief Contains the data structure for a generic locked queue
    @author Galen Helfter - Allegorithmic
    @date 20190104
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_DETAILS_LOCKED_QUEUE_H
#define _SUBSTANCE_CONNECTOR_DETAILS_LOCKED_QUEUE_H

#include <substance/connector/details/thread.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

typedef void (*connector_locked_queue_node_handler_fp)(void *node);

typedef struct _connector_locked_queue_node
{
    struct _connector_locked_queue_node *next;
    void *contents;
} connector_locked_queue_node_t;

typedef struct _connector_locked_queue
{
    struct _connector_locked_queue_node *front;
    struct _connector_locked_queue_node *end;

    /* Allow a customized destructor for all nodes */
    connector_locked_queue_node_handler_fp handler;
    connector_mutex_t lock;
} connector_locked_queue_t;

/* Initializes the mutex lock and the pointers in the queue. Undefined
 * behavior if the queue has already been initialized. */
void connector_locked_queue_init(connector_locked_queue_t *queue,
                            connector_locked_queue_node_handler_fp handler);

/* Clears the given queue, removing all nodes and passing them to the handler
 * function. */
void connector_locked_queue_clear(connector_locked_queue_t *queue);

/* Enqueue the given node onto the queue, will perform a lock on the queue
 * to ensure proper addition. */
void connector_locked_enqueue(connector_locked_queue_t *queue,
                         connector_locked_queue_node_t *node);

/* Dequeue the first node off the queue. Will perform a lock on the queue to
 * ensure proper removal. */
connector_locked_queue_node_t* connector_locked_dequeue(connector_locked_queue_t *queue);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_DETAILS_LOCKED_QUEUE_H */

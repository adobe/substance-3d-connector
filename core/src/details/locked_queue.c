/** @file locked_queue.c
    @brief Contains the data structure for a generic locked queue
    @author Galen Helfter - Allegorithmic
    @date 20190104
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/details/locked_queue.h>
#include <substance/connector/common.h>
#include <substance/connector/details/memory.h>

static void dummy_handler(void *node)
{
    SUBSTANCE_CONNECTOR_UNUSED(node);

    /* Do nothing */
}

void connector_locked_queue_init(connector_locked_queue_t *queue,
                            connector_locked_queue_node_handler_fp handler)
{
    queue->front = NULL;
    queue->end = NULL;

    queue->lock = connector_mutex_create();

    /* Use a dummy handler for the queue if there is none set */
    if (handler == NULL)
    {
        queue->handler = dummy_handler;
    }
    else
    {
        queue->handler = handler;
    }
}

/* Clears the queue, but does not destroy it */
void connector_locked_queue_clear(connector_locked_queue_t *queue)
{
    connector_locked_queue_node_t *front = NULL;

    connector_mutex_lock(&queue->lock);

    while (queue->front != NULL)
    {
        front = queue->front;
        queue->front = front->next;

        queue->handler(front);
    }

    connector_mutex_unlock(&queue->lock);
}

void connector_locked_enqueue(connector_locked_queue_t *queue,
                         connector_locked_queue_node_t *node)
{
    connector_locked_queue_node_t *end = NULL;

    connector_mutex_lock(&queue->lock);

    node->next = NULL;
    end = queue->end;
    queue->end = node;

    if (queue->front == NULL)
    {
        queue->front = queue->end;
    }
    if (end != NULL)
    {
        end->next = node;
    }

    connector_mutex_unlock(&queue->lock);
}

connector_locked_queue_node_t* connector_locked_dequeue(connector_locked_queue_t *queue)
{
    connector_locked_queue_node_t *node = NULL;

    connector_mutex_lock(&queue->lock);

    if (queue->front != NULL)
    {
        node = queue->front;
        queue->front = node->next;
        if (queue->front == NULL)
        {
            queue->end = NULL;
        }
    }

    connector_mutex_unlock(&queue->lock);

    return node;
}

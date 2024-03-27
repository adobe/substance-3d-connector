/** @file message_queue.c
    @brief Contains all messages to be processed both inbound and outbound
    @author Galen Helfter - Allegorithmic
    @date 20181103
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/atomic.h>
#include <substance/connector/details/thread.h>
#include <substance/connector/details/locked_queue.h>
#include <substance/connector/details/message_queue.h>
#include <substance/connector/details/message.h>
#include <substance/connector/details/memory.h>

#include <stddef.h>
#include <stdlib.h>

enum MessageQueueState
{
    MESSAGE_QUEUE_SHUTDOWN         = 0u,  /* Queue is currently shut down */
    MESSAGE_QUEUE_INIT_STARTED     = 1u,  /* Initialization has started */
    MESSAGE_QUEUE_INITIALIZED      = 2u,  /* Initialization is completed */
    MESSAGE_QUEUE_SHUTDOWN_STARTED = 3u,  /* Shutdown has started */
    MESSAGE_QUEUE_INVALID                 /* Invalid status */
};

/* TODO: add some sort of message node pooling system */
static connector_locked_queue_t inbound_queue;
static connector_locked_queue_t outbound_queue;

static unsigned int message_queue_state = MESSAGE_QUEUE_SHUTDOWN;

static void enqueue_message(connector_locked_queue_t *queue, connector_message_t *message)
{
    connector_locked_queue_node_t *node = connector_allocate(sizeof(connector_locked_queue_node_t));
    node->next = NULL;
    node->contents = message;
    connector_locked_enqueue(queue, node);
}

static connector_message_t* dequeue_message(connector_locked_queue_t *queue)
{
    connector_message_t* message = NULL;
    connector_locked_queue_node_t *node = connector_locked_dequeue(queue);

    if (node != NULL)
    {
        message = node->contents;
        connector_free(node);
    }

    return message;
}

static void handle_message(void *message)
{
    connector_locked_queue_node_t *node = message;
    connector_message_t *message_struct = node->contents;

    connector_clear_message(message_struct);
    connector_free(message_struct);
    connector_free(node);
}

unsigned int connector_init_message_queue_subsystem(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    unsigned int initialized = MESSAGE_QUEUE_INVALID;

    CONNECTOR_ATOMIC_COMPARE_EXCHANGE(message_queue_state,
                                 MESSAGE_QUEUE_SHUTDOWN,
                                 MESSAGE_QUEUE_INIT_STARTED,
                                 initialized);

    if (initialized == MESSAGE_QUEUE_SHUTDOWN)
    {
        /* Initialize message queues */
        connector_locked_queue_init(&inbound_queue, handle_message);
        connector_locked_queue_init(&outbound_queue, handle_message);

        CONNECTOR_ATOMIC_COMPARE_EXCHANGE(message_queue_state,
                                     MESSAGE_QUEUE_INIT_STARTED,
                                     MESSAGE_QUEUE_INITIALIZED,
                                     initialized);

        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}

unsigned int connector_shutdown_message_queue_subsystem(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    unsigned int initialized = MESSAGE_QUEUE_INVALID;

    CONNECTOR_ATOMIC_COMPARE_EXCHANGE(message_queue_state,
                                 MESSAGE_QUEUE_INITIALIZED,
                                 MESSAGE_QUEUE_SHUTDOWN_STARTED,
                                 initialized);

    if (initialized == MESSAGE_QUEUE_INITIALIZED)
    {
        /* Clear the queues */
        connector_locked_queue_clear(&inbound_queue);
        connector_locked_queue_clear(&outbound_queue);
        connector_mutex_destroy(&inbound_queue.lock);
        connector_mutex_destroy(&outbound_queue.lock);

        CONNECTOR_ATOMIC_COMPARE_EXCHANGE(message_queue_state,
                                     MESSAGE_QUEUE_SHUTDOWN_STARTED,
                                     MESSAGE_QUEUE_SHUTDOWN,
                                     initialized);

        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}

void connector_enqueue_inbound_message(connector_message_t *message)
{
    enqueue_message(&inbound_queue, message);
}

connector_message_t* connector_acquire_inbound_message(void)
{
    return dequeue_message(&inbound_queue);
}

void connector_enqueue_outbound_message(connector_message_t *message)
{
    enqueue_message(&outbound_queue, message);
}

connector_message_t* connector_acquire_outbound_message(void)
{
    return dequeue_message(&outbound_queue);
}

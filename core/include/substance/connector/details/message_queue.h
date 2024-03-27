/** @file message_queue.h
    @brief Contains all messages to be processed both inbound and outbound
    @author Galen Helfter - Allegorithmic
    @date 20181103
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_MESSAGE_QUEUE_H
#define _SUBSTANCE_CONNECTOR_MESSAGE_QUEUE_H

#include <substance/connector/details/message.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* Perform any initialization operations for setup of the message queues */
unsigned int connector_init_message_queue_subsystem(void);

/* Performs shutdown operations on the inbound and outbound queues */
unsigned int connector_shutdown_message_queue_subsystem(void);

/* Emplaces the given message at the end of the end of the inbound queue */
void connector_enqueue_inbound_message(connector_message_t *message);

/* Acquires the front inbound message off the inbound message queue */
connector_message_t* connector_acquire_inbound_message(void);

/* Emplaces the given message at the end of the end of the outbound queue */
void connector_enqueue_outbound_message(connector_message_t *message);

/* Acquires the front outbound message off the outbound message queue */
connector_message_t* connector_acquire_outbound_message(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_MESSAGE_QUEUE_H */

/** @file available_queue.h
    @brief Contains all unassigned connections that can be acquired
           by the read threads.
    @author Galen Helfter - Allegorithmic
    @date 20190108
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_AVAILABLE_QUEUE_H
#define _SUBSTANCE_CONNECTOR_AVAILABLE_QUEUE_H

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* Initializes the available connection queue. The connection queue will be
 * sized to the defined context count. */
unsigned int connector_available_queue_init(void);

/* Destroys the available connection queue, freeing any memory used by it.
 * The read threads that will pull elements off of this queue should have
 * been terminated first. */
unsigned int connector_available_queue_shutdown(void);

/* Appends the context to the available queue. This returns a success code
 * code on a successful push. */
unsigned int connector_available_queue_push(unsigned int context);

/* Attempts to pop a context off of the available queue. This returns a
 * success code on a successful operation. It will return an error code
 * if the queue is empty. */
unsigned int connector_available_queue_pop(unsigned int *context);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_AVAILABLE_QUEUE_H */

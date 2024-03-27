/** @file uint_queue.h
    @brief Contains a lock-free, fixed-size unsigned integer queue
    @author Galen Helfter - Allegorithmic
    @date 20190108
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_UINT_QUEUE_H
#define _SUBSTANCE_CONNECTOR_UINT_QUEUE_H

#include <stdint.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

typedef struct _connector_uint_queue connector_uint_queue_t;

/* Thread-safe push operation onto the unsigned integer queue. This operation
 * will attempt to place the given element onto the queue. It will return a
 * success code if successful, and an error code if it failed. */
unsigned int connector_uint_queue_push(connector_uint_queue_t *queue, unsigned int elem);

/* Thread-safe pop operation from the unsigned integer queue. This operation
 * will attempt to retrieve an element from the queue. It will return a success
 * code if successful, and an error code if it failed. On success, the popped
 * element is stored in the retval pointer. */
unsigned int connector_uint_queue_pop(connector_uint_queue_t *queue,
                                 unsigned int *retval);

/* Creates and initializes an unsigned integer queue with the given size. On
 * failure, this operation will return a NULL pointer, and on success it will
 * return a valid queue. The queue is guaranteed to hold at least size
 * elements, but may hold more. */
connector_uint_queue_t* connector_uint_queue_create(uint32_t size);

/* Destroys a given unsigned integer queue, deallocating all memory from it.
 * The queue should not be in use by any threads, and should be called after
 * any join calls on threads that may have been using it. The pointer to the
 * queue is invalid after a successful operation. Returns a success code on
 * success and an error code on failure. */
unsigned int connector_uint_queue_destroy(connector_uint_queue_t *queue);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_UINT_QUEUE_H */

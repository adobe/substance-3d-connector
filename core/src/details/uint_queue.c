/** @file uint_queue.c
    @brief Contains a lock-free, fixed-size unsigned integer queue
    @author Galen Helfter - Allegorithmic
    @date 20190108
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/atomic.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/uint_queue.h>

#include <stdint.h>
#include <string.h>

static uint32_t convert_to_index(uint32_t position, uint32_t length);

/* Internal definition for opaque queue type */
struct _connector_uint_queue
{
    uint32_t write_index;
    uint32_t read_index;
    uint32_t max_index;
    uint32_t size;
    unsigned int *array;
};

static uint32_t convert_to_index(uint32_t position, uint32_t length)
{
    return position % length;
}

unsigned int connector_uint_queue_push(connector_uint_queue_t *queue, unsigned int elem)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    uint32_t write_index = 0u;
    uint32_t read_index = 0u;
    uint32_t atomic_result = 0u;

    if (queue != NULL)
    {
        do
        {
            write_index = queue->write_index;
            read_index = queue->read_index;

            if (convert_to_index(write_index + 1, queue->size)
                == convert_to_index(read_index, queue->size))
            {
                /* Full queue, exit with an error */
                retcode = SUBSTANCE_CONNECTOR_ERROR;
                break;
            }

            CONNECTOR_ATOMIC_COMPARE_EXCHANGE(queue->write_index, write_index,
                                         (write_index + 1), atomic_result);
        } while (atomic_result != write_index);

        if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            queue->array[convert_to_index(write_index, queue->size)] = elem;

            /* Update the maximum read index */
            do
            {
                CONNECTOR_ATOMIC_COMPARE_EXCHANGE(queue->max_index, write_index,
                                            (write_index + 1), atomic_result);
            } while (atomic_result != write_index);
        }
    }
    else
    {
        retcode = SUBSTANCE_CONNECTOR_INVALID;
    }

    return retcode;
}

unsigned int connector_uint_queue_pop(connector_uint_queue_t *queue,
                                 unsigned int *retval)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    uint32_t read_index = 0u;
    uint32_t max_index = 0u;
    uint32_t atomic_result = 0u;
    unsigned int data = 0u;

    while (SUBSTANCE_CONNECTOR_TRUE)
    {
        read_index = queue->read_index;
        max_index = queue->max_index;

        if (convert_to_index(read_index, queue->size)
            == convert_to_index(max_index, queue->size))
        {
            /* Either the queue is empty or the data needs to be committed.
             * Exit with error. */
            retcode = SUBSTANCE_CONNECTOR_ERROR;
            break;
        }

        if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            data = queue->array[convert_to_index(read_index, queue->size)];

            CONNECTOR_ATOMIC_COMPARE_EXCHANGE(queue->read_index, read_index,
                                         (read_index + 1), atomic_result);

            if (atomic_result == read_index)
            {
                *retval = data;
                break;
            }
        }
    }

    return retcode;
}

connector_uint_queue_t* connector_uint_queue_create(uint32_t size)
{
    connector_uint_queue_t *queue = NULL;
    unsigned int success = SUBSTANCE_CONNECTOR_ERROR;

    queue = connector_allocate(sizeof(connector_uint_queue_t));
    if (size > 0u && queue != NULL)
    {
        /* Store extra elements on the end, as at a fill of size - 1,
         * (size - 1) + 1 % size == 0, which would be the read index when
         * the buffer is approaching full. Since an unsigned integer is
         * likely 32-bits on a 64-bit machine, add an extra element if
         * even to keep it aligned on 64-bit boundaries. */
        size += (1 + ((size & 0x01u) ^ 0x01u));

        queue->array = connector_allocate(sizeof(unsigned int) * size);
        if (queue->array != NULL)
        {
            memset(queue->array, 0x00, sizeof(unsigned int) * size);
            queue->size = size;
            queue->write_index = 0u;
            queue->read_index = 0u;
            queue->max_index = 0u;
            success = SUBSTANCE_CONNECTOR_SUCCESS;
        }
    }

    if (success != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        connector_uint_queue_destroy(queue);
    }

    return queue;
}

/* Might look at having this receive a double pointer, to do an atomic
 * swap, but this shouldn't be necessary so long as the queue is destroyed
 * in the proper location, i.e., on a shutdown call after the threads have
 * been joined. */
unsigned int connector_uint_queue_destroy(connector_uint_queue_t *queue)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (queue != NULL)
    {
        connector_free(queue->array);
        connector_free(queue);
        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}

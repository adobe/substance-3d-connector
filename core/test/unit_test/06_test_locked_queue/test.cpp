//! @file test.cpp
//! @brief Tests for the locked queue
//! @author Galen Helfter - Allegorithmic
//! @date 20190527
//! @copyright Allegorithmic. All rights reserved.

#include <substance/connector/details/locked_queue.h>
#include <common/test_common.h>

#include <vector>
#include <thread>

#include <cstdlib>

#define TEST_COUNT 2u

// begin connector_test_single_threaded block

static const char * _connector_test_single_threaded_errors[] =
{
    "Failed to insert and retrieve"
};

extern "C"
{
    static void _dummy_node_handler(void * /* node */)
    {
        // Do nothing
    }
}

static constexpr unsigned int node_count = 50u;

static unsigned int _connector_test_single_threaded()
{
    unsigned int result = 0u;

    connector_locked_queue_t queue;

    connector_locked_queue_init(&queue, _dummy_node_handler);

    connector_locked_queue_node_t nodes[node_count];

    for (unsigned int i = 0u; i < node_count; ++i)
    {
        connector_locked_enqueue(&queue, nodes + i);
    }

    for (unsigned int i = 0u; i < node_count; ++i)
    {
        connector_locked_queue_node_t *node = connector_locked_dequeue(&queue);

        if (node != (nodes + i))
        {
            result = 1u;
        }
    }

    connector_mutex_destroy(&queue.lock);

    return result;
}

// end connector_test_single_threaded block

// begin connector_test_multi_threaded_producer block

static const char * _connector_test_multi_threaded_producer_errors[] =
{
    "Failed to retrieve all nodes from multiple producer threads"
};

static unsigned int _connector_test_multi_threaded_producer()
{
    unsigned int result = 0u;

    connector_locked_queue_t queue;

    connector_locked_queue_init(&queue, _dummy_node_handler);

    connector_locked_queue_node_t nodes[node_count];

    std::vector<std::thread> threads;

    for (unsigned int i = 0u; i < node_count; ++i)
    {
        threads.push_back(std::thread(connector_locked_enqueue, &queue, (nodes + i)));
    }

    for (unsigned int i = 0u; i < node_count; ++i)
    {
        threads[i].join();
    }

    for (unsigned int i = 0u; i < node_count; ++i)
    {
        connector_locked_queue_node_t *node = connector_locked_dequeue(&queue);

        // Compare the pointers - all returned values should be within
        // the contiguous array of nodes
        if ((node < nodes) || (node >= (nodes + node_count)))
        {
            result = 1u;
        }
    }

    connector_mutex_destroy(&queue.lock);

    return result;
}

// end linm_test_multi_threaded_producer block

// List of tests for iteration

static const char * _connector_test_names[TEST_COUNT] =
{
    "test_single_threaded",
    "test_multi_threaded_producer"
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_single_threaded_errors,
    _connector_test_multi_threaded_producer_errors
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_single_threaded,
    _connector_test_multi_threaded_producer
};

_CONNECTOR_TEST_MAIN

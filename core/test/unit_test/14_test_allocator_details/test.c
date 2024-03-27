/** @file test.c
    @brief Testing internal memory allocator details
    @author Galen Helfter - Adobe
    @date 20190711
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/errorcodes.h>
#include <substance/connector/connector.h>
#include <substance/connector/types.h>
#include <substance/connector/details/memory.h>

#include <common/test_common.h>

#include <stdlib.h>

#define TEST_COUNT 2u

static unsigned int _test_alloc_flag = 0u;
static unsigned int _test_dealloc_flag = 0u;

static void* _test_alloc(size_t size)
{
    _test_alloc_flag += 1u;
    return malloc(size);
}

static void _test_dealloc(void *ptr)
{
    _test_dealloc_flag += 1u;
    free(ptr);
}

/* begin connector_test_internal_allocator block */

static unsigned int *_block = NULL;

static unsigned int _test_internal_allocate()
{
    unsigned int result = 0u;

    connector_set_allocator(&_test_alloc);

    if ((_block = connector_allocate(sizeof(unsigned int))) == NULL)
    {
        result = 1u;
    }
    else if (_test_alloc_flag != 1u)
    {
        result = 2u;
    }

    return result;
}
static unsigned int _test_internal_deallocate()
{
    unsigned int result = 0u;

    connector_set_deallocator(&_test_dealloc);

    connector_free(_block);

    if (_test_dealloc_flag != 1u)
    {
        result = 3u;
    }

    return result;
}

static const char * _connector_test_internal_allocator_errors[] =
{
    "Failed to allocate memory",
    "Allocation did not call custom allocator",
    "Deallocation did not call custom deallocator",
    "Failed to clear allocators"
};

static unsigned int _connector_test_internal_allocator()
{
    unsigned int result = 0u;
    unsigned int retval = 0u;

    _test_alloc_flag = 0u;
    _test_dealloc_flag = 0u;

    if ((retval = _test_internal_allocate()) != 0u)
    {
        result = retval;
    }
    else if ((retval = _test_internal_deallocate()) != 0u)
    {
        result = retval;
    }
    else if (connector_clear_allocators() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 4u;
    }

    return result;
}

/* end connector_test_internal_allocator block */

/* begin connector_test_clear_allocator block */

static const char * _connector_test_clear_allocator_errors[] =
{
    "Failed to clear allocators",
    "Failed to allocate memory",
    "Failed to properly clear allocator",
    "Failed to properly clear deallocator"
};

static unsigned int _connector_test_clear_allocator()
{
    unsigned int result = 0u;

    /* Clear allocation flags */
    _test_alloc_flag = 0u;
    _test_dealloc_flag = 0u;

    /* Set the allocators */
    connector_set_allocator(&_test_alloc);
    connector_set_deallocator(&_test_dealloc);

    if (connector_clear_allocators() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 1u;
    }
    else if ((_block = connector_allocate(sizeof(unsigned int))) == NULL)
    {
        result = 2u;
    }
    else if (_test_alloc_flag != 0u)
    {
        result = 3u;
    }
    else
    {
        connector_free(_block);

        if (_test_dealloc_flag != 0u)
        {
            result = 4u;
        }
    }

    return result;
}

/* end connector_test_clear_allocator block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_internal_allocator",
    "test_clear_allocator"
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_internal_allocator_errors,
    _connector_test_clear_allocator_errors
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_internal_allocator,
    _connector_test_clear_allocator
};

/* Test main function */
_CONNECTOR_TEST_MAIN

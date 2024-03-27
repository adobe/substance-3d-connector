/** @file test.c
    @brief Testing atomic operations
    @author Galen Helfter - Adobe
    @date 20190625
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/details/atomic.h>

#include <common/test_common.h>

#include <stdint.h>

#define TEST_COUNT 3u

/* begin connector_test_atomic_compare_exchange block */

static const char * _connector_test_atomic_compare_exchange_errors[] =
{
    "Failed a proper compare-exchange",
    "Invalid results from failed compare-exchange"
};

static unsigned int _connector_test_atomic_compare_exchange()
{
    unsigned int result = 0u;

    const uint32_t _original_value = 0x17u;
    const uint32_t _target_value = 0x32u;

    uint32_t _value_1 = _original_value;
    uint32_t _value_2 = _original_value;
    uint32_t _expected_1 = _original_value;
    uint32_t _expected_2 = 0x0u;

    uint32_t _result_1 = 0x0u;
    uint32_t _result_2 = 0x0u;

    CONNECTOR_ATOMIC_COMPARE_EXCHANGE(_value_1, _expected_1, _target_value,
                                 _result_1);

    CONNECTOR_ATOMIC_COMPARE_EXCHANGE(_value_2, _expected_2, _target_value,
                                 _result_2);

    if (_value_1 != _target_value || _result_1 != _original_value)
    {
        result = 1u;
    }
    else if (_value_2 != _original_value || _result_2 != _original_value)
    {
        result = 2u;
    }

    return result;
}

/* end connector_test_atomic_compare_exchange block */

/* begin connector_test_atomic_and block */

static const char * _connector_test_atomic_and_errors[] =
{
    "Number and zero is not zero",
    "Return value of number and zero is not the original value",
    "Number and a sequence of ones is not the number",
    "Return value of number and a sequence of ones is not the original value",
    "Failed to extract a bit mask",
    "Original value not stored in result when extracting mask"
};

static unsigned int _connector_test_atomic_and()
{
    unsigned int result = 0u;

    const uint32_t _original_sequence = 0xffffffffu;
    const uint32_t _original_value = 0x1234u;
    const uint32_t _original_mask = 0xff;

    uint32_t _value_0 = _original_value;
    uint32_t _value_1 = _original_value;
    uint32_t _value_2 = _original_value;

    uint32_t _retval_0 = 0x0u;
    uint32_t _retval_1 = 0x3357u;
    uint32_t _retval_2 = 0x0u;

    const uint32_t _result_2 = _value_2 & _original_mask;

    CONNECTOR_ATOMIC_AND(_value_0, 0x00, _retval_0);
    CONNECTOR_ATOMIC_AND(_value_1, _original_sequence, _retval_1);
    CONNECTOR_ATOMIC_AND(_value_2, _original_mask, _retval_2);

    if (_value_0 != 0x00u)
    {
        result = 1u;
    }
    else if (_retval_0 != _original_value)
    {
        result = 2u;
    }
    else if (_value_1 != _original_value)
    {
        result = 3u;
    }
    else if (_retval_1 != _original_value)
    {
        result = 4u;
    }
    else if (_value_2 != _result_2)
    {
        result = 5u;
    }
    else if (_retval_2 != _original_value)
    {
        result = 6u;
    }

    return result;
}

/* end connector_test_atomic_and block */

/* begin connector_test_atomic_or block */

static const char * _connector_test_atomic_or_errors[] =
{
    "Number or a sequence of ones is not the sequence of ones",
    "Return of number or a sequence of ones is not the original value",
    "Number or zero is not the number",
    "Return of number or zero is not the original value"
};

static unsigned int _connector_test_atomic_or()
{
    unsigned int result = 0u;

    const uint32_t _original_sequence = 0xffffffffu;
    const uint32_t _original_value = 0x1234u;

    uint32_t _value_0 = _original_value;
    uint32_t _value_1 = _original_value;

    uint32_t _retval_0 = 0u;
    uint32_t _retval_1 = 0x3357u;

    CONNECTOR_ATOMIC_OR(_value_0, _original_sequence, _retval_0);

    CONNECTOR_ATOMIC_OR(_value_1, 0x00u, _retval_1);

    if (_value_0 != _original_sequence)
    {
        result = 1u;
    }
    else if (_retval_0 != _original_value)
    {
        result = 2u;
    }
    else if (_value_1 != _original_value)
    {
        result = 3u;
    }
    else if (_retval_1 != _original_value)
    {
        result = 4u;
    }

    return result;
}

/* end connector_test_atomic_or block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_atomic_compare_exchange",
    "test_atomic_and",
    "test_atomic_or"
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_atomic_compare_exchange_errors,
    _connector_test_atomic_and_errors,
    _connector_test_atomic_or_errors
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_atomic_compare_exchange,
    _connector_test_atomic_and,
    _connector_test_atomic_or
};

/* Test main function */
_CONNECTOR_TEST_MAIN

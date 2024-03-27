/** @file test.c
    @brief Testing that the context configuration is properly constructed
    @author Galen Helfter - Adobe
    @date 20190719
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/context_struct.h>

#include <common/test_common.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TEST_COUNT 2u

/* begin connector_test_bitfield_conflicts block */

static const char * _connector_test_bitfield_conflicts_errors[] =
{
    "Communication type and connection state should not conflict",
    "Communication type and handshake status should not conflict",
    "Connection state and handshake status should not conflict",
};

static unsigned int _connector_test_bitfield_conflicts()
{
    unsigned int result = 0u;

    if ((SUBSTANCE_CONNECTOR_COMM_MASK & SUBSTANCE_CONNECTOR_CONN_MASK) != 0u)
    {
        result = 1u;
    }
    else if ((SUBSTANCE_CONNECTOR_COMM_MASK & SUBSTANCE_CONNECTOR_HANDSHAKE_MASK) != 0u)
    {
        result = 2u;
    }
    else if ((SUBSTANCE_CONNECTOR_CONN_MASK & SUBSTANCE_CONNECTOR_HANDSHAKE_MASK) != 0u)
    {
        result = 3u;
    }

    return result;
}

/* end connector_test_bitfield_conflicts block */

/* begin test_configuration_enum_conflicts */

static const char * _connector_test_configuration_enum_conflicts_errors[] =
{
    "SubstanceConnectorHandshake enum conflicts with SubstanceConnectorCommunication",
    "SubstanceConnectorHandshake enum conflicts with SubstanceConnectorConnState",
    "SubstanceConnectorConnState enum conflicts with SubstanceConnectorCommunication",
    "SubstanceConnectorConnState enum conflicts with SubstanceConnectorHandshake",
    "SubstanceConnectorCommunication enum conflicts with SubstanceConnectorConnState",
    "SubstanceConnectorCommunication enum conflicts with SubstanceConnectorHandshake",
};

static unsigned int _connector_test_configuration_enum_conflicts()
{
    unsigned int result = 0u;

    /* Test SubstanceConnectorHandshake enum */
    if (result == 0u)
    {
        for (unsigned int value = SUBSTANCE_CONNECTOR_HANDSHAKE_SENT;
             value <= SUBSTANCE_CONNECTOR_HANDSHAKE_MASK; value <<= 1u)
        {
            for (unsigned int comm_value = SUBSTANCE_CONNECTOR_COMM_TCP;
                 comm_value <= SUBSTANCE_CONNECTOR_COMM_MASK; comm_value <<= 1u)
            {
                if (value == comm_value)
                {
                    result = 1u;
                    break;
                }
            }

            for (unsigned int conn_value = SUBSTANCE_CONNECTOR_CONN_OPEN;
                 conn_value <= SUBSTANCE_CONNECTOR_CONN_MASK; conn_value <<= 1u)
            {
                if (value == conn_value)
                {
                    result = 2u;
                    break;
                }
            }

            if (result != 0u)
            {
                break;
            }
        }
    }

    /* Test SubstanceConnectorConnState enum */
    if (result == 0u)
    {
        for (unsigned int value = SUBSTANCE_CONNECTOR_CONN_OPEN;
             value <= SUBSTANCE_CONNECTOR_CONN_MASK; value <<= 1u)
        {
            for (unsigned int comm_value = SUBSTANCE_CONNECTOR_COMM_TCP;
                 comm_value <= SUBSTANCE_CONNECTOR_COMM_MASK; comm_value <<= 1u)
            {
                if (value == comm_value)
                {
                    result = 3u;
                    break;
                }
            }

            for (unsigned int handshake_value = SUBSTANCE_CONNECTOR_HANDSHAKE_SENT;
                 handshake_value <= SUBSTANCE_CONNECTOR_HANDSHAKE_MASK;
                 handshake_value <<= 1u)
            {
                if (value == handshake_value)
                {
                    result = 4u;
                    break;
                }
            }

            if (result != 0u)
            {
                break;
            }
        }
    }

    /* Test SubstanceConnectorCommunication enum */
    for (unsigned int value = SUBSTANCE_CONNECTOR_COMM_TCP;
         value <= SUBSTANCE_CONNECTOR_COMM_MASK; value <<= 1u)
    {
        for (unsigned int conn_value = SUBSTANCE_CONNECTOR_CONN_OPEN;
             conn_value <= SUBSTANCE_CONNECTOR_CONN_MASK; conn_value <<= 1u)
        {
            if (value == conn_value)
            {
                result = 5u;
                break;
            }
        }

        for (unsigned int handshake_value = SUBSTANCE_CONNECTOR_HANDSHAKE_SENT;
             handshake_value <= SUBSTANCE_CONNECTOR_HANDSHAKE_MASK;
             handshake_value <<= 1u)
        {
            if (value == handshake_value)
            {
                result = 6u;
                break;
            }
        }

        if (result != 0u)
        {
            break;
        }
    }

    return result;
}

/* end test_configuration_enum_conflicts */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_bitfield_conflicts",
    "test_configuration_enum_conflicts",
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_bitfield_conflicts_errors,
    _connector_test_configuration_enum_conflicts_errors,
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_bitfield_conflicts,
    _connector_test_configuration_enum_conflicts,
};

/* Test main function */
_CONNECTOR_TEST_MAIN

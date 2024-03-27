/** @file internal_uuids.c
    @brief Contains constants for internal uuid types
    @author Galen Helfter - Adobe
    @date 20190613
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/details/internal_uuids.h>

const substance_connector_uuid_t connector_internal_handshake_uuid =
{
    /* 02572bc5-2d84-450a-9e01-d22c66b1abb1 */
    {0x02572bc5u, 0x2d84450au, 0x9e01d22cu, 0x66b1abb1u}
};

const substance_connector_uuid_t connector_internal_connection_closed_uuid =
{
    /* 04705ddf-16d4-4489-af6c-6e3a93f1959d */
    {0x04705ddfu, 0x16d44489u, 0xaf6c6e3au, 0x93f1959du}
};

const substance_connector_uuid_t connector_internal_log_uuid =
{
    /* 3f7fa3ce-164c-4661-8bb6-b8c4ea80c93b */
    {0x3f7fa3ceu, 0x164c4661u, 0x8bb6b8c4u, 0xea80c93bu}
};

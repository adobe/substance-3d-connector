/** @file message_header.h
    @brief Contains message header type definitions
    @author Galen Helfter - Allegorithmic
    @date 20190418
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_MESSAGE_HEADER_H
#define _SUBSTANCE_CONNECTOR_MESSAGE_HEADER_H

#include <stdint.h>

#include <substance/connector/types.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* Macros for operating on the description */
#define CONNECTOR_PROTOCOL_VERSION(x) ((x) & 0xf000u)
#define CONNECTOR_MESSAGE_TYPE(x) ((x) & 0x0fffu)

/* Magic number to identify connector messages */
#define CONNECTOR_MESSAGE_IDENTIFIER 0x03c9u

/* Magic number to identify whether a message is internal. It contains all
 * of the bits that the standard identifier has, which means that the
 * CONNECTOR_IDENTIFY_MESSAGE macro will return true on this as well. */
#define CONNECTOR_INTERNAL_IDENTIFIER 0x03db

/* Macro to identify whether the first two bytes contains the connector tag */
#define CONNECTOR_IDENTIFY_MESSAGE(x) ((((x) & 0x0fffu) & CONNECTOR_MESSAGE_IDENTIFIER)\
                                   == CONNECTOR_MESSAGE_IDENTIFIER)

#define CONNECTOR_IDENTIFY_INTERNAL(x) ((((x) & 0x0fffu) & CONNECTOR_INTERNAL_IDENTIFIER)\
                                   == CONNECTOR_INTERNAL_IDENTIFIER)

enum ConnectorHeaderVersions
{
    CONNECTOR_HEADER_R1 = 0x0000u,

    /* Final header code that can be handled by only having four
     * bits to hold the protocol revision number */
    CONNECTOR_HEADER_R16 = 0xf000u
};

/* Revision one of the Substance connector protocol, 6 bytes in length */
typedef struct _connector_message_header_r1
{
    /* 16 bits in network byte order. Includes the following:
     * 12 bits: Connector identifier
     * 4 bits: Protocol version */
    uint16_t description;

    /* Message body length - maximum of 2 ** 32. This puts a maximum
     * of 4 GiB - 1 for a single data length. */
    uint32_t message_length;

    /* 128-bit message id, used to determine the message type */
    substance_connector_uuid_t message_id;
} connector_message_header_r1_t;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_MESSAGE_HEADER_H */

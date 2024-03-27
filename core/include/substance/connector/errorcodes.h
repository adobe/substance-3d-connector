/** @file errorcodes.h
    @brief Contains the error codes for the connector framework
    @author Galen Helfter - Allegorithmic
    @date 20181015
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_ERRORCODES_H
#define _SUBSTANCE_CONNECTOR_ERRORCODES_H

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

enum SubstanceConnectorError
{
    SUBSTANCE_CONNECTOR_SUCCESS     = 0u,  /* Successful operation */
    SUBSTANCE_CONNECTOR_ERROR       = 1u,  /* General, nonspecific error */
    SUBSTANCE_CONNECTOR_BADALLOC    = 2u,  /* Failed memory allocation */
    SUBSTANCE_CONNECTOR_NOTLOADED   = 3u,  /* Dynamic module is not loaded */
    SUBSTANCE_CONNECTOR_CONN_FAIL   = 4u,  /* Connection fail, SIGPIPE */
    SUBSTANCE_CONNECTOR_SEND_FAIL   = 5u,  /* Write to a connection timed out */
    SUBSTANCE_CONNECTOR_UNSUPPORTED = 6u,  /* Platform unsupported for call */
    SUBSTANCE_CONNECTOR_INVALID     = 7u,  /* Invalid argument provided */
    SUBSTANCE_CONNECTOR_READ_FAIL   = 8u,  /* Failed read request */
    SUBSTANCE_CONNECTOR_OPEN_FAIL   = 9u,  /* Faied to open a connection */
    SUBSTANCE_CONNECTOR_ERROR_MAX   = 10u  /* Maximum current error codes */
};

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_ERRORCODES_H */

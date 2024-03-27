/** @file communication.h
    @brief Contains communication thread functionality
    @author Galen Helfter - Allegorithmic
    @date 20181217
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_DETAILS_COMMUNICATION_H
#define _SUBSTANCE_CONNECTOR_DETAILS_COMMUNICATION_H

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* Signals any single sleeping read thread to awaken from its condition and
 * attempt to process any new available connections. */
unsigned int connector_flag_read(void);

/* Signals any waiting write thread to awaken from its condition and attempt
 * to process new items on the write queue. */
unsigned int connector_flag_write(void);

/* Initializes all of the communication threads, include the writing threads
 * and reading threads. */
unsigned int connector_init_comm_subsystem(void);

/* Flags all communication threads to shut down. Will perform a join on all
 * of these threads and block until they all finish and shut down. Cleans up
 * all resources used by the communication system. */
unsigned int connector_shutdown_comm_subsystem(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_DETAILS_COMMUNICATION_H */

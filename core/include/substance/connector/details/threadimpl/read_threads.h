/** @file read_threads.h
    @brief Contains the functionality for the read threads
    @author Galen Helfter - Adobe
    @date 20190624
    @copyright Adobe. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_DETAILS_THREADIMPL_READ_THREADS_H
#define _SUBSTANCE_CONNECTOR_DETAILS_THREADIMPL_READ_THREADS_H

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* Initialize the read threads, handling their internal state. Returns an
 * errorcode representing success. */
unsigned int connector_init_read_threads(void);

/* Perform a shutdown of the read threads. This will close them, clear their
 * resources and set them up to be reinitialized. */
unsigned int connector_shutdown_read_threads(void);

/* Implementation of flagging the read threads, allowing it to be handled
 * internally. */
unsigned int connector_flag_read_impl(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_DETAILS_THREADIMPL_READ_THREADS_H */

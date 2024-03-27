/** @file dispatch.h
    @brief Contains the functionality for dispatch thread processing
    @author Galen Helfter - Allegorithmic
    @date 20181215
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_DISPATCH_H
#define _SUBSTANCE_CONNECTOR_DISPATCH_H

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* Initialize the dispatch threads, startin them with the dispatch callback
 * function. */
unsigned int connector_init_dispatch_subsystem(unsigned int signal_main);

/* Forces termination of all dispatch threads, and cleans up any resources
 * used in the internal system. Note that this will block and wait with a
 * join call on each thread. */
unsigned int connector_shutdown_dispatch_subsystem(void);

/* Tells the dispatch threads to consume any processes they need to */
unsigned int connector_flag_dispatch(void);

/* Await a signal from the dispatch threads. */
unsigned int connector_await_dispatch(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_DISPATCH_H */

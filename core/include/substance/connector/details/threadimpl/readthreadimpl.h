/** @file readthreadimpl.h
    @brief Contains implementation functions for parts of the read threads
    @author Galen Helfter - Adobe
    @date 20190729
    @copyright Adobe. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_DETAILS_THREADIMPL_READTHREADIMPL_H
#define _SUBSTANCE_CONNECTOR_DETAILS_THREADIMPL_READTHREADIMPL_H

#include <substance/connector/details/thread.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* Function to signal main once the read thread has acquired its lock */
typedef void (*connector_read_signal_main_fp)();

struct _connector_read_thread;

unsigned int connector_read_thread_handle_open(unsigned int context);

unsigned int connector_read_thread_handle_connected(unsigned int context);

unsigned int connector_read_thread_handle_context(unsigned int context);

unsigned int connector_read_thread_await_condition(struct _connector_read_thread *thread,
                                              connector_cond_t *condition,
                                              connector_mutex_t *lock,
                                              unsigned int *shutdown_flag,
                                              connector_read_signal_main_fp notify);

unsigned int connector_read_thread_handle_poll(struct _connector_read_thread *thread);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_DETAILS_THREADIMPL_READTHREADIMPL_H */

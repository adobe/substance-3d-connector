/** @file thread.h
    @brief Contains threading declarations agnostic of platform
    @author Galen Helfter - Allegorithmic
    @date 20181121
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_THREAD_H
#define _SUBSTANCE_CONNECTOR_THREAD_H

#include <substance/connector/common.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* Currently we drive the threading through raw typedefs. This could be
 * changed to forward declare a structure as an opaque type and work
 * through that instead, but that would complicate it some and change the
 * threading API. */
#if defined(SUBSTANCE_CONNECTOR_POSIX)
#include <pthread.h>
typedef pthread_mutex_t connector_mutex_t;
typedef pthread_cond_t connector_cond_t;
typedef pthread_t connector_thread_t;
typedef void* connector_thread_return_t;
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
/* Note: This is a very heavy header. Should look at trying to
 * manually define it. */
#include <Windows.h>
typedef SRWLOCK connector_mutex_t;
typedef CONDITION_VARIABLE connector_cond_t;
typedef HANDLE connector_thread_t;
typedef unsigned long connector_thread_return_t;
#endif

typedef connector_thread_return_t (*connector_thread_routine)(void*);

/* Creates a new mutex on the given platform, performing any initialization.
 * The mutex will be returned in an unlocked state. */
connector_mutex_t connector_mutex_create(void);

/* Destroys the given mutex. The mutex must be unlocked, as destroying a
 * locked mutex is undefined behavior. */
void connector_mutex_destroy(connector_mutex_t *mutex);

/* Performs a standard, blocking lock call on the given mutex */
void connector_mutex_lock(connector_mutex_t *mutex);

/* Unlocks the given mutex */
void connector_mutex_unlock(connector_mutex_t *mutex);

/* Creates a new thread executing the given routine */
connector_thread_t connector_thread_create(connector_thread_routine routine, void *arg);

/* Waits for the specified thread to terminate */
void connector_thread_join(connector_thread_t *thread);

/* Destroys the given thread on the current platform */
void connector_thread_destroy(connector_thread_t *thread);

/* Creates a condition variable for the given platform */
void connector_condition_create(connector_cond_t *cond);

/* Destroys a valid condition variable */
void connector_condition_destroy(connector_cond_t *cond);

/* Given a lock and a condition variable, puts the current thread to sleep
 * until flagged again by a later event */
void connector_condition_wait(connector_cond_t *cond, connector_mutex_t *lock);

/* Given a condition variable, signals a single consumer thread to wake up
 * and start processing */
void connector_condition_signal(connector_cond_t *cond);

/* Given a condition variable, signals all awaiting threads to wake up
 * and start processing */
void connector_condition_broadcast(connector_cond_t *cond);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_THREAD_H */

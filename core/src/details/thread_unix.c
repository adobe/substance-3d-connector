/** @file thread_unix.c
    @brief Contains threading implementations for Unix systems
    @author Galen Helfter - Allegorithmic
    @date 20181121
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/details/thread.h>
#include <substance/connector/common.h>

/* Threading operations map to pthread implementations on Unix systems */

connector_mutex_t connector_mutex_create(void)
{
    connector_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    return mutex;
}

void connector_mutex_lock(connector_mutex_t *mutex)
{
    pthread_mutex_lock(mutex);
}

void connector_mutex_unlock(connector_mutex_t *mutex)
{
    pthread_mutex_unlock(mutex);
}

void connector_mutex_destroy(connector_mutex_t *mutex)
{
    pthread_mutex_destroy(mutex);
}

connector_thread_t connector_thread_create(connector_thread_routine routine, void *arg)
{
    connector_thread_t thread;
    pthread_create(&thread, NULL, routine, arg);

    return thread;
}

void connector_thread_join(connector_thread_t *thread)
{
    void *value = NULL;
    pthread_join(*thread, &value);
}

void connector_thread_destroy(connector_thread_t *thread)
{
    SUBSTANCE_CONNECTOR_UNUSED(thread);

    /* Do nothing */
}

void connector_condition_create(connector_cond_t *cond)
{
    pthread_cond_init(cond, NULL);
}

void connector_condition_destroy(connector_cond_t *cond)
{
    pthread_cond_destroy(cond);
}

void connector_condition_wait(connector_cond_t *cond, connector_mutex_t *lock)
{
    pthread_cond_wait(cond, lock);
}

void connector_condition_signal(connector_cond_t *cond)
{
    pthread_cond_signal(cond);
}

void connector_condition_broadcast(connector_cond_t *cond)
{
    pthread_cond_broadcast(cond);
}

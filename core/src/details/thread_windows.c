/** @file thread_windows.c
    @brief Contains threading implementations for Windows systems
    @author Galen Helfter - Allegorithmic
    @date 20181121
    @copyright Allegorithmic. All rights reserved.
*/

/* Windows headers */
#include <Windows.h>

#include <substance/connector/details/thread.h>

connector_mutex_t connector_mutex_create(void)
{
    connector_mutex_t mutex = SRWLOCK_INIT;
    return mutex;
}

void connector_mutex_lock(connector_mutex_t *mutex)
{
    AcquireSRWLockExclusive(mutex);
}

void connector_mutex_unlock(connector_mutex_t *mutex)
{
    ReleaseSRWLockExclusive(mutex);
}

void connector_mutex_destroy(connector_mutex_t *mutex)
{
    /* SRW Locks do not need to be explicitly destroyed on windows */
}

connector_thread_t connector_thread_create(connector_thread_routine routine, void *arg)
{
    HANDLE thread = CreateThread(NULL, 0u, routine, arg, 0, NULL);

    return thread;
}

void connector_thread_join(connector_thread_t *thread)
{
    WaitForSingleObject(*thread, INFINITE);
}

void connector_thread_destroy(connector_thread_t *thread)
{
    CloseHandle(*thread);
    *thread = NULL;
}

void connector_condition_create(connector_cond_t *cond)
{
    InitializeConditionVariable(cond);
}

void connector_condition_destroy(connector_cond_t *cond)
{
    /* Stub for now, do nothing */
}

void connector_condition_wait(connector_cond_t *cond, connector_mutex_t *lock)
{
    SleepConditionVariableSRW(cond, lock, 0xffff, 0);
}

void connector_condition_signal(connector_cond_t *cond)
{
    WakeConditionVariable(cond);
}

void connector_condition_broadcast(connector_cond_t *cond)
{
    WakeAllConditionVariable(cond);
}

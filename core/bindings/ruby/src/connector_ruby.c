/** @file connector_ruby.c
    @brief Defines the Ruby bindings for the connector API
    @author Galen Helfter - Allegorithmic
    @date 20181221
    @copyright Allegorithmic. All rights reserved.
*/

#include <ruby.h>
#include <ruby/thread.h>

#include <stdio.h>
#include <string.h>

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/connector.h>

/* We will need to reuse and compile with the threading implementation for
 * native synchronization, as well as include the locked queue implementation
 * for the callback queue. */
#include <substance/connector/details/atomic.h>
#include <substance/connector/details/locked_queue.h>
#include <substance/connector/details/thread.h>

typedef struct _connector_rb_callback_payload
{
    unsigned int context;
    const char *message;
} connector_rb_callback_payload_t;

static connector_locked_queue_t callback_queue;
static VALUE callback_thread;

/* Conditions for signaling the Ruby thread */
static connector_mutex_t callback_queue_lock;
static connector_cond_t callback_condition;

/* Both 32 bit and 64 bit processors should have support for atomic 32 bit
 * integers */
static uint32_t connector_rb_shutdown_code = 0u;

/* Static function declarations */
static void connector_rb_trampoline(unsigned int context, const char *message_type,
                               const char *message);
static void* connector_rb_thread_locked_routine(void *arg);
static void* connector_rb_thread_unlocked_routine(void *arg);

/* Native trampoline function for Connector to enter Ruby */
static void connector_rb_trampoline(unsigned int context, const char *message_type,
                               const char *message)
{
    connector_locked_queue_node_t *node = NULL;

    /* This will be a bit hacky - We will need to push this somewhere the Ruby
     * thread can read it, then notify the thread. Unlike Python, Ruby does not
     * currently have a way for a foreign thread to acquire its GVL. */

    /* On shutdown, return and do nothing */
    if (connector_rb_shutdown_code == 0u)
    {
        /* Note:: A lock-free queueing mechanism may be significantly more
         * performant than performing such a heated lock */
        /* Append to the queue */
        connector_locked_enqueue(&callback_queue, node);

        /* Acquire the Ruby thread context lock before signaling it */
        connector_mutex_lock(&callback_queue_lock);

        /* Signal condition variable to awaken the Ruby thread */
        connector_condition_broadcast(&callback_condition);

        /* Release lock */
        connector_mutex_unlock(&callback_queue_lock);
    }
}

static void* connector_rb_thread_locked_routine(void *arg)
{
    /* Safely entered back into Ruby - can now call API functions again */
    connector_locked_queue_node_t *node = (connector_locked_queue_node_t*) arg;

    if (node != NULL)
    {
        /* Acquire the registered callback from the stored map */
    }

    return NULL;
}

static void* connector_rb_thread_unlocked_routine(void *arg)
{
    /* Unlocked code - we must not call any Ruby code here. */
    connector_locked_queue_node_t *node = NULL;

    while (connector_rb_shutdown_code == 0u)
    {
        connector_mutex_lock(&callback_queue_lock);

        while (node == NULL)
        {
            connector_condition_wait(&callback_condition, &callback_queue_lock);

            if (connector_rb_shutdown_code != 0u)
            {
                /* Return from the unlocked thread back to the original thread
                 * call */
                connector_mutex_unlock(&callback_queue_lock);
                goto thread_exit;
            }

            /* Perform dequeue operation */
            node = connector_locked_dequeue(&callback_queue);
        }

        /* NOTE:: Using these locks for the queue is kludgy as well, again see
         * if the queue can be made non-blocking/atomic */
        connector_mutex_unlock(&callback_queue_lock);

        while (node != NULL && connector_rb_shutdown_code == 0u)
        {
            /* Enter back into managed Ruby code to perform callback */
            rb_thread_call_with_gvl(connector_rb_thread_locked_routine,
                                    (void*) node);

            /* Perform dequeue operation to acquire a new callback event */
            node = connector_locked_dequeue(&callback_queue);
        }
    }

thread_exit:
    fprintf(stdout, "Shutting down...\n");

    return NULL;
}

static VALUE connector_rb_thread_routine(VALUE arg)
{
    /* Enter an unmanaged point for most of the threading operation, allowing
     * Ruby to continue without blocking. This call must enter back into a
     * managed section before performing any Ruby calls. */
    rb_thread_call_without_gvl(connector_rb_thread_unlocked_routine,
                               NULL, NULL, NULL);

    return Qnil;
}

static VALUE connector_rb_version()
{
    const char *version = substance_connector_version();
    return rb_str_new(version, strlen(version));
}

static VALUE connector_rb_init()
{
    unsigned int retcode = substance_connector_init();

    /* Register trampoline function */
    if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        CONNECTOR_ATOMIC_SET_0(connector_rb_shutdown_code);
        retcode = substance_connector_add_trampoline(connector_rb_trampoline);

        /* Start callback thread. Note that this must be a thread managed
         * by Ruby for it to properly work with their GVL. */
        callback_thread = rb_thread_create(connector_rb_thread_routine, NULL);
    }

    return UINT2NUM(retcode);
}

static VALUE connector_rb_shutdown()
{
    unsigned int retcode = 0u;

    /* Set shutdown code - will force all callbacks to halt */
    CONNECTOR_ATOMIC_SET_1(connector_rb_shutdown_code);

    /* Acquire condition lock - keeps callbacks from occuring
     * during the shutdown */
    connector_mutex_lock(&callback_queue_lock);

    retcode = substance_connector_shutdown();

    connector_condition_broadcast(&callback_condition);

    /* Release queue lock */
    connector_mutex_unlock(&callback_queue_lock);

    return UINT2NUM(retcode);
}

static VALUE connector_rb_add_callback(VALUE message_type, VALUE callback)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_SUCCESS;

    /* Acquire the ID of the callback from the object */

    return UINT2NUM(retcode);
}

SUBSTANCE_CONNECTOR_EXPORT
void Init_connector()
{
    /* Set up module structure */
    VALUE substance_mod = rb_define_module("Substance");
    VALUE connector_mod = rb_define_module_under(substance_mod, "Connector");

    /* Initialize queue and mutex */
    callback_queue_lock = connector_mutex_create();
    connector_condition_create(&callback_condition);

    /* Define module methods */
    rb_define_module_function(connector_mod, "version", connector_rb_version, 0);
    rb_define_module_function(connector_mod, "init", connector_rb_init, 0);
    rb_define_module_function(connector_mod, "shutdown", connector_rb_shutdown, 0);
    rb_define_module_function(connector_mod, "add_callback",
                              connector_rb_add_callback, 2);
}

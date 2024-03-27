/** @file callbacks.h
    @brief Stores the list of trampoline functions for the language bindings
    @author Galen Helfter - Allegorithmic
    @date 20181127
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_CALLBACKS_H
#define _SUBSTANCE_CONNECTOR_CALLBACKS_H

#include <substance/connector/types.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* The definition of trampoline used here is:
 *     A subroutine that is used to convert the caller's convention into
 *     the callee's convention.
 * In this case, the trampoline system will be used by the language bindings
 * to convert the C function pointer into a more specific call at that
 * language's level. This function is intended to jump from the C level
 * into a binding function. */

/* Initializes the trampoline system, initializing the internal trampoline
 * list and getting it ready to accept new language binding trampoline
 * functions. */
unsigned int connector_init_trampoline_subsystem(void);

/* Shuts down the trampoline system, clearing any internal data and emptying
 * any lists. */
unsigned int connector_shutdown_trampoline_subsystem(void);

/* Iterate over the list of trampoline functions, calling each one with the
 * context, message_type and message parameters. */
unsigned int connector_notify_trampolines(unsigned int context,
                                     const substance_connector_uuid_t *type,
                                     const char *message);

/* Adds a trampoline function pointer to the trampoline list. After this, any
 * call to notify trampolines will call the given function. */
unsigned int connector_add_trampoline(substance_connector_trampoline_fp trampoline);

/* Removes the given trampoline function pointer from the list. After this,
 * the trampoline will no longer be called by any callbacks. */
unsigned int connector_remove_trampoline(substance_connector_trampoline_fp trampoline);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_CALLBACKS_H */

/** @file trampoline.h
    @brief Python trampoline functionality to go from C to Python
    @author Galen Helfter - Adobe
    @date 20190915
    @copyright Adobe. All rights reserved.
*/

#pragma once

#include <Python.h>

#include <substance/connector/types.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/** @brief Trampoline function to register with the connector C library
    @param context Integer descriptor representing the connection
    @param message_type uuid representing the type of message received
    @param message String containing the message payload
*/
void connector_python_trampoline(unsigned int context,
                            const substance_connector_uuid_t *message_type,
                            const char *message);

/** @brief Register a Python function as a trampoline for connector to call into
    @param self Python object that the method is called on
    @param args List of arguments passed by Python
    @return Python object result, which is Py_None on success or NULL on failure
*/
PyObject* connector_python_register_trampoline(PyObject *self, PyObject *args);

/** @brief Clear any references to any Python objects on shutdown */
void connector_python_shutdown_trampoline(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

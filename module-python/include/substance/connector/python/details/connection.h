/** @file connection.h
    @brief Python bindings for connection-based functions
    @author Galen Helfter - Adobe
    @date 20191213
    @copyright Adobe. All rights reserved.
*/

#pragma once

#include <Python.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/** @brief Python wrapper around the Connctor open tcp call
    @param self Python object representing the object the method is invoked on
    @param args Python object representing the argument list
    @return Python object representing the connection context
*/
PyObject* connector_python_open_tcp(PyObject *self, PyObject *args);

/** @brief Python wrapper around the connector write message call
    @param self Python object representing the object the method is invoked on
    @param args Python object representing the argument list
    @return Python object representing success, 0 on success and non-zero
            on failure
*/
PyObject* connector_python_write_message(PyObject *self, PyObject *args);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

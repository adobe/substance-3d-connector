/** @file broadcast.h
    @brief Python bindings for broadcast/default connector library functions
    @author Galen Helfter - Adobe
    @date 20190915
    @copyright Adobe. All rights reserved.
*/

#pragma once

#include <Python.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/** @brief Python wrapper around the connector open default tcp call
    @param self Python object representing the object the method is invoked on
    @param args Python object representing the argument list
    @return Python object representing the context, throws an exception
            on failure
*/
PyObject* connector_python_open_default_tcp(PyObject *self, PyObject *args);

/** @brief Python wrapper around the connector broadcast tcp call
    @param self Python object representing the object the method is invoked on
    @param args Python object representing the argument list
    @return Python object representing the errorcode result
*/
PyObject* connector_python_broadcast_tcp(PyObject *self, PyObject *args);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

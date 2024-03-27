/** @file initshutdown.h
    @brief Python bindings for initialization and shutdown functions
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

/** @brief Python wrapper around the connector initialization call
    @param self Python object representing the object the method is invoked on
    @param args Python object representing the argument list
    @return Python object representing success, 0 on success and non-zero
            on failure
*/
PyObject* connector_python_init(PyObject *self, PyObject *args);

/** @brief Python wrapper around the connector shutdown call
    @param self Python object representing the object the method is invoked on
    @param args Python object representing the argument list
    @return Python object representing success, 0 on success and non-zero
            on failure
*/
PyObject* connector_python_shutdown(PyObject *self, PyObject *args);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

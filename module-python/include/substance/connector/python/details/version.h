/** @file version.h
    @brief Function to acquire the version of the connector library
    @author Galen Helfter - Adobe
    @date 20191024
    @copyright Adobe. All rights reserved.
*/

#pragma once

#include <Python.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/** @brief Python binding to acquire the version
    @param self Python object that the method is called on
    @param args List of arguments passed by Python
    @return Python string object representing the semantic version
*/
PyObject* connector_python_version(PyObject *self, PyObject *args);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

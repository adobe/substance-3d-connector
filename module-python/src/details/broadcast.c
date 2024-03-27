/** @file connection.c
    @brief Python bindings for broadcast/default connector library functions
    @author Galen Helfter - Adobe
    @date 20190915
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/python/details/connection.h>

#include <Python.h>

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/connector.h>

PyObject* connector_python_open_default_tcp(PyObject *self, PyObject *args)
{
    PyObject *result = NULL;

    unsigned int errorcode = 0u;
    unsigned int context = 0u;

    errorcode = substance_connector_open_default_tcp(&context);

    if (errorcode == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = PyLong_FromUnsignedLong(context);
    }
    else
    {
#if PY_MAJOR_VERSION >= 3  /* Python 3 */
        PyErr_SetString(PyExc_ConnectionError, "Failed to open TCP connection.");
#else /* Python 2 */
        PyErr_SetString(PyExc_IOError, "Failed to open TCP connection.");
#endif
    }

    return result;
}

PyObject* connector_python_broadcast_tcp(PyObject *self, PyObject *args)
{
    PyObject *result = NULL;
    unsigned int errorcode = 0u;

    errorcode = substance_connector_broadcast_tcp();

    result = PyLong_FromUnsignedLong(errorcode);

    return result;
}

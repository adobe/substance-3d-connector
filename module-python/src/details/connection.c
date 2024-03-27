/** @file connection.c
    @brief Python bindings for connection-based functions
    @author Galen Helfter - Adobe
    @date 20191213
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/python/details/connection.h>

#include <substance/connector/errorcodes.h>
#include <substance/connector/connector.h>

#include <substance/connector/python/details/uuid.h>

PyObject* connector_python_open_tcp(PyObject *self, PyObject *args)
{
    unsigned long errorcode = 0ul;
    unsigned long port = 0ul;
    unsigned int context = 0u;
    PyObject *result = NULL;

    /* Extract out the port from the argument list */
    if (PyArg_ParseTuple(args, "k", &port) != 0)
    {
        errorcode = substance_connector_open_tcp((unsigned int) port, &context);

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
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "A port number must be passed in");
    }

    return result;
}

PyObject* connector_python_write_message(PyObject *self, PyObject *args)
{
    PyObject *result = NULL;

    /* Arguments to acquire from Python */
    unsigned int context = 0u;
    PyObject *uuid = NULL;
    const char *message = NULL;
    substance_connector_uuid_t message_type;

    unsigned long errorcode = SUBSTANCE_CONNECTOR_ERROR;

    if (PyArg_ParseTuple(args, "k|O|s", &context, &uuid, &message) != 0)
    {
        connector_python_assemble_uuid(uuid, &message_type);

        errorcode = substance_connector_write_message(context, &message_type,
                                                 message);

        result = PyLong_FromUnsignedLong(errorcode);
    }

    return result;
}

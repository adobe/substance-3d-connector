/** @file trampoline.c
    @brief Python trampoline functionality to go from C to Python
    @author Galen Helfter - Adobe
    @date 20190915
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/python/details/trampoline.h>

#include <Python.h>
#include <stdint.h>

#include <substance/connector/python/details/uuid.h>

static PyObject *trampoline = NULL;

void connector_python_trampoline(unsigned int context,
                            const substance_connector_uuid_t *message_type,
                            const char *message)
{
    PyObject *result = NULL;
    PyObject *context_obj = NULL;
    PyObject *message_obj = NULL;
    PyObject *uuid_obj = NULL;

    PyGILState_STATE gil_state;

    /* Acquire the Global Interpreter Lock */
    gil_state = PyGILState_Ensure();

    if (trampoline != NULL)
    {
        context_obj = PyLong_FromUnsignedLong((unsigned long) context);
        message_obj = PyUnicode_FromString(message);
        uuid_obj = PyList_New(0);

        connector_python_split_uuid(message_type, uuid_obj);

        result = PyObject_CallFunctionObjArgs(trampoline, context_obj,
                                              uuid_obj, message_obj, NULL);

        /* We've received a reference to the result of the function call.
         * This new reference must be decremented. It may have also returned
         * NULL if an exception was called, so Py_XDECREF must be used. */
        Py_XDECREF(result);

        /* Decrease the references of all created objects and set
         * their pointers to NULL */
        Py_DECREF(context_obj);
        Py_DECREF(message_obj);
        Py_DECREF(uuid_obj);

        context_obj = NULL;
        message_obj = NULL;
        uuid_obj = NULL;
    }

    /* Release the Global Interpreter Lock - do not call the Python API after */
    PyGILState_Release(gil_state);
}

PyObject* connector_python_register_trampoline(PyObject *self, PyObject *args)
{
    PyObject *callback = NULL;

    if (PyArg_ParseTuple(args, "O", &callback) != 0)
    {
        if (trampoline != NULL)
        {
            /* If a previously set trampoline is to be overridden, then the
             * reference stored to the existing one must be decremented before
             * clearing the variable. */
            Py_DECREF(trampoline);
            trampoline = NULL;
        }

        /* Increment the reference count to the callback object while retaining
         * a pointer to it in the module */
        Py_INCREF(callback);
        trampoline = callback;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

void connector_python_shutdown_trampoline(void)
{
    Py_XDECREF(trampoline);
    trampoline = NULL;
}

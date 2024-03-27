/** @file initshutdown.h
    @brief Python bindings for initialization and shutdown functions
    @author Galen Helfter - Adobe
    @date 20191213
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/python/details/initshutdown.h>

#include <substance/connector/connector.h>
#include <substance/connector/python/details/trampoline.h>

PyObject* connector_python_init(PyObject *self, PyObject *args)
{
    unsigned long errorcode = 0ul;
    char *name = NULL;

    if (PyArg_ParseTuple(args, "s", &name) != 0)
    {
        errorcode = (unsigned long) substance_connector_init(name);

        /* Add internal trampoline function for jumping into Python from C */
        substance_connector_add_trampoline(connector_python_trampoline);
    }

    return PyLong_FromUnsignedLong(errorcode);
}

PyObject* connector_python_shutdown(PyObject *self, PyObject *args)
{
    unsigned long errorcode = 0ul;

    /* Remove trampoline function from internal API */
    substance_connector_remove_trampoline(connector_python_trampoline);

    /* Remove references to the Python trampoline object */
    connector_python_shutdown_trampoline();

    errorcode = (unsigned long) substance_connector_shutdown();

    return PyLong_FromUnsignedLong(errorcode);
}

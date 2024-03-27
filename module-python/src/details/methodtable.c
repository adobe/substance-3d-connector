/** @file methodtable.c
    @brief Contains the Python module method table for the connector bindings
    @author Galen Helfter - Adobe
    @date 20191213
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/python/details/methodtable.h>

#include <substance/connector/python/details/broadcast.h>
#include <substance/connector/python/details/connection.h>
#include <substance/connector/python/details/initshutdown.h>
#include <substance/connector/python/details/trampoline.h>
#include <substance/connector/python/details/version.h>

/* Define the module function table */
PyMethodDef connector_python_module_methods[] =
{
    {
        "version",
        connector_python_version,
        METH_VARARGS,
        "Returns the version of the native connector library"
    },
    {
        "init",
        connector_python_init,
        METH_VARARGS,
        "Initializes the connector module, returning an error if already initialized"
    },
    {
        "shutdown",
        connector_python_shutdown,
        METH_VARARGS,
        "Shuts down the connector module, returning an error if already shut down"
    },
    {
        "register_trampoline",
        connector_python_register_trampoline,
        METH_VARARGS,
        "Registers a trampoline function for the module to call back into Python"
    },
    {
        "write_message",
        connector_python_write_message,
        METH_VARARGS,
        "Writes the given message to the specified context"
    },
    {
        "open_tcp",
        connector_python_open_tcp,
        METH_VARARGS,
        "Given a port, creates an open TCP context and returns the integer id"
    },
    {
        "open_default_tcp",
        connector_python_open_default_tcp,
        METH_VARARGS,
        "Opens a new tcp context, taking whatever port is available from the OS"
    },
    {
        "broadcast_tcp",
        connector_python_broadcast_tcp,
        METH_VARARGS,
        "Performs a broadcast connection to all available instances of connector"
    },
    {NULL, NULL, 0, NULL} /* Null terminated sentinal */
};

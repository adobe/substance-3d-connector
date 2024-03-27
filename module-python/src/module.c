/** @file module.c
    @brief Defines the Python 2 and 3 bindings for the connector API
    @author Galen Helfter - Allegorithmic
    @date 20181026
    @copyright Allegorithmic. All rights reserved.
*/

#include <Python.h>

#include <substance/connector/python/details/methodtable.h>

#define SUBSTANCE_CONNECTOR_PYTHON_DOCSTRING "Substance connector Python module"

#if defined(_WIN64) || defined(_WIN32)
#define CONNECTOR_SOURCE_EXPORT __declspec(dllexport)
#else
#define CONNECTOR_SOURCE_EXPORT __attribute__((visibility("default")))
#endif

#if PY_MAJOR_VERSION >= 3  /* Python 3 */
static struct PyModuleDef connector_module =
{
    PyModuleDef_HEAD_INIT,
    "connector",
    SUBSTANCE_CONNECTOR_PYTHON_DOCSTRING,
    -1,
    connector_python_module_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

/* Function must be named PyInit_{module name} in Python 3, as that is the
 * exported symbol that Python will attempt to bind to */
CONNECTOR_SOURCE_EXPORT
PyMODINIT_FUNC PyInit_connector(void)
{
    return PyModule_Create(&connector_module);
}
#else  /* Python 2 */
/* Function must be named init{module name}, as that is the exported symbol
 * that Python will attempt to bind to */
CONNECTOR_SOURCE_EXPORT
PyMODINIT_FUNC initconnector(void)
{
    PyObject *m = NULL;

    m = Py_InitModule("connector", connector_python_module_methods);

    if (m != NULL)
    {
        /* Do nothing for now */
    }
}
#endif

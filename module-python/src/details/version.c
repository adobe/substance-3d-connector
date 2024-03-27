/** @file version.c
    @brief Function to acquire the version of the connector library
    @author Galen Helfter - Adobe
    @date 20191024
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/python/details/version.h>

#include <substance/connector/connector.h>

PyObject* connector_python_version(PyObject *self, PyObject *args)
{
    const char *string = substance_connector_version();

    return PyUnicode_FromString(string);
}

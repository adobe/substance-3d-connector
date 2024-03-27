/** @file uuid.c
    @brief Contains functions for working with uuids and converting to and from
           Python and native data structures
    @author Galen Helfter - Adobe
    @date 20191218
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/python/details/uuid.h>

void connector_python_assemble_uuid(PyObject *list, substance_connector_uuid_t *uuid)
{
    size_t i = 0u;
    PyObject *element = NULL;

    for (i = 0u; i < 4u; ++i)
    {
        element = PyList_GetItem(list, i);

        /* Python will set an exception if the list is not of the
         * expected size */
        if (element == NULL)
        {
            break;
        }

        uuid->elements[i] = PyLong_AsUnsignedLong(element);
    }
}

void connector_python_split_uuid(const substance_connector_uuid_t *uuid, PyObject *list)
{
    size_t i = 0u;
    PyObject *object = NULL;

    for (i = 0u; i < 4u; ++i)
    {
        object = PyLong_FromUnsignedLong(uuid->elements[i]);
        PyList_Append(list, object);

        /* Appending to a Python list increases the reference count, and
         * must be decremented here */
        Py_DECREF(object);
    }
}

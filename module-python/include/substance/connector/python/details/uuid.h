/** @file uuid.h
    @brief Contains functions for working with uuids and converting to and from
           Python and native data structures
    @author Galen Helfter - Adobe
    @date 20191218
    @copyright Adobe. All rights reserved.
*/

#pragma once

#include <Python.h>

#include <substance/connector/types.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/** @brief Retrieves the entries from a Python list and converts to a uuid
    @param list Python list object, a list of four integers
    @param uuid Native uuid structure to store the value in
*/
void connector_python_assemble_uuid(PyObject *list, substance_connector_uuid_t *uuid);

/** @brief Converts a native uuid structure to a Python list
    @param uuid Native uuid structure
    @param list Python list object to append the integer values to
*/
void connector_python_split_uuid(const substance_connector_uuid_t *uuid, PyObject *list);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

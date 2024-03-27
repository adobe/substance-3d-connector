/** @file methodtable.h
    @brief Contains the Python module method table for the connector bindings
    @author Galen Helfter - Adobe
    @date 20191213
    @copyright Adobe. All rights reserved.
*/

#pragma once

#include <Python.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/** @brief Table containing all the Python methods for the module */
extern PyMethodDef connector_python_module_methods[];

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/** @file uuid_utils.c
    @brief Contains utility functions for working with UUIDs
    @author Galen Helfter - Allegorithmic
    @date 20190420
    @copyright Allegorithmic. All rights reserved.
*/

#include <stdint.h>

#include <substance/connector/types.h>
#include <substance/connector/details/uuid_utils.h>

int connector_compare_uuid(const struct _substance_connector_uuid *id0,
                      const struct _substance_connector_uuid *id1)
{
    int result = 0;
    size_t i = 0u;

    for (i = 0u; i < 4u; ++i)
    {
        if (id0->elements[i] < id1->elements[i])
        {
            result = -1;
            break;
        }
        else if (id0->elements[i] > id1->elements[i])
        {
            result = 1;
            break;
        }
    }

    return result;
}

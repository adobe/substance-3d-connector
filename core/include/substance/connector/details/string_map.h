/** @file string_map.h
    @brief Contains an implementation for a map with a string key
    @author Galen Helfter - Allegorithmic
    @date 20181221
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_STRING_MAP_H
#define _SUBSTANCE_CONNECTOR_STRING_MAP_H

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* Opaque type to keep the entire implementation encapsulated */
typedef struct _connector_string_map connector_string_map_t;

/* Initializes and creates a new string map. This function will return NULL if
 * the map failed to initialize. */
connector_string_map_t* connector_string_map_init();

/* Destroys the given map object, clearing any resources used by it. The
 * pointer is invalid after calling this function. */
unsigned int connector_string_map_destroy(connector_string_map_t *map);

/* Looks up the key in the map. Returns a copy of the entry if found, or NULL
 * if the key was not found. The returned entry must be freed by the caller
 * using connector_free when it is finished. */
char* connector_string_map_search(connector_string_map_t *map, const char *key);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_STRING_MAP_H */

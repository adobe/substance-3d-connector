/** @file string_map.c
    @brief Contains an implementation for a map with a string key
    @author Galen Helfter - Allegorithmic
    @date 20190108
    @copyright Allegorithmic. All rights reserved.
*/

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/string_map.h>
#include <substance/connector/details/string_utils.h>

/* Use a 64-bit hash when built on a 64-bit machine, and a 32-bit when built
 * on a 32-bit machine. */
#if defined(SUBSTANCE_CONNECTOR_ARCH_64)
#define CONNECTOR_HASH_TYPE uint64_t
#define CONNECTOR_FNV_OFFSET_BASIS 0xcbf29ce484222325
#define CONNECTOR_FNV_PRIME        0x100000001b3
#elif defined(SUBSTANCE_CONNECTOR_ARCH_32)
#define CONNECTOR_HASH_TYPE uint32_t
#define CONNECTOR_FNV_OFFSET_BASIS 0x811c9dc5
#define CONNECTOR_FNV_PRIME        0x1000193
#endif

/* Initial map size */
#ifndef CONNECTOR_MAP_BUCKET_SIZE
#define CONNECTOR_MAP_BUCKET_SIZE 512u
#endif /* CONNECTOR_MAP_BUCKET_SIZE */

/* Define internal data structure in the module, as to allow for the
 * structure and implementation to be changed. This is currently
 * implemented using a hash map data structure. */
struct _connector_string_map
{
    char **keys;
    char **entries;
    size_t bucket_size;
};

/* Static function declarations */
static CONNECTOR_HASH_TYPE hash_to_index(CONNECTOR_HASH_TYPE hashcode, size_t length);
static CONNECTOR_HASH_TYPE compute_fnv1a_hash(const char *str);

/* Compute an FNV-1a hash for the string. This is not a cryptographic hash
 * function, and is only used for computing entries into the hash table. */
static CONNECTOR_HASH_TYPE compute_fnv1a_hash(const char *str)
{
    CONNECTOR_HASH_TYPE hash = CONNECTOR_FNV_OFFSET_BASIS;
    size_t string_length = 0u;
    size_t i = 0u;

    string_length = strlen(str);
    for (i = 0u; i < string_length; ++i)
    {
        hash ^= str[i];
        hash *= CONNECTOR_FNV_PRIME;
    }

    return hash;
}

static CONNECTOR_HASH_TYPE hash_to_index(CONNECTOR_HASH_TYPE hashcode, size_t length)
{
    return hashcode % length; /* Convert to an index */
}

connector_string_map_t* connector_string_map_init()
{
    connector_string_map_t *map = connector_allocate(sizeof(connector_string_map_t));

    if (map != NULL)
    {
        memset(map, 0x00, sizeof(connector_string_map_t));

        /* Allocate a single array for both the keys and entries */
        map->keys = connector_allocate(2 * sizeof(char*) * CONNECTOR_MAP_BUCKET_SIZE);
        map->entries = map->keys + CONNECTOR_MAP_BUCKET_SIZE;
        map->bucket_size = CONNECTOR_MAP_BUCKET_SIZE;

        memset(map->keys, 0x00, 2 * sizeof(char*) * CONNECTOR_MAP_BUCKET_SIZE);
    }

    return map;
}

unsigned int connector_string_map_destroy(connector_string_map_t *map)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    size_t i = 0u;

    if (map != NULL)
    {
        /* Clear all entries in the buckets */
        for (i = 0u; i < map->bucket_size; ++i)
        {
            connector_free(map->keys[i]);
            map->keys[i] = NULL;
            map->entries[i] = NULL;
        }

        /* Delete the internal table and the map structure */
        connector_free(map->keys);
        connector_free(map);

        map = NULL;

        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}

unsigned int connector_string_map_insert(connector_string_map_t *map, const char *key,
                                    const char *val)
{
    CONNECTOR_HASH_TYPE hashcode = 0u;
    CONNECTOR_HASH_TYPE hash = 0u;

    /* Compute the hash for the key */
    hashcode = compute_fnv1a_hash(key);
    hash = hash_to_index(hashcode, map->bucket_size);

    /* Insert the key/value pair into the table */

    return 0u;
}

char* connector_string_map_search(connector_string_map_t *map, const char *key)
{
    char *result = NULL;
    CONNECTOR_HASH_TYPE hashcode = 0u;
    CONNECTOR_HASH_TYPE hash = 0u;

    if (key != NULL && map != NULL)
    {
        /* Compute the hash for the key */
        hashcode = compute_fnv1a_hash(key);
        hash = hash_to_index(hashcode, map->bucket_size);

        /* Check whether the key is in the slot */
        if (map->keys[hash] != NULL && strcmp(map->keys[hash], key) == 0)
        {
            /* Return a copy of the entry, as the table could be modified
             * while the caller might need it. */
            result = connector_strdup(map->entries[hash]);
        }
    }

    return result;
}

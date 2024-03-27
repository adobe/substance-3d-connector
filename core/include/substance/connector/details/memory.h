/** @file memory.h
    @brief Contains memory operations, such as for shared memory, etc.
    @author Galen Helfter - Allegorithmic
    @date 20181030
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_DETAILS_MEMORY_H
#define _SUBSTANCE_CONNECTOR_DETAILS_MEMORY_H

#include <stddef.h>

#include <substance/connector/common.h>
#include <substance/connector/types.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

typedef struct _connector_shared_mem
{
    void *handle; /* Always call read and use this handle */

    size_t size;  /* Size of the shared memory handle */

#if defined(SUBSTANCE_CONNECTOR_WIN32)
    void *file_map; /* Only used on Windows - stores mapped file handle */
    wchar_t *path;  /* Only used on Windows - stores the path to the memory */
#elif defined(SUBSTANCE_CONNECTOR_POSIX)
    char *path;    /* Only used on POSIX - stores the path to the memory */
    int fd;        /* Only used on POSIX - file descriptor for open memory */
#endif
} connector_shared_mem_t;

/* Resets the allocators to the system default */
unsigned int connector_clear_allocators(void);

/* Sets the internal memory allocator to the given function pointer. An
 * appropriate deallocator should also be set with a connector_set_deallocator
 * call. */
void connector_set_allocator(substance_connector_memory_allocate_fp allocator);

/* Sets the internal deallocator or free function. This should be set to
 * match the memory allocator function. */
void connector_set_deallocator(substance_connector_memory_free_fp deallocator);

/* Standard allocator for system memory, allocating memory with the set
 * internal allocator. */
void* connector_allocate(size_t size);

/* Array allocator function, operating similarly to the connector_allocate
 * function. */
void* connector_array_allocate(size_t num, size_t size);

/* Deallocates the given memory, using the internal deallocator. This should
 * have been allocated using the connector_allocate function. */
void connector_free(void *ptr);

/* Memory handling for shared memory objects */
/* Acquisition expects that the size member has been set in the shared
 * memory object */
void connector_acquire_shared_memory(connector_shared_mem_t *mem);

/* Frees the given shared memory object. Will set all pointers to NULL and
 * clear the size. */
void connector_free_shared_memory(connector_shared_mem_t *mem);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_DETAILS_MEMORY_H */

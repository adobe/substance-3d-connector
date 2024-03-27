/** @file memory.c
    @brief Contains memory operations, such as for shared memory, etc.
    @author Galen Helfter - Allegorithmic
    @date 20181030
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/memory.h>

#if defined(SUBSTANCE_CONNECTOR_POSIX)
#include <sys/mman.h>
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#include <windows.h>
#endif

#include <stdlib.h>

static substance_connector_memory_allocate_fp connector_memory_allocate = malloc;
static substance_connector_memory_free_fp connector_memory_free = free;

unsigned int connector_clear_allocators(void)
{
    connector_memory_allocate = malloc;
    connector_memory_free = free;

    return SUBSTANCE_CONNECTOR_SUCCESS;
}

void connector_set_allocator(substance_connector_memory_allocate_fp allocator)
{
    connector_memory_allocate = allocator;
}

void connector_set_deallocator(substance_connector_memory_free_fp deallocator)
{
    connector_memory_free = deallocator;
}

void* connector_allocate(size_t size)
{
    return connector_memory_allocate(size);
}

void* connector_array_allocate(size_t num, size_t size)
{
    return connector_memory_allocate(num * size);
}

void connector_free(void *ptr)
{
    connector_memory_free(ptr);
}

#if defined(SUBSTANCE_CONNECTOR_POSIX)
/* Posix functions for handling shared memory */
void connector_acquire_shared_memory(connector_shared_mem_t *mem)
{
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED;

    /* TODO:: Implement shared memory */
    mem->handle = mmap(NULL, mem->size, prot, flags, -1, 0);
}

void connector_free_shared_memory(connector_shared_mem_t *mem)
{
    munmap(mem->handle, mem->size);
    mem->handle = NULL;
    mem->size = 0u;
    mem->path = NULL;
    mem->fd = 0;
}
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
/* Windows functions for handling shared memory */
void connector_acquire_shared_memory(connector_shared_mem_t *mem)
{
    mem->file_map = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,
                                      PAGE_READWRITE, 0, (DWORD) mem->size,
                                      NULL); /* #TODO:: Handle file name */
    mem->handle = MapViewOfFile(mem->file_map, FILE_MAP_ALL_ACCESS, 0, 0, mem->size);
}

void connector_free_shared_memory(connector_shared_mem_t *mem)
{
    UnmapViewOfFile(mem->handle);
    CloseHandle(mem->file_map);
    mem->handle = NULL;
    mem->size = 0u;
    mem->file_map = NULL;
    mem->path = NULL;
}
#endif

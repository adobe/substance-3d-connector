/** @file string_utils.h
    @brief Contains internal string utility functions
    @author Galen Helfter - Allegorithmic
    @date 20190107
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_DETAILS_STRING_UTILS_H
#define _SUBSTANCE_CONNECTOR_DETAILS_STRING_UTILS_H

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* Implementation of the memrchr function, which returns a pointer to the
 * last instance of the character c in the buffer. If the buffer does not
 * contain the character, the function returns NULL. */
void* connector_memrchr(const void *buffer, int c, size_t length);

/* Duplicates the string using the internally set allocator. The returned
 * string must be freed by the allocator. This assumes a UTF-8 string for
 * all platforms, and is not suitable for duplicating paths */
char* connector_strdup(const char *s);

/* Performs the same operations as strndup, but with the internal
 * allocator used to allocate the new memory. The returned string
 * must be freed by the caller. */
char* connector_strndup(const char *s, size_t n);

/* Similar to the chomp function in Perl. This function will remove all of the
 * instances of the delimeter character from the end of the string. It will
 * return the number of characters removed. */
size_t connector_chomp(char *s, char delim);

/* Performs a search for the length of the string, checking whether there is
 * a null terminator in the buffer. */
unsigned int connector_untrusted_strlen(const char *buffer, size_t buffer_length,
                                   size_t *length);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_DETAILS_STRING_UTILS_H */

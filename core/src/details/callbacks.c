/** @file callbacks.c
    @brief Stores the list of trampoline functions for the language bindings
    @author Galen Helfter - Allegorithmic
    @date 20181127
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/types.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/callbacks.h>
#include <substance/connector/details/memory.h>

typedef struct _connector_trampoline_node
{
    substance_connector_trampoline_fp trampoline;
    struct _connector_trampoline_node *next;
} connector_trampoline_node_t;

typedef struct _connector_trampoline_list
{
    connector_trampoline_node_t *front;
} connector_trampoline_list_t;

static connector_trampoline_list_t trampoline_list;

unsigned int connector_init_trampoline_subsystem(void)
{
    trampoline_list.front = NULL;

    return SUBSTANCE_CONNECTOR_SUCCESS;
}

unsigned int connector_shutdown_trampoline_subsystem(void)
{
    connector_trampoline_node_t *next = NULL;
    connector_trampoline_node_t *node = trampoline_list.front;

    while (node != NULL)
    {
        next = node->next;
        connector_free(node);
        node = next;
    }

    return SUBSTANCE_CONNECTOR_SUCCESS;
}

unsigned int connector_notify_trampolines(unsigned int context,
                                     const substance_connector_uuid_t *type,
                                     const char *message)
{
    connector_trampoline_node_t *node = trampoline_list.front;

    while (node != NULL)
    {
        node->trampoline(context, type, message);

        node = node->next;
    }

    return SUBSTANCE_CONNECTOR_SUCCESS;
}

unsigned int connector_add_trampoline(substance_connector_trampoline_fp trampoline)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_INVALID;
    connector_trampoline_node_t *node = NULL;

    if (trampoline != NULL)
    {
        node = connector_allocate(sizeof(connector_trampoline_node_t));

        node->trampoline = trampoline;
        node->next = trampoline_list.front;

        trampoline_list.front = node;

        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}

unsigned int connector_remove_trampoline(substance_connector_trampoline_fp trampoline)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_INVALID;
    connector_trampoline_node_t *prev = NULL;
    connector_trampoline_node_t *node = trampoline_list.front;

    while (node != NULL && trampoline != NULL)
    {
        if (node->trampoline == trampoline)
        {
            if (prev != NULL)
            {
                prev->next = node->next;
            }

            if (node == trampoline_list.front)
            {
                trampoline_list.front = node->next;
            }

            connector_free(node);
            retcode = SUBSTANCE_CONNECTOR_SUCCESS;
            break;
        }

        prev = node;
        node = node->next;
    }

    return retcode;
}

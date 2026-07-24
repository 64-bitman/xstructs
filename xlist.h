/* SPDX-License-Identifier: Unlicense */

#pragma once

// IWYU pragma: begin_keep
#include <stdbool.h>
#include <stddef.h>
// IWYU pragma: end_keep

#define _XLIST_UNUSED __attribute__((__unused__))

/*
 * Declare the unique link struct
 */
#define xlist_declare(name)                                                    \
    struct xlist_##name                                                        \
    {                                                                          \
        struct xlist_##name *next;                                             \
        struct xlist_##name *prev;                                             \
    }

/*
 * Define the list functions specific for this type. Done separately because the
 * actual struct is needed (not a just pointer).
 */
#define xlist_define(name, type, link_name)                                    \
    _XLIST_UNUSED static inline void xlist_init_##name(                        \
        struct xlist_##name *link                                              \
    )                                                                          \
    {                                                                          \
        link->next = link->prev = link;                                        \
    }                                                                          \
    /* Insert "node" after "list". Note that "node" link does not have to be   \
     * initialized */                                                          \
    _XLIST_UNUSED static inline void xlist_insert_after_##name(                \
        struct xlist_##name *list, type *node                                  \
    )                                                                          \
    {                                                                          \
        struct xlist_##name *link = &node->link_name;                          \
        link->next = list->next;                                               \
        list->next->prev = link;                                               \
        link->prev = list;                                                     \
        list->next = link;                                                     \
    }                                                                          \
    _XLIST_UNUSED static inline void xlist_unlink_##name(type *node)           \
    {                                                                          \
        struct xlist_##name *link = &node->link_name;                          \
        link->prev->next = link->next;                                         \
        link->next->prev = link->prev;                                         \
        link->next = link->prev = link;                                        \
    }                                                                          \
    _XLIST_UNUSED static inline bool xlist_empty_##name(                       \
        struct xlist_##name *list                                              \
    )                                                                          \
    {                                                                          \
        return list->next == list;                                             \
    }                                                                          \
    _XLIST_UNUSED static inline type *xlist_ptr_##name(                        \
        struct xlist_##name *link                                              \
    )                                                                          \
    {                                                                          \
        return ((type *)(void *)((uint8_t *)(link) -                           \
                                 offsetof(type, link_name)));                  \
    }

#define xlist_foreach(name, list, pos)                                         \
    for (struct xlist_##name *__xlist_it_##name = (list)->next;                \
         __xlist_it_##name != (list) &&                                        \
         ((pos = xlist_ptr_##name(__xlist_it_##name)), 1);                     \
         __xlist_it_##name = __xlist_it_##name->next)

/*
 * Safe to remove current list link.
 */
#define xlist_foreach_safe(name, list, pos)                                    \
    for (struct xlist_##name *__xlist_it_##name = (list)->next,                \
                             *__xlist_tmp_##name = __xlist_it_##name->next;    \
         __xlist_it_##name != (list) &&                                        \
         ((pos = xlist_ptr_##name(__xlist_it_##name)), 1);                     \
         __xlist_it_##name = __xlist_tmp_##name,                               \
                             __xlist_tmp_##name = __xlist_it_##name->next)

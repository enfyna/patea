#pragma once

#include <stdbool.h>
#include <stddef.h>

#define global static
#define internal static
#define local_persist static

bool starts_with(const char* text, const char* prefix);

typedef struct {
    void** items;
    size_t count;
    size_t capacity;
} da;

#define da_cap_init 8

#define da_init(type, arr)                                \
    do {                                                  \
        (arr).count = 0;                                  \
        (arr).capacity = da_cap_init;                     \
        (arr).items = malloc(sizeof(type) * da_cap_init); \
    } while (0)

#define da_free(arr)                             \
    do {                                         \
        for (size_t i = 0; i < (arr).count; i++) \
            free((arr).items[i]);                \
        (arr).count = 0;                         \
        (arr).capacity = 0;                      \
        free((arr).items);                       \
    } while (0)

#define da_push(arr, type, item) da_push_to_id(arr, type, item, (arr).count)
#define da_push_to_id(arr, type, item, id)                                     \
    do {                                                                       \
        if ((arr).count >= (arr).capacity                                      \
            || (size_t)(id) >= (arr).capacity) {                               \
            (arr).capacity *= 2;                                               \
            (arr).items = realloc((arr).items, sizeof(type) * (arr).capacity); \
        }                                                                      \
        (arr).items[(id)] = item;                                              \
        (arr).count += 1;                                                      \
    } while (0)

#define da_for(it, arr) for (size_t(it) = 1; (it) <= (arr).count; (it)++)
#define da_foreach(it, type, arr)                                                           \
    for (size_t __it_dont_use = 1, __loop_dont_use = 1; __loop_dont_use; __loop_dont_use--) \
        for (type it = (arr).items[__it_dont_use];                                          \
            __it_dont_use <= (arr).count; __it_dont_use++, it = (arr).items[__it_dont_use])

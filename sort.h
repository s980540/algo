#ifndef _SORT_H
#define _SORT_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "menu.h"
#include "types.h"
#include "utility.h"

#define CONFIG_SORT_LOG_ENABLE      (TRUE)
#define CONFIG_SORT_DEBUG_ENABLE    (TRUE)

#if (CONFIG_SORT_DEBUG_ENABLE)
    #define SORT_ASSERT(cond)   ASSERT(cond)
#else
    #define SORT_ASSERT(cond)
#endif

#if (CONFIG_SORT_LOG_ENABLE)
    #define SORT_PRINT_ARRAY(p, size, digit)    PRINT_ARRAY(p, size, digit)
#else
    #define SORT_PRINT_ARRAY(p, size, digit)
#endif

#define DUMMY_SIZE  (128)   // number of elements of a dummy char array for temporarily sorted values

typedef int (*compare_func_t)(const void *, const void *, size_t size);
typedef void (*swap_func_t)(void *, void *, size_t size);
typedef void (*sort_func_t)(void *base, size_t num, size_t size, compare_func_t comp, swap_func_t swap);

void quick_sort(void *base, size_t num, size_t size, compare_func_t comp, swap_func_t swap);

void max_heap_insert(void *base, void *key, size_t idx, size_t size, compare_func_t comp, swap_func_t swap);
void *heap_extract_max(void *base, void *ibase, size_t size, compare_func_t comp, swap_func_t swap);
void heap_sort(void *base, void *ibase, size_t num, size_t size, compare_func_t comp, swap_func_t swap);

void merge_sort(void *base, size_t num, size_t size, compare_func_t comp, swap_func_t assign);
void insertion_sort(void *base, size_t num, size_t size, compare_func_t comp, swap_func_t assign);

#endif
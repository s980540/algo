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

typedef int (*PCOMPARE_FUNC)(const void *, const void *, size_t size);
typedef void (*PSWAP_FUNC)(void *, void *, size_t size);
typedef void (*PSORT_FUNC)(void *base, size_t num, size_t size, PCOMPARE_FUNC comp, PSWAP_FUNC swap);

ret_code menu_func_sort(int argc, char **argv);
void heap_sort(void *base, void *ibase, size_t num, size_t size, PCOMPARE_FUNC comp, PSWAP_FUNC swap);
void max_heapify(void *base, void *ibase, size_t idx, size_t num, size_t size, PCOMPARE_FUNC comp, PSWAP_FUNC swap);
void build_max_heap(void *base, void *ibase, size_t num, size_t size, PCOMPARE_FUNC comp, PSWAP_FUNC swap);
void *heap_extract_max(void *base, void *ibase, size_t size, PCOMPARE_FUNC comp, PSWAP_FUNC swap);
void *binary_search(void *base, void *key, size_t size, size_t num, PCOMPARE_FUNC comp);
int is_alignment(const void *base, int align);
void u32_swap(void *a, void *b, size_t size);
void u64_swap(void *a, void *b, size_t size);
void generic_swap(void *a, void *b, size_t size);

#endif
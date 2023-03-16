#ifndef _SEARCH_H
#define _SEARCH_H

#include <stdio.h>

#include "types.h"

typedef int (*compare_func_t)(const void *, const void *, size_t size);
typedef void (*swap_func_t)(void *, void *, size_t size);
typedef void (*sort_func_t)(void *base, size_t num, size_t size, compare_func_t comp, swap_func_t swap);

void *binary_search(void *base, void *key, size_t size, size_t num, compare_func_t comp);
void *binary_search_r(void *base, void *key, size_t lower, size_t mid, size_t upper, size_t size, compare_func_t comp);
int *binary_search_int(int *nums, int key, size_t numsSize);

#endif  // ~ _SEARCH_H
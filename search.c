#include "search.h"

void *binary_search(void *base, void *key, size_t size, size_t num, compare_func_t comp)
{
    size_t lower = 0;
    size_t mid = num >> 1;
    size_t upper = num - 1;
    int result;

    while (mid >= lower && upper >= mid) {

        result = comp(key, base + mid * size, size);
        if ((result < 0) && (mid == 0))
            return NULL;

        if (result == 0)
            return base + mid * size;
        else if (result < 0)
            upper = mid - 1;
        else if (result > 0)
            lower = mid + 1;

        mid = lower + ((upper - lower) >> 1);
    }

    return NULL;
}

void *binary_search_r(
    void *base,
    void *key,
    size_t lower,
    size_t mid,
    size_t upper,
    size_t size,
    compare_func_t comp)
{
    if (mid >= lower && upper >= mid) {
        int result = comp(key, base + mid * size, size);

        if (result == 0)
            return base + mid * size;
        else if (result < 0)
            return binary_search_r(base, key, lower, lower + (((mid - 1) - lower) >> 1), mid - 1, size, comp);
        else if (result > 0)
            return binary_search_r(base, key, mid + 1, (mid + 1) + ((upper - (mid + 1)) >> 1), upper, size, comp);
    }

    return NULL;
}
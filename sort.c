#include "sort.h"

static char _dummy[DUMMY_SIZE];
static u64 u64_dummy;

static int is_alignment(const void *base, int align)
{
    return ((u32)base & (align - 1)) == 0;
}

static void u32_swap(void *a, void *b, size_t size)
{
    u32 t = *(u32 *)a;
    *(u32 *)a = *(u32 *)b;
    *(u32 *)b = t;
}

static void u64_swap(void *a, void *b, size_t size)
{
    u64 t = *(u64 *)a;
    *(u64 *)a = *(u64 *)b;
    *(u64 *)b = t;
}

static void generic_swap(void *a, void *b, size_t size)
{
    char t;
    while (size--) {
        t = *(char *)a;
        *(char *)a = *(char *)b;
        *(char *)b = t;
    }
}

static void u32_assign(void *a, void *b, size_t size)
{
    *(u32 *)a = *(u32 *)b;
}

static void u64_assign(void *a, void *b, size_t size)
{
    *(u64 *)a = *(u64 *)b;
}

static void generic_assign(void *a, void *b, size_t size)
{
    while (size--) {
        *(char *)a = *(char *)b;
    }
}

void quick_sort(
    void *base,
    size_t num,
    size_t size,
    compare_func_t comp,
    swap_func_t swap)
{
    if (!swap) {
        if (size == 4 && is_alignment(base, 4))
            swap = u32_swap;
        else if (size == 8 && is_alignment(base, 8))
            swap = u64_swap;
        else
            swap = generic_swap;
    }
}

void quick_sort_r(
    void *base,
    size_t num,
    size_t size,
    compare_func_t comp,
    swap_func_t swap)
{
    if (!swap) {
        if (size == 4 && is_alignment(base, 4))
            swap = u32_swap;
        else if (size == 8 && is_alignment(base, 8))
            swap = u64_swap;
        else
            swap = generic_swap;
    }
}

static void max_heapify(
    void *base,
    void *ibase,
    size_t idx,
    size_t num,
    size_t size,
    compare_func_t comp,
    swap_func_t swap)
{
    size_t l = 2 * idx + 1;
    size_t r = 2 * idx + 2;
    size_t largest = idx;

    if ((l < num) && (comp(base + l * size, base + idx * size, size) > 0))
        largest = l;

    if ((r < num) && (comp(base + r * size, base + largest * size, size) > 0))
        largest = r;

    if (largest != idx) {
        swap(base + idx * size, base + largest * size, size);
        if (ibase)
            swap(ibase + idx * size, ibase + largest * size, size);
        max_heapify(base, ibase, largest, num, size, comp, swap);
    }
}

static void build_max_heap(
    void *base,
    void *ibase,
    size_t num,
    size_t size,
    compare_func_t comp,
    swap_func_t swap)
{
    size_t i;
    for (i = (num - 1) >> 1; ; i--) {
        max_heapify(base, ibase, i, num, size, comp, swap);
        if (i == 0)
            break;
    }
}

void heap_sort(
    void *base,
    void *ibase,
    size_t num,
    size_t size,
    compare_func_t comp,
    swap_func_t swap)
{
    size_t i;

    printf("%s\n", __FUNCTION__);

    if (!swap) {
        if (size == 4 && is_alignment(base, 4))
            swap = u32_swap;
        else if (size == 8 && is_alignment(base, 8))
            swap = u64_swap;
        else
            swap = generic_swap;
    }

    // using bottom-up manner to covert A into a max-heap
    build_max_heap(base, ibase, num, size, comp, swap);

    for (i = num - 1; i > 0; i--) {
        swap(base, base + i * size, size);
        if (ibase)
            swap(ibase, ibase + i * size, size);
        num = num - 1;              // decrease heap size;
        max_heapify(base, ibase, 0, num, size, comp, swap);
    }

}

static size_t _heap_size = 0;
static size_t _heap_size_max = 0;

static void *heap_maximum(void *base)
{
    return base;
}

void *heap_extract_max(
    void *base,
    void *ibase,
    size_t size,
    compare_func_t comp,
    swap_func_t swap)
{
    if (_heap_size < 1)
        return NULL;

    swap(base, base + (_heap_size - 1) * size, size);
    if (ibase)
        swap(ibase, ibase + (_heap_size - 1) * size, size);
    _heap_size--;
    max_heapify(base, ibase, 0, _heap_size, size, comp, swap);

    return base + _heap_size * size;
}

#define PARENT(i) ((i - 1) >> 1)

static void heap_increase_key(
    void *base,
    void *key,
    size_t idx,
    size_t size,
    compare_func_t comp,
    swap_func_t swap)
{
    printf("idx:%d key:%d curr:%d\n", idx, *(int *)key, *(int *)(base + idx * size));
    if (comp(key, base + idx * size, size) < 0)
        SORT_ASSERT(comp(key, base + idx * size, size) >= 0);

    memcpy(base + idx * size, key, size);

    while (idx > 0) {
        size_t p = (idx - 1) >> 1;
        if (comp(base + p * size, base + idx * size, size) < 0)
            swap(base + p * size, base + idx * size, size);
        else
            break;

        idx = p;
    }
}

void max_heap_insert(
    void *base,
    void *key,
    size_t idx,
    size_t size,
    compare_func_t comp,
    swap_func_t swap)
{
    heap_increase_key(base, key, idx, size, comp, swap);
    _heap_size++;
}

// static void *_start;
// static void *_end;

static void _merge(
    void *base,
    size_t num,
    size_t size,
    compare_func_t comp,
    swap_func_t assign)
{
    size_t i, j, k;
    size_t n1 = num >> 1;
    size_t n2 = num - n1;
    void *L = malloc(size * n1);
    void *R = malloc(size * n2);

    memcpy(L, base, n1 * size);
    memcpy(R, base + n1 * size, n2 * size);

    // SORT_PRINT_ARRAY(((int *)base), num, 5);
    // printf("\n\n");

    for (i = 0, j = 0, k = 0; i < n1 && j < n2; k++) {
        if (comp(L + i * size, R + j * size, size) <= 0)
            assign(base + k * size, L + i++ * size, size);
        else
            assign(base + k * size, R + j++ * size, size);
    }

    while (i < n1)
        assign(base + k++ * size, L + i++ * size, size);

    while (j < n2)
        assign(base + k++ * size, R + j++ * size, size);

    SORT_ASSERT(k == (i + j));
    SORT_ASSERT(k == num);
    // SORT_PRINT_ARRAY(((int *)base), num, 5);

    free(L);
    free(R);
}

static void _merge_sort(
    void *base,
    size_t num,
    size_t size,
    compare_func_t comp,
    swap_func_t assign)
{
    if (num > 1) {
        size_t n1 = num >> 1;
        size_t n2 = num - n1;

        // printf("base:%d, n1: %d, n2: %d\n", base, n1, n2);
        // SORT_ASSERT(num == (n1 + n2));
        // SORT_ASSERT(base >= _start);
        // SORT_ASSERT(base + (n1 + n2) * size <= _end);

        _merge_sort(base, n1, size, comp, assign);
        _merge_sort(base + n1 * size, n2, size, comp, assign);
        _merge(base, num, size, comp, assign);
    }
}

void merge_sort(
    void *base,
    size_t num,
    size_t size,
    compare_func_t comp,
    swap_func_t assign)
{
    printf("%s\n", __FUNCTION__);

    if (!assign) {
        if (size == 4 && is_alignment(base, 4))
            assign = u32_assign;
        else if (size == 8 && is_alignment(base, 8))
            assign = u32_assign;
        else
            assign = generic_assign;
    }

    // _start = base;
    // _end = base + num * size;

    _merge_sort(base, num, size, comp, assign);
}

void insertion_sort(
    void *base,
    size_t num,
    size_t size,
    compare_func_t comp,
    swap_func_t assign)
{
    size_t i, j;
    void *dummy;

    printf("%s\n", __FUNCTION__);

    SORT_ASSERT(size < DUMMY_SIZE);

    if (!assign) {
        if (size == 4 && is_alignment(base, 4)) {
            assign = u32_assign;
            dummy = &u64_dummy;
        } else if (size == 8 && is_alignment(base, 8)) {
            assign = u64_assign;
            dummy = &u64_dummy;
        } else {
            assign = generic_assign;
            dummy = _dummy;
        }
    }

    for (j = 1; j < num; j++) {
        // key = base[j]
        assign(dummy, base + j * size, size);
        for (i = j - 1; ; i--) {

            // base[i] > key
            if (comp(base + i * size , dummy, size) > 0) {
                // base[i + 1] = base[i]
                assign(base + (i + 1) * size, base + i * size, size);
            } else {
                // base[i + 1] = key
                assign(base + (i + 1) * size, dummy, size);
                break;
            }

            if (i == 0) {
                assign(base + i * size, dummy, size);
                break;
            }
        }
    }
}
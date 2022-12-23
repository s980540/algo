#include "sort_demo.h"

static int int_comp(const void *a, const void *b, size_t size)
{
    return *(int *)a - *(int *)b;
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

void heap_sort_demo(int num)
{
    int *a, *b, *c, *t, i, r = 1;
    u32 sum1 = 0, sum2 = 0;

    a = malloc(sizeof(int) * num);
    b = malloc(sizeof(int) * num);
    c = malloc(sizeof(int) * num);

    for (r = 1, i = 0; i < num; i++) {
        r = (r * 725861) % 6599;
        a[i] = b[i] = c[i] = r;
        sum1 += r;
    }

    SORT_PRINT_ARRAY(a, num, 5);

    heap_sort(a, 0, num, sizeof(*a), int_comp, NULL);
    merge_sort(b, num, sizeof(*b), int_comp, NULL);
    insertion_sort(c, num, sizeof(*c), int_comp, NULL);

    SORT_PRINT_ARRAY(a, num, 5);

    for (i = 0; i < num - 1; i++)
        SORT_ASSERT(a[i] <= a[i + 1]);

    for (i = 0; i < num; i++)
        sum2 += a[i];

    SORT_ASSERT(memcmp(a, b, sizeof(*a) * num) == 0);
    SORT_ASSERT(memcmp(a, c, sizeof(*a) * num) == 0);
    SORT_ASSERT(sum1 == sum2);

    for (i = 0; i < num; i++)
        b[i] = *(int *)binary_search(a, a + i, sizeof(*a), num, int_comp);

    for (i = 0; i < num; i++)
        c[i] = *(int *)binary_search_r(a, a + i, 0, num >> 1, num - 1, sizeof(*a), int_comp);

    SORT_ASSERT(memcmp(a, b, sizeof(*a) * num) == 0);
    SORT_ASSERT(memcmp(a, c, sizeof(*a) * num) == 0);

    printf("heap_sort pass\n");

    free(a);
    free(b);
    free(c);
}

void merge_sort_demo(int num)
{
    int *a, *b, *c, *t, i, r = 1;
    u32 sum1 = 0, sum2 = 0;

    a = malloc(sizeof(int) * num);
    b = malloc(sizeof(int) * num);
    c = malloc(sizeof(int) * num);

    for (r = 1, i = 0; i < num; i++) {
        r = (r * 725861) % 6599;
        a[i] = b[i] = c[i] = r;
        sum1 += r;
    }

    SORT_PRINT_ARRAY(a, num, 5);

    merge_sort(a, num, sizeof(*a), int_comp, NULL);
    heap_sort(b, 0, num, sizeof(*b), int_comp, NULL);
    insertion_sort(c, num, sizeof(*c), int_comp, NULL);

    SORT_PRINT_ARRAY(a, num, 5);

    for (i = 0; i < num - 1; i++)
        SORT_ASSERT(a[i] <= a[i + 1]);

    for (i = 0; i < num; i++)
        sum2 += a[i];

    SORT_ASSERT(memcmp(a, b, sizeof(*a) * num) == 0);
    SORT_ASSERT(memcmp(a, c, sizeof(*a) * num) == 0);
    SORT_ASSERT(sum1 == sum2);

    for (i = 0; i < num; i++)
        b[i] = *(int *)binary_search(a, a + i, sizeof(*a), num, int_comp);

    for (i = 0; i < num; i++)
        c[i] = *(int *)binary_search_r(a, a + i, 0, num >> 1, num - 1, sizeof(*a), int_comp);

    SORT_ASSERT(memcmp(a, b, sizeof(*a) * num) == 0);
    SORT_ASSERT(memcmp(a, c, sizeof(*a) * num) == 0);

    printf("merge_sort pass\n");

    free(a);
    free(b);
    free(c);
}

void insertion_sort_demo(int num)
{
    int *a, *b, *c, *t, i, r = 1;
    u32 sum1 = 0, sum2 = 0;

    a = malloc(sizeof(int) * num);
    b = malloc(sizeof(int) * num);
    c = malloc(sizeof(int) * num);

    for (r = 1, i = 0; i < num; i++) {
        r = (r * 725861) % 6599;
        a[i] = b[i] = c[i] = r;
        sum1 += r;
    }

    SORT_PRINT_ARRAY(a, num, 5);

    insertion_sort(a, num, sizeof(*a), int_comp, NULL);
    merge_sort(b, num, sizeof(*b), int_comp, NULL);
    heap_sort(c, 0, num, sizeof(*c), int_comp, NULL);

    SORT_PRINT_ARRAY(a, num, 5);

    for (i = 0; i < num - 1; i++)
        SORT_ASSERT(a[i] <= a[i + 1]);

    for (i = 0; i < num; i++)
        sum2 += a[i];

    SORT_ASSERT(memcmp(a, b, sizeof(*a) * num) == 0);
    SORT_ASSERT(memcmp(a, c, sizeof(*a) * num) == 0);
    SORT_ASSERT(sum1 == sum2);

    for (i = 0; i < num; i++)
        b[i] = *(int *)binary_search(a, a + i, sizeof(*a), num, int_comp);

    for (i = 0; i < num; i++)
        c[i] = *(int *)binary_search_r(a, a + i, 0, num >> 1, num - 1, sizeof(*a), int_comp);

    SORT_ASSERT(memcmp(a, b, sizeof(*a) * num) == 0);
    SORT_ASSERT(memcmp(a, c, sizeof(*a) * num) == 0);

    printf("insertion_sort pass\n");

    free(a);
    free(b);
    free(c);
}

// static size_t _heap_size = 0;
// static size_t _heap_size_max = 0;

// void max_heap_insert_demo(int num)
// {
//     int *a, *b, i, r = 1;
//     int size, temp, max = 1000000;
//     u32 sum1 = 0, sum2 = 0;

//     a = malloc(sizeof(int) * num);
//     b = malloc(sizeof(int) * num);
//     memset(a, 0, sizeof(int) * num);
//     // _heap_size_max = num;
//     for (i = 0; i < num; i++) {
//         r *= 2;
//         max_heap_insert(a, &r, _heap_size, sizeof(*a), int_comp, u32_swap);
//         sum1 += r;
//     }

//     SORT_PRINT_ARRAY(a, num, 5);

//     for (i = 0; i < num; i++) {
//         b[i] = *(int *)heap_extract_max(a, 0, sizeof(*a), int_comp, u32_swap);
//         SORT_PRINT_ARRAY(a, num, 5);

//         // SORT_ASSERT(b[i] <= max);
//         // max = b[i];
//     }

//     SORT_PRINT_ARRAY(b, num, 5);

//     for (i = 0; i < num; i++)
//         sum2 += b[i];

//     if (sum1 != sum2)
//         printf("max_heap_insert failed\n");
//     else
//         printf("max_heap_insert pass\n");

//     free(a);
//     free(b);
// }
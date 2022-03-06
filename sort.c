#include "sort.h"

typedef enum _OPT_CODE_SORT
{
    OPT_CODE_SORT_HELP = 0,
    OPT_CODE_SORT_DEMO_INSERTION_SORT = 1,
    OPT_CODE_SORT_DEMO_MERGE_SORT,
    OPT_CODE_SORT_DEMO_HEAP_SORT,
    OPT_CODE_SORT_DEMO_MAX_HEAP_INSERT,
    OPT_CODE_SORT_DEMO_QUICK_SORT
} OPT_CODE_SORT;

static const struct _MENU_OPTION sort_options[] =
{
    {"--help",                  OPT_CODE_SORT_HELP,                 '-', "Display this summary"},
    {"--demo-insertion-sort",   OPT_CODE_SORT_DEMO_INSERTION_SORT,  'a', "Demo insertion sort"},
    {"--demo-merge-sort",       OPT_CODE_SORT_DEMO_MERGE_SORT,      'a', "Demo merge sort"},
    {"--demo-heap-sort",        OPT_CODE_SORT_DEMO_HEAP_SORT,       'a', "Demo heap sort"},
    {"--demo-max-heap-insert",  OPT_CODE_SORT_DEMO_MAX_HEAP_INSERT, 'a', "Demo max heap insert"},
    {"--demo-quick-sort",       OPT_CODE_SORT_DEMO_QUICK_SORT,      'a', "Demo quick sort"},
    {NULL}
};

typedef int (*PCOMPARE_FUNC)(const void *, const void *, size_t size);
typedef void (*PSWAP_FUNC)(void *, void *, size_t size);
typedef void (*PSORT_FUNC)(void *base, size_t num, size_t size, PCOMPARE_FUNC comp, PSWAP_FUNC swap);

static char _dummy[DUMMY_SIZE];
static u64 u64_dummy;

static int is_alignment(const void *base, int align);

static int is_alignment(const void *base, int align)
{
    return ((u32)base & (align - 1)) == 0;
}

void u32_swap(void *a, void *b, size_t size)
{
    u32 t = *(u32 *)a;
    *(u32 *)a = *(u32 *)b;
    *(u32 *)b = t;
}

void u64_swap(void *a, void *b, size_t size)
{
    u64 t = *(u64 *)a;
    *(u64 *)a = *(u64 *)b;
    *(u64 *)b = t;
}

void generic_swap(void *a, void *b, size_t size)
{
    char t;
    while (size--) {
        t = *(char *)a;
        *(char *)a = *(char *)b;
        *(char *)b = t;
    }
}

void u32_assign(void *a, void *b, size_t size)
{
    *(u32 *)a = *(u32 *)b;
}

void u64_assign(void *a, void *b, size_t size)
{
    *(u64 *)a = *(u64 *)b;
}

void generic_assign(void *a, void *b, size_t size)
{
    while (size--) {
        *(char *)a = *(char *)b;
    }
}

int int_comp(const void *a, const void *b, size_t size)
{
    // printf("int_comp %d %5d %5d\n", *(int *)a > *(int *)b, *(int *)a, *(int *)b);
    return *(int *)a - *(int *)b;
}

void quick_sort(
    void *base,
    size_t num,
    size_t size,
    PCOMPARE_FUNC comp,
    PSWAP_FUNC swap)
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
    PCOMPARE_FUNC comp,
    PSWAP_FUNC swap)
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
    size_t idx,
    size_t num,
    size_t size,
    PCOMPARE_FUNC comp,
    PSWAP_FUNC swap)
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
        max_heapify(base, largest, num, size, comp, swap);
    }
}

static void build_max_heap(
    void *base,
    size_t num,
    size_t size,
    PCOMPARE_FUNC comp,
    PSWAP_FUNC swap)
{
    size_t i;
    for (i = (num - 1) >> 1; ; i--) {
        max_heapify(base, i, num, size, comp, swap);
        if (i == 0)
            break;
    }
}

void heap_sort(
    void *base,
    size_t num,
    size_t size,
    PCOMPARE_FUNC comp,
    PSWAP_FUNC swap)
{
    size_t i;

    if (!swap) {
        if (size == 4 && is_alignment(base, 4))
            swap = u32_swap;
        else if (size == 8 && is_alignment(base, 8))
            swap = u64_swap;
        else
            swap = generic_swap;
    }

    // using bottom-up manner to covert A into a max-heap
    build_max_heap(base, num, size, comp, swap);

    for (i = num - 1; i > 0; i--) {
        swap(base, base + i * size, size);
        num = num - 1;              // decrease heap size;
        max_heapify(base, 0, num, size, comp, swap);
    }

}

static size_t _heap_size = 0;
static size_t _heap_size_max = 0;

static void *heap_maximum(void *base)
{
    return base;
}

static void *heap_extract_max(
    void *base,
    size_t size,
    PCOMPARE_FUNC comp,
    PSWAP_FUNC swap)
{
    if (_heap_size < 1)
        return NULL;

    swap(base, base + (_heap_size - 1) * size, size);
    _heap_size--;
    max_heapify(base, 0, _heap_size, size, comp, swap);

    return base + _heap_size * size;
}

#define PARENT(i) ((i - 1) >> 1)

static void heap_increase_key(
    void *base,
    void *key,
    size_t idx,
    size_t size,
    PCOMPARE_FUNC comp,
    PSWAP_FUNC swap)
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

static void max_heap_insert(
    void *base,
    void *key,
    size_t idx,
    size_t size,
    PCOMPARE_FUNC comp,
    PSWAP_FUNC swap)
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
    PCOMPARE_FUNC comp,
    PSWAP_FUNC assign)
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
    PCOMPARE_FUNC comp,
    PSWAP_FUNC assign)
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
    PCOMPARE_FUNC comp,
    PSWAP_FUNC assign)
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
    PCOMPARE_FUNC comp,
    PSWAP_FUNC assign)
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

void *binary_search_r(
    void *base,
    void *key,
    size_t lower,
    size_t mid,
    size_t upper,
    size_t size,
    PCOMPARE_FUNC comp)
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

void *binary_search(void *base, void *key, size_t size, size_t num, PCOMPARE_FUNC comp)
{
    size_t lower = 0;
    size_t mid = num >> 1;
    size_t upper = num - 1;
    int result;

    while (mid >= lower && upper >= mid) {
        result = comp(key, base + mid * size, size);

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

void heap_sort_test(int num)
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

    heap_sort(a, num, sizeof(*a), int_comp, NULL);
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

    printf("heap_sort pass\n");

    for (i = 0; i < num; i++)
        b[i] = *(int *)binary_search(a, a + i, sizeof(*a), num, int_comp);

    for (i = 0; i < num; i++)
        c[i] = *(int *)binary_search_r(a, a + i, 0, num >> 1, num - 1, sizeof(*a), int_comp);

    SORT_ASSERT(memcmp(a, b, sizeof(*a) * num) == 0);
    SORT_ASSERT(memcmp(a, c, sizeof(*a) * num) == 0);

    free(a);
    free(b);
    free(c);
}

void max_heap_insert_test(int num)
{
    int *a, *b, i, r = 1;
    int size, temp, max = 1000000;
    u32 sum1 = 0, sum2 = 0;

    a = malloc(sizeof(int) * num);
    b = malloc(sizeof(int) * num);
    memset(a, 0, sizeof(int) * num);
    // _heap_size_max = num;
    for (i = 0; i < num; i++) {
        r *= 2;
        max_heap_insert(a, &r, _heap_size, sizeof(*a), int_comp, u32_swap);
        sum1 += r;
    }

    SORT_PRINT_ARRAY(a, num, 5);

    for (i = 0; i < num; i++) {
        b[i] = *(int *)heap_extract_max(a, sizeof(*a), int_comp, u32_swap);
        SORT_PRINT_ARRAY(a, num, 5);

        // SORT_ASSERT(b[i] <= max);
        // max = b[i];
    }

    SORT_PRINT_ARRAY(b, num, 5);

    for (i = 0; i < num; i++)
        sum2 += b[i];

    if (sum1 != sum2)
        printf("max_heap_insert failed\n");
    else
        printf("max_heap_insert pass\n");

    free(a);
    free(b);
}

void merge_sort_test(int num)
{
    int *a, *b, i, r = 1;
    u32 sum1 = 0, sum2 = 0;

    a = malloc(sizeof(int) * num);
    b = malloc(sizeof(int) * num);

    for (r = 1, i = 0; i < num; i++) {
        r = (r * 725861) % 6599;
        a[i] = b[i] = r;
        sum1 += a[i];
    }

    SORT_PRINT_ARRAY(a, num, 5);

    merge_sort(a, num, sizeof(*a), int_comp, NULL);
    insertion_sort(b, num, sizeof(*b), int_comp, NULL);

    SORT_PRINT_ARRAY(a, num, 5);

    SORT_ASSERT(memcmp(a, b, sizeof(*a) * num) == 0);

    for (i = 0; i < num - 1; i++)
        SORT_ASSERT(a[i] <= a[i + 1]);

    for (i = 0; i < num; i++)
        sum2 += a[i];

    SORT_ASSERT(sum1 == sum2);

    free(a);
    free(b);
}

void insertion_sort_test(int num)
{
    int *a, i, r = 1;
    u32 sum1 = 0, sum2 = 0;

    a = malloc(sizeof(int) * num);

    for (i = 0; i < num; i++) {
        r = (r * 725861) % 6599;
        a[i] = r;
        sum1 += r;
    }

    SORT_PRINT_ARRAY(a, num, 5);

    insertion_sort(a, num, sizeof(*a), int_comp, NULL);

    SORT_PRINT_ARRAY(a, num, 5);

    for (i = 0; i < num; i++)
        sum2 += a[i];

    for (i = 0; i < num - 1; i++) {
        if (a[i] > a[i + 1]) {
            printf("insertion_sort failed (a[%d]=%d a[%d]=%d)\n",
                i, a[i], i + 1, a[i + 1]);
            break;
        }
    }

    if (sum1 != sum2)
        printf("insertion_sort failed (2)\n");

    free(a);
}

static void *stoi(void *i, const void *s)
{
    *(int *)i = atoi(s);

    return i;
}

ret_code menu_func_sort(int argc, char **argv)
{
    MENU_RET_CODE ret;
    int opt_code;

    printf("%s\n", __FUNCTION__);

    ret = menu_opt_init(argc, 3, argv, sort_options);
    if (ret != MENU_RET_SUCCESS) {
        printf("menu_opt_init failed (%d)\n", ret);
        menu_opt_help("algo sort", sort_options);
        return ret;
    }

    while (1) {
        ret = menu_get_opt_code(&opt_code, sort_options);
        if (ret != MENU_RET_SUCCESS) {
            if (ret != MENU_RET_EOF) {
                printf("menu_get_opt_code failed (%d)\n", ret);
                menu_opt_help("algo sort", sort_options);
                return ret;
            }

            break;
        }

        switch (opt_code) {
        case OPT_CODE_SORT_HELP:
            menu_opt_help("algo sort", sort_options);
            break;

        case OPT_CODE_SORT_DEMO_INSERTION_SORT:
        {
            int num;
            num = *(int *)menu_get_arg(0, &num, stoi);
            insertion_sort_test(num);
            break;
        }

        case OPT_CODE_SORT_DEMO_MERGE_SORT:
        {
            int num;
            // num = *(int *)menu_get_arg(0, &num, stoi);
            merge_sort_test(*(int *)menu_get_arg(0, &num, stoi));
            break;
        }

        case OPT_CODE_SORT_DEMO_HEAP_SORT:
        {
            int num;
            num = *(int *)menu_get_arg(0, &num, stoi);
            heap_sort_test(num);
            break;
        }

        case OPT_CODE_SORT_DEMO_MAX_HEAP_INSERT:
        {
            int num;
            num = *(int *)menu_get_arg(0, &num, stoi);
            max_heap_insert_test(num);
            break;
        }

        default:
            break;
        }
    }

    return MENU_RET_SUCCESS;
}

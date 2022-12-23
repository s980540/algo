#include "menu_sort.h"

const menu_option_t menu_config_sort =
    {"sort",                    's', menu_func_sort, "Demo sort algorithm."};

static const menu_option_t _menu_options[] =
{
    {"--help",                  '-', NULL,                  "Display this summary"},
    {"--demo-insertion-sort",   'a', demo_insertion_sort,   "Demo insertion sort"},
    {"--demo-merge-sort",       'a', demo_merge_sort,       "Demo merge sort"},
    {"--demo-heap-sort",        'a', demo_heap_sort,        "Demo heap sort"},

    {NULL}
};

ret_code menu_func_sort(int argc, char **argv)
{
    int ret;
    const char *func_name = menu_config_sort.name;

    ret = menu_opt_check(_menu_options);
    if (ret != MENU_RET_SUCCESS) {
        printf("menu_opt_check failed (%d)\n", ret);
        exit(ret);
    }

    ret = menu_opt_process(argc, argv, func_name, _menu_options);
    if (ret != MENU_RET_SUCCESS) {
        if (ret != MENU_RET_EOF) {
            printf("menu_opt_process failed (%d)\n", ret);
            menu_func_help(func_name, _menu_options);
            exit(ret);
        }
    }

    return ret;
}

// Convert string to inteager
static void *stoi(void *i, const void *s)
{
    static int _i;
    _i = atoi(s);

    if (i != NULL) {
        *(int *)i = _i;
        return i;
    } else
        return &_i;
}

ret_code demo_insertion_sort(int argc, char **argv)
{
    int num;

    // get an arg and convert it to an inteager
    if (menu_get_arg_inc(&num, stoi) == NULL)
        return MENU_RET_FAIL;

    insertion_sort_demo(num);
    return MENU_RET_SUCCESS;
}

ret_code demo_merge_sort(int argc, char **argv)
{
    int num;

    // get an arg and convert it to an inteager
    if (menu_get_arg_inc(&num, stoi) == NULL)
        return MENU_RET_FAIL;

    merge_sort_demo(num);
    return MENU_RET_SUCCESS;
}

ret_code demo_heap_sort(int argc, char **argv)
{
    int num;

    // get an arg and convert it to an inteager
    if (menu_get_arg_inc(&num, stoi) == NULL)
        return MENU_RET_FAIL;

    heap_sort_demo(num);
    return MENU_RET_SUCCESS;
}
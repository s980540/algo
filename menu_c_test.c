#include "menu_c_test.h"

const menu_option_t  menu_config_c_test =
    {"c-test",  's',    menu_func_c_test,   "Test different functions"};

static const menu_option_t _menu_options[] =
{
    {"--help",                      '-',    NULL,                       "Display this summary.",},
    {"--run",                       '-',    run_c_test,                 "Run C test functions.",},
    {"--run-songlist-to-wikitable", '-',    run_songlist2wikitable,     "Convert songlist to wiki table.",},
    {"--run-csv-to-gallerytable",   '-',    run_csv2gallerytable,       "Convert image csv to gallery table.",},
    {"--run-c-test-quanta",         'a',    run_c_test_quanta,          "Calculate the sum of a sequence.",},

    {NULL}
};

// static void run_expr(int type, int base, unsigned long prec, char *str);

ret_code menu_func_c_test(int argc, char **argv)
{
    int ret;
    const char *func_name = menu_config_c_test.name;

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

ret_code run_c_test(int argc, char **argv)
{
    // c_test_inventec();
    // c_test_elen_1();
    // c_test_elen_2();
    // c_test();
    // c_test_generate_dynamic_random_array_int();
    // c_test_heap_sort_int();
    // c_test_binary_search();
    // c_test_twoSum2();
    // c_test_threeSumClosest();
    c_test_getLucky();
    return MENU_RET_SUCCESS;
}

ret_code run_songlist2wikitable(int argc, char **argv)
{
    return songlist_to_wikitable();
}

ret_code run_csv2gallerytable(int argc, char ** argv)
{
    return csv_to_gallerytable();
}

/**
 * @brief Convert a string to an integer.
 * @param i The final converted result
 * @param s The input value
 * @retval a void pointer point to result
 */
static void *stoi(void *i, const void *s)
{
    if (i != NULL) {
        *(int *)i = atoi(s);
        return i;
    } else
        return NULL;
}

ret_code run_c_test_quanta(int argc, char **argv)
{
    int n;
    menu_get_arg_inc(&n, stoi);
    printf("n: %d\n", n);
    c_test_quanta(n);
    return MENU_RET_SUCCESS;
}
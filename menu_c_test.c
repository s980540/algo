#include "menu_c_test.h"

const menu_option_t  menu_config_c_test =
    {"c-test",  's',    menu_func_c_test,   "Test different functions"};

static const menu_option_t _menu_options[] =
{
    {"--help",      '-',     NULL,          "Display this summary.",},
    {"--run",       '-',     run_c_test,    "Run C test functions.",},

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
    c_test();
    return MENU_RET_SUCCESS;
}
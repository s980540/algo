#include "menu_tree.h"

const menu_option_t  menu_config_tree =
    {"tree",    's',    menu_func_tree,     "Demo tree algorithm."};

static const menu_option_t _menu_options[] =
{
    {"--help",  '-',    NULL,               "Display this summary."},
    {"--demo",  '-',    demo_tree,          "Demo tree traverse algorithm."},
    {"--diff",  '-',    demo_tree_diff,     "Demo the derivative algorithm with tree structure."},
    {"--diffr", '-',    demo_tree_diff_r,   "Demo the recursive derivative algorithm with tree structure."},

    {NULL}
};

ret_code menu_func_tree(int argc, char **argv)
{
    int ret;
    const char *func_name = menu_config_tree.name;

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

ret_code demo_tree(int argc, char **argv)
{
    tree_traverse_demo();
    return MENU_RET_SUCCESS;
}

ret_code demo_tree_diff(int argc, char **argv)
{
    tree_differentiation_demo();
    return MENU_RET_SUCCESS;
}

ret_code demo_tree_diff_r(int argc, char **argv)
{
    tree_differentiation_r_demo();
    return MENU_RET_SUCCESS;
}
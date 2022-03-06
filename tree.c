#include "tree.h"

typedef enum _OPT_CODE_TREE
{
    OPT_CODE_TREE_HELP = 0,
    OPT_CODE_TREE_DEMO = 1
} OPT_CODE_TREE;

static const struct _MENU_OPTION tree_options[] =
{
    {"--help", OPT_CODE_TREE_HELP, '-', "Display this summary"},
    {"--demo", OPT_CODE_TREE_DEMO, '-', "Demo tree algorithm"},

    {NULL}
};

ret_code menu_func_tree(int argc, char **argv)
{
    MENU_RET_CODE ret;
    int opt_code;

    printf("%s\n", __FUNCTION__);

    ret = menu_opt_init(argc, 3, argv, tree_options);
    if (ret != MENU_RET_SUCCESS) {
        printf("menu_opt_init failed (%d)\n", ret);

        /* If fail to get an option, we show available options for user */
        menu_opt_help("algo tree", tree_options);
        return ret;
    }

    while (1) {
        ret = menu_get_opt_code(&opt_code, tree_options);
        if (ret != MENU_RET_SUCCESS) {
            if (ret != MENU_RET_EOF) {
                printf("menu_get_opt_code failed (%d)\n", ret);

                /* If fail to get an option, we show available options for user */
                menu_opt_help("algo tree", tree_options);
                return ret;
            }
            break;
        }

        switch (opt_code) {
        case OPT_CODE_TREE_HELP:
            menu_opt_help("algo tree", tree_options);
            break;
    
        case OPT_CODE_TREE_DEMO:
            tree_test();
            break;
        
        default:
            break;
        }
    }

    return ret;
}

void tree_test(void)
{
    printf("Hello tree algorithm!\n");
}
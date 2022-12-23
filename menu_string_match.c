#include "menu_string_match.h"

const menu_option_t menu_config_string_match =
    {"string-match", 's', menu_func_string_match, "Implementation of string match algorithms"};

static const menu_option_t _menu_options[] =
{
    {"--help", '-', NULL, "Display this summary."},
    {"--demo", '-', demo_string_match, "Demo string match algorithm."},
    {"--native-string-match", 's', run_native_string_match, "Evaluate if a sentence include a specific string."},

    {NULL}
};

ret_code menu_func_string_match(int argc, char **argv)
{
    int ret;
    const char *func_name = menu_config_string_match.name;

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

ret_code demo_string_match(int argc, char **argv)
{
    char *src = "detabcgfabdcabc";
    char *tar = "abc";
    int result;

    if (native_string_match(src, tar, &result) > 0)
        printf("String is found. (%d)\n", result);
    else
        printf("String is not found.\n");

    return MENU_RET_SUCCESS;
}

ret_code run_native_string_match(int argc, char **argv)
{
    int result = INT_MAX;
    const char *s1, *s2;

    s1 = menu_get_arg_inc(NULL, NULL);
    if (s1 == NULL)
        goto exit;

    s2 = menu_get_arg_inc(NULL, NULL);
    if (s2 == NULL)
        goto exit;

    if (native_string_match(s1, s2, &result) > 0)
        printf("String is found. (%d)\n", result);
    else
        printf("String is not found.\n");


    return MENU_RET_SUCCESS;

exit:
    return MENU_RET_FAIL;
}

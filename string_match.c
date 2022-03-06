

#include "string_match.h"

typedef enum _OPT_CODE_STRING_MATCH {
    OPT_CODE_STRING_MATCH_HELP = 0,
    OPT_CODE_STRING_MATCH_DEMO = 1,
    OPT_CODE_STRING_MATCH_NATIVE,

    OPT_CODE_STRING_MATCH_COUNT
} OPT_CODE_STRING_MATCH;

static const struct _MENU_OPTION string_match_options[] = {
    {"--help",  OPT_CODE_STRING_MATCH_HELP, '-', "Display this summary"},
    {"--demo",  OPT_CODE_STRING_MATCH_DEMO, '-', "Demo string match algorithm"},
    {"--native-string-match", OPT_CODE_STRING_MATCH_NATIVE, '-', "Demo native string match"},

    {NULL}
};

ret_code menu_func_string_match(int argc, char **argv)
{
    MENU_RET_CODE ret;
    int opt_code;

    printf("%s\n", __FUNCTION__);

    ret = menu_opt_init(argc, 3, argv, string_match_options);
    if (ret != MENU_RET_SUCCESS) {
        printf("menu_opt_init failed (%d)\n", ret);

        /* If fail to get an option, we show available options for user */
        menu_opt_help("algo string", string_match_options);
        return ret;
    }

    while (1) {
        ret = menu_get_opt_code(&opt_code, string_match_options);
        if (ret != MENU_RET_SUCCESS) {
            if (ret != MENU_RET_EOF) {
                printf("menu_get_opt_code failed (%d)\n", ret);

                /* If fail to get an option, we show available options for user */
                menu_opt_help("algo string", string_match_options);
            }
            break;
        }

        switch (opt_code) {
        case OPT_CODE_STRING_MATCH_HELP:
            menu_opt_help("algo string", string_match_options);
            break;

        case OPT_CODE_STRING_MATCH_DEMO:
            string_match_test();
            break;

        case OPT_CODE_STRING_MATCH_NATIVE:
            break;

        default:
            break;
        }
    }

    return MENU_RET_SUCCESS;
}

void string_match_test(void)
{
    char *src = "detabcgfabdcabc";
    char *tar = "abc";
    u32 result;

    if (native_string_match(src, tar, &result) > 0)
        printf("String is found. (%d)\n", result);
    else
        printf("String is not found\n.");

    return;
}

int native_string_match(const char *src, const char *tar, u32 *rlt)
{
#if CONFIG_STRING_MATCH_DEBUG_MESSAGE
    if (src == NULL || tar == NULL)
        __fatal_error(1);
#endif

    u32 tmp = 0, slen = 0, tlen = 0, i, j;

    slen = strlen_of(src);
    tlen = strlen_of(tar);

    if (!slen || !tlen)
        return(0);

    i = 0;
    while (i + tlen <= slen) {
        j = 0;
        while (1) {
            if ((j >= tlen) || (src[i + j] != tar[j]))
                break;

            j++;
        }

        if (j == tlen) {
        #if CONFIG_STRING_MATCH_DEBUG_MESSAGE
            if (tmp == UINT_MAX)
                __fatal_error(1);
        #endif

            tmp++;

            /* Adjust the index to the next character of the current string end. */
            i = i + tlen;
        } else {
            i++;
        }
    }

    if (rlt != NULL)
        *rlt = tmp;

    return(tmp > 0);
}


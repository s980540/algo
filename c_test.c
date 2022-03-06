#include "c_test.h"

typedef enum _OPT_CODE_C_TEST
{
    OPT_CODE_C_TEST_HELP = 0,
    OPT_CODE_C_TEST_DEMO = 1,

} OPT_CODE_C_TEST;

static const struct _MENU_OPTION c_test_options[] =
{
    {"--help", OPT_CODE_C_TEST_HELP, '-', "Display this summary"},
    {"--demo", OPT_CODE_C_TEST_DEMO, '-', "Demo C programming language"},

    {NULL}
};

void swap(char *a, char *b)
{
    char t;

    t = *a;  // x = a ^ b
    *a = *b;  // a' = x ^ b
    *b = t;  // b' = x ^ a' 
}

void invert_string(char *s, size_t size)
{
    size_t i, j;

    if (!size)
        return;

    for (i = 0, j = size - 1; i < j; i++, j--)
        swap(s + i, s + j);
}

void invert_sentence(char *s)
{
    size_t i = 0;

    for (i = 0; s[i] != '\0'; i++)
        ;
    invert_string(s, i);

    i = 0;
    while (1) {
        if (s[i] == ' ' || s[i] == '\0') {
            invert_string(s, i);
            s = s + i + 1;
            i = 0;
            if (s[i] == '\0')
                break;
            else
                continue;
        }
        i++;
    }
}

void printd(int n)
{
    if (n < 0) {
        printf("-");
        n = -n;
    }

    if (n >= 10)
        printd(n/10);

    printf("%c", n % 10 + '0');
}

void c_test(void)
{
    // char s[] = "This is a pen";
    char s[] = "The master branch at origin should be familiar to every Git user. Parallel to the master branch, another branch exists called develop.";
    char *t = malloc(sizeof(s));

    printf("sizeof(s): %d\n", sizeof(s));
    memcpy(t, s, sizeof(s));
    printd(123456789);
    printf("\n");

    printf("%s\n", t);
    invert_sentence(t);
    printf("%s\n", t);
}

ret_code menu_func_c_test(int argc, char **argv)
{
    MENU_RET_CODE ret;
    int opt_code;

    printf("%s\n", __FUNCTION__);

    ret = menu_opt_init(argc, 3, argv, c_test_options);
    if (ret != MENU_RET_SUCCESS) {
        printf("menu_opt_init failed (%d)\n", ret);
        menu_opt_help("algo c-test", c_test_options);
        return ret; 
    }

    while (1) {
        ret = menu_get_opt_code(&opt_code, c_test_options);
        if (ret != MENU_RET_SUCCESS) {
            if (ret != MENU_RET_EOF) {
                printf("menu_get_opt_code failed (%d)\n", ret);

                menu_opt_help("algo c-test", c_test_options);
            }
            break;
        }

        switch (opt_code) {
        case OPT_CODE_C_TEST_HELP:
            menu_opt_help("algo c-test", c_test_options);
            break;

        case OPT_CODE_C_TEST_DEMO:
            c_test();
            break;

        default:
            break;
        }
    }

    return MENU_RET_SUCCESS;
}

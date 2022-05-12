#include "menu_bignum.h"

typedef enum _OPT_CODE_BIGNUM
{
    OPT_CODE_BIGNUM_HELP = 0,
    OPT_CODE_BIGNUM_DEMO = 1,
} OPT_CODE_BIGNUM;

static const struct _MENU_OPTION bignum_options[] =
{
    {"--help", OPT_CODE_BIGNUM_HELP, '-', "Display this summary"},
    {"--demo", OPT_CODE_BIGNUM_DEMO, 's', "Demo big num"},

    {NULL}
};
#if (CONFIG_ENABLE_LIBEXPR)
void run_expr (int type, int base, unsigned long prec, char *str)
{
    int outbase = (base == 0 ? 10 : base);
    int ret;

    switch (type) {
    case 'z':
    default:
    {
        mpz_t  res, var_a, var_b;

        mpz_init(res);
        mpz_init_set_ui(var_a, 55L);
        mpz_init_set_ui(var_b, 99L);

        ret = mpz_expr(res, base, str, var_a, var_b, NULL);
        printf ("\"%s\" base %d: ", str, base);
        if (ret == MPEXPR_RESULT_OK) {
            printf ("result ");
            mpz_out_str (stdout, outbase, res);
            printf ("\n");
        } else
            printf ("invalid (return code %d)\n", ret);

        mpz_clear (res);
        mpz_clear (var_a);
        mpz_clear (var_b);
    }
        break;

    case 'q':
    {
        mpq_t  res, var_a, var_b;

        mpq_init (res);
        mpq_init (var_a);
        mpq_init (var_b);

        mpq_set_ui (var_a, 55L, 1);
        mpq_set_ui (var_b, 99L, 1);

        ret = mpq_expr(res, base, str, var_a, var_b, NULL);
        printf ("\"%s\" base %d: ", str, base);
        if (ret == MPEXPR_RESULT_OK) {
            printf ("result ");
            mpq_out_str (stdout, outbase, res);
            printf ("\n");
        }
        else
            printf ("invalid (return code %d)\n", ret);

        mpq_clear (res);
        mpq_clear (var_a);
        mpq_clear (var_b);
    }
        break;

    case 'f':
    {
        mpf_t  res, var_a, var_b;

        mpf_init2 (res, prec);
        mpf_init_set_ui (var_a, 55L);
        mpf_init_set_ui (var_b, 99L);

        ret = mpf_expr(res, base, str, var_a, var_b, NULL);
        printf ("\"%s\" base %d: ", str, base);
        if (ret == MPEXPR_RESULT_OK) {
            printf ("result ");
            mpf_out_str (stdout, outbase, (size_t) 0, res);
            printf ("\n");
        } else
            printf ("invalid (return code %d)\n", ret);

        mpf_clear (res);
        mpf_clear (var_a);
        mpf_clear (var_b);
    }
        break;
    }
}
#endif
ret_code menu_func_bignum(int argc, char **argv)
{
    MENU_RET_CODE ret;
    int opt_code;

    printf("%s\n", __FUNCTION__);

    ret = menu_opt_init(argc, 3, argv, bignum_options);
    if (ret != MENU_RET_SUCCESS) {
        printf("menu_opt_init failed (%d)\n", ret);

        /* If fail to get an option, we show available options for user */
        menu_opt_help("algo bignum", bignum_options);
        return ret;
    }

    while (1) {
        ret = menu_get_opt_code(&opt_code, bignum_options);
        if (ret != MENU_RET_SUCCESS) {
            if (ret != MENU_RET_EOF) {
                printf("menu_get_opt_code failed (%d)\n", ret);

                /* If fail to get an option, we show available options for user */
                menu_opt_help("algo bignum", bignum_options);
                return ret;
            }
            break;
        }

        switch (opt_code) {
        case OPT_CODE_BIGNUM_HELP:
            menu_opt_help("algo bignum", bignum_options);
            break;

        case OPT_CODE_BIGNUM_DEMO:
            // menu_get_arg(0, NULL, NULL);
            bignum_test((char *)menu_get_arg(0, NULL, NULL));
            break;

        default:
            break;
        }
    }

    return ret;
}

void bignum_test(char *str)
{
#if !(CONFIG_ENABLE_LIBEXPR)
    int base = 10;
    int outbase = (base == 0 ? 10 : base);

    printf("bignum_test start>>>\n");

    mpz_t mpz_a;
    mpz_init_set_ui(mpz_a, 55L);

    printf ("base %d: ", base);
    printf ("result ");
    mpz_out_str (stdout, outbase, mpz_a);
    printf ("\n");

    // HMODULE handle = LoadLibrary("C:\\Workspace\\GMP\\.libs\\libgmp-10.dll");
    // FreeLibrary(handle);

    printf("bignum_test end<<<\n");
#else
    int type = 'f';
    int base = 0;
    unsigned long prec = 32;
    char *arg = str;

    run_expr(type, base, prec, arg);
#endif
}
#include "menu_bignum.h"


const menu_option_t  menu_config_bignum =
    {"bignum",  's',    menu_func_bignum,   "Big number related implementations."};

static const menu_option_t _menu_options[] =
{
    {"--help", '-',     NULL,               "Display this summary.",},
    {"--demo", '-',     demo_bignum,        "Demo GMP library.",},
    {"--expr", 's',     bignum_expr,        "Evaluate the math expression.",    "--expr [-z] [-q] [-f] [-p prec] [-b base] expression"},

    {NULL}
};

static void run_expr(int type, int base, unsigned long prec, char *str);

ret_code menu_func_bignum(int argc, char **argv)
{
    int ret;
    const char *func_name = menu_config_bignum.name;

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

ret_code demo_bignum(int argc, char **argv)
{
    int type = 'f';
    int base = 0;
    unsigned long prec = 32;
    char *arg = "2**64";

    run_expr(type, base, prec, arg);
    return MENU_RET_SUCCESS;
}

ret_code bignum_expr(int argc, char **argv)
{
    int ret = MENU_RET_FAIL;
    int type = 'z';
    int base = 0;
    unsigned long prec = 64;
    int seen_expr = 0;
    int opt;
    char *arg;

    while (1) {
        arg = menu_get_arg_inc(NULL, NULL);
        if (arg == NULL)
            break;

        if (arg[0] == '-') {
            while (1) {
                opt = *++arg;

                switch (opt) {
                case '\0':
                    goto end_opt;

                case 'f':
                case 'q':
                case 'z':
                    type = opt;
                    break;

                case 'b':
                    if (*++arg == '\0') {
                        arg = menu_get_arg(NULL, NULL);
                        if (arg == NULL) {
need_arg:
                            fprintf (stderr, "Need argument for -%c\n", opt);
                            goto exit;
                        }
                    }
                    base = atoi(arg);
                    goto end_opt;

                case 'p':
                    if (*++arg == '\0') {
                        arg = menu_get_arg(NULL, NULL);
                        if (arg == NULL)
                            goto need_arg;
                    }
                    prec = atoi(arg);
                    goto end_opt;

                case '-':
                    if (*++arg != '\0') {
                        /* no "--foo" options */
                        fprintf (stderr, "Unrecognised option --%s\n", arg);
                        goto exit;
                    }

                    /* stop option interpretation at "--" */
                    while (1) {
                        arg = menu_get_arg(NULL, NULL);
                        if (arg == NULL)
                            goto done;
                        run_expr(type, base, prec, arg);
                        seen_expr = 1;
                    }

                default:
                    fprintf (stderr, "Unrecognised option -%c\n", opt);
                    goto exit;
                }
            }
end_opt:
            ;
        } else {
            // run expression with default settings
            run_expr(type, base, prec, arg);
            seen_expr = 1;
        }
    }
done:
    if (!seen_expr) {
        printf("No expression...\n", argv[0]);
        goto exit;
    }

    ret = MENU_RET_SUCCESS;
exit:
    return ret;
}

void run_expr(int type, int base, unsigned long prec, char *str)
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

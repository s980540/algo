#include "menu_opt.h"

/* Global Variables */
bool end_of_program = 0;

/* Static Function Declarations */
static const char *argtype2str(const struct _MENU_OPTION *opt);

/* Static Variables */
static char cmdin[256];
static char *_argv_buf[16];

static const struct _MENU_OPTION *menu_options;
static const struct _MENU_OPTION menu_default_options[] = {
    {NULL}
};

#if 0
const struct _MENU_OPTION var_opt[] = {
    {"l",           SET_LBA,                        'v', "SET_LBA"},
    {"s",           SET_SECTOR_COUNT,               'v', "SET_SECTOR_COUNT"},
    {"d",           SET_PHYDRIVE,                   'v', "SET_PHYDRIVE"},
    {"p",           SET_PARTITION_ENTRY_NUM,        'v', "SET_PARTITION_ENTRY_NUM"},
    {"b",           SET_EXTENDED_PAR,               'v', "SET_EXTENDED_PAR"},
    {NULL}
};
#endif

const static char menu_opt_help_str[] = "--";
const static char menu_opt_more_str[] = "---";

static char **menu_opt_argv;
static int menu_opt_arg_count;
static int menu_opt_index;
static int menu_opt_arg_type = 0;
static char *menu_opt_param;
static int g_val;
static char *g_dunno;
// static const struct _MENU_OPTION *g_unknown;
// static const struct _MENU_OPTION *menu_default_options;
static char prog_name_buf[OPT_MAX_PROG_NAME_LEN];

char *get_prog_name(const char *argv)
{
    size_t i, n;
    const char *p;
    char *q;

    /* Find the last '/', '\' or ':' */
    for (p = argv + strlen(argv); --p > argv;) {
        if (*p == '/' || *p == '\\' || *p == ':') {
            p++;
            break;
        }
    }

    /* Strip off trailing nonsense. */
    n = strlen(p);
    if ((n > 4) && ((strcmp(&p[n - 4], ".exe") == 0) || (strcmp(&p[n - 4], ".EXE") == 0)))
        n -= 4;

    /* Copy over the prog name, in lowercae. */
    if (n > sizeof(prog_name_buf) - 1)
        n = sizeof(prog_name_buf) - 1;

    for (q = prog_name_buf, i = 0; i < n; i++, p++)
        *q++ = isupper(*p) ? tolower(*p) : *p;

    *q = '\0';

    return(prog_name_buf);
}

/**
  * @brief  Initialize menu option module
  * @param  prog_name
  * @param  argc
  * @param  argv
  * @retval None
  */
ret_code menu_opt_init(
    char **prog_name,
    int argc,
    char **argv,
    const MENU_OPTION *_menu_options)
{
    ret_code ret = MENU_OPT_RET_SUCESS;
    const struct _MENU_OPTION *opt;

    /* Initialize related variable in this module */
    menu_opt_arg_count = argc;
    menu_opt_argv = argv;
    menu_opt_index = 1;
    *prog_name = get_prog_name(argv[0]);
    menu_opt_arg_type = 0;

    if (_menu_options != NULL) {
        menu_options = _menu_options;
    } else {
        menu_options = menu_default_options;
    }

    /* Check if option name is legit */
    for (opt = menu_options; opt->name; opt++) {
        const struct _MENU_OPTION *next_opt;

        /* Make sure opt name is legit */
        if (opt->name[0] == '-'
        ||  opt->name[0] == '\0') {
            return MENU_OPT_RET_INVALID_NAME;
        }

        // /* Check if code is correct */
        // if (opt->menu_opt_code <= 0
        // ||  opt->menu_opt_code >= MENU_OPT_CODE_COUNT) {
        //     return MENU_OPT_RET_INVALID_CODE;
        // }

        /* Check if arg type is correct */
        switch (opt->arg_type) {
        case 0:
        case '-':
        case '/':
        case '<':
        case '>':
        case 'E':
        case 'F':
        case 'M':
        case 'U':
        case 'f':
        case 'l':
        case 'n':
        case 'p':
        case 'v':
        case 's':
        case 'u':
        case 'c':
            break;

        default:
            return MENU_OPT_RET_INVALID_ARG_TYPE;
        }

        /* Make sure there are no duplicates. */
        for (next_opt = opt + 1; next_opt->name; next_opt++) {
            if (strcmp(opt->name, next_opt->name)) {
                return MENU_OPT_RET_DUPLICATED_OPT;
            }
        }
    }

    return ret;
}

ret_code menu_opt_getchar(void)
{
    int i;
    char *p, c;

    printf("Algo> ");

    /* Get user's input from console */
    for (i = 0;;) {
        c = getchar();

        /* c == 10, 0xA, LF, line feed */
        if (c == 10) {
            cmdin[i] = '\0';
            break;
        }
        /* If the user press Ctrl+c to end the execution of the program */
        else if (c == EOF) {
            // end_of_program = 1;
            // debug
            printf("Ctrl + C!\n");
            return MENU_OPT_RET_EOF;
        } else {
            cmdin[i++] = c;
        }
    }

    /* Convert string from console to _argv_buf[] */
    for (i = 0, p = cmdin, menu_opt_argv = _argv_buf; i < 16; ) {

        if (p == NULL || *p == '\0') {
            break;
        }
        /* Skip all space before the next command */
        else if (*p == ' ') {
            p++;
            continue;
        }

        _argv_buf[i] = p;

        if ((p = strchr(p, ' ')) != NULL)
            *p++ = '\0';

        i++;
    }

    /* Clean up next cmd buffer to indicate no more cmd here */
    if (i < 16)
        _argv_buf[i] = NULL;

    /*
     * Because the first element of _argv_buf is not the prog name,
     * so we set menu_opt_index to 0.
     */
    menu_opt_index = 0;

    return MENU_OPT_RET_SUCESS;
}

ret_code menu_opt_next(int *menu_opt_code)
{
    ret_code ret = MENU_OPT_RET_SUCESS;
    const struct _MENU_OPTION *opt;
    char *p;
    int ival;
    long lval;
    unsigned long ulval;

    menu_opt_param = NULL;

    /* Look at current arg; at the end of the list? */
    if ((p = menu_opt_argv[menu_opt_index++]) == NULL) {
        return MENU_OPT_RET_EOF;
    }

#if (CONFIG_MENU_OPT_DEBUG_MESSAGE)
    /* Use for debugging */
    printf("menu_opt_argv ::%s\n", p);
#endif

    /* If word doesn't start with a -, we're done. */
    if (*p != '-' && *p != '-') {
        return MENU_OPT_RET_EOF;
    }

    /* Hit "--"? We're done. */
    if (strcmp(p, "--") == 0 || strcmp(p, "##") == 0) {
        return MENU_OPT_RET_EOF;
    }

    /* Allow -mmm and --mmm */
    if (*++p == '-' || *p == '-') {
        p++;
    }

    /* If we have --flag=foo, snip it off. */
    if ((menu_opt_param = strchr(p, '=')) != NULL)
        *menu_opt_param++ = '\0';

    for (opt = menu_options; opt->name; opt++) {
        /* If not this option, move on to the next one. */
        if (strcmp(p, opt->name) != 0)
            continue;

        /* If it doesn't take a value, make sure none was given. */
        if (opt->arg_type == 0 || opt->arg_type == '-') {
            if (menu_opt_param) {
                return MENU_OPT_RET_INVALID_ARG_TYPE;
            }

            *menu_opt_code = opt->menu_opt_code;
            menu_opt_arg_type = 0;

            return MENU_OPT_RET_SUCESS;
        }

        /* Want a value; get the next param if =foo not use. */
        if (menu_opt_param == NULL) {
            if (menu_opt_argv[menu_opt_index] == NULL) {
                return MENU_OPT_RET_INVALID_ARG;
            } else {
                menu_opt_param = menu_opt_argv[menu_opt_index++];
            }
        }

        /* Syntax-check value. */
        switch (opt->arg_type) {
        default:
        case 's':
        case 'v':
            menu_opt_arg_type = opt->arg_type;
            break;

        case '/':
        case '>':
        case 'p':
        case 'n':
        case 'M':
        case 'U':
        case 'l':
        case 'u':
        case 'c':
        case 'E':
        case 'F':
        case 'f':
            return MENU_OPT_RET_INVALID_ARG_TYPE;
        }

        *menu_opt_code = opt->menu_opt_code;

        return MENU_OPT_RET_SUCESS;
    }

    return MENU_OPT_RET_UNKOWN_CMD;
}

void opt_help(void)
{
    const struct _MENU_OPTION *opt;
    int i;
    int sp;
    int w = 5;
    char s[80 + 1];
    char *p;
    const char *help;

    sp = menu_options[0].name != menu_opt_help_str;

    /* Find the widest help. */
    for (opt = menu_options; opt->name; opt++) {
        if (opt->name == menu_opt_more_str)
            continue;

        i = 2 + (int)strlen(opt->name);

        if (opt->arg_type != '-')
            i += 1 + strlen(argtype2str(opt));

        if (i < OPT_MAX_HELP_STR_LEN && i > w)
            w = i;
    }

    if (sp)
        printf("Usage: %s [options]\nValid options are:\n",prog_name_buf);

    /* Now let's print. */
    for (opt = menu_options; opt->name; opt++) {
        help = opt->help_str ? opt->help_str : "(No additional info)";

        if (opt->name == menu_opt_help_str) {
            printf("%s",help);
            continue;
        }

        /* Pad out prefix */
        memset(s, ' ', sizeof(s) - 1);
        s[sizeof(s) - 1] = '\0';

        if (opt->name == menu_opt_more_str) {
            s[w] = '\0';
            printf("%s  %s\n", s, help);
            continue;
        }

        /* Build up the "-flag [param]" part. */
        p = s;
        *p++ = ' ';
        *p++ = '-';
        if (opt->name[0])
            p += strlen(strcpy(p, opt->name));
        else
            *p++ = '*';

        if (opt->arg_type != '-') {
            *p++ = ' ';
            p += strlen(strcpy(p, argtype2str(opt)));
        }

        *p = ' ';

        if ((int)(p- s) >= OPT_MAX_HELP_STR_LEN) {
            *p = '\0';
            printf("%s\n",s);
            memset(s, ' ', sizeof(s));
        }

        s[w] = '\0';
        printf("%s  %s\n", s, help);
    }
    printf("\n");
}

static const char *argtype2str(const struct _MENU_OPTION *opt)
{
    switch (opt->arg_type) {
    case 0:
    case '-':
        return("");

    case 'v':
        return("val");

    case '/':
        return("dentry");

    case '<':
        return("infile");

    case '>':
        return("outfile");

    case 'p':
        return("+int");

    case 'n':
        return("int");

    case 'l':
        return("long");

    case 'u':
        return("ulong");

    case 'E':
        return("PEM|DER|ENGINE");

    case 'F':
        return("PEM|DER");

    case 'f':
        return("format");

    case 'M':
        return("intmax");

    case 'U':
        return("uintmax");
    }

    return("param");
}

#if 1
void *opt_get_arg(void)
{
    int i, len;
    char ch;

    if (menu_opt_arg_type == 0)
        __fatal_error(1);

    if (menu_opt_arg_type == 'v') {
        len = strlen_of(menu_opt_param);

        if (len >= 2) {
            /* =0x or =0X */
            if (menu_opt_param[0] == '0' && (menu_opt_param[1] == 'x' || menu_opt_param[1] == 'X')) {
                __fatal_error(len > 16);
                u64 *p = str_to_hex(menu_opt_param + 2);
                __fatal_error(!p);

                return(p);
            /* =@ */
            } else {
                for (i = 0; i < len; i++) {
                    ch = menu_opt_param[i];
                    __fatal_error(ch < '0' || ch > '9');
                }

                g_val = atoi(menu_opt_param);
                return(&g_val);
            }
        } else {
            __fatal_error((menu_opt_param[0] < '0' || menu_opt_param[0] > '9'));
            g_val = atoi(menu_opt_param);

            return(&g_val);
        }

    } else if (menu_opt_arg_type == 's') {
        return(menu_opt_param);
    }

    return(menu_opt_param);
}
#else
void *opt_get_arg(void)
{
    int i, len;
    char ch;

    if (menu_opt_arg_type == 0)
        __fatal_error(1);

    len = strlen_of(menu_opt_param);

    if (len >= 2) {
        /* =0x or =0X */
        if (menu_opt_param[0] == '0' && (menu_opt_param[1] == 'x' || menu_opt_param[1] == 'X')) {
            __fatal_error(len > 16);
            u64 *p = str_to_hex(menu_opt_param + 2);
            __fatal_error(!p);

            return(p);
        /* =@ */
        } else if (menu_opt_param[0] == '@') {
            return(menu_opt_param + 1);
        } else {
            for (i = 0; i < len; i++) {
                ch = menu_opt_param[i];
                __fatal_error(ch < '0' || ch > '9');
            }

            g_val = atoi(menu_opt_param);
            return(&g_val);
        }
    } else {
        __fatal_error((menu_opt_param[0] < '0' || menu_opt_param[0] > '9'));
        g_val = atoi(menu_opt_param);

        return(&g_val);
    }

    return(menu_opt_param);
}
#endif

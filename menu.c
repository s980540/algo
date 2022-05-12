#include "string_match.h"

#include "menu.h"

/* Global Variables */
bool end_of_program = 0;

/* Static Function Declarations */
static const char *argtype2str(const struct _MENU_OPTION *opt);

/* Static Variables */
static char cmdin[256];
static char *_argv_buf[16];

static const struct _MENU_FUNCTION *_functions;
static const struct _MENU_FUNCTION _default_functions[] = {
    {"help",    MENU_FUNC_HELP,         menu_func_help,         "Display this summary"},
    {"string",  MENU_FUNC_STRING_MATCH, menu_func_string_match, "Demo string match algorithm"},
    {NULL}
};

// static const struct _MENU_OPTION *_options;
// static const struct _MENU_OPTION _default_options[] = {
//     {NULL}
// };

const static char menu_help_str[] = "--";
const static char menu_more_str[] = "---";

static char **m_argv;
static int m_argc;
static int m_argi;
static int m_argt = 0;
// static char *m_param;
static int m_param_idx = 0;
static int m_param_cnt = 0;
static int _value;

static char _prog_name[MENU_MAX_PROG_NAME_LEN];
static char _func_name[MENU_MAX_FUNC_NAME_LEN];

char *menu_get_prog_name(const char *s)
{
    size_t i, n;
    const char *p;
    char *q;

    /* Find the last '/', '\' or ':' */
    for (p = s + strlen(s); --p > s;) {
        if (*p == '/' || *p == '\\' || *p == ':') {
            p++;
            break;
        }
    }

    /* Strip off trailing nonsense. */
    n = strlen(p);
    if ((n > 4)
    && ((strcmp(&p[n - 4], ".exe") == 0) || (strcmp(&p[n - 4], ".EXE") == 0)))
        n -= 4;

    /* Copy over the prog name, in lowercae. */
    if (n > sizeof(_prog_name) - 1)
        n = sizeof(_prog_name) - 1;

    for (q = _prog_name, i = 0; i < n; i++, p++)
        *q++ = isupper(*p) ? tolower(*p) : *p;

    *q = '\0';

    return(_prog_name);
}

/**
  * @brief  Initialize menu option module
  * @param  prog_name
  * @param  argc
  * @param  argv
  * @retval None
  */
ret_code menu_func_init(
    int argc,
    char **argv,
    const MENU_FUNCTION *menu_functions)
{
    const struct _MENU_FUNCTION *func;
    // m_argc = argc;
    // m_argv = argv;
    // m_argi = 1;
    // m_argt = 0;
    // *prog_name = menu_get_prog_name(argv[0]);

    if (menu_functions != NULL) {
        _functions = menu_functions;
    } else {
        _functions = _default_functions;
    }

    /* Check if option name is legit */
    for (func = menu_functions; func->name; func++) {
        const struct _MENU_FUNCTION *next_func;

        /* Make sure opt name is legit */
        if (func->name[0] == '\0') {
            return MENU_RET_INVALID_NAME;
        }

        /* Make sure there are no duplicates. */
        for (next_func = func + 1; next_func->name; next_func++) {
            if (strcmp(func->name, next_func->name) == 0) {
                return MENU_RET_DUPLICATED_OPT;
            }
        }
    }

    return MENU_RET_SUCCESS;
}

/**
  * @brief  Initialize menu option module
  * @param  prog_name
  * @param  argc
  * @param  argv
  * @retval None
  */
ret_code menu_opt_init(
    int argc,
    int argc_min,
    char **argv,
    const MENU_OPTION *menu_options)
{
    const struct _MENU_OPTION *opt;

    if (argc < argc_min)
        return MENU_RET_INVALID_ARG;

    /* Initialize related variable in this module */
    m_argc = argc;
    m_argv = argv;
    m_argi = 2;
    m_argt = 0;

    // if (menu_options != NULL) {
    //     _options = menu_options;
    // } else {
    //     _options = _default_options;
    // }

    /* Check if option name is legit */
    for (opt = menu_options; opt->name; opt++) {
        const struct _MENU_OPTION *next_opt;

        /* Make sure opt name is legit */
        if (opt->name[0] == '\0') {
            return MENU_RET_INVALID_NAME;
        }

        // /* Check if code is correct */
        // if (opt->code <= 0
        // ||  opt->code >= MENU_FUNC_CODE_COUNT) {
        //     return MENU_RET_INVALID_CODE;
        // }

        /* Check if arg type is correct */
        switch (opt->arg_type) {
        case 0:
        case '-':
        case 'a':
        case 'r':
        case 'g':
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
        case 's':
        case 'u':
        case 'c':
            break;

        default:
            return MENU_RET_INVALID_ARG_TYPE;
        }

        /* Make sure there are no duplicates. */
        for (next_opt = opt + 1; next_opt->name; next_opt++) {
            printf("menu_opt_init %s %s\n", opt->name, next_opt->name);
            if (strcmp(opt->name, next_opt->name) == 0) {
                return MENU_RET_DUPLICATED_OPT;
            }
        }
    }

    return MENU_RET_SUCCESS;
}

/**
  * @brief  Get user's input from console
  * @param  None
  * @retval None
  */
ret_code menu_getchar(void)
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
            return MENU_RET_EOF;
        } else {
            cmdin[i++] = c;
        }
    }

    /* Convert string from console to _argv_buf[] */
    for (i = 0, p = cmdin, m_argv = _argv_buf; i < 16; ) {

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
     * so we set m_argi to 0.
     */
    m_argi = 0;

    return MENU_RET_SUCCESS;
}

ret_code menu_func_process(
    int argc,
    char **argv,
    const MENU_FUNCTION *menu_functions)
{
    MENU_RET_CODE ret;
    char *p;

    if (argc < 2)
        return MENU_RET_EOF;

    p = argv[1];

#if (CONFIG_MENU_OPT_DEBUG_MESSAGE)
    printf("m_argv ::%s\n", p, menu_functions->name);
#endif

    for (; menu_functions->name; menu_functions++) {
        /* If not this option, move on to the next one. */
        if (strcmp(p, menu_functions->name) != 0)
            continue;

        /* Excute associate operation */
        if (menu_functions->operation)
            ret = menu_functions->operation(argc, argv);

        return ret;
    }

    return MENU_RET_UNKOWN_CMD;
}

ret_code menu_get_opt_code(
    int *opt_code,
    const MENU_OPTION *menu_options
    )
{
    const struct _MENU_OPTION *opt;
    char *p;
    int ival;
    long lval;
    unsigned long ulval;

    // m_param = NULL;
    // m_param = 0;
    m_param_cnt = 0;

    /* Look at current arg; at the end of the list? */
    if ((p = m_argv[m_argi++]) == NULL) {
        return MENU_RET_EOF;
    }

#if (CONFIG_MENU_OPT_DEBUG_MESSAGE)
    /* Use for debugging */
    printf("m_argv ::%s\n", p);
#endif

    // /* If word doesn't start with a -, we're done. */
    // if (*p != '-' && *p != '-') {
    //     return MENU_RET_EOF;
    // }

    // /* Hit "--"? We're done. */
    // if (strcmp(p, "--") == 0 || strcmp(p, "##") == 0) {
    //     return MENU_RET_EOF;
    // }

    // /* Allow -mmm and --mmm */
    // if (*++p == '-' || *p == '-') {
    //     p++;
    // }

    // /* If we have --flag=foo, snip it off. */
    // if ((m_param = strchr(p, '=')) != NULL)
    //     *m_param++ = '\0';

    for (opt = menu_options; opt->name; opt++) {
        /* If not this option, move on to the next one. */
        if (strcmp(p, opt->name) != 0)
            continue;

        /* If it doesn't take a value, make sure none was given. */
        if (opt->arg_type == 0 || opt->arg_type == '-') {
            // if (m_param)
            //     return MENU_RET_INVALID_ARG_TYPE;

            *opt_code = opt->code;
            m_argt = 0;

            return MENU_RET_SUCCESS;
        }

        /* Want a value; get the next param if =foo not use. */
        // if (m_param == NULL) {
            if (m_argv[m_argi] == NULL)
                return MENU_RET_INVALID_ARG;
            else
                m_param_idx = m_argi++;
                // m_param = m_argv[m_argi++];
        // }

        /* Syntax-check value. */
        switch (opt->arg_type) {
        default:
        case 's':
        case 'a':
            m_argt = opt->arg_type;
            m_param_cnt = 1;
            break;

        case 'r':
            m_argt = opt->arg_type;
            m_param_cnt = 2;
            break;

        case 'g':
            m_argt = opt->arg_type;
            m_param_cnt = 3;
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
            return MENU_RET_INVALID_ARG_TYPE;
        }

        *opt_code = opt->code;

        return MENU_RET_SUCCESS;
    }

    return MENU_RET_UNKOWN_CMD;
}


/* Disaplay available menu options */
void menu_opt_help(
    const char *func_name,
    const struct _MENU_OPTION *menu_options)
{
    const struct _MENU_OPTION *opt;
    int i;
    int w = 5;
    char s_buf[80 + 1];
    char *p;
    const char *s;

    /* Find the widest help. */
    for (opt = menu_options; opt->name; opt++) {
        // ST: Consider to remove this code
        if (opt->name == menu_more_str)
            continue;

        i = 2 + (int)strlen(opt->name);

        if (opt->arg_type != '-')
            i += 1 + strlen(argtype2str(opt));

        if (i < MENU_OPT_MAX_HELP_STR_LEN && i > w)
            w = i;
    }

    if (menu_options[0].name != menu_help_str) {
        printf("\nUsage: %s [options]\n\nValid options are:\n\n",
            func_name ? func_name : "");
    }

    /* Now let's print. */
    for (opt = menu_options; opt->name; opt++) {
        s = opt->help_str ? opt->help_str : "(No additional info)";

        // ST: Consider to remove this code
        if (opt->name == menu_help_str) {
            printf("%s", s);
            continue;
        }

        /* Pad out prefix */
        memset(s_buf, ' ', sizeof(s_buf) - 1);
        s_buf[sizeof(s_buf) - 1] = '\0';

        // ST: Consider to remove this code
        if (opt->name == menu_more_str) {
            s_buf[w] = '\0';
            printf("%s  %s\n", s_buf, s);
            continue;
        }

        /* Build up the "-flag [param]" part. */
        p = s_buf;
        *p++ = ' ';
        // *p++ = '-';
        if (opt->name[0])
            p += strlen(strcpy(p, opt->name));
        else
            *p++ = '*';

        if (opt->arg_type != '-') {
            *p++ = ' ';
            p += strlen(strcpy(p, argtype2str(opt)));
        }

        *p = ' ';

        if ((int)(p - s_buf) >= MENU_OPT_MAX_HELP_STR_LEN) {
            *p = '\0';
            printf("%s\n", s_buf);
            memset(s_buf, ' ', sizeof(s_buf));
        }

        s_buf[w] = '\0';
        printf("%s  %s\n", s_buf, s);
    }

    printf("\n");
}

static const char *argtype2str(const struct _MENU_OPTION *opt)
{
    switch (opt->arg_type) {
    case 0:
    case '-':
        return("");

    case 'a':
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

/**
 * @brief Using convert function to convert a selected argument to a desire result
 * @param index The index of selected argument
 * @param result The final converted result
 * @param convert The convert function (function pointer)
 * @retval a void pointer point to result
 */
void *menu_get_arg(int index, void *result, PARG_CONVERT_FUNC convert)
{
    if ((index >= m_param_cnt) || ((m_param_idx + index) > m_argc))
        return NULL;

    if (convert != NULL)
        return convert(result, m_argv[m_param_idx + index]);
    else {
        if (result != NULL)
            result = m_argv[m_param_idx + index];

        return m_argv[m_param_idx + index];
    }
}

void *opt_get_arg(void)
{
    // int i, len;
    // char ch;

    // MENU_ASSERT(m_argt != 0);

    // if (m_argt == 'a') {
    //     len = strlen_of(m_param);

    //     if (len >= 2) {
    //         /* =0x or =0X */
    //         if (m_param[0] == '0' && (m_param[1] == 'x' || m_param[1] == 'X')) {

    //             MENU_ASSERT(len <= 16);

    //             u64 *p = str_to_hex(m_param + 2);

    //             MENU_ASSERT(p != NULL);

    //             return(p);
    //         /* =@ */
    //         } else {
    //             for (i = 0; i < len; i++) {
    //                 ch = m_param[i];
    //                 __fatal_error(ch < '0' || ch > '9');
    //             }

    //             _value = atoi(m_param);
    //             return(&_value);
    //         }
    //     } else {
    //         __fatal_error((m_param[0] < '0' || m_param[0] > '9'));
    //         _value = atoi(m_param);

    //         return(&_value);
    //     }

    // } else if (m_argt == 's') {
    //     return(m_param);
    // }

    // return(m_param);
}

ret_code menu_func_help(
    int argc,
    char **argv)
{
    const struct _MENU_FUNCTION *func;
    int i;
    int w = 5;
    char s_buf[80 + 1];
    const char *s;
    char *p;

    for (func = _functions; func->name; func++) {
        i = 2 + strlen(func->name);

        if (i < MENU_FUNC_MAX_HELP_STR_LEN && i > w) {
            w = i;
        }
    }

    printf("\nUsage: %s [options]\n\nValid options are:\n\n",
            menu_get_prog_name(argv[0]));

    for (func = _functions; func->name; func++) {
        s = func->help_str ? func->help_str : "(No additional info)";

        /* Pad out prefix */
        memset(s_buf, ' ', sizeof(s_buf) - 1);
        s_buf[sizeof(s_buf) - 1] = '\0';

        /* Build up the "-flag [param]" part. */
        p = s_buf;
        *p++ = ' ';
        // *p++ = '-';

        if (func->name[0])
            p += strlen(strcpy(p, func->name));
        else
            *p++ = '*';

        *p = ' ';

        s_buf[w] = '\0';
        printf("%s  %s\n", s_buf, s);
    }

    return MENU_RET_SUCCESS;
}

#if 0
// static const struct _MENU_OPTION _menu_options[] = {
//     {"help",                MENU_FUNC_HELP,                     '-', "Display this summary"},
//     {"ksoc-coe-to-sram",    MENU_FUNC_KSOC_COE_TO_SRAM,         '-', "Convert .coe file to .c file (const char array)"},
//     {"serial",              MENU_FUNC_SERIAL_COMM_TEST,         '-', "Serial communication test"},
//     {"serial2",             MENU_FUNC_SERIAL_COMM_TEST2,        '-', "Serial communication test 2"},
//     {"ksoc-bin-to-array",   MENU_FUNC_KSOC_BIN_TO_ARRAY,        '-', "Convert a bin file to a file (const char array)"},
//     {"io-parser",           MENU_FUNC_KSOC_IO_SCRIPT_TO_FILE,   's', "Convert a I/O script to a file"},
//     {"date",                MENU_FUNC_KSOC_SET_BIN_DATETIME,    's', "Set bin datetime"},
//     {"ver",                 MENU_FUNC_KSOC_SET_BIN_VERSION,     's', "Set bin version"},
//     {"bin",                 MENU_FUNC_KSOC_SET_BIN_NAME,        's', "Rename bin file"},
//     {"v",                   MENU_FUNC_DEBUG_VERBOSE,            '-', "Enable verbose debug message"},
// #if 0
//     {"strmatch",    MENU_FUNC_STRING_MATCH,          '-', "Demo & test the functionality of string_match"},
//     {"ll",          MENU_FUNC_TEST_LIST,                  '-', "Demo & test the functionality of Doubly linked-list"},
//     {"trd",         TEST_READ_DISK,                 '-', "Demo & test the functionality of winapi readfile"},
//     {"twd",         TEST_WRITE_DISK,                '-', "Demo & test the functionality of winapi writefile"},
//     {"lsd",         LIST_PHYDRIVE,                  '-', "LIST_PHYDRIVE"},
//     {"rd",          READ_DISK,                      '-', "READ_DISK"},
//     {"wd",          WRITE_DISK,                     '-', "WRITE_DISK"},
//     {"rv",          READ_VOLUME,                    '-', "READ_VOLUME"},
//     {"wv",          WRITE_VOLUME,                   '-', "WRITE_VOLUME"},
//     {"prsb",        PRINT_SECTOR_BUFFER,            '-', "PRINT_SECTOR_BUFFER"},
//     {"tmbr",        TEST_MBR,                       '-', "TEST_MBR"},
//     {"mp",          MBR_PRINT,                      '-', "MBR_PRINT"},
//     {"l",           SET_LBA,                        'a', "SET_LBA"},
//     {"s",           SET_SECTOR_COUNT,               'a', "SET_SECTOR_COUNT"},
//     {"d",           SET_PHYDRIVE,                   'a', "SET_PHYDRIVE"},
//     {"v",           SET_VOLUME_LABLE,               's', "SET_VOLUME_LABLE"},
//     {"p",           SET_PARTITION_ENTRY_NUM,        'a', "SET_PARTITION_ENTRY_NUM"},
//     {"b",           SET_EXTENDED_PAR,               'a', "SET_EXTENDED_PAR"},
//     {"c",           SET_CLUSTER,                    'a', "SET_CLUSTER"},
//     {"r",           MBR_PAR_READ,                   '-', "MBR_PAR_READ"},
//     {"a",           MBR_PAR_ADD,                    '-', "MBR_PAR_ADD"},
//     {"pc",          MBR_PAR_CLEAR,                  '-', "MBR_PAR_CLR"},
//     {"mppe",        MBR_PAR_PRINT_PAR_ENTRY,        '-', "MBR_PAR_PRINT_PAR_ENTRY"},
//     {"mpped",       MBR_PAR_PRINT_PAR_ENTRY_DETAIL, '-', "MBR_PAR_PRINT_PAR_ENTRY_DETAIL"},
//     {"mppa",        MBR_PAR_PRINT_ALL,              '-', "MBR_PAR_PRINT_ALL"},
//     {"lw",          LBA_WRITE,                      '-', "LBA_WRITE"},
//     {"lc",          LBA_CLEAR,                      '-', "LBA_CLEAR"},
//     {"lcr",         LBA_CLEAR_RANGE,                '-', "LBA_CLEAR_RANGE"},
//     {"lca",         LBA_CLEAR_ALL,                  '-', "LBA_CLEAR_ALL"},
//     {"vlc",         VOLUME_LBA_CLEAR,               '-', "VOLUME_LBA_CLEAR"},
//     {"sc",          SB_COPY,                        '-', "SB_COPY"},
//     {"sp",          SB_PASTE,                       '-', "SB_PASTE"},
//     {"tgpt",        TEST_GPT,                       '-', "TEST_GPT"},
//     {"gpghd",       GPT_PAR_PRINT_GPT_HEADER_DETAIL,'-', "GPT_PAR_PRINT_GPT_HEADER_DETAIL"},
//     {"gpped",       GPT_PAR_PRINT_PAR_ENTRY_DETAIL, '-', "GPT_PAR_PRINT_PAR_ENTRY_DETAIL"},
//     {"tfat",        TEST_FAT,                       '-', "TEST_FAT"},
//     {"f32mnt",      FAT32_MOUNT,                    '-', "FAT32_MOUNT"},
//     {"f32pfid",     FAT32_PRINT_FS_INFO_DETAIL,     '-', "FAT32_PRINT_FS_INFO_DETAIL"},
//     {"f32stat",     FAT_STAT,                       '-', "FAT_STAT"},
//     {"f32cv",       FAT_COPY_VOLUME,                '-', "FAT_COPY_VOLUME"},
//     {"frc",         FAT_READ_CLUSTER,               '-', "FAT_READ_CLUSTER"},
//     {"fpc",         FAT_PRINT_CLUSTER,              '-', "FAT_PRINT_CLUSTER"},
//     {"frf",         FAT_READ_FSECT,                 '-', "FAT_READ_FSECT"},
//     {"aes",         AES_TEST,                       '-', "AES_TEST"},
// #endif
//     {"pau",         PAUSE,                          '-', "PAUSE"},
//     {"e",           END_OF_PROGRAM,                 '-', "END_OF_PROGRAM"},

//     {NULL}
// };
#if 0
const struct _MENU_OPTION var_opt[] = {
    {"l",           SET_LBA,                        'a', "SET_LBA"},
    {"s",           SET_SECTOR_COUNT,               'a', "SET_SECTOR_COUNT"},
    {"d",           SET_PHYDRIVE,                   'a', "SET_PHYDRIVE"},
    {"p",           SET_PARTITION_ENTRY_NUM,        'a', "SET_PARTITION_ENTRY_NUM"},
    {"b",           SET_EXTENDED_PAR,               'a', "SET_EXTENDED_PAR"},
    {NULL}
};
#endif

ret_code menu_process(int menu_func_code)
{
    char *prog_name = NULL;
    const char *file_name, *version = NULL, *datetime = NULL;
    int ret;

    switch (menu_func_code) {
    // case MENU_FUNC_EOF:
    // case MENU_FUNC_ERR:
    //     printf("%s: Use -help to get useful messages.\n", prog_name);
    //     break;

    case MENU_FUNC_HELP:
        menu_opt_help();
        break;

    case MENU_FUNC_KSOC_COE_TO_SRAM:
        ksoc_coe_to_sram();
        break;

    case MENU_FUNC_SERIAL_COMM_TEST:
        serial_communication_test();
        break;

    case MENU_FUNC_SERIAL_COMM_TEST2:
        serial_communication_test2();
        break;

    case MENU_FUNC_KSOC_BIN_TO_ARRAY:
        ksoc_bin_to_c_array();
        break;

    case MENU_FUNC_KSOC_IO_SCRIPT_TO_FILE:
        file_name = opt_get_arg();
        ret = ksoc_io_script_parser(file_name, flag_verbose_debug_message);
        if (ret)
            printf("ksoc_io_script_parser failed (ret %d)\n", ret);
        flag_verbose_debug_message = false;
        break;

    case MENU_FUNC_KSOC_SET_BIN_DATETIME:
        datetime = opt_get_arg();
        printf("datetime: %s\n", datetime);
        break;

    case MENU_FUNC_KSOC_SET_BIN_VERSION:
        version = opt_get_arg();
        printf("version: %s\n", version);
        break;

    case MENU_FUNC_KSOC_SET_BIN_NAME:
        file_name = opt_get_arg();
        printf("file_name: %s\n", file_name);
        ret = ksoc_set_bin_name(file_name, version, datetime);
        if (ret)
            printf("ksoc_set_bin_name failed (ret %d)\n", ret);
        datetime = NULL;
        version = NULL;
        file_name = NULL;
        break;

    case MENU_FUNC_DEBUG_VERBOSE:
        flag_verbose_debug_message = true;
        break;

#if 0
    case MENU_FUNC_STRING_MATCH:
        string_match_test();
        break;

    case MENU_FUNC_TEST_LIST:
        test_list();
        break;

    case TEST_MBR:
        test_mbr();
        break;

    case TEST_READ_DISK:
        test_readmbr();
        break;

    case TEST_WRITE_DISK:
        test_writembr();
        break;

    case LIST_PHYDRIVE:
        list_disk();
        break;

    case READ_DISK:
        read_disk(udaccs.disk_idx, udaccs.lba, udaccs.sectcnt, sector_buffer);
        break;

    case WRITE_DISK:
        write_disk(udaccs.disk_idx, udaccs.lba, udaccs.sectcnt, sector_buffer);
        break;

    case READ_VOLUME:
        __fat_read_volume(udaccs.lba, udaccs.sectcnt, sector_buffer, fat_volume_dst);
        break;

    case WRITE_VOLUME:
        __fat_write_volume(udaccs.lba, udaccs.sectcnt, sector_buffer, fat_volume_dst);
        break;

    case SET_LBA:
        udaccs.lba = GET_ARGUMENT(u32);
    #if (CONFIG_MENU_OPT_DEBUG_MESSAGE)
        printf("lba: %#x\n", udaccs.lba);
    #endif
        break;

    case SET_CLUSTER:
        udaccs.cluster = GET_ARGUMENT(u32);
    #if (CONFIG_MENU_OPT_DEBUG_MESSAGE)
        printf("cluster: %#x\n");
    #endif
        break;

    case SET_SECTOR_COUNT:
        udaccs.sectcnt = GET_ARGUMENT(u64);
    #if (CONFIG_MENU_OPT_DEBUG_MESSAGE)
        printf("sectcnt: %#x\n", udaccs.sectcnt);
    #endif
        break;

    case SET_PHYDRIVE:
        udaccs.disk_idx = GET_ARGUMENT(int);
    #if (CONFIG_MENU_OPT_DEBUG_MESSAGE)
        printf("disk_idx: %#x\n", udaccs.disk_idx);
    #endif
        break;

    case SET_VOLUME_LABLE:
    {
        u8 *str = opt_get_arg();
        u32 len = strlen_of(str) + 1;

        if (udaccs.volume_lable != NULL)
            safe_free(udaccs.volume_lable);

        udaccs.volume_lable = malloc(len);
        memcpy(udaccs.volume_lable, str, len);

    #if (CONFIG_MENU_OPT_DEBUG_MESSAGE)
        printf("volume_lable: %s\n", udaccs.volume_lable);
        if (strcmp(udaccs.volume_lable, "C") == 0 ||
            strcmp(udaccs.volume_lable, "c") == 0)
            __fatal_error(1);

        if (len != 2)
            __fatal_error(1);
    #endif
        break;
    }

    case SET_PARTITION_ENTRY_NUM:
        udaccs.par_entry_idx = GET_ARGUMENT(unsigned char);
    #if (CONFIG_MENU_OPT_DEBUG_MESSAGE)
        printf("par_entry_idx: %#x\n", udaccs.par_entry_idx);
    #endif
        break;

    case SET_EXTENDED_PAR:
        udaccs.epar = GET_ARGUMENT(u32);
    #if (CONFIG_MENU_OPT_DEBUG_MESSAGE)
        printf("base: %#x\n", udaccs.epar);
    #endif
        break;

    case PRINT_SECTOR_BUFFER:
        print_sector_buffer(udaccs.lba, udaccs.sectcnt, udaccs.buf);
        break;

    case MBR_PRINT:
        mbr_print_raw(&udaccs);
        break;

    case MBR_PAR_PRINT_PAR_ENTRY:
        mbr_par_print_par_entry_all(&udaccs);
        break;

    case MBR_PAR_PRINT_PAR_ENTRY_DETAIL:
        mbr_par_print_par_entry_detail(&udaccs);
        break;

    case MBR_PAR_READ:
        mbr_par_read();
        mbr_par_print_all_par();
        break;

    case MBR_PAR_ADD:
        if (mbr_disk_mgr == NULL) {
            printf("#-add failed. Must call init_mbr_disk(...) first.\n\n");
            break;
        }

        if (udaccs.epar && !list_is_empty(&mbr_disk_mgr->lpar_head) ||
            mbr_disk_mgr->ppar_count >= 3)
            mbr_par_add_ext();
        else
            mbr_par_add();

        mbr_par_write();
        mbr_par_print_all_par();
        break;

    case MBR_PAR_PRINT_ALL:
        mbr_par_print_all_par();
        break;

    case MBR_PAR_CLEAR:
        mbr_par_clear();
        break;

    case LBA_WRITE:
        lba_write();
        break;

    case LBA_CLEAR:
        lba_clear();
        break;

    case LBA_CLEAR_RANGE:
        lba_clear_range();
        break;

    case LBA_CLEAR_ALL:
        lba_clear_all();
        break;

    case VOLUME_LBA_CLEAR:
        fat_volume_lba_clear();
        break;

    case SB_COPY:
        sb_copy();
        break;

    case SB_PASTE:
        sb_paste();
        break;

    case TEST_GPT:
        test_gpt();
        break;

    case GPT_PAR_PRINT_GPT_HEADER_DETAIL:
        gpt_par_print_gpt_header_detail(&udaccs);
        break;

    case GPT_PAR_PRINT_PAR_ENTRY_DETAIL:
        gpt_par_print_par_entry_detail(&udaccs);
        break;

    case TEST_FAT:
        test_fat();
        break;

    case FAT32_MOUNT:
        fat32_mount(&udaccs);
        break;

    case FAT32_PRINT_FS_INFO_DETAIL:
        fat32_print_fs_info_detail(&udaccs);
        break;

    case FAT_STAT:
        if (udaccs.disk_idx == DISK_IDX)
            fat_volume_src->oper->stat(fat_volume_src);
        else
            fat_volume_dst->oper->stat(fat_volume_dst);

        break;

    case FAT_COPY_VOLUME:
        fat_volume_src->oper->copy_volume(fat_volume_dst, fat_volume_src);
        break;

    case FAT_READ_CLUSTER:
        fat_read_cluster(udaccs.disk_idx, udaccs.cluster, fat_volume_src);
        break;

    case FAT_READ_FSECT:
        fat_read_fat_sect(udaccs.disk_idx, udaccs.cluster, fat_volume_src);
        break;

    case FAT_PRINT_CLUSTER:
        fat_print_cluster(udaccs.cluster, fat_volume_src);
        break;

    case AES_TEST:
        aes_test();
        break;
#endif
    case PAUSE:
        system("pause");
        printf("\n");
        break;

    case END_OF_PROGRAM:
        // goto exit;
        return MENU_RET_EOF;
    }

    return MENU_RET_SUCCESS;
}
#endif
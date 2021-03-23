#include "opt.h"

const struct _OPT func_opt[] = {
    {"help",                MENU_OPT_HELP,                  '-', "Display this summary"},
    {"ksoc-coe-to-sram",    ALGO_KSOC_COE_TO_SRAM,          '-', "Convert .coe file to .c file (const char array)"},
    {"serial",              ALGO_SERIAL_COMM_TEST,          '-', "Serial communication test"},
    {"serial2",             ALGO_SERIAL_COMM_TEST2,         '-', "Serial communication test 2"},
    {"ksoc-bin-to-array",   ALGO_KSOC_BIN_TO_ARRAY,         '-', "Convert a bin file to a file (const char array)"},
    {"io-parser",           ALGO_KSOC_IO_SCRIPT_TO_FILE,    's', "Convert a I/O script to a file"},
    {"date",                ALGO_KSOC_SET_BIN_DATETIME,     's', "Set bin datetime"},
    {"ver",                 ALGO_KSOC_SET_BIN_VERSION,      's', "Set bin version"},
    {"bin",                 ALGO_KSOC_SET_BIN_NAME,         's', "Rename bin file"},
    {"v",                   ALGO_DEBUG_VERBOSE,             '-', "Enable verbose debug message"},
#if 0
    {"strmatch",    ALG_TEST_STRING_MATCH,          '-', "Demo & test the functionality of string_match"},
    {"ll",          ALG_TEST_LIST,                  '-', "Demo & test the functionality of Doubly linked-list"},
    {"trd",         TEST_READ_DISK,                 '-', "Demo & test the functionality of winapi readfile"},
    {"twd",         TEST_WRITE_DISK,                '-', "Demo & test the functionality of winapi writefile"},
    {"lsd",         LIST_PHYDRIVE,                  '-', "LIST_PHYDRIVE"},
    {"rd",          READ_DISK,                      '-', "READ_DISK"},
    {"wd",          WRITE_DISK,                     '-', "WRITE_DISK"},
    {"rv",          READ_VOLUME,                    '-', "READ_VOLUME"},
    {"wv",          WRITE_VOLUME,                   '-', "WRITE_VOLUME"},
    {"prsb",        PRINT_SECTOR_BUFFER,            '-', "PRINT_SECTOR_BUFFER"},
    {"tmbr",        TEST_MBR,                       '-', "TEST_MBR"},
    {"mp",          MBR_PRINT,                      '-', "MBR_PRINT"},
    {"l",           SET_LBA,                        'v', "SET_LBA"},
    {"s",           SET_SECTOR_COUNT,               'v', "SET_SECTOR_COUNT"},
    {"d",           SET_PHYDRIVE,                   'v', "SET_PHYDRIVE"},
    {"v",           SET_VOLUME_LABLE,               's', "SET_VOLUME_LABLE"},
    {"p",           SET_PARTITION_ENTRY_NUM,        'v', "SET_PARTITION_ENTRY_NUM"},
    {"b",           SET_EXTENDED_PAR,               'v', "SET_EXTENDED_PAR"},
    {"c",           SET_CLUSTER,                    'v', "SET_CLUSTER"},
    {"r",           MBR_PAR_READ,                   '-', "MBR_PAR_READ"},
    {"a",           MBR_PAR_ADD,                    '-', "MBR_PAR_ADD"},
    {"pc",          MBR_PAR_CLEAR,                  '-', "MBR_PAR_CLR"},
    {"mppe",        MBR_PAR_PRINT_PAR_ENTRY,        '-', "MBR_PAR_PRINT_PAR_ENTRY"},
    {"mpped",       MBR_PAR_PRINT_PAR_ENTRY_DETAIL, '-', "MBR_PAR_PRINT_PAR_ENTRY_DETAIL"}, 
    {"mppa",        MBR_PAR_PRINT_ALL,              '-', "MBR_PAR_PRINT_ALL"},
    {"lw",          LBA_WRITE,                      '-', "LBA_WRITE"},
    {"lc",          LBA_CLEAR,                      '-', "LBA_CLEAR"},
    {"lcr",         LBA_CLEAR_RANGE,                '-', "LBA_CLEAR_RANGE"},
    {"lca",         LBA_CLEAR_ALL,                  '-', "LBA_CLEAR_ALL"},
    {"vlc",         VOLUME_LBA_CLEAR,               '-', "VOLUME_LBA_CLEAR"},
    {"sc",          SB_COPY,                        '-', "SB_COPY"},
    {"sp",          SB_PASTE,                       '-', "SB_PASTE"},
    {"tgpt",        TEST_GPT,                       '-', "TEST_GPT"},
    {"gpghd",       GPT_PAR_PRINT_GPT_HEADER_DETAIL,'-', "GPT_PAR_PRINT_GPT_HEADER_DETAIL"},
    {"gpped",       GPT_PAR_PRINT_PAR_ENTRY_DETAIL, '-', "GPT_PAR_PRINT_PAR_ENTRY_DETAIL"}, 
    {"tfat",        TEST_FAT,                       '-', "TEST_FAT"},
    {"f32mnt",      FAT32_MOUNT,                    '-', "FAT32_MOUNT"},
    {"f32pfid",     FAT32_PRINT_FS_INFO_DETAIL,     '-', "FAT32_PRINT_FS_INFO_DETAIL"},
    {"f32stat",     FAT_STAT,                       '-', "FAT_STAT"},
    {"f32cv",       FAT_COPY_VOLUME,                '-', "FAT_COPY_VOLUME"},
    {"frc",         FAT_READ_CLUSTER,               '-', "FAT_READ_CLUSTER"},
    {"fpc",         FAT_PRINT_CLUSTER,              '-', "FAT_PRINT_CLUSTER"},
    {"frf",         FAT_READ_FSECT,                 '-', "FAT_READ_FSECT"},
    {"aes",         AES_TEST,                       '-', "AES_TEST"},
#endif
    {"pau",         PAUSE,                          '-', "PAUSE"},
    {"e",           END_OF_PROGRAM,                 '-', "END_OF_PROGRAM"},
        
    {NULL}
};

#if 0
const struct _OPT var_opt[] = {
    {"l",           SET_LBA,                        'v', "SET_LBA"},
    {"s",           SET_SECTOR_COUNT,               'v', "SET_SECTOR_COUNT"},
    {"d",           SET_PHYDRIVE,                   'v', "SET_PHYDRIVE"},
    {"p",           SET_PARTITION_ENTRY_NUM,        'v', "SET_PARTITION_ENTRY_NUM"},
    {"b",           SET_EXTENDED_PAR,               'v', "SET_EXTENDED_PAR"},
    {NULL}
};
#endif

const static char g_opt_help_str[] = "--";
const static char g_opt_more_str[] = "---";

static char **g_argv;
static int g_argc;
static int g_opt_idx;
static int g_opt_arg_type = 0;
static char *g_arg;
static int g_val;
static char *g_dunno;
static const struct _OPT *g_unknown;
static const struct _OPT *g_opt;
static char g_prog_name[OPT_MAX_PROG_NAME_LEN];

char cmdin[256];
char *argv_cmdbuf[16];

static const char *argtype2str(const struct _OPT *opt);
bool end_of_program = 0;

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
    if (n > sizeof(g_prog_name) - 1)
        n = sizeof(g_prog_name) - 1;

    for (q = g_prog_name, i = 0; i < n; i++, p++)
        *q++ = isupper(*p) ? tolower(*p) : *p;

    *q = '\0';

    return(g_prog_name);
}

void opt_init(char **prog, int argc, char **argv, const struct _OPT *opt)
{   
    g_argc = argc;
    g_argv = argv;
    g_opt_idx = 1;
    g_opt = opt;
    *prog = get_prog_name(argv[0]);
    g_unknown = NULL;
    g_opt_arg_type = 0;

    for ( ; opt->name; opt++) {
        const struct _OPT *next_opt;
        int i, d;

        if (opt->name == g_opt_help_str || opt->name == g_opt_more_str)
            continue;

        i = opt->arg_type;

        /* Make sure options are legit. */
        __fatal_error(opt->name[0] == '-');
        __fatal_error(opt->opt_code <= 0);

        switch (i) {
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
                __fatal_error(1);
        }

        /* Make sure there are no duplicates. */
        for (next_opt = opt + 1; next_opt->name; next_opt++)
            __fatal_error(!strcmp(opt->name, next_opt->name));

        if (opt->name[0] == '\0') {
            __fatal_error(1);
            /*
            if (g_unknown != NULL)
                __fatal_error(1);
            
            g_unknown = opt;

            if ((g_unknown->arg_type != 0) && (g_unknown->arg_type != '-'))
                __fatal_error(1);
            */
        }
    }

    return;
}

void opt_get(void)
{
    int i;
    char *p, c;

    printf("Algo> ");

    for (i = 0;;) {
        c = getchar();

        if (c == 10) {
            cmdin[i] = '\0';
            break;
        } else if (c == -1) {
            end_of_program = 1;
            return;
        } else {
            cmdin[i++] = c;
        }    
    }

    for (i = 0, p = cmdin, g_argv = argv_cmdbuf; i < 16;) {

        if (p == NULL || *p == '\0') {
            break;
        } else if (*p == ' ') {
            p++;
            continue;
        }

        argv_cmdbuf[i] = p;

        if ((p = strchr(p, ' ')) != NULL)
            *p++ = '\0';

        i++;
    }

    /* clean up next cmd buffer to indicate no more cmd here */
    if (i < 16)
        argv_cmdbuf[i] = 0;

    g_opt_idx = 0;
}

void opt_next(int *opt_code)
{
    char *p;
    const struct _OPT *opt;
    int ival;
    long lval;
    unsigned long ulval;

    /* Look at current arg; at the end of the list? */
    g_arg = NULL;
    if ((p = g_argv[g_opt_idx]) == NULL) {
        *opt_code = MENU_OPT_EOF;
        return;
    }

#ifdef OPT_DEBUG_CONFIG
    /* Use for debugging */
    printf("g_argv ::%s\n", p);
#endif

    /* If word doesn't start with a -, we're done. */
    if (*p != '-' && *p != '-') {
        *opt_code = MENU_OPT_EOF;
        return;
    }

    /* Hit "--"? We're done. */
    g_opt_idx++;
    if (strcmp(p, "--") == 0 || strcmp(p, "##") == 0) {
        *opt_code = MENU_OPT_EOF;
        return;
    }
    
    /* Allow -mmm and --mmm */
    if (*++p == '-' || *p == '-') {
        p++;
    }

    /* If we have --flag=foo, snip it off. */
    if ((g_arg = strchr(p, '=')) != NULL)
            *g_arg++ = '\0';

    *opt_code = MENU_OPT_EOF;
    for (opt = g_opt; opt->name; opt++) {
        /* If not this option, move on to the next one. */
        if (strcmp(p, opt->name) != 0)
            continue;
    
        /* If it doesn't take a value, make sure none was given. */
        if (opt->arg_type == 0 || opt->arg_type == '-') {
            if (g_arg) {
                *opt_code = MENU_OPT_ERR;
                __fatal_error(1);
            }
    
            *opt_code = opt->opt_code;
            g_opt_arg_type = 0;

            return;
        }

        /* Want a value; get the next param if =foo not use. */
        if (g_arg == NULL) {
            __fatal_error(1);
            /*
            if (g_argv[g_opt_idx] == NULL) {
                *opt_code = MENU_OPT_ERR;
                __fatal_error(1);
            }
                
            g_arg = g_argv[g_opt_idx++];
            */
        }

        /* Syntax-check value. */
        switch (opt->arg_type) {
        default:
        case 's':
        case 'v':
            g_opt_arg_type = opt->arg_type;
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
            __fatal_error(1);
        }

        *opt_code = opt->opt_code;
        return;
    }
    /*
    if (g_unknown != NULL) {
        g_dunno = p;
        *opt_code = g_unknown->opt_code;
        return;
    }
    */

    return;
}

void opt_help(const struct _OPT *opt)
{
    const struct _OPT *o;
    int i;
    int sp;
    int w = 5;
    char s[80 + 1];
    char *p;
    const char *help;
    
    sp = opt[0].name != g_opt_help_str;

    /* Find the widest help. */
    for (o = opt; o->name; o++) {
        if (o->name == g_opt_more_str)
            continue;
    
        i = 2 + (int)strlen(o->name);

        if (o->arg_type != '-')
            i += 1 + strlen(argtype2str(o));

        if (i < OPT_MAX_HELP_STR_LEN && i > w)
            w = i;
    }

    if (sp)
        printf("Usage: %s [options]\nValid options are:\n",g_prog_name);
    
    /* Now let's print. */
    for (o = opt; o->name; o++) {
        help = o->help_str ? o->help_str : "(No additional info)";

        if (o->name == g_opt_help_str) {
            printf("%s",help);
            continue;
        }
        
        /* Pad out prefix */
        memset(s, ' ', sizeof(s) - 1);
        s[sizeof(s) - 1] = '\0';
        
        if (o->name == g_opt_more_str) {
            s[w] = '\0';
            printf("%s  %s\n", s, help);
            continue;
        }
        
        /* Build up the "-flag [param]" part. */
        p = s;
        *p++ = ' ';
        *p++ = '-';
        if (o->name[0])
            p += strlen(strcpy(p, o->name));
        else
            *p++ = '*';
            
        if (o->arg_type != '-') {
            *p++ = ' ';
            p += strlen(strcpy(p, argtype2str(o)));
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

static const char *argtype2str(const struct _OPT *opt)
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

    if (g_opt_arg_type == 0)
        __fatal_error(1);

    if (g_opt_arg_type == 'v') {
        len = strlen_of(g_arg);

        if (len >= 2) {
            /* =0x or =0X */
            if (g_arg[0] == '0' && (g_arg[1] == 'x' || g_arg[1] == 'X')) {
                __fatal_error(len > 16);
                u64 *p = str_to_hex(g_arg + 2);
                __fatal_error(!p);    

                return(p);
            /* =@ */
            } else {
                for (i = 0; i < len; i++) {
                    ch = g_arg[i];
                    __fatal_error(ch < '0' || ch > '9');
                }

                g_val = atoi(g_arg);
                return(&g_val);
            }
        } else {
            __fatal_error((g_arg[0] < '0' || g_arg[0] > '9'));
            g_val = atoi(g_arg);

            return(&g_val);
        }

    } else if (g_opt_arg_type == 's') {
        return(g_arg);
    }

    return(g_arg);
}
#else
void *opt_get_arg(void)
{
    int i, len;
    char ch;

    if (g_opt_arg_type == 0)
        __fatal_error(1);

    len = strlen_of(g_arg);

    if (len >= 2) {
        /* =0x or =0X */
        if (g_arg[0] == '0' && (g_arg[1] == 'x' || g_arg[1] == 'X')) {
            __fatal_error(len > 16);
            u64 *p = str_to_hex(g_arg + 2);
            __fatal_error(!p);    

            return(p);
        /* =@ */
        } else if (g_arg[0] == '@') {
            return(g_arg + 1);
        } else {
            for (i = 0; i < len; i++) {
                ch = g_arg[i];
                __fatal_error(ch < '0' || ch > '9');
            }

            g_val = atoi(g_arg);
            return(&g_val);
        }
    } else {
        __fatal_error((g_arg[0] < '0' || g_arg[0] > '9'));
        g_val = atoi(g_arg);

        return(&g_val);
    }
        
    return(g_arg);
}
#endif

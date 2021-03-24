#ifndef _MENU_OPT_H
#define _MENU_OPT_H

#include <string.h>
#include <ctype.h>

#include "global.h"

#define CONFIG_MENU_OPT_DEBUG_MESSAGE       (1)

#define OPT_MAX_PROG_NAME_LEN   (40)
#define OPT_MAX_HELP_STR_LEN    (100)

#define GET_ARGUMENT(type) \
    *(type *)opt_get_arg()

#define MENU_OPT_ASSERT(cond)                               \
    do {                                                    \
        if (!(cond)) {                                      \
            printf("[%s][%s][%d] assertion failed!\n",      \
                    __FILE__, __FUNCTION__, __LINE__);      \
            while(1);                                       \
        }                                                   \
    } while(0)

typedef enum _MENU_OPT_RET_CODE
{
    MENU_OPT_RET_SUCESS = 0,
    MENU_OPT_RET_FAIL = 1,
    MENU_OPT_RET_INVALID_NAME,
    MENU_OPT_RET_INVALID_CODE,
    MENU_OPT_RET_INVALID_ARG_TYPE,
    MENU_OPT_RET_DUPLICATED_OPT,
    MENU_OPT_RET_INVALID_ARG,
    MENU_OPT_RET_UNKOWN_CMD,
    MENU_OPT_RET_EOF,

    MENU_OPT_RET_CODE_COUNT
} MENU_OPT_RET_CODE;

typedef struct _MENU_OPTION
{
    const char *name;
    int menu_opt_code;
    int arg_type;
    const char *help_str;
} MENU_OPTION;

char *get_prog_name(const char *argv);
ret_code menu_opt_init(char **prog_name, int argc, char **argv, const struct _MENU_OPTION *_menu_options);
void opt_help(void);
ret_code menu_opt_next(int *menu_opt_code);
void *opt_get_arg(void);
ret_code menu_opt_getchar(void);

extern bool end_of_program;

#endif

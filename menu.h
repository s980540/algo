#ifndef _MENU_H
#define _MENU_H

#include <string.h>
#include <ctype.h>

#include "global.h"

#define CONFIG_MENU_OPT_DEBUG_MESSAGE   (1)
#define OCNFIG_MENU_DEBUG_ENABLE        (TRUE)

#define MENU_MAX_PROG_NAME_LEN          (40)
#define MENU_MAX_FUNC_NAME_LEN          (40)
#define MENU_FUNC_MAX_HELP_STR_LEN      (100)
#define MENU_OPT_MAX_HELP_STR_LEN       (100)
#define MENU_FUNC_MAX_HELP

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

#if (CONFIG_MENU_DEBUG_ENABLE)
    #define MENU_ASSERT(cond)   ASSERT(cond)
#else
    #define MENU_ASSERT(cond)
#endif

typedef enum _MENU_RET_CODE
{
    MENU_RET_SUCCESS = 0,
    MENU_RET_FAIL = 1,
    MENU_RET_INVALID_NAME,
    MENU_RET_INVALID_CODE,
    MENU_RET_INVALID_ARG_TYPE,
    MENU_RET_DUPLICATED_OPT,
    MENU_RET_INVALID_ARG,
    MENU_RET_INVALID_INDEX,
    MENU_RET_UNKOWN_CMD,
    MENU_RET_EOF,

    MENU_RET_CODE_COUNT
} MENU_RET_CODE;

typedef enum _MENU_FUNCTION_CODE
{
    MENU_FUNC_ERR = 0,
    MENU_FUNC_EOF = 1,
    MENU_FUNC_HELP,
    MENU_FUNC_KSOC_COE_TO_SRAM,
    MENU_FUNC_SERIAL_COMM_TEST,
    MENU_FUNC_SERIAL_COMM_TEST2,
    MENU_FUNC_KSOC_BIN_TO_ARRAY,
    MENU_FUNC_KSOC_IO_SCRIPT_TO_FILE,
    MENU_FUNC_KSOC_SET_BIN_DATETIME,
    MENU_FUNC_KSOC_SET_BIN_VERSION,
    MENU_FUNC_KSOC_SET_BIN_NAME,
    MENU_FUNC_DEBUG_VERBOSE,
    /* algorithm related functions */
    MENU_FUNC_STRING_MATCH,
    MENU_FUNC_LIST,
    MENU_FUNC_C_TEST,
    MENU_FUNC_SORT,
    MENU_FUNC_TREE,
    // MENU_FUNC_TEST_SLL,
    // MENU_FUNC_TEST_LIST,
    /* file system related functions */
    TEST_READ_DISK,
    TEST_WRITE_DISK,
    TEST_WRITE_PHYDRIVE,
    READ_DISK,
    WRITE_DISK,
    READ_VOLUME,
    WRITE_VOLUME,
    READ_FSECT,
    LIST_PHYDRIVE,
    PRINT_SECTOR_BUFFER,
    TEST_MBR,
    SET_LBA,
    SET_CLUSTER,
    SET_SECTOR_COUNT,
    SET_PHYDRIVE,
    SET_VOLUME_LABLE,
    SET_PARTITION_ENTRY_NUM,
    SET_EXTENDED_PAR,
    MBR_PRINT,
    MBR_PAR_PRINT_PAR_ENTRY,
    MBR_PAR_PRINT_PAR_ENTRY_DETAIL,
    MBR_PAR_READ,
    MBR_PAR_ADD,
    MBR_PAR_PRINT_ALL,
    MBR_PAR_CLEAR,
    LBA_CLEAR,
    SB_COPY,
    LBA_CLEAR_RANGE,
    LBA_CLEAR_ALL,
    VOLUME_LBA_CLEAR,
    SB_PASTE,
    LBA_WRITE,
    /* gpt related function */
    TEST_GPT,
    GPT_PAR_PRINT_GPT_HEADER_DETAIL,
    GPT_PAR_PRINT_PAR_ENTRY_DETAIL,
    TEST_FAT,
    FAT32_MOUNT,
    FAT32_PRINT_FS_INFO_DETAIL,
    FAT_STAT,
    FAT_COPY_VOLUME,
    FAT_READ_CLUSTER,
    FAT_READ_FSECT,
    FAT_PRINT_CLUSTER,
    AES_TEST,
    PAUSE,
    END_OF_PROGRAM,

    MENU_FUNC_CODE_COUNT
} MENU_FUNCTION_CODE;

typedef struct _MENU_OPTION
{
    const char *name;
    int code;
    int arg_type;
    const char *help_str;
} MENU_OPTION;

typedef struct _MENU_FUNCTION
{
    const char *name;
    int code;
    // ret_code (*function)(int argc, char **argv);
    ret_code (*operation)(int argc, char **argv);
    const char *help_str;
} MENU_FUNCTION;

/**
 * @brief A convert function used to convert a input value to a desire result
 * @param r The final converted result
 * @param v The input value
 * @retval a void pointer point to result
 */
typedef void *(*PARG_CONVERT_FUNC)(void *r, const void *v);

char *menu_get_prog_name(const char *s);
void get_name(char *buf, const char *s);
void *opt_get_arg(void);
ret_code menu_getchar(void);

// ret_code menu_process(int menu_func_code);

ret_code menu_func_init(int argc, char **argv, const MENU_FUNCTION *menu_functions);
ret_code menu_func_help(int argc, char **argv);
ret_code menu_func_process(int argc, char **argv, const MENU_FUNCTION *menu_functions);

ret_code menu_opt_init(int argc, int argc_min, char **argv, const struct _MENU_OPTION *menu_options);
void menu_opt_help(const char *func_name, const struct _MENU_OPTION *menu_options);
ret_code menu_get_opt_code(int *opt_code, const MENU_OPTION *menu_options);
void *menu_get_arg(int index, void *result, PARG_CONVERT_FUNC convert);

extern bool end_of_program;

#endif

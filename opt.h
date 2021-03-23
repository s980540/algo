#ifndef _OPT_H
#define _OPT_H

#include <string.h>
#include <ctype.h>

#include "global.h"

#define OPT_DEBUG_CONFIG

typedef struct _OPT {
    const char *name;
    int opt_code;
    int arg_type;
    const char *help_str;
};

#define OPT_MAX_PROG_NAME_LEN   (40)
#define OPT_MAX_HELP_STR_LEN    (100)

enum _MENU_OPT_CODE_T {
    MENU_OPT_ERR = -1,
    MENU_OPT_EOF = 0,
    MENU_OPT_HELP = 1,
    ALGO_KSOC_COE_TO_SRAM,
    ALGO_SERIAL_COMM_TEST,
    ALGO_SERIAL_COMM_TEST2,
    ALGO_KSOC_BIN_TO_ARRAY,
    ALGO_KSOC_IO_SCRIPT_TO_FILE,
    ALGO_KSOC_SET_BIN_DATETIME,
    ALGO_KSOC_SET_BIN_VERSION,
    ALGO_KSOC_SET_BIN_NAME,
    ALGO_DEBUG_VERBOSE,
    /* algorithm related functions */
    ALG_TEST_STRING_MATCH,
    ALG_TEST_SLL,
    ALG_TEST_LIST,
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
    END_OF_PROGRAM
}; 

#define GET_ARGUMENT(type) \
    *(type *)opt_get_arg()

char *get_prog_name(const char *argv);
void opt_init(char **prog, int argc, char **argv, const struct _OPT *opt);
void opt_next(int *opt_code);
void opt_help(const struct _OPT *opt);
void *opt_get_arg(void);
void opt_get(void);

extern const struct _OPT func_opt[];
extern const struct _OPT var_opt[];
extern bool end_of_program;

#endif


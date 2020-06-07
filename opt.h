#ifndef _OPT_H
#define _OPT_H

#include <string.h>
#include <ctype.h>

#include "global.h"

#define OPT_DEBUG_CONFIG

struct _opt {
        const char *name;
        int opt_code;
        int arg_type;
        const char *help_str;
};

#define OPT_MAX_PROG_NAME_LEN   (40)
#define OPT_MAX_HELP_STR_LEN    (100)

enum _menu_opt_code_t {
        MENU_OPT_ERR = -1,
        MENU_OPT_EOF = 0,
        MENU_OPT_HELP = 1,
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

#define get_arg(type) \
        *(type *)opt_get_arg()

char *get_prog_name(const char *argv);
void opt_init(char **prog, int argc, char **argv, const struct _opt *opt);
void opt_next(int *opt_code);
void opt_help(const struct _opt *opt);
void *opt_get_arg(void);
void opt_get(void);

extern const struct _opt func_opt[];
extern const struct _opt var_opt[];
extern bool end_of_program;

#endif


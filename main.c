#include "global.h"
#include "menu_opt.h"
#include "data_struct.h"
#include "string_match.h"
#include "mbr.h"
#include "gpt.h"
#include "fat.h"
#include "evp.h"
#include "file.h"
#include "file_ksoc_array_generation.h"
#include "serial.h"

typedef enum _MENU_OPTION_CODE
{
    MENU_OPT_ERR = 0,
    MENU_OPT_EOF = 1,
    MENU_OPT_HELP,
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
    END_OF_PROGRAM,

    MENU_OPT_CODE_COUNT
} MENU_OPTION_CODE;

static const struct _MENU_OPTION menu_options[] = {
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

bool flag_verbose_debug_message = false;

void flag_init(void)
{
    flag_verbose_debug_message = false;
}

ret_code menu_opt_code_handle(int menu_opt_code)
{
    char *prog_name = NULL;
    const char *file_name, *version = NULL, *datetime = NULL;
    int ret;

    switch (menu_opt_code) {
    // case MENU_OPT_EOF:
    // case MENU_OPT_ERR:
    //     printf("%s: Use -help to get useful messages.\n", prog_name);
    //     break;

    case MENU_OPT_HELP:
        opt_help();
        break;

    case ALGO_KSOC_COE_TO_SRAM:
        ksoc_coe_to_sram();
        break;

    case ALGO_SERIAL_COMM_TEST:
        serial_communication_test();
        break;

    case ALGO_SERIAL_COMM_TEST2:
        serial_communication_test2();
        break;

    case ALGO_KSOC_BIN_TO_ARRAY:
        ksoc_bin_to_c_array();
        break;

    case ALGO_KSOC_IO_SCRIPT_TO_FILE:
        file_name = opt_get_arg();
        ret = ksoc_io_script_parser(file_name, flag_verbose_debug_message);
        if (ret)
            printf("ksoc_io_script_parser failed (ret %d)\n", ret);
        flag_verbose_debug_message = false;
        break;

    case ALGO_KSOC_SET_BIN_DATETIME:
        datetime = opt_get_arg();
        printf("datetime: %s\n", datetime);
        break;

    case ALGO_KSOC_SET_BIN_VERSION:
        version = opt_get_arg();
        printf("version: %s\n", version);
        break;

    case ALGO_KSOC_SET_BIN_NAME:
        file_name = opt_get_arg();
        printf("file_name: %s\n", file_name);
        ret = ksoc_set_bin_name(file_name, version, datetime);
        if (ret)
            printf("ksoc_set_bin_name failed (ret %d)\n", ret);
        datetime = NULL;
        version = NULL;
        file_name = NULL;
        break;

    case ALGO_DEBUG_VERBOSE:
        flag_verbose_debug_message = true;
        break;

#if 0
    case ALG_TEST_STRING_MATCH:
        test_string_match();
        break;

    case ALG_TEST_LIST:
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

        if (udaccs.epar && !list_empty(&mbr_disk_mgr->lpar_head) ||
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
        return MENU_OPT_RET_EOF;
    }

    return MENU_OPT_RET_SUCESS;
}

int main(int argc, char *argv[])
{
    char *prog_name = NULL;
    int menu_opt_code = MENU_OPT_EOF;
    const char *file_name, *version = NULL, *datetime = NULL;
    int ret;

    // init_sect_buffer();

    flag_init();

    // debug
    printf("argc=%d\n", argc);
    for (int i = 0; ; i++) {
        if (argv[i] == NULL)
            break;

        printf("argv[%d]=%s\n", i, argv[i]);
    }

    /* Initialize menu option module */
    ret = menu_opt_init(&prog_name, argc, argv, menu_options);

    // /* Parse argv and argc and reture a option code */
    // ret = menu_opt_next(&menu_opt_code);
    // if (ret != MENU_OPT_RET_SUCESS)
    //     goto exit;

    /* loop until menu_opt_next return EOF */
	while (1) {
        /* Parse argv and argc and reture a option code */
        while (1) {
            ret = menu_opt_next(&menu_opt_code);
            if (ret != MENU_OPT_RET_SUCESS) {
                if (ret != MENU_OPT_RET_EOF)
                    printf("%s: Use -help to get useful messages.\n", prog_name);

                break;
            }

            ret = menu_opt_code_handle(menu_opt_code);
            if (ret != MENU_OPT_RET_SUCESS) {
                goto exit;
            }
        }

        /* Get user's input from console */
        ret = menu_opt_getchar();
        if (ret != MENU_OPT_RET_SUCESS) {
            goto exit;
        }
    }

exit:
    // free_sect_buffer();
    // free_mbr_disk(mbr_disk_mgr);
    // free_fat_volume(fat_volume_dst);
    // free_fat_volume(fat_volume_src);
    // safe_free(udaccs.volume_lable);
    // safe_free(gdaccs.volume_lable);

    return(0);
}

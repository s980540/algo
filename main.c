#include "global.h"
#include "opt.h"
#include "data_struct.h"
#include "string_match.h"
#include "mbr.h"
#include "gpt.h"
#include "fat.h"
#include "evp.h"
#include "file.h"
#include "file_ksoc_array_generation.h"
#include "serial.h"

int main(int argc, char *argv[])
{
    char *prog_name = NULL;
    int opt_code = MENU_OPT_EOF;
    const char *file_name, *version = NULL, *datetime = NULL;
    int ret;
    bool enable_debug_verbose = false;

    init_sect_buffer();

    opt_init(&prog_name, argc, argv, func_opt);
    opt_next(&opt_code);

	while (!end_of_program) {
        while (opt_code != MENU_OPT_EOF) {
            switch (opt_code) {
            case MENU_OPT_EOF:
            case MENU_OPT_ERR:
                printf("%s: Use -help to get useful messages.\n", prog_name);
                break;

            case MENU_OPT_HELP:
                opt_help(func_opt);
                //opt_help(var_opt);
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
                ret = ksoc_io_script_parser(file_name, enable_debug_verbose);
                if (ret)
                    printf("ksoc_io_script_parser failed (ret %d)\n", ret);
                enable_debug_verbose = false;
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
                enable_debug_verbose = true;
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
            #ifdef OPT_DEBUG_CONFIG
                printf("lba: %#x\n", udaccs.lba);
            #endif
                break;

            case SET_CLUSTER:
                udaccs.cluster = GET_ARGUMENT(u32);
            #ifdef OPT_DEBUG_CONFIG
                printf("cluster: %#x\n");
            #endif
                break;

            case SET_SECTOR_COUNT:
                udaccs.sectcnt = GET_ARGUMENT(u64);
            #ifdef OPT_DEBUG_CONFIG
                printf("sectcnt: %#x\n", udaccs.sectcnt);
            #endif
                break;

            case SET_PHYDRIVE:
                udaccs.disk_idx = GET_ARGUMENT(int);
            #ifdef OPT_DEBUG_CONFIG
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

            #ifdef OPT_DEBUG_CONFIG
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
            #ifdef OPT_DEBUG_CONFIG
                printf("par_entry_idx: %#x\n", udaccs.par_entry_idx);
            #endif
                break;

            case SET_EXTENDED_PAR:
                udaccs.epar = GET_ARGUMENT(u32);
            #ifdef OPT_DEBUG_CONFIG
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
                goto exit;
            }

            opt_next(&opt_code);
        }

        opt_get();
        opt_next(&opt_code);
}

exit:
    free_sect_buffer();
    free_mbr_disk(mbr_disk_mgr);
    free_fat_volume(fat_volume_dst);
    free_fat_volume(fat_volume_src);
    safe_free(udaccs.volume_lable);
    safe_free(gdaccs.volume_lable);

    return(0);
}

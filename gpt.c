#include "gpt.h"

struct _gpt_par_type_guid
{
        u8 par_type_guid[16];
        char *par_type_str;
};

static struct _gpt_par_type_guid gpt_par_type_guid[] = {
        {0x00, 0x00, 0x00, 0x00,     0x00, 0x00,     0x00, 0x00,     0x00, 0x00,     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, "Unused entry"},
        {0x28, 0x73, 0x2A, 0xC1,     0x1F, 0xF8,     0xD2, 0x11,     0xBA, 0x4B,     0x00, 0xA0, 0xC9, 0x3E, 0xC9, 0x3B, "EFI System partition"},
        {0xA2, 0xA0, 0xD0, 0xEB,     0xE5, 0xB9,     0x33, 0x44,     0x87, 0xC0,     0x68, 0xB6, 0xB7, 0x26, 0x99, 0xC7, "Windows Basic data partition"},
        {0x16, 0xE3, 0xC9, 0xE3,     0x5C, 0x0B,     0xB8, 0x4D,     0x81, 0x7D,     0xF9, 0x2D, 0xF0, 0x02, 0x15, 0xAE,"Microsoft Reserved Partition"}
};

#ifdef GPT_DISK_DEBUG_MSG
#define printd(...) \
        printf(__VA_ARGS__)

#define print_ch(count, ch) \
        print_ch(count, ch)

#define print_cond(cond, ...) \
        __printd_cond(cond, __VA_ARGS__)
#else
#define printd(...)
#define print_ch(count, ch)
#define print_cond(cond, ...)
#endif

void test_gpt(void)
{
        printd("sizeof(struct _gpt): %#x\n", sizeof(struct _gpt));
}

void gpt_print_raw(void)
{
        /* TODO */
}

void gpt_par_print_gpt_header_detail(const struct _disk_access *accs)
{
        int i, j;
        struct _gpt_header *gh = &((struct _gpt *)accs->buf)->gpt_header;
        unsigned char *temp = (unsigned char *)gh;

        /* Print header */
        print_ch(61, '-');
        printd("\nGPT HEADER:\n");
        
        print_ch(12, ' ');
        for (i = 0; i < 16; i++) {
                printd("%02x ", i);
                print_cond(i == 7, "  ");
        }
        printd("\n");

        for (j = 0; j < 8; j++) {
                for (i = 0; i < 16; i++) {
                        /* Print leading address */
                        print_cond(i == 0, "0x%08x: ", ((accs->lba + 1) << 9) + j * 16);

                        /* Print raw data */
                        printd("%02x ", temp[j * 16 + i]);
                        print_cond(i == 7, "  ");
                }
                printd("\n");
        }
        printd("\n");

        printd("signature:\n>> ");
        for (i = 0; i < 8; i++) {
                printd("%02x", gh->signature[i]);
        }
        printd("\n");
        
        if (gh->signature[0] != 0) {
                printd("\n>> ");

                for (i = 0; i < 8; i++) {
                        printd("%c", gh->signature[i]);
                }
                printd("\n");
        }
        print_ch(34, '-');
        printd("\n");

        printd("revision:\n>> ");
        for (i = 0; i < 4; i++) {
                printd("%02x", gh->revision[i]);
        }
        printd("\n\n");

        printd("header_size: %#x\n", gh->header_size);
        printd("header_crc32: %#x\n", gh->header_crc32);
        printd("curr_lba: %#llx\n", gh->curr_lba);

        printd("backup_lba: %#llx\n", gh->backup_lba);
        printd("first_lba: %#llx\n", gh->first_lba);
        printd("last_lba: %#llx\n", gh->last_lba);

        printd("disk_guid:\n>> ");
        for (i = 0; i < 16; i++) {
                printd("%02x", gh->disk_guid[i]);
                print_cond(i == 7, "  ");
        }
        printd("\n\n");

        printd("par_entry_lba: %#llx\n", gh->par_entry_lba);
        printd("par_entry_num: %#x\n", gh->par_entry_num);
        printd("par_entry_size: %#x\n", gh->par_entry_size);

        printd("par_entry_crc32: %#x\n", gh->par_entry_crc32);
}

void gpt_par_print_par_entry_detail(const struct _disk_access *accs)
{
        int i, j;
        unsigned char par_entry_idx = accs->par_entry_idx;
        unsigned char *temp = ((unsigned char *)((struct _gpt *)accs->buf)->gpt_par_entry) + sizeof(struct _gpt_par_entry) * par_entry_idx;
        const struct _gpt_par_entry *gpt_par_entry = ((struct _gpt *)accs->buf)->gpt_par_entry + par_entry_idx;

        if (par_entry_idx >= 128)
                return;

        /* Print header */
        print_ch(61, '-');
        printd("\nGPT PARITION_ENTRY %d:\n", par_entry_idx);

        print_ch(12, ' ');
        for (i = 0; i < 16; i++) {
                printd("%02x ", i);
                print_cond(i == 7, "  ");
        }
        printd("\n");

        for (j = 0; j < 8; j++) {
                for (i = 0; i < 16; i++) {
                        /* Print leading address */
                        print_cond (i == 0, "0x%08x: ", ((accs->lba + 2) << 9) + par_entry_idx * 128 + j * 16);

                        /* Print data */
                        printd("%02x ", temp[j * 16 + i]);
                        print_cond(i == 7, "  ");
                }
                printd("\n");
        }
        printd("\n");

        char *par_type_str;

        for (i = 0; i < 4; i++) {
                if (memcmp(&gpt_par_entry->par_type_guid, &gpt_par_type_guid[i].par_type_guid, 16) == 0)
                        par_type_str = gpt_par_type_guid[i].par_type_str;
        }

        printd("par_type_guid: (%s)\n>> ", par_type_str);
        for (i = 0; i < 16; i++) {
                printd("%02x", gpt_par_entry->par_type_guid[i]);
                print_cond(i == 7, "  ");
        }
        printd("\n\n");

        printd("uniq_par_guid:\n>> ");
        for (i = 0; i < 16; i++) {
                printd("%02x", gpt_par_entry->uniq_par_guid[i]);
                print_cond(i == 7, "  ");
        }
        printd("\n\n");
        
        printd("first_lba: %#llx\n",      gpt_par_entry->first_lba);
        printd("last_lba: %#llx\n",       gpt_par_entry->last_lba);
        printd("attribute_flag: %#llx\n", gpt_par_entry->attribute_flag);

        printd("par_name:\n");
        for (j = 0; j < 5; j++) {
                for (i = 0; i < 16; i++) {
                        break_cond(j * 16 + i == 72);
                        print_cond(i == 0, ">> ");
                        printd("%02x",          gpt_par_entry->par_name[j * 16 + i]);
                        print_cond(i == 7, "  ");
                }
                break_cond(j * 16 + i == 72);
                printd("\n");
        }
        printd("\n");

        if (gpt_par_entry->par_name[0] != 0) {
                printd("\n");

                for (j = 0; j < 3; j++) {
                        for (i = 0; i < 32; i++) {
                                break_cond(j * 32 + i == 36);
                                print_cond(i == 0, ">> ");
                                break_cond(gpt_par_entry->par_name[(j * 32 + i) << 1] == 0);
                                printd("%c", gpt_par_entry->par_name[(j * 32 + i) << 1]);
                        }
                        break_cond(j * 32 + i == 36);
                        break_cond(gpt_par_entry->par_name[(j * 32 + i) << 1] == 0);
                        printd("\n");
                }
                printd("\n");
        }
        print_ch(34, '-');
        printd("\n");
}

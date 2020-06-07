#ifndef _GPT_H
#define _GPT_H

#include "mbr.h"
#include "types.h"
#include "utility.h"

//#define GPT_DISK_DEBUG_MSG

struct _gpt_header
{
	u8 signature[8];                /*  0,  8 */
	u8 revision[4];                 /*  8,  4 */
	u32 header_size;                /* 12,  4 */
	u32 header_crc32;               /* 16,  4 */
	u8 reserved1[4];                /* 20,  4 */
	u64 curr_lba;                   /* 24,  8 */
	u64 backup_lba;                 /* 32,  8 */
	u64 first_lba;                  /* 40,  8 */
	u64 last_lba;                   /* 48,  8 */
	u8 disk_guid[16];               /* 56, 16 */
	u64 par_entry_lba;              /* 72,  8 */
	u32 par_entry_num;              /* 80,  4 */
	u32 par_entry_size;             /* 84,  4 */
	u32 par_entry_crc32;            /* 88,  4 */
	u8 reserved2[SECTOR_SIZE - 92]; /* 92, SECTOR_SIZE - 92 */
	/* SECTOR_SIZE */
}__attribute__((packed));

struct _gpt_par_entry
{
	u8 par_type_guid[16];   /*  0, 16 */
	u8 uniq_par_guid[16];   /* 16, 16 */
	u64 first_lba;          /* 32,  8 */
	u64 last_lba;           /* 40,  8 */
	u64 attribute_flag;     /* 48,  8 */
	u8 par_name[72];        /* 56, 72 */
	/* 128 */
}__attribute__((packed));

struct _gpt
{
	struct _mbr protective_mbr;                     /* LBA 0      */
	struct _gpt_header gpt_header;                  /* LBA 1      */
	struct _gpt_par_entry gpt_par_entry[128];       /* LBA 2 - 33 */
}__attribute__((packed));

struct _secondary_gpt
{
	struct _gpt_par_entry gpt_par_entry[128];
	struct _gpt_header gpt_header;
	struct _mbr protective_mbr;
}__attribute__((packed));

struct _gpt_par_list
{
	struct _gpt_par_entry;
	struct list_head list;
	struct _bs_fat32 *fat32;
};

struct _gpt_disk
{
	struct list_head par_head;
	u32 par_count;
	/* TODO */
};

void test_gpt(void);
void gpt_par_print_gpt_header_detail(const struct _disk_access *accs);
void gpt_par_print_par_entry_detail(const struct _disk_access *accs);

#endif /* !_GPT_H */

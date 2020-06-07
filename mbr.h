#ifndef _MBR_H
#define _MBR_H

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "error.h"
#include "data_struct.h"
#include "utility.h"

#define MBR_DISK_DEBUG_MODE
//#define MBR_DISK_DEBUG_MSG

#ifdef MBR_DISK_DEBUG_MODE
#define MBR_PAR_READ_DEBUG_CONFIG
#define MBR_PAR_WRITE_DEBUG_CONFIG
#define MBR_PAR_ADD_DEBUG_CONFIG
#define MBR_PAR_ADD_EXT_DEBUG_CONFIG
#define MBR_PAR_DEL_DEBUG_CONFIG
#define MBR_PAR_DEL_EXT_DEBUG_CONFIG
#define MBR_PAR_ALLOC_DEBUG_CONFIG
#define MBR_PAR_REQ_DEBUG_CONFIG
#endif

#define MBR_RESERVED_SIZE               0x800
#define MBR_PAR_OFFSET                  MBR_RESERVED_SIZE
/*
#define DISK_LBA_START                  0x00000000
//#define DISK_LBA_END                    0xEF1000
//#define DISK_LBA_END                    0x74706800
#define DISK_LBA_END                    0x100000000LL
#define DISK_SECTOR_COUNT               (DISK_LBA_END - DISK_LBA_START)
#define DISK_RESERVED_LBA_START         (DISK_LBA_END - DISK_RESERVED_SECTOR_COUNT)
#define DISK_RESERVED_SECTOR_COUNT      0x1000
*/
#define MBR_PAR_TYPE_SHIFT              (8L)
#define MBR_PAR_TYPE_MASK               ((1 << MBR_PAR_TYPE_SHIFT) - 1)
#define MBR_PAR_LBA_BASE_MASK           (~MBR_PAR_TYPE_MASK)

struct _chs_t
{
        unsigned char head;
        union
        {
                unsigned char sector;
                struct
                {
                    unsigned char _sect:6;
                    unsigned char _cylinder_h:2;
                }__attribute__((packed));
        };
        unsigned char cylinder;
};

struct _mbr_par_entry
{
        unsigned char disk_status;  /*  0,  1 */
        struct _chs_t chs_start;    /*  1,  3 */
        unsigned char par_type;     /*  4,  1 */
        struct _chs_t chs_end;      /*  5,  3 */
        u32 lba;                    /*  8,  4 */
        u32 sectcnt;                /* 12,  4 */
        /* 16 */
}__attribute__((packed));

struct _mbr
{
        unsigned char bootstrap_code[446];      /*   0, 446 */
        struct _mbr_par_entry par_entry[4];     /* 446,  64 */
        u16 sig;
        /* 512 */
};

/* DO NOT CHANGE THE ORDER OF THE DATA MEMBER */
struct _mbr_par_list
{
        struct _mbr_par_entry;  /*  0, 16 */
        struct list_head list;  /* 16,  8 */
        union
        {
                u32 base;       /* 24,  4 */
                struct
                {
                    u32 _type:8;
                    u32 _base:24;
                }__attribute__((packed));
        };
        /* 28 */
        //struct _fat *fat;
};

struct _mbr_disk
{
        struct list_head ppar_head;
        struct list_head lpar_head;
        u32 epar_lba_start;
        u64 epar_lba_end;
        u32 ppar_count;                 /* Deprecated, only used for debugging */
};

enum _mbr_pat_list_type
{
        LIST_FREE = 0,
        LIST_MBR = 1,
        LIST_PAR = 2
};

void test_readmbr(void);
void test_writembr(void);
void test_mbr(void);
void init_mbr_disk(struct _mbr_disk **mbr_disk);
void free_mbr_disk(struct _mbr_disk *mbr_disk);
bool mbr_par_alloc(struct _mbr_par_list *req, struct list_head *head);
void mbr_print_raw(const struct _disk_access *accs);
void mbr_par_print_par_entry_all(const struct _disk_access *accs);
void mbr_par_print_par_entry_detail(const struct _disk_access *accs);
void mbr_par_print_par(void);
void mbr_par_print_free_par(void);
void mbr_par_print_all_par(void);
void mbr_par_write(void);
void mbr_par_read(void);
void mbr_par_add(void);
void mbr_par_add_ext(void);
struct _mbr_par_list *mbr_par_req(struct list_head *head, u32 sectcnt);
void mbr_par_del(void);
void mbr_par_del_all(void);
void mbr_par_gc(void);
void mbr_par_cmpr(void);
void mbr_par_clear(void);

extern struct _mbr_disk *mbr_disk_mgr;
extern struct _mbr_disk *mbr_disk_mgr_bak;

#endif  /* !_MBR_H */

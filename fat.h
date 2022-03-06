#ifndef _FAT_H
#define _FAT_H

#include "types.h"
#include "utility.h"
#include "list.h"

#define FAT_DEBUG_MODE
#define FAT_DEBUG_MSG

#define DISK_IDX 1
//#define FAT_BUF_SIZE_SHIFT              (22)
#define FAT_BUF_SIZE_SHIFT              (16)
#define FAT_BUF_SIZE                    (1 << FAT_BUF_SIZE_SHIFT)
#define FAT_BUF_SIZE_MASK               (FAT_BUF_SIZE - 1)

#define FAT_FMAP_SIZE_SHIFT             (FAT_BUF_SIZE_SHIFT - 3)
#define FAT_FMAP_SIZE                   (1 << FAT_FMAP_SIZE_SHIFT)
#define FAT_FMAP_SIZE_MASK              (FAT_FMAP_SIZE - 1)

#define FAT_BUF_SECTCNT_SHIFT           (FAT_BUF_SIZE_SHIFT - 9)
#define FAT_BUF_SECTCNT                 (1 << FAT_BUF_SECTCNT_SHIFT)
#define FAT_BUF_SECTCNT_MASK            (FAT_BUF_SECTCNT - 1)

//#define cbuf_SIZE_SHIFT          (13)
//#define cbuf_SIZE                (1 << cbuf_SIZE_SHIFT)
//#define cbuf_SIZE_MASK           (cbuf_SIZE - 1)

//#define cbuf_SECTCNT_SHIFT       (cbuf_SIZE_SHIFT - 9)
//#define cbuf_SECTCNT             (1 << cbuf_SECTCNT_SHIFT)
//#define CLSUTER_BUF_SECTCNT_MASK        (cbuf_SECTCNT - 1)

#define FAT16_ENTRY_VAL_SHIFT           (16)
#define FAT32_ENTRY_VAL_SHIFT           (28)

#define FAT16_ENTRY_VAL_MASK            ((1 << FAT16_ENTRY_VAL_SHIFT) - 1)
#define FAT32_ENTRY_VAL_MASK            ((1 << FAT32_ENTRY_VAL_SHIFT) - 1)

#define FAT16_ENTRY_VAL_INV_MASK        (~FAT16_ENTRY_VAL_MASK)
#define FAT32_ENTRY_VAL_INV_MASK        (~FAT32_ENTRY_VAL_MASK)

#define FAT16_CLR_SHUT_BIT_MASK         (0x8000)
#define FAT32_CLR_SHUT_BIT_MASK         (0x08000000)

#define FAT16_HDD_ERR_BIT_MASK          (0x4000)
#define FAT32_HDD_ERR_BIT_MASK          (0x04000000)

#define FAT_MAX_LIST_CNT 65536

enum _fat_flag_bit_t
{
        __FAT_FLAG_ROOT_DIR = 0,
        __FAT_FLAG_LIST_FULL = 1,
        __FAT_FLAG_DENTRY_EXIST = 2,
        __FAT_FLAG_CLR_DIR = 3
};

#define FAT_FLAG_BIT(bit)       (1 << (bit))
#define FAT_FLAG_ROOT_DIR       (FAT_FLAG_BIT(__FAT_FLAG_ROOT_DIR))
#define FAT_FLAG_LIST_FULL      (FAT_FLAG_BIT(__FAT_FLAG_LIST_FULL))
#define FAT_FLAG_DENTRY_EXIST   (FAT_FLAG_BIT(__FAT_FLAG_DENTRY_EXIST))
#define FAT_FLAG_CLR_DIR        (FAT_FLAG_BIT(__FAT_FLAG_CLR_DIR)) 

/* Common Boot Sector & Bios Parameter Block Structure */
struct _bs_common
{
        u8 jmp_boot[3];                 /*   0,  3 */
        u8 oem_name[8];                 /*   3,  8 */
        u16 bytes_per_sect;             /*  11,  2 */
        u8 sects_per_cluster;           /*  13,  1 */
        u16 resv_sectcnt;               /*  14,  2 */
        u8 fat_num;                     /*  16,  1 */
        u16 root_entry_cnt;             /*  17,  2 */
        u16 total_sectcnt16;            /*  19,  2 */
        u8 media;                       /*  21,  1 */
        u16 fat16_size;                 /*  22,  2 */
        u16 sects_per_track;            /*  24,  2 */
        u16 head_num;                   /*  26,  2 */
        u32 hidden_sect_num;            /*  28,  4 */
        u32 total_sectcnt32;            /*  32,  4 */
        /* 36 */
}__attribute__((packed));

/* Boot Sector & Bios Parameter Block Structure for FAT 12/16 */
struct _bs_fat16
{
        u8 drive_num;                   /*  36,   1 */
        u8 reserved1;                   /*  37,   1 */
        u8 boot_sig;                    /*  38,   1 */
        u32 volume_id;                  /*  39,   4 */
        u8 volume_lable[11];            /*  43,  11 */
        u64 file_sys_type;              /*  54,   8 */
        u8 reserved2[448];              /*  62, 448 */
        u16 sig;                        /* 510,   2 */
        /* 512 */
#if (SECTOR_SIZE > 512)
        u8 reserved3[SECTOR_SIZE - 512 - 36];/* 512, SECTOR_SIZE - 512 */
#endif
        /* SECTOR_SIZE */
}__attribute__((packed));

/* Boot Sector & Bios Parameter Block Structure for FAT 32 */
struct _bs_fat32
{
        u32 fat32_size;                 /*  36,   4 */
        u16 ext_flag;                   /*  40,   2 */
        u16 fs_ver;                     /*  42,   2 */
        u32 root_cluster;               /*  44,   4 */
        u16 fs_info;                    /*  48,   2 */
        u16 backup_boot_sector;         /*  50,   2 */
        u8 bs_reserved1[12];            /*  52,  12 */
        u8 drive_num;                   /*  64,   1 */
        u8 bs_reserved2;                /*  65,   1 */
        u8 boot_sig;                    /*  66,   1 */
        u32 volume_id;                  /*  67,   4 */
        u8 volume_lable[11];            /*  71,  11 */
        u64 file_sys_type;              /*  82,   8 */
        u8 bs_reserved3[420];           /*  90, 420 */
        u16 sig;                        /* 510,   2 */
        /* 512 */
#if (SECTOR_SIZE > 512)
        u8 reserved4[SECTOR_SIZE - 512];/* 512, SECTOR_SIZE - 512 */
#endif
        /* SECTOR_SIZE */
}__attribute__((packed));

struct _bs
{
        struct _bs_common;              /*   0,  36 */
        union                           /*  36,  SECTOR_SIZE - 36 */
        {
                struct _bs_fat16 f16;
                struct _bs_fat32 f32;
        };
};

struct _fs_info
{
        u32 lead_sig;                   /*   0,   4 */
        u8 fsi_reserved1[480];          /*   4, 480 */
        u32 struct_sig;                 /* 484,   4 */
        u32 free_cluster_cnt;           /* 488,   4 */
        u32 next_free_cluster;          /* 492,   4 */
        u8 fsi_reserved2[12];           /* 496,  12 */
        u32 trail_sig;                  /* 508,   4 */
        /* 512 */
#if (SECTOR_SIZE > 512)
        u8 reserved3[SECTOR_SIZE - 512];
#endif
        /* SECTOR_SIZE */
}__attribute__((packed));

struct _bs_resv
{
        u8 bsr_reserved1[510];
        u16 sig;
        /* 512 */
#if (SECTOR_SIZE > 512)
        u8 bsr_reserved2[SECTOR_SIZE - 512];
#endif
        /* SECTOR_SIZE */
}__attribute__((packed));

struct _bs_fat
{
        struct _bs;
        struct _fs_info;
        struct _bs_resv;
}__attribute__((packed));

struct _fat_dentry
{
        struct _fat_dentry_attr *ldentry;
        struct _fat_dentry_attr *dentry;
        //struct list_head list;
};

struct _fat_dentry_copy_list
{
        struct _fat_dentry_attr *dst;
        struct _fat_dentry_attr *src;
        struct list_head list;
};

struct _fat_dir_list
{
        u32 pdir_c;
        u32 cdir_c;
};

struct _fat_dir_copy_list
{
        u32 flag;
        struct _fat_dir_list dst;
        struct _fat_dir_list src;
        struct list_head *pdir_copy_head;
        struct list_head cdir_copy_head;
        struct list_head list;
};

struct _fat_volume
{
        /* bs oper_common */
        u8 jmp_boot[3];
        u8 oem_name[8];
        u16 bytes_per_sect;
        u8 sects_per_cluster;
        u16 resv_sectcnt;
        u8 fat_num;
        u16 root_entry_cnt;
        u8 media;
        u16 sects_per_track;
        u16 head_num;
        u32 hidden_sect_num;

        /* */
        u8 fat_type;
        u32 fat_sectcnt;

        u32 volume_sectbase;
        u32 fat_sectbase;
        u32 root_dir_sectbase;
        u32 root_dir_sectcnt;
        u32 data_sectbase;
        u32 data_sectcnt;
        u32 total_sectcnt;
        u32 cluster_cnt;
        //u8 fentry_size_shift;
        //u8 bytes_per_sect_shift;
        //u32 bytes_per_sect_mask;

        u32 rdir_c;

        u32 sect_size;
        u32 sect_size_mask;
        u8 sect_size_shift;

        u8 *cbuf;
        u32 dsz;
        /*
        u32 dir_entry_buf_mask;
        u32 dir_entry_buf_size;
        u32 dir_entry_buf_sectcnt;
        */
        u8 *dentry_buf;
        
        u32 cbuf_sectcnt;
        u32 cbuf_size;
        u32 cbuf_size_mask;

        u32 cluster_size;
        u32 cluster_size_mask;
        u32 cluster_sectcnt;

        u8 *fbuf;
        u32 fbuf_sectcnt;
        u32 fbuf_size;
        u32 fbuf_size_mask;

        u32 *dbuf;
        u32 dbuf_sectcnt;
        u32 dbuf_size;
        u32 dbuf_size_mask;
        
        u32 curr_cluster;

        u8 disk_idx;
        u8 *volume_lable;

        u8 *free_map;

        u32 fsect;
        u32 fsect_curr;
        u32 dentry_c;
        u32 dentry_cnext;
        u32 data_cstart;
        u32 data_c;
        u32 data_cnext;
        u32 free_c;
        u32 temp_c;
        u32 cmpr_c;
        //u32 fentry_val;
        u32 fentry_mask;
        //struct _fat_dentry *dentry;
        struct _fat_dentry;
        u32 dentry_offset;
        u32 dentry_size;

        struct list_head rdir_copy_head;
        struct list_head *dir_copy_head;
        u32 pdir_c;
        u32 list_cnt;
        u32 flag;
        
        struct _fat_oper *oper;
};

struct _fat_oper
{
        u32 (*fat_sectbase)(const struct _fat_volume *fat);
        u32 (*root_dir_sectbase)(const struct _fat_volume *fat);
        u32 (*root_dir_sectcnt)(const struct _fat_volume *fat);
        u32 (*data_sectbase)(const struct _fat_volume *fat);
        u32 (*data_sectcnt)(const struct _fat_volume *fat);
        u32 (*sectof)(const u32 cluster, const struct _fat_volume *fat);
        
        u32 (*fat_sectcnt)(const struct _fat_volume *fat);
        u32 (*fat_sectof)(const u32 cluster, const struct _fat_volume *fat);
        u32 (*fentry_offsetof)(const u32 cluster, const struct _fat_volume *fat);
        u32 (*get_fentry)(const u32 cluster, const struct _fat_volume *fat);
        void (*set_fentry)(const u32 cluster, const u32 val, const struct _fat_volume *fat);
        void (*stat)(struct _fat_volume *fat);
        void (*parse_dentry)(const u32 cluster, struct _fat_volume *fat);
        u32 (*fentry_idxof)(const u32 cluster, const struct _fat_volume *fat);
        void (*build_free_map)(const struct _fat_volume *fat);
        void (*copy_volume)(struct _fat_volume *src, struct _fat_volume *dst);
};

struct _entry_val
{
        u32 entry_val : 28;
        u32 reserved  : 4;
}__attribute__((packed));

struct _fat_disk_foramt_table
{
        u32 disk_size;
        u8 sects_per_cluster;
}__attribute__((packed));

struct _fat_dentry_attr
{
        u8 name[11];                    /*   0,   11 */
        u8 attr;                        /*  11,    1 */
        u8 nt_resv;                     /*  12,    1 */
        u8 create_time_10th;            /*  13,    1 */
        u16 create_time;                /*  14,    2 */
        u16 create_date;                /*  16,    2 */
        u16 last_access_date;           /*  18,    2 */
        u16 first_cluster_h;            /*  20,    2 */
        u16 last_write_time;            /*  22,    2 */
        u16 last_write_date;            /*  24,    2 */
        u16 first_cluster_l;            /*  26,    2 */
        u32 file_size;                  /*  28,    4 */
        /* 32 */
}__attribute__((packed));

struct _fat_ldentry
{
        u8 ord;                         /*   0,    1 */
        u8 name1[10];                   /*   1,   10 */
        u8 attr;                        /*  11,    1 */
        u8 type;                        /*  12,    1 */
        u8 chksum;                      /*  13,    1 */
        u8 name2[12];                   /*  14,   12 */
        u16 first_cluster_l;            /*  26,    2 */
        u8 name3[4];                    /*  28,    4 */
}__attribute__((packed));

struct _fat_stat
{
        u32 bad_cluster_cnt;
        u32 free_cluster_cnt;
        u32 resv1_cluster_cnt;
        u32 resv2_cluster_cnt;
        u32 data_cluster_cnt;
        u32 eoc_cnt;
        u32 total_entry_cnt;     
};

enum _fat_type
{
        FAT_TYPE_UNKNOWN = 0,
        FAT_TYPE_FAT12 = 1,
        FAT_TYPE_FAT16 = 2,
        FAT_TYPE_FAT32,
        FAT_TYPE_EXFAT,
};

/* msdos */
enum _fentry_val_t
{
        FREE_CLUSTER        = 0x00000000,
        EV_RESV1            = 0x00000001,
        DATA_CLUSTER_START  = 0x00000002,
        DATA_CLUSTER_END    = 0x0FFFFFEF,
        EV_RESV2_START      = 0x0FFFFFF0,
        EV_RESV2_END        = 0x0FFFFFF6,
        BAD_CLUSTER         = 0x0FFFFFF7,
        EOC                 = 0x0FFFFFF8,
        EOC_END             = 0x0FFFFFFF
};

enum _fat_file_attr
{
        ATTR_READ_ONLY  = 0x01,
        ATTR_HIDDEN     = 0x02,
        ATTR_SYSTEM     = 0x04,
        ATTR_VOLUME_ID  = 0x08,
        ATTR_DIRECTORY  = 0x10,
        ATTR_ARCHIVE    = 0x20,
};

#define ATTR_HIDDEN_SYS_DIR     (ATTR_DIRECTORY | \
                                 ATTR_SYSTEM | \
                                 ATTR_HIDDEN)

#define ATTR_LONG_NAME  (ATTR_READ_ONLY | \
                         ATTR_HIDDEN | \
                         ATTR_SYSTEM | \
                         ATTR_VOLUME_ID)

#define ATTR_LONG_NAME_MASK     (ATTR_READ_ONLY | \
                                 ATTR_HIDDEN | \
                                 ATTR_SYSTEM | \
                                 ATTR_VOLUME_ID | \
                                 ATTR_DIRECTORY | \
                                 ATTR_ARCHIVE)

#define __fat16_offsetof(cluster) \
        (cluster << 1)

#define __fat32_offsetof(cluster) \
        (cluster << 2)

#define valof(buf, offset, type) \
        *(type *)((buf) + (offset))

#define __fat16_get_fentry(buf, offset)  \
        valof(buf, offset, u16)

#define __fat32_get_fentry(buf, offset) \
        valof(buf, offset, u32) & FAT32_ENTRY_VAL_MASK

#define to_entry_val(buf, offset, type, val) \
        valof(buf, offset, type) = val

#define __fat16_set_fentry(buf, offset, val) \
        valof(buf, offset, u16) = val

#define __fat32_set_fentry(buf, offset, val) \
        valof(buf, offset, u32) = valof(buf, offset, u32) & FAT32_ENTRY_VAL_INV_MASK | (val & FAT32_ENTRY_VAL_MASK)

#define __fatx_fentry_idxof(cluster) \
        (cluster)

#define is_eoc(val, fat) \
        ((val) >= (EOC & (fat)->fentry_mask))

#define is_not_eoc(val, fat) \
        ((val) < (EOC & (fat)->fentry_mask))

#define is_free_c(val, fat) \
        ((val) == (FREE_CLUSTER & (fat)->fentry_mask)) 

#define is_not_data_c(val, fat) \
        (((val) < (DATA_CLUSTER_START & (fat)->fentry_mask)) || \
         ((val) > (DATA_CLUSTER_END & (fat)->fentry_mask)))

#define is_not_cc(val, fat) \
        (is_not_data_c(val, fat) && \
         is_not_eoc(val, fat))

#define is_dir(attr) \
        (!(((attr) & ATTR_LONG_NAME_MASK) == ATTR_LONG_NAME) && \
         !((attr) == ATTR_VOLUME_ID) && \
         !((attr) == ATTR_HIDDEN_SYS_DIR) && \
         ((attr) & ATTR_DIRECTORY)) 

#define is_not_dir(attr) \
        ((((attr) & ATTR_LONG_NAME_MASK) == ATTR_LONG_NAME) || \
         ((attr) == ATTR_VOLUME_ID) || \
         ((attr) == ATTR_HIDDEN_SYS_DIR) || \
         !((attr) & ATTR_DIRECTORY))

#define is_file(attr) \
        (!((attr) == ATTR_VOLUME_ID) && \
         !((attr) == ATTR_HIDDEN_SYS_DIR) && \
         !(((attr) & ATTR_LONG_NAME_MASK) == ATTR_LONG_NAME))

#define is_not_file(attr) \
        (((attr) == ATTR_VOLUME_ID) || \
         ((attr) == ATTR_HIDDEN_SYS_DIR) || \
         (((attr) & ATTR_LONG_NAME_MASK) == ATTR_LONG_NAME))

void test_fat(void);
void init_fat_volume(struct _fat_volume **fat_volume);
void free_fat_volume(struct _fat_volume *fat_volume);
void fat32_mount(const struct _disk_access *accs);
void fat32_print_fs_info_detail(const struct _disk_access *accs);

void fat_read_fat_sect(const int disk_idx, const u32 cluster, const struct _fat_volume *fat);
void fat_read_cluster(const int disk_idx, const u32 cluster, const struct _fat_volume *fat);
void fat_print_cluster(const u32 cluster, const struct _fat_volume *fat);
void __fat_write_volume(const u32 lba, const u32 sectcnt, void *buf, const struct _fat_volume *fat);
void __fat_read_volume(const u32 lba, const u32 sectcnt, void *buf, const struct _fat_volume *fat);
void __fat_write_volume_debug(const u32 lba, const u32 sectcnt, void *buf, const struct _fat_volume *fat, const char *str);
void __fat_read_volume_debug(const u32 lba, const u32 sectcnt, void *buf, const struct _fat_volume *fat, const char *str);
void fat_volume_lba_clear(void);

extern struct _fat_volume *fat_volume_src;
extern struct _fat_volume *fat_volume_dst;

#endif  /* !_FAT_H */

#include "fat.h"
#include "mbr.h"

const char *volume_lable_prefix = "\\\\.\\";

struct _fat_volume *fat_volume_src = NULL;
struct _fat_volume *fat_volume_dst = NULL;
struct _fat_stat fat_stat_mgr = {.bad_cluster_cnt = 0, .free_cluster_cnt = 0, .resv1_cluster_cnt = 0, .resv2_cluster_cnt = 0, .data_cluster_cnt = 0, .eoc_cnt = 0, .total_entry_cnt = 0};

struct _fat_disk_foramt_table fat16_disk_foramt_table[] = {
        {      8400,  0},
        {     32680,  2},
        {    262144,  4},
        {    524288,  8},
        {   1048576, 16},       /* disk up to 512 MiB, 8 KiB cluster */
        {   2097152, 32},
        {   4194304, 64},
        {0xFFFFFFFF,  0}
};

struct _fat_disk_foramt_table fat32_disk_format_table[] = {
        {     66600,  0},
        {    532480,  1},
        {  16777216,  8},
        {  33554432, 16},       /* disk up to 16 GiB, 16 KiB cluster */
        {  67108864, 32},       /* disk up to 32 GiB, 32 KiB cluster */
        {0xFFFFFFFF, 64}        /* disk larger than 32 GiB, 64 KiB cluster */
};

#ifdef FAT_DEBUG_MSG
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

#ifdef FAT_DEBUG_MODE
#define fat_write_volume(lba, sectcnt, buf, fat, str) \
        __fat_write_volume_debug(lba, sectcnt, buf, fat, str)
#define fat_read_volume(lba, sectcnt, buf, fat, str) \
        __fat_read_volume_debug(lba, sectcnt, buf, fat, str)
#define fat32_check_dentry(buf, size) \
        __fat32_check_dentry(buf, size)
#define fat32_print_dentry(i, dentry_attr) \
        __fat32_print_dentry(i, dentry_attr)
#define fat32_print_ldentry(dentry_attr) \
        __fat32_print_ldentry(dentry_attr)
#define fatal_error(cond) \
        __fatal_error(cond)
#else
#define fat_write_volume(lba, sectcnt, buf, fat, str) \
        __fat_write_volume(lba, sectcnt, buf, fat)
#define fat_read_volume(lba, sectcnt, buf, fat, str) \
        __fat_read_volume(lba, sectcnt, buf, fat)
#define fat32_check_dentry(buf, size)
#define fat32_print_dentry(i, dentry_attr)
#define fat32_print_ldentry(dentry_attr)
#define fatal_error(cond)
#endif

static u32 fatx_fat_sectbase(const struct _fat_volume *fat)
{
        return fat->volume_sectbase + fat->resv_sectcnt;
}

static u32 fatx_root_dir_sectbase(const struct _fat_volume *fat)
{
        return fat->volume_sectbase + fat->resv_sectcnt + (fat->fat_num * fat->fat_sectcnt);
}

static u32 fatx_root_dir_sectcnt(const struct _fat_volume *fat)
{
        return ((fat->root_entry_cnt << 5) + (fat->sect_size - 1)) / fat->sect_size;
}

static u32 fatx_data_sectbase(const struct _fat_volume *fat)
{
        return fat->volume_sectbase + fat->resv_sectcnt + fat->fat_num * fat->fat_sectcnt + fat->root_dir_sectcnt;
}

static u32 fatx_data_sectcnt(const struct _fat_volume *fat)
{
        return fat->total_sectcnt - (fat->data_sectbase - fat->volume_sectbase);
}

static u32 fatx_sectof(const u32 cluster, const struct _fat_volume *fat)
{     
        return (cluster - 2) * fat->cluster_sectcnt + fat->data_sectbase;
}

static u32 fat16_size(const struct _fat_volume *fat)
{
        u32 temp = (256 * fat->cluster_sectcnt + fat->fat_num);
        return (fat->total_sectcnt - fat->resv_sectcnt + (temp - 1)) / temp;
}

static u32 fat32_size(const struct _fat_volume *fat)
{
        u32 temp = (128 * fat->cluster_sectcnt + (fat->fat_num >> 1));
        return (fat->total_sectcnt - fat->resv_sectcnt + (temp - 1)) / temp;
}

static u32 fat16_fat_sectof(const u32 cluster, const struct _fat_volume *fat)
{
        return fat->fat_sectbase + (__fat16_offsetof(cluster) / fat->fbuf_size) * fat->fbuf_sectcnt;
}

static u32 fat32_fat_sectof(const u32 cluster, const struct _fat_volume *fat)
{
        return fat->fat_sectbase + (__fat32_offsetof(cluster) / fat->fbuf_size) * fat->fbuf_sectcnt;
}

static u32 fat16_fentry_offsetof(const u32 cluster, const struct _fat_volume *fat)
{
        return __fat16_offsetof(cluster) & fat->fbuf_size_mask;
}

static u32 fat32_fentry_offsetof(const u32 cluster, const struct _fat_volume *fat)
{
        return __fat32_offsetof(cluster) & fat->fbuf_size_mask;
}

//static u32 fatx_fentry_idxof(const u32 cluster, const struct _fat_volume *fat)
//{
        //return __fatx_fentry_idxof(cluster) & (fat->fbuf_size_mask >> 2);
//}

static u32 fat16_get_fentry(const u32 cluster, const struct _fat_volume *fat)
{
        return __fat16_get_fentry(fat->fbuf, (cluster << 1) & fat->fbuf_size_mask);
}

static u32 fat32_get_fentry(const u32 cluster, const struct _fat_volume *fat)
{
        return __fat32_get_fentry(fat->fbuf, (cluster << 2) & fat->fbuf_size_mask);
}

static void fat16_set_fentry(const u32 cluster, const u32 val, const struct _fat_volume *fat)
{
        __fat16_set_fentry(fat->fbuf, (__fat16_offsetof(cluster) & fat->fbuf_size_mask), val);
}

static void fat32_set_fentry(const u32 cluster, const u32 val, const struct _fat_volume *fat)
{
        __fat32_set_fentry(fat->fbuf, (__fat32_offsetof(cluster) & fat->fbuf_size_mask), val);
}

static void __fat32_stat(u32 cluster, u32 entry_val, struct _fat_volume *fat, struct _fat_stat *fat_stat)
{
        if (entry_val == FREE_CLUSTER)
                fat_stat->free_cluster_cnt++;
        else if (entry_val == EV_RESV1)
                fat_stat->resv1_cluster_cnt++;
        else if ((entry_val >= DATA_CLUSTER_START) && (entry_val <= DATA_CLUSTER_END))
                fat_stat->data_cluster_cnt++;
        else if ((entry_val >= EV_RESV2_START) && (entry_val <= EV_RESV2_END))
                fat_stat->resv2_cluster_cnt++;
        else if (entry_val == BAD_CLUSTER)
                fat_stat->bad_cluster_cnt++;
        else if (entry_val >= EOC)
                fat_stat->eoc_cnt++;

        fat_stat->total_entry_cnt = fat_stat->free_cluster_cnt +
                                    fat_stat->resv1_cluster_cnt +
                                    fat_stat->data_cluster_cnt +
                                    fat_stat->resv2_cluster_cnt +
                                    fat_stat->bad_cluster_cnt +
                                    fat_stat->eoc_cnt;

        printd("fsect: 0x%08x, fentry(0x%08x): 0x%08x \n[all: %#x, free: %#x, resv1: %#x, data: %#x, resv2: %#x, bad: %#x, eoc: %#x]\n", 
                fat->oper->fat_sectof(cluster, fat),
                cluster,
                entry_val,
                fat_stat->total_entry_cnt, 
                fat_stat->free_cluster_cnt, 
                fat_stat->resv1_cluster_cnt, 
                fat_stat->data_cluster_cnt,
                fat_stat->resv2_cluster_cnt,
                fat_stat->bad_cluster_cnt,
                fat_stat->eoc_cnt);

        if (cluster == fat->cluster_cnt - 1)
                printd("\n");
}

static void __fat32_print_dentry(u32 i, const struct _fat_dentry_attr *dentry_attr)
{
        u32 j;

        printd("[%s] i: 0x%08x, addr: 0x%08x, n0: 0x%08x, ", __FUNCTION__, i, dentry_attr, dentry_attr->name[0]);
        
        for (j = 0; j < 11; j++) {
                printd("%c", dentry_attr->name[j]);
        }
        printd(", ");
        for (j = 0; j < 11; j++) {
                printd("%02x", dentry_attr->name[j]);
        }
        printd("\n");

        printd("attr: 0x%02x, file_size: 0x%08x, first_cluster: 0x%08x\n", dentry_attr->attr, dentry_attr->file_size, dentry_attr->first_cluster_h << 16 | dentry_attr->first_cluster_l);
        print_ch(32, '-');
        printd("\n");        
}

static void __fat32_print_ldentry(const struct _fat_ldentry *ldentry)
{
        u32 j;

        printd("[%s] ord: %02x, attr: %02x, type: %02x, cluster: %04x\n", __FUNCTION__, ldentry->ord, ldentry->attr, ldentry->type, ldentry->first_cluster_l);
        
        printd("long name: ", ldentry->ord);
        for (j = 0; j < 10; j+=2) {
                printd("%c", ldentry->name1[j]);
        }

        for (j = 0; j < 12; j+=2) {
                printd("%c", ldentry->name2[j]);
        }

        for (j = 0; j < 4; j+=2) {
                printd("%c", ldentry->name3[j]);
        }
        printd(", ");
        for (j = 0; j < 10; j+=2) {
                printd("%02x", ldentry->name1[j]);
        }

        for (j = 0; j < 12; j+=2) {
                printd("%02x", ldentry->name2[j]);
        }

        for (j = 0; j < 4; j+=2) {
                printd("%02x", ldentry->name3[j]);
        }
        printd("\n");
        print_ch(32, '-');
        printd("\n");
}

static void fat32_check_cc(struct _fat_dentry_attr *dentry_attr, struct _fat_volume *fat)
{
        u32 cluster, file_clustercnt, fentry_val;

        file_clustercnt = (dentry_attr->file_size + (fat->cluster_size - 1)) / fat->cluster_size;
        cluster = dentry_attr->first_cluster_h << 16 | dentry_attr->first_cluster_l;

        printd("[chk_cc] cluster: %#x, file_clustercnt: %#x\n", cluster, file_clustercnt);

        while (1) {
                fat->fsect_curr = fat->oper->fat_sectof(cluster, fat);
                if (fat->fsect != fat->fsect_curr) {
                        fat->fsect = fat->fsect_curr;
                        fat_read_volume(fat->fsect, fat->fbuf_sectcnt, fat->fbuf, fat, "chk_cc");
                }

                fentry_val = fat->oper->get_fentry(cluster, fat);

                printd("file_clustercnt: %#x, cluster: %#x, fentry_val: %#x\n", file_clustercnt, cluster, fentry_val);

                if (file_clustercnt == 0 || file_clustercnt == 1) {
                        if (fentry_val < EOC)
                                __fatal_error(1);

                        break;
                } else {
                        if (fentry_val >= EOC
                            /* todo: check all invalid value */)
                                __fatal_error(1);

                        cluster = fat->oper->get_fentry(cluster, fat);
                }
                file_clustercnt--;
        }
}

static void __fat32_check_dentry(u8* dentry_buf, u32 size)
{
        struct _fat_dentry_attr *dentry_attr;
        u32 i;
        u8 n0;
    
        for (i = 0; i < size; i += 32) {
                dentry_attr = (struct _fat_dentry_attr *)(dentry_buf + i);
                n0 = dentry_attr->name[0];

                if (n0 == 0xE5)
                        continue;

                if (n0 == 0)
                        break;

                fat32_print_dentry(i, dentry_attr);
                if ((dentry_attr->attr & ATTR_LONG_NAME_MASK) == ATTR_LONG_NAME)
                        fat32_print_ldentry((struct _fat_ldentry *)dentry_attr);
        }
}

static void fat32_parse_dentry(const u32 cluster, struct _fat_volume *fat)
{
        struct _fat_dentry_attr *dentry_attr;
        u32 i, j;
        u8 n0;

        if (fat->dentry_c != cluster) {
                fat->dentry_c = cluster;
                fat_read_volume(fat->oper->sectof(fat->dentry_c, fat), fat->cbuf_sectcnt, fat->dentry_buf, fat, "parse_dir");
                printd("[%s][%c] dir_c: %#x, dir_sect: %#x\n", __FUNCTION__, __LINE__, cluster, fat->oper->sectof(fat->dentry_c, fat));                                        
        }

        for (i = 0; i < fat->cbuf_sectcnt << 9; i += 32) {

                dentry_attr = (struct _fat_dentry_attr *)(fat->dentry_buf + i);

                n0 = dentry_attr->name[0];

                if (n0 == 0xE5)
                        continue;

                if (n0 == 0)
                        break;

                fat32_print_dentry(i, dentry_attr);

                /* long dentry entry */
                if ((dentry_attr->attr & ATTR_LONG_NAME_MASK) == ATTR_LONG_NAME) {
                        fat32_print_ldentry((struct _fat_ldentry *)dentry_attr);
                        continue;
                }

                if (dentry_attr->name[0] != 0x2E)
                        fat32_check_cc(dentry_attr, fat);
                //fat32_check_dentry_crc32(dentry_attr, fat);

                if ((dentry_attr->attr == ATTR_VOLUME_ID) || (dentry_attr->attr == ATTR_HIDDEN_SYS_DIR))
                        continue;

                if ((dentry_attr->attr == ATTR_DIRECTORY) && (dentry_attr->name[0] != 0x2E))
                        fat32_parse_dentry(dentry_attr->first_cluster_h << 16 | dentry_attr->first_cluster_l, fat);
        }
}

/* if fsect is out of date, read a new fat, (unlikely, if fat buf is very large) */
static void fat_read_fat(const u32 cluster, struct _fat_volume *fat, u8 writeback)
{
        fatal_error(fat->fbuf == NULL);

        fat->fsect_curr = fat->oper->fat_sectof(cluster, fat);
        if (fat->fsect != fat->fsect_curr) {
                printf("fat->fsect: %x, fat->fsect_curr: %x\n", fat->fsect, fat->fsect_curr);
                if (writeback && (fat->fsect > 0))
                        fat_write_volume(fat->fsect, fat->fbuf_sectcnt, fat->fbuf, fat, "fat");
                        
                fat->fsect = fat->fsect_curr;
                fat_read_volume(fat->fsect, fat->fbuf_sectcnt, fat->fbuf, fat, "fat");
        }
}

/* 1. find a free cluster as a new dentry cluster (may spend lots of time, if fbuf is small) */
/* 2. always search next free cluster */
static u32 fat_get_free_c(struct _fat_volume *fat)
{
        while (1) {
                fat->free_c++;
                fatal_error(is_not_data_c(fat->free_c, fat));
                fat_read_fat(fat->free_c, fat, 1);

                /* (likely, if free space is large, unlikely if free space is small) */
                if (is_free_c(fat->oper->get_fentry(fat->free_c, fat), fat)) {
                        printd("free_c: %x\n", fat->free_c);
                        return fat->free_c;
                }
        }
}

static u32 fatx_get_fentry(u32 cluster, struct _fat_volume *fat)
{
        fat_read_fat(cluster, fat, 1);
        return fat->oper->get_fentry(cluster, fat);
}  

static void fatx_set_fentry(u32 cluster, u32 val, struct _fat_volume *fat)
{
        fat_read_fat(cluster, fat, 1);
        fat->oper->set_fentry(cluster, val, fat);
} 

//#define DENTRY_ATTR_INIT(name, ptr) \
    //struct _fat_dentry_attr *(name) = (struct _fat_dentry_attr *)(ptr);

static void fat32_find_dentry_src(struct _fat_volume *fats)
{
        u32 offset, size;
        u8 n0;

        printd("\n[%s][%d] dentry_buf: 0x%08x, offset: 0x%08x, addr: 0x%08x\n", __FUNCTION__, __LINE__, fats->dentry_buf, fats->dentry_offset, (fats->dentry_buf + fats->dentry_offset));

        while (1) {
                // if offset reach to dentry entry buf end, (unlikely, if file name is short)
                if (fats->dentry_offset == fats->cbuf_size) { 
                        if (is_eoc(fats->dentry_cnext, fats)) {
                                fats->dentry_size = 0;
                                return;
                        }

                        fats->dentry_offset = 0;
                        fats->dentry_c = fats->dentry_cnext;
                        fatal_error(is_not_cc(fats->dentry_c, fats));

                        fat_read_fat(fats->dentry_c, fats, 1);
                        fats->dentry_cnext = fats->oper->get_fentry(fats->dentry_c, fats);

                        fat_read_volume(fats->oper->sectof(fats->dentry_c, fats), fats->cbuf_sectcnt, 
                                        fats->dentry_buf, fats, "dentry_c");                            
                }

                for (size = 0, offset = fats->dentry_offset, fats->ldentry = NULL, fats->dentry = NULL;
                     offset < fats->cbuf_size; offset += fats->dsz) {

                        struct _fat_dentry_attr *dentry_attr = (struct _fat_dentry_attr *)(fats->dentry_buf + offset);

                        n0 = dentry_attr->name[0];

                        if (n0 == 0xE5)
                                continue;

                        if (n0 == 0)
                                break;

                        fat32_print_dentry(offset, dentry_attr);
                
                        // jump over long  dentry entry size and record the size
                        if ((dentry_attr->attr & ATTR_LONG_NAME_MASK) == ATTR_LONG_NAME) {
                                fats->ldentry = dentry_attr;
                                size = (((struct _fat_ldentry *)dentry_attr)->ord & 0x3F) * fats->dsz;

                                if (offset + size > fats->cbuf_size)
                                        size = (fats->cbuf_size - offset);

                                offset += (size - fats->dsz);

                                fat32_print_ldentry((struct _fat_ldentry *)dentry_attr);
                                fatal_error(offset >= fats->cbuf_size);
                                continue;
                        }

                        if ((dentry_attr->attr == ATTR_VOLUME_ID) || (dentry_attr->attr == ATTR_HIDDEN_SYS_DIR)) {
                                size = 0;
                                continue;
                        }

                        if (size == 0)
                                fats->ldentry = dentry_attr;

                        fats->dentry = dentry_attr;
                        size += fats->dsz;
                        offset += fats->dsz;
                        fatal_error(!(dentry_attr->attr & ATTR_DIRECTORY) && !fats->dentry->file_size && (fats->dentry->first_cluster_h << U16_SHIFT | fats->dentry->first_cluster_l));   
                        break;
                }

                fats->dentry_offset = offset;
                fats->dentry_size = size;

                printd("[%s][%d] ldentry: 0x%08x, dentry: 0x%08x, size: 0x%08x, offset: 0x%08x, addr: 0x%08x\n", __FUNCTION__, __LINE__, fats->ldentry, fats->dentry, size, fats->dentry_offset, (fats->dentry_buf + fats->dentry_offset));

                // if long dentry entry does not have a head
                fatal_error((size > 0 && fats->dentry == NULL) && is_eoc(fats->dentry_cnext, fats));
                fatal_error((fats->dentry_offset != fats->cbuf_size) && (size == 0 && n0 != 0));

                if (size > 0 || (size == 0 && is_eoc(fats->dentry_cnext, fats)))
                        return;
        }
}

static void fat32_find_dentry_dst(struct _fat_volume *fatd, struct _fat_volume *fats)
{
        u32 offset, size;
        u8 n0;

        printd("\n[%s][%d] dentry_buf: 0x%08x, offset: 0x%08x, addr: 0x%08x\n", __FUNCTION__, __LINE__, fatd->dentry_buf, fatd->dentry_offset, (fatd->dentry_buf + fatd->dentry_offset));

        while (1) {
                if (fatd->dentry_offset == fatd->cbuf_size) {
                        if (is_eoc(fatd->dentry_cnext, fatd)) {
                                fat_get_free_c(fatd);
                                fatd->oper->set_fentry(fatd->free_c, EOC_END, fatd);
                                
                                fat_read_fat(fatd->dentry_c, fatd, 1);
                                fatd->oper->set_fentry(fatd->dentry_c, fatd->free_c, fatd);
                                fatd->dentry_c = fatd->free_c;

                                memset(fatd->dentry_buf, 0, fatd->cbuf_size);
                        } else {
                                printd_var(2, fatd->dentry_c, fatd->dentry_cnext);
                                fatd->dentry_c = fatd->dentry_cnext;
                                fatal_error(is_not_cc(fatd->dentry_c, fatd));

                                fat_read_fat(fatd->dentry_c, fatd, 1);
                                fatd->dentry_cnext = fatd->oper->get_fentry(fatd->dentry_c, fatd); 

                                fat_read_volume(fatd->oper->sectof(fatd->dentry_c, fatd), fatd->cbuf_sectcnt, 
                                                fatd->dentry_buf, fatd, "dentry_c");
                                if (fatd->flag & FAT_FLAG_CLR_DIR)
                                        memset(fatd->dentry_buf, 0, fatd->cbuf_size);
                        }
                        fatd->dentry_offset = 0;
                }

                for (size = 0, offset = fatd->dentry_offset, fatd->ldentry = NULL, fatd->dentry = NULL;
                     offset < fatd->cbuf_size && size < fats->dentry_size; offset += fatd->dsz) {

                        struct _fat_dentry_attr *dentry_attr = (struct _fat_dentry_attr *)(fatd->dentry_buf + offset);

                        n0 = dentry_attr->name[0];

                        if (n0 == 0x0 || n0 == 0xE5) {
                                dentry_attr->name[0] = 0xE5;
                                if (size == 0)
                                        fatd->ldentry = dentry_attr;

                                fatd->dentry = dentry_attr;
                                size += fatd->dsz;
                        } else {
                                size = 0;
                        }
                }

                fatd->dentry_offset = offset;
                fatd->dentry_size = size;

                printd("[%s][%d] ldentry: 0x%08x, dentry: 0x%08x, size: 0x%08x, offset: 0x%08x, addr: 0x%08x\n", 
                        __FUNCTION__, __LINE__, fatd->ldentry, fatd->dentry, size, fatd->dentry_offset, (fatd->dentry_buf + fatd->dentry_offset));

                if (size == fats->dentry_size)
                        return;

                fatal_error(size > fats->dentry_size);          
        }
}

static void fat32_set_dentry(struct _fat_dentry_attr *dst, struct _fat_volume *fatd, 
                             struct _fat_dentry_attr *src, struct _fat_volume *fats)
{
        if (src->name[0] == 0x2e) {
                if (src->name[1] == 0x2e) {
                        // .. (parent)
                        fatd->temp_c = fatd->pdir_c == fatd->rdir_c ? 0 : fatd->pdir_c;
                } else {
                        // . (self)
                        fatd->temp_c = fatd->dentry_c;
                }
                fats->temp_c = 0;
        } else {
                fatd->temp_c = fat_get_free_c(fatd);
                fats->temp_c = src->first_cluster_h << U16_SHIFT | src->first_cluster_l;
        }
        
        dst->first_cluster_h = fatd->temp_c >> U16_SHIFT;
        dst->first_cluster_l = fatd->temp_c & U16_MASK;

        fatd->data_cnext = fatd->temp_c;
        fats->data_cnext = fats->temp_c;
        
        printd("self(.): %x parent(..): %x\n", fatd->dentry_c, fatd->pdir_c);
        printd("fats->temp_c: %x\n", fats->temp_c);
        printd("fatd->temp_c: %x\n", fatd->temp_c);
}

static void fat32_copy_file(struct _fat_dentry_attr *dir_entry_dst, struct _fat_volume *fatd, struct _fat_dentry_attr *dir_entry_src, struct _fat_volume *fats)
{
        printd("[%s][%d] start\n", __FUNCTION__, __LINE__);

        fats->data_cnext = dir_entry_src->first_cluster_h << U16_SHIFT | dir_entry_src->first_cluster_l;
        fatd->data_cnext = fat_get_free_c(fatd);
        dir_entry_dst->first_cluster_h = fatd->data_cnext >> U16_SHIFT;
        dir_entry_dst->first_cluster_l = fatd->data_cnext & U16_MASK;

        while (1) {

                if (is_eoc(fats->data_cnext, fats))
                        break;

                fats->data_c = fats->data_cnext;
                fatd->data_c = fatd->data_cnext;

                fat_read_fat(fats->data_c, fats, 1);

                fats->data_cnext = fats->oper->get_fentry(fats->data_c, fats);
                fatd->data_cnext = is_eoc(fats->data_cnext, fats) ? EOC_END : fat_get_free_c(fatd);

                // update fentry_val to point to new dentry entry cluster
                fat_read_fat(fatd->data_c, fatd, 1);
                fatd->oper->set_fentry(fatd->data_c, fatd->data_cnext, fatd);

                // write back
                //fat_write_volume(fatd->fsect, fatd->fbuf_sectcnt, fatd->fbuf, fatd, "fat");
                
                // read src data cluster
                fat_read_volume(fats->oper->sectof(fats->data_c, fats), fats->cbuf_sectcnt, 
                                fats->cbuf, fats, "src_file");

                // copy src cluster to dst cluster
                fat_write_volume(fatd->oper->sectof(fatd->data_c, fatd), fatd->cbuf_sectcnt, 
                                 fats->cbuf, fatd, "dst_file");
        }
        printd("[%s][%d] end\n\n", __FUNCTION__, __LINE__);
}

static void fat32_copy_file_seq(struct _fat_volume *fatd, struct _fat_volume *fats)
{
        printd("[%s][%d] start\n", __FUNCTION__, __LINE__);

        u32 sect_read, seq_sectcnt, sect_write;
        u32 sectcnt_read, sectcnt_write;

        fatal_error(is_not_data_c(fats->data_cnext, fats) || is_not_data_c(fatd->data_cnext, fatd));

        while (1) {
                if (is_eoc(fats->data_cnext, fats))
                        break;

                // read src
                sectcnt_read = 0;
                while (1) {
                        seq_sectcnt = 0;
                        if ((sectcnt_read + seq_sectcnt) == fats->dbuf_sectcnt ||
                            is_eoc(fats->data_cnext, fats))
                                break;
                        
                        sect_read = fats->oper->sectof(fats->data_cnext, fats);

                        // cal seq src
                        while (1) {
                                seq_sectcnt += fats->cbuf_sectcnt;
                                fats->data_c = fats->data_cnext;
                                fat_read_fat(fats->data_c, fats, 1);
                                fats->data_cnext = fats->oper->get_fentry(fats->data_c, fats);

                                printd_var(3, sectcnt_read, seq_sectcnt, fats->dbuf_sectcnt);
                                fatal_error((sectcnt_read + seq_sectcnt) > fats->dbuf_sectcnt);

                                if (((sectcnt_read + seq_sectcnt) == fats->dbuf_sectcnt) ||
                                    is_eoc(fats->data_cnext, fats) || 
                                    ((fats->data_c + 1) != fats->data_cnext))
                                        break;
                        }

                        // read src data cluster seq
                        fat_read_volume(sect_read, seq_sectcnt, fats->dbuf + (sectcnt_read << 9), fats, "src file");
                        sectcnt_read += seq_sectcnt;
                }

                // write dst
                sectcnt_write = 0; 
                while (1) {
                        seq_sectcnt = 0;
                        if (sectcnt_read == 0 || (sectcnt_write + seq_sectcnt >= sectcnt_read))
                                break;

                        sect_write = fatd->oper->sectof(fatd->data_cnext, fatd);

                        // cal seq dst
                        while (1) {
                                seq_sectcnt += fatd->cbuf_sectcnt;
                                fatd->data_c = fatd->data_cnext;
                                fat_read_fat(fatd->data_c, fatd, 1);
                                fatd->data_cnext = (is_eoc(fats->data_cnext, fats) && (sectcnt_write + seq_sectcnt >= sectcnt_read)) 
                                                    ? EOC_END : fat_get_free_c(fatd);
                                fatd->oper->set_fentry(fatd->data_c, fatd->data_cnext, fatd);

                                printd_var(4, sectcnt_write, seq_sectcnt, fats->dbuf_sectcnt, sectcnt_read);
                                fatal_error((sectcnt_write + seq_sectcnt) > sectcnt_read ||
                                            (sectcnt_write + seq_sectcnt) > fats->dbuf_sectcnt);
                                
                                if (sectcnt_write + seq_sectcnt >= sectcnt_read || 
                                    ((fatd->data_c + 1) != fatd->data_cnext))
                                        break;
                        }
                        
                        // copy src cluster to dst cluster seq
                        fat_write_volume(sect_write, seq_sectcnt, 
                                         fats->dbuf + (sectcnt_write << 9), fatd, "dst file");
                        sectcnt_write += seq_sectcnt;
                        //sectcnt_read = sectcnt_read > sectcnt_write ? sectcnt_read - seq_sectcnt : 0;
                #ifdef FAT_DEBUG_MODE
                        fats->cmpr_c = fats->temp_c;
                        fatd->cmpr_c = fatd->temp_c;
                        while (1) {
                                 fat_read_fat(fats->cmpr_c, fats, 1);
                                 fat_read_fat(fatd->cmpr_c, fatd, 1);
                                 fat_read_volume(fats->oper->sectof(fats->cmpr_c, fats), fats->cbuf_sectcnt, fats->cbuf, fats, 0);
                                 fat_read_volume(fatd->oper->sectof(fatd->cmpr_c, fats), fatd->cbuf_sectcnt, fatd->cbuf, fatd, 0);

                                 if (!memcmp(fats->cbuf, fatd->cbuf, fats->cbuf_size))
                                        fatal_error(1);
                                 fats->cmpr_c = fats->oper->get_fentry(fats->cmpr_c, fats);
                                 fatd->cmpr_c = fatd->oper->get_fentry(fatd->cmpr_c, fatd);
                        }
                #endif
                }
        }

        printd("[%s][%d] end\n\n", __FUNCTION__, __LINE__);
}

static void fat32_init(struct _fat_volume *fat)
{
    // init
    fat->fsect = 0;
    fat->fsect_curr = 0;
    fat->dentry_c = fat->rdir_c;
    fat->dentry_cnext = fat->dentry_c;
    fat->free_c = fat->rdir_c;
    //fat->dir_parent_cluster = EOC;
    //fat->dir_child_cluster = EOC;
    
    fat->ldentry = 0;
    fat->dentry = 0;
    fat->dentry_offset = fat->cbuf_size;
    fat->dentry_size = 0;
    fat->list_cnt= 0;
    fat->flag = 0;
    INIT_LIST_HEAD(&fat->rdir_copy_head);
    fat->dir_copy_head = NULL;
    //INIT_LIST_HEAD(fat->dir_copy_head);
}

static void print_dentry_attr(struct _fat_dentry_attr* dentry_attr, char *str)
{
        printd("%s\n", __FUNCTION__);
        printd_var(1, dentry_attr->attr);
        printd_var(1, dentry_attr->create_date);
        printd_var(1, dentry_attr->create_time);
        printd_var(1, dentry_attr->create_time_10th);
        printd_var(1, dentry_attr->file_size);
        printd_var(1, dentry_attr->first_cluster_h);
        printd_var(1, dentry_attr->first_cluster_l);
        printd_var(1, dentry_attr->last_access_date);
        printd_var(1, dentry_attr->last_write_date);
        printd_var(1, dentry_attr->last_write_time);
        printd_var(1, dentry_attr->name);
        printd_var(1, dentry_attr->nt_resv);
        print_ch(32, '=');
        printd("\n");
}

// copy root dentry
static void fat32_copy_dentry_file(struct _fat_volume *fatd, struct _fat_volume *fats)
{
        struct _fat_dentry_copy_list *dentry_copy, *j;
        struct _fat_dir_copy_list *dir_copy;
        LIST_HEAD(dentry_copy_head);
        bool dentry_exist = false;
        fats->dentry_offset = fats->cbuf_size;
        fatd->dentry_offset = fatd->cbuf_size;

        while (1) {
                fat32_find_dentry_src(fats);
                fat32_find_dentry_dst(fatd, fats);

                if (fats->dentry_size > 0) {
                        dentry_exist = true;
                        fatal_error(fatd->ldentry == NULL || fats->ldentry == NULL);
                        printd_var2("[memcpy] ", 3, fatd->ldentry, fats->ldentry, fats->dentry_size);
                        memcpy(fatd->ldentry, fats->ldentry, fats->dentry_size);

                        if (fats->dentry != NULL) {
                                fatal_error(is_not_file(fats->dentry->attr));
                                
                                dentry_copy = malloc(sizeof(typeof(*dentry_copy)));
                                dentry_copy->src = fats->dentry;
                                dentry_copy->dst = fatd->dentry;
                                printd_var2("[list_add_tail] ", 2, dentry_copy->src, dentry_copy->dst);

                                list_add_tail(&dentry_copy->list, &dentry_copy_head);       
                        }
                } 

                if (fats->dentry_size == 0 || fatd->dentry_offset == fatd->cbuf_size) {
                        //
                        if (dentry_exist == true) {
                                dentry_exist = false;
                                list_for_each_entry_safe(dentry_copy, j, &dentry_copy_head, list) {

                                        fat32_set_dentry(dentry_copy->dst, fatd, dentry_copy->src, fats);

                                        if (is_dir(dentry_copy->src->attr)) {
                                                fatal_error(dentry_copy->src->file_size > 0);
                                                if (dentry_copy->src->name[0] != 0x2e) {
                                                        fat_read_fat(fatd->temp_c, fatd, 1);
                                                        fatd->oper->set_fentry(fatd->temp_c, EOC_END, fatd);
                                                }
                                        }
                                        
                                        if (dentry_copy->src->file_size)
                                                fat32_copy_file_seq(fatd, fats);

                                        if ((dentry_copy->src->attr & ATTR_DIRECTORY) && dentry_copy->src->name[0] != 0x2e) {
                                        #if 0
                                                // clear dir
                                                memset(fatd->dbuf, 0, fatd->cbuf_sectcnt << 9);
                                                fat_write_volume(fatd->oper->sectof(fatd->temp_c, fatd), fatd->cbuf_sectcnt, fatd->dbuf, fatd, "clear dir"); 
                                        #endif
                                                dir_copy = malloc(sizeof(typeof(*dir_copy)));
                                                dir_copy->dst.pdir_c = fatd->dentry_c;
                                                dir_copy->dst.cdir_c = fatd->temp_c;
                                                dir_copy->src.pdir_c = fats->dentry_c;
                                                dir_copy->src.cdir_c = fats->temp_c;
                                                dir_copy->flag |= FAT_FLAG_CLR_DIR;
                                                printd_var(4, fatd->dentry_c, fatd->temp_c, fats->dentry_c, fats->temp_c);
                                                
                                                dir_copy->pdir_copy_head = fats->dir_copy_head;
                                                INIT_LIST_HEAD(&dir_copy->cdir_copy_head);
                                                list_add_tail(&dir_copy->list, fats->dir_copy_head);
                                                printd_var(4, dentry_copy, dentry_copy->dst->attr, dentry_copy->src->attr, fats->dir_copy_head);
                                        }
                                        list_del(&dentry_copy->list);
                                        safe_free(dentry_copy);
                                }
                                fatal_error(!list_empty(&dentry_copy_head));
                                fat32_check_dentry(fatd->dentry_buf, fatd->cbuf_size);
                                fat_write_volume(fatd->oper->sectof(fatd->dentry_c, fatd), fatd->cbuf_sectcnt, fatd->dentry_buf, fatd, "fatd->dentry_buf");
                        }

                        // if no dentry, update fat
                        if (fats->dentry_size == 0) {
                                //fat_write_volume(fatd->fsect, fatd->fbuf_sectcnt, fatd->fbuf, fatd, "fat");
                                return;
                        }
                }
        }
}

static void fat32_copy_volume(struct _fat_volume *fatd, struct _fat_volume *fats)
{
        struct _fat_dir_copy_list *dir_copy;

        if (fatd->volume_lable == NULL || fats->volume_lable == NULL) {
                fatal_error(1);
                return;
        }

        fat32_init(fats);
        fat32_init(fatd);
            
        // go to root dir
        fats->dentry_cnext = fats->rdir_c;
        fatd->dentry_cnext = fatd->rdir_c;
        fats->dir_copy_head = &fats->rdir_copy_head;
        printd("rhead: %x\n", fats->rdir_copy_head);

        // copy dentry & file
        fat32_copy_dentry_file(fatd, fats);

        while (1) {
                if (!list_empty(fats->dir_copy_head)) {
                        dir_copy = list_first_entry(fats->dir_copy_head, typeof(*dir_copy), list);
                         
                        // go to child dir
                        fats->dir_copy_head = &dir_copy->cdir_copy_head;
                        fatd->pdir_c = dir_copy->dst.pdir_c;
                        fatd->dentry_cnext = dir_copy->dst.cdir_c;
                        //fatd->pdirs_c = xxx
                        fats->dentry_cnext = dir_copy->src.cdir_c;
                        fatd->flag = dir_copy->flag;

                        printd_var(2, dir_copy, dir_copy->pdir_copy_head);
                        printd_var(3, fats->dir_copy_head, fats->dentry_cnext, fatd->dentry_cnext);

                        // copy child dir's dentry & file
                        fat32_copy_dentry_file(fatd, fats);
                } else if (fats->dir_copy_head != &fats->rdir_copy_head) {
                        dir_copy = list_entry(fats->dir_copy_head, typeof(*dir_copy), cdir_copy_head);

                        // go back to parent dir
                        fats->dir_copy_head = dir_copy->pdir_copy_head;
                        list_del(&dir_copy->list);
                        safe_free(dir_copy);
                } else {
                        // all finished
                        // if no dentry, update fat
                        //if (fats->dentry_size == 0) {
                                fat_write_volume(fatd->fsect, fatd->fbuf_sectcnt, fatd->fbuf, fatd, "fat");
                                //return;
                        //}
                        break;
                }
        }
}

static void fat32_build_free_map(const struct _fat_volume *fat)
{
        u32 i, entry_val;
        u32 *fbuf = (u32 *)fat->fbuf;

        printd("fat32_build_free_map\n");

        memset(fat->free_map, 0, fat->fbuf_sectcnt << 9);

        for (i = 0; i < fat->fbuf_sectcnt << 7; i++) {
                
                entry_val = fbuf[i] & FAT32_ENTRY_VAL_MASK;

                if (entry_val == FREE_CLUSTER || entry_val == EV_RESV1)
                        continue;
                
                fat->free_map[i >> 3] |= (1 << i);
        }
}

static void fat32_stat(struct _fat_volume *fat)
{
        u32 cluster, entry_idx, entry_val;

        if (fat->oper->fat_sectof(fat->cluster_cnt, fat) > fat->fat_sectbase + fat->fat_sectcnt)
                return;

        fat->fsect = 0;
        fat->fsect_curr = 0;

        for (cluster = 2; cluster < 3/*fat->cluster_cnt*/; cluster++) {

                fat->fsect_curr = fat->oper->fat_sectof(cluster, fat);

                /* read new a fat */
                if (fat->fsect != fat->fsect_curr)
                        fat_read_volume(fat->oper->fat_sectof(cluster, fat), fat->fbuf_sectcnt, fat->fbuf, fat, "fat_stat");

                /* read an entry value of cluster x on fat */
                entry_val = fat->oper->get_fentry(cluster, fat);

                __fat32_stat(cluster, entry_val, fat, &fat_stat_mgr);

                if (entry_val == BAD_CLUSTER || entry_val == FREE_CLUSTER)
                        continue;

                fat32_parse_dentry(cluster, fat);
        }
        printd("\n");
}

void fat_read_fat_sect(const int disk_idx, const u32 cluster, const struct _fat_volume *fat)
{
        __fat_read_volume(fat->oper->fat_sectof(cluster, fat), fat->fbuf_sectcnt, fat->fbuf, fat);
}

void fat_read_cluster(const int disk_idx, const u32 cluster, const struct _fat_volume *fat)
{
        __fat_read_volume(fat->oper->sectof(cluster, fat), fat->cbuf_sectcnt, fat->dentry_buf, fat);
}

void fat_print_cluster(const u32 cluster, const struct _fat_volume *fat)
{
        printd("#PRINT CLUSTER, sectof(%#x): %#x, sectcnt: %#x\n", cluster, fat->oper->sectof(cluster, fat), fat->cbuf_sectcnt);
        print_sector_buffer(fat->oper->sectof(cluster, fat), 
                            fat->cbuf_sectcnt, 
                            fat->dentry_buf);
}

static struct _fat_oper fat_default_oper = 
{
        fatx_fat_sectbase,
        fatx_root_dir_sectbase,
        fatx_root_dir_sectcnt,
        fatx_data_sectbase,
        fatx_data_sectcnt,
        fatx_sectof,

        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0
};

static struct _fat_oper fat16_oper = 
{
        fatx_fat_sectbase,
        fatx_root_dir_sectbase,
        fatx_root_dir_sectcnt,
        fatx_data_sectbase,
        fatx_data_sectcnt,
        fatx_sectof,

        fat16_size,
        fat16_fat_sectof,
        fat16_fentry_offsetof,
        fat16_get_fentry,
        fat16_set_fentry,
        0,
        0,
        0,//fatx_fentry_idxof,
        0,
        0
};

static struct _fat_oper fat32_oper = 
{
        fatx_fat_sectbase,
        fatx_root_dir_sectbase,
        fatx_root_dir_sectcnt,
        fatx_data_sectbase,
        fatx_data_sectcnt,
        
        fatx_sectof,
        fat32_size,
        fat32_fat_sectof,
        fat32_fentry_offsetof,
        fat32_get_fentry,

        fat32_set_fentry,
        fat32_stat,
        fat32_parse_dentry,
        0,//fatx_fentry_idxof,
        fat32_build_free_map,
        fat32_copy_volume
};

void test_fat(void)
{
        printd_var(1, sizeof(struct _bs_common));
        printd_var(1, sizeof(struct _bs_fat16));
        printd_var(1, sizeof(struct _bs_fat32));
        printd_var(1, sizeof(struct _bs));
}

void init_fat_volume(struct _fat_volume **fat_volume)
{
        (*fat_volume) = malloc(sizeof(struct _fat_volume));
        memset((*fat_volume), 0, sizeof(struct _fat_volume));
        (*fat_volume)->oper = &fat_default_oper;
}

void free_fat_volume(struct _fat_volume *fat_volume)
{
        if (fat_volume != NULL) {
                safe_free(fat_volume->dentry_buf);
                safe_free(fat_volume->cbuf);
                safe_free(fat_volume->fbuf);
                safe_free(fat_volume->free_map);
                safe_free(fat_volume->volume_lable);
                safe_free(fat_volume);
        }
}

void fat32_mount(const struct _disk_access *accs)
{
        int i, j;
        const struct _bs *bs = ((struct _bs *)accs->buf);
        unsigned char *temp = (unsigned char *)bs;

        /* Print header */
        print_ch(61, '-');
        printd("\nFAT32 BOOT SECTOR:\n");

        print_ch(12, ' ');
        for (i = 0; i < 16; i++) {
                printd("%02x ", i);
                print_cond(i == 7, "  ");
        }
        printd("\n");

        for (j = 0; j < 8; j++) {
                for (i = 0; i < 16; i++) {
                        /* Print leading address */
                        print_cond (i == 0, "0x%08x: ", (accs->lba << 9) + j * 16);

                        /* Print raw data */
                        printd("%02x ", temp[j * 16 + i]);
                        print_cond(i == 7, "  ");
                }
                printd("\n");
        }
        printd("\n");

        printd("jmp_boot:\n>> ");
        for (i = 0; i < 3; i++) {
                printd("%02x", bs->jmp_boot[i]);
        }
        printd("\n");

        printd("oem_name:\n>> ");
        for (i = 0; i < 8; i++) {
                printd("%02x", bs->oem_name[i]);
                print_cond(i == 7, "  ");
        }
        printd("\n");

        if (bs->oem_name[0] != 0) {
                printd("\n>> ");

                for (i = 0; i < 8; i++) {
                        break_cond(bs->oem_name[i] == 0);
                        printd("%c", bs->oem_name[i]);
                }
                printd("\n");
        }
        print_ch(34, '-');
        printd("\n");

        /* struct _bs_common */
        printd_var(1, bs->bytes_per_sect);
        printd_var(1, bs->sects_per_cluster);
        printd_var(1, bs->resv_sectcnt);
        
        printd_var(1, bs->fat_num);
        printd_var(1, bs->root_entry_cnt);
        printd_var(1, bs->total_sectcnt16);
        
        printd_var(1, bs->media);
        printd_var(1, bs->fat16_size);
        printd_var(1, bs->sects_per_track);
        
        printd_var(1, bs->head_num);
        printd_var(1, bs->hidden_sect_num);
        printd_var(1, bs->total_sectcnt32);
        
        printd_var(1, bs->f32.fat32_size);
        printd_var(1, bs->f32.ext_flag);
        printd_var(1, bs->f32.fs_ver);

        printd_var(1, bs->f32.root_cluster);
        printd_var(1, bs->f32.fs_info);
        printd_var(1, bs->f32.backup_boot_sector);

        printd_var(1, bs->f32.drive_num);
        printd_var(1, bs->f32.boot_sig);
        printd_var(1, bs->f32.volume_id);

        printd("bs->f32.volume_lable:\n>> ");
        for (i = 0; i < 11; i++) {
                printd("%02x", bs->f32.volume_lable[i]);
                print_cond(i == 7, "  ");
        }
        printd("\n");

        if (bs->f32.volume_lable[0] != 0) {
                printd("\n>> ");

                for (i = 0; i < 8; i++) {
                        break_cond(bs->f32.volume_lable[i] == 0);
                        printd("%c", bs->f32.volume_lable[i]);
                }
                printd("\n");
        }
        print_ch(34, '-');
        printd("\n");

        printd("bs->f32.file_sys_type:\n>> ");
        for (i = 0; i < 8; i++) {
                printd("%02x", ((u8 *)&bs->f32.file_sys_type)[i]);
                print_cond(i == 7, "  ");
        }
        printd("\n");

        if (((u8 *)&bs->f32.file_sys_type)[0] != 0) {
                printd("\n>> ");

                for (i = 0; i < 8; i++) {
                        break_cond(((u8 *)&bs->f32.file_sys_type)[i] == 0);
                        printd("%c", ((u8 *)&bs->f32.file_sys_type)[i]);
                }
                printd("\n");
        }
        print_ch(34, '-');
        printd("\n");

        printd("bs->f32.sig: %#x\n",                bs->f32.sig);

        if (bs->f32.sig != 0xAA55) {
                __fatal_error(1);
                return;
        }

        struct _fat_volume *fat = NULL;

        init_fat_volume(udaccs.disk_idx == DISK_IDX ? &fat_volume_src : &fat_volume_dst);
        fat = (udaccs.disk_idx == DISK_IDX ? fat_volume_src : fat_volume_dst); 

        fat->disk_idx = udaccs.disk_idx;
        if (fat->volume_lable != NULL)
                safe_free(fat->volume_lable);

        if (udaccs.volume_lable == NULL) {
                __fatal_error(1);
                return;
        }                

        u32 len = strlen_of(udaccs.volume_lable) + 1;
        fat->volume_lable = malloc(len);
        memcpy(fat->volume_lable, udaccs.volume_lable, len);

        fat->sect_size = bs->bytes_per_sect;
        fat->sect_size_mask = fat->sect_size - 1;
        fat->sect_size_shift = 9;       //**
        fat->cluster_sectcnt = bs->sects_per_cluster;

        fat->cluster_size = fat->cluster_sectcnt << fat->sect_size_shift;
        fat->cluster_size_mask = fat->cluster_size - 1;

        fat->cbuf_size = fat->cluster_size;
        fat->cbuf_size_mask = fat->cbuf_size - 1;
        fat->cbuf = malloc(fat->cbuf_size);
        fat->cbuf_sectcnt = fat->cluster_sectcnt;

        //fat->dir_entry_buf_size = fat->cluster_size;
        //fat->dir_entry_buf_mask - fat->dir_entry_buf_size - 1;
        fat->dentry_buf = malloc(fat->cbuf_size);
        //fat->dir_entry_buf_sectcnt = fat->cluster_sectcnt;
        
        fat->fbuf_size = FAT_BUF_SIZE;
        fat->fbuf_size_mask = FAT_BUF_SIZE_MASK;
        fat->fbuf = malloc(FAT_BUF_SIZE);
        fat->fbuf_sectcnt = FAT_BUF_SECTCNT;

        fat->dbuf_size = fat->cluster_sectcnt << fat->sect_size_shift << 13;
        fat->dbuf_size_mask = fat->dbuf_size - 1;
        fat->dbuf_sectcnt = fat->cluster_sectcnt << 13;
        //if (udaccs.disk_idx == DISK_IDX)
                fat->dbuf = malloc(fat->dbuf_size);

        fat->free_map = 0/*malloc(FAT_FMAP_SIZE)*/;
        
        fat->resv_sectcnt = bs->resv_sectcnt;
        fat->root_entry_cnt = bs->root_entry_cnt;
        fat->fat_num = bs->fat_num;
        fat->fat_sectcnt = (bs->fat16_size ? bs->fat16_size : bs->f32.fat32_size);
        fat->total_sectcnt = (bs->total_sectcnt16 ? bs->total_sectcnt16 : bs->total_sectcnt32);
        fat->volume_sectbase = fat->disk_idx == DISK_IDX ? 0x800 : 0x800;        //**
        fat->fat_sectbase = fatx_fat_sectbase(fat);
        fat->root_dir_sectbase = fat->oper->root_dir_sectbase(fat);
        fat->root_dir_sectcnt = fat->oper->root_dir_sectcnt(fat);
        fat->data_sectbase = fat->oper->data_sectbase(fat);
        fat->data_sectcnt = fat->oper->data_sectcnt(fat);

        fat->cluster_cnt = fat->data_sectcnt / fat->cluster_sectcnt;

        if (fat->cluster_cnt < 4058) {
                fat->fat_type = FAT_TYPE_FAT12;
                fat->oper = 0;
                //fat->fentry_size_shift = 0;
                fat->fentry_mask = 0xFFF;
                fat->rdir_c = 2;
        } else if (fat->cluster_cnt < 65525) {
                fat->fat_type = FAT_TYPE_FAT16;
                fat->oper = &fat16_oper;
                //fat->fentry_size_shift = 1;
                fat->fentry_mask = 0xFFFF;
                fat->rdir_c = 2;
        } else {
                fat->fat_type = FAT_TYPE_FAT32;
                fat->oper = &fat32_oper;
                fat->dsz = 32;
                //fat->fentry_size_shift = 2;
                fat->fentry_mask = 0xFFFFFFF;
                fat->rdir_c = bs->f32.root_cluster;
        }

        printd("\n");
        printd("* fat_volume_lable: %s\n",              fat->volume_lable);
        printd("* fat_type: %d\n",                      fat->fat_type);
        printd("* cluster_size: (%#x, %d KiB)\n",       fat->sect_size * fat->cluster_sectcnt, 
                                                        fat->sect_size * fat->cluster_sectcnt >> 10);
        printd("* root_dir_sectcnt: %#x\n",             fat->root_dir_sectcnt);
        printd("* first_fat: %#x\n",                    fat->resv_sectcnt + accs->lba);
        printd("* volume_sectbase: %#x\n",              fat->volume_sectbase);
        printd("* data_sectbase: (%#x, %d KiB)\n",      fat->data_sectbase, fat->data_sectbase >> 1);
        printd("* data_sectcnt: %#x\n",                 fat->data_sectcnt);
        printd("* cluster_cnt: %#x\n",                  fat->cluster_cnt);
        printd("sizeof(struct _bs_fat32): %#x\n",       sizeof(struct _bs_fat32));
        printd("\n");
}

void fat32_print_fs_info_detail(const struct _disk_access *accs)
{
        int i, j;
        const struct _fs_info *fsi = ((struct _fs_info *)accs->buf);
        unsigned char *temp = (unsigned char *)fsi;

        /* Print header */
        print_ch(61, '-');
        printd("\nFAT32 FS INFO:\n");

        print_ch(12, ' ');
        for (i = 0; i < 16; i++) {
                printd("%02x ", i);
                print_cond(i == 7, "  ");
        }
        printd("\n");

        for (j = 0; j < 32; j++) {
                for (i = 0; i < 16; i++) {
                        /* Print leading address */
                        print_cond (i == 0, "0x%08x: ", (accs->lba << 9) + j * 16);

                        /* Print raw data */
                        printd("%02x ", temp[j * 16 + i]);
                        print_cond(i == 7, "  ");
                }
                printd("\n");
        }
        printd("\n");

        printd("lead_sig: %#x\n",          fsi->lead_sig);
        if (fsi->lead_sig != 0x41615252)
                system("pause");

        printd("struct_sig: %#x\n",        fsi->struct_sig);
        if (fsi->struct_sig != 0x61417272)
                system("pause");

        printd("free_cluster_cnt: %#x\n",  fsi->free_cluster_cnt);
        printd("next_free_cluster: %#x\n", fsi->next_free_cluster);
        printd("trail_sig: %#x\n",         fsi->trail_sig);
        if (fsi->trail_sig != 0xaa550000)
                system("pause");
}

void fat32_find_fs_info(void)
{
        /* TODO */
}

void fat_mount(struct _fat_volume *fat)
{
        /* TODO */
}

void __fat_write_volume(const u32 lba, const u32 sectcnt, void *buf, const struct _fat_volume *fat)
{
        s64 lba_bytecnt;
        char *fullpath;
        HANDLE handle;
        DWORD sect_bytecnt, bytewritten;
        long sectlow, secthigh;

        sect_bytecnt = sectcnt * 512;

        __fatal_error(strcmp("C", fat->volume_lable) == 0);

        fullpath = (char *)malloc(strlen_of(volume_lable_prefix) + 3);
        sprintf(fullpath, "%s%s%s", volume_lable_prefix, fat->volume_lable, ":");
        printd("#Write ::%s, LBA: %#x(%#x), SECTCNT: %#x\n", fullpath, lba, lba - fat->volume_sectbase, sectcnt);
        __fatal_error(lba < fat->volume_sectbase);

        handle = CreateFile(fullpath,
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);

        if (handle == INVALID_HANDLE_VALUE) {
                printd("#CreateFile failed\n");
                return;
        }

        if (!DeviceIoControl(handle, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &bytewritten, NULL)) {
                DWORD error = GetLastError();
                printd("FSCTL_DISMOUNT_VOLUME error: %x\n", error);
                goto exit;
        }

        if (!DeviceIoControl(handle, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &bytewritten, NULL)) {
                DWORD error = GetLastError();
                printd("FSCTL_LOCK_VOLUME error: %x\n", error);
                goto exit;
        }

        lba_bytecnt = (lba - fat->volume_sectbase) * 512;
        sectlow  = lba_bytecnt & 0xFFFFFFFF;
        secthigh = (lba_bytecnt >> 32) & 0xFFFFFFFF;
        SetLastError(0);
        SetFilePointer(handle, sectlow, &secthigh, FILE_BEGIN);

        if (!WriteFile(handle, buf, sect_bytecnt, &bytewritten, NULL)) {
                DWORD error = GetLastError();
                printd("#WriteFile failed: %x\n", error);
        }

        if (!DeviceIoControl(handle, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &bytewritten, NULL)) {
                DWORD error = GetLastError();
                printd("FSCTL_UNLOCK_VOLUME error: %x\n", error);
        }

exit:
        CloseHandle(handle);
        free(fullpath); 
}

void __fat_read_volume(const u32 lba, const u32 sectcnt, void *buf, const struct _fat_volume *fat)
{
        s64 lba_bytecnt;
        char *fullpath;
        HANDLE handle;
        DWORD sect_bytecnt, byteread;
        long sectlow, secthigh;

        sect_bytecnt = sectcnt * 512;

        fullpath = (char *)malloc(strlen_of(volume_lable_prefix) + 3);
        sprintf(fullpath, "%s%s%s", volume_lable_prefix, fat->volume_lable, ":");
        printd("#Read ::%s, LBA: %#x(%#x), SECTCNT: %#x\n", fullpath, lba, lba - fat->volume_sectbase, sectcnt);
        __fatal_error(lba < fat->volume_sectbase);

        handle = CreateFile(fullpath,
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);

        if (handle == INVALID_HANDLE_VALUE) {
                printd("#CreateFile failed\n");
                return;
        }

        lba_bytecnt = (lba - fat->volume_sectbase) * 512;
        sectlow  = lba_bytecnt & 0xFFFFFFFF;
        secthigh = (lba_bytecnt >> 32) & 0xFFFFFFFF;
        SetFilePointer(handle, sectlow, &secthigh, FILE_BEGIN);

        if (!ReadFile(handle, buf, sect_bytecnt, &byteread, NULL))
                printd("#ReadFile failed\n");

        CloseHandle(handle);
        free(fullpath);
}

void __fat_write_volume_debug(const u32 lba, const u32 sectcnt, void *buf, const struct _fat_volume *fat, const char *str)
{
        s64 lba_bytecnt;
        char *fullpath = 0, *dbg_header = 0;
        HANDLE handle;
        DWORD sect_bytecnt, bytewritten;
        long sectlow, secthigh;

        sect_bytecnt = sectcnt * 512;

        __fatal_error(strcmp("C", fat->volume_lable) == 0);

        fullpath = (char *)malloc(strlen_of(volume_lable_prefix) + 3);
        sprintf(fullpath, "%s%s%s", volume_lable_prefix, fat->volume_lable, ":");

        if (str) {
                dbg_header = (char *)malloc(strlen_of(str) + 4);
                sprintf(dbg_header, "[%s] ", str);
        }
        printd("%s#Write ::%s, LBA: %#x(%#x), SECTCNT: %#x\n", str ? dbg_header : "", fullpath, lba, lba - fat->volume_sectbase, sectcnt);     
        __fatal_error(lba < fat->volume_sectbase);

        handle = CreateFile(fullpath,
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);

        if (handle == INVALID_HANDLE_VALUE) {
                printd("#CreateFile failed\n");
                return;
        }

        if (!DeviceIoControl(handle, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &bytewritten, NULL)) {
                DWORD error = GetLastError();
                printd("FSCTL_DISMOUNT_VOLUME error: %x\n", error);
                goto exit;
        }

        if (!DeviceIoControl(handle, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &bytewritten, NULL)) {
                DWORD error = GetLastError();
                printd("FSCTL_LOCK_VOLUME error: %x\n", error);
                goto exit;
        }

        lba_bytecnt = (lba - fat->volume_sectbase) * 512;
        sectlow  = lba_bytecnt & 0xFFFFFFFF;
        secthigh = (lba_bytecnt >> 32) & 0xFFFFFFFF;
        SetLastError(0);
        SetFilePointer(handle, sectlow, &secthigh, FILE_BEGIN);

        if (!WriteFile(handle, buf, sect_bytecnt, &bytewritten, NULL)) {
                DWORD error = GetLastError();
                printd("#WriteFile failed: %x\n", error);
        }

        if (!DeviceIoControl(handle, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &bytewritten, NULL)) {
                DWORD error = GetLastError();
                printd("FSCTL_UNLOCK_VOLUME error: %x\n", error);
        }

exit:
        CloseHandle(handle);
        safe_free(fullpath); 
        safe_free(dbg_header); 
}

void __fat_read_volume_debug(const u32 lba, const u32 sectcnt, void *buf, const struct _fat_volume *fat, const char *str)
{
        s64 lba_bytecnt;
        char *fullpath = 0, *dbg_header = 0;
        HANDLE handle;
        DWORD sect_bytecnt, byteread;
        long sectlow, secthigh;

        sect_bytecnt = sectcnt * 512;

        fullpath = (char *)malloc(strlen_of(volume_lable_prefix) + 3);
        sprintf(fullpath, "%s%s%s", volume_lable_prefix, fat->volume_lable, ":");

        if (str) {
                dbg_header = (char *)malloc(strlen_of(str) + 4);
                sprintf(dbg_header, "[%s] ", str);
        }
        printd("%s#Read ::%s, LBA: %#x(%#x), SECTCNT: %#x\n", str ? dbg_header : "", fullpath, lba, lba - fat->volume_sectbase, sectcnt);
        __fatal_error(lba < fat->volume_sectbase);

        handle = CreateFile(fullpath,
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);

        if (handle == INVALID_HANDLE_VALUE) {
                printd("#CreateFile failed\n");
                return;
        }

        lba_bytecnt = (lba - fat->volume_sectbase) * 512;
        sectlow  = lba_bytecnt & 0xFFFFFFFF;
        secthigh = (lba_bytecnt >> 32) & 0xFFFFFFFF;
        SetFilePointer(handle, sectlow, &secthigh, FILE_BEGIN);

        if (!ReadFile(handle, buf, sect_bytecnt, &byteread, NULL))
                printd("#ReadFile failed\n");

        CloseHandle(handle);
        free(fullpath);
        safe_free(dbg_header); 
}

void fat_volume_lba_clear(void)
{
        memset(sector_buffer, 0, 512);
        __fat_write_volume(udaccs.lba, 1, sector_buffer, fat_volume_dst);
}


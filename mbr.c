#include "mbr.h"

/* mbr_disk_mgr */
struct _mbr_disk *mbr_disk_mgr = NULL;
struct _mbr_disk *mbr_disk_mgr_bak = NULL;

#ifdef MBR_DISK_DEBUG_MSG
#define printd(...) \
        printd(__VA_ARGS__)
#define print_ch(count, ch) \
        print_ch(count, ch)
#define print_cond(cond, ...) \
        __printd_cond(cond, __VA_ARGS__)
#else
#define printd(...)
#define print_ch(count, ch)
#define print_cond(cond, ...)
#endif

void test_readmbr(void)
{
        read_disk(udaccs.disk_idx, 0, 1, sector_buffer);  
}

void test_writembr(void)
{
        gdaccs.buf = sector_buffer;
        memset(gdaccs.buf, 0, sizeof(struct _mbr));

        ((struct _mbr *)gdaccs.buf)->par_entry[0].par_type = 0x06;
        ((struct _mbr *)gdaccs.buf)->par_entry[0].lba = 0x800;
        ((struct _mbr *)gdaccs.buf)->par_entry[0].sectcnt = 0x40000;
        ((struct _mbr *)gdaccs.buf)->sig = 0xAA55;
        
        write_disk(udaccs.disk_idx, 0, 1, sector_buffer);
}

void test_mbr(void)
{
        int i, j;

        struct _mbr mbr;
        unsigned char *temp = (unsigned char *)&mbr;

        for (i = 256; i < 512; i++) {
                temp[i] = i;

                if (i >= 446 && i < 510)
                        printf("0x%02x ", temp[i]);
        }

        printf("\n");
        printf("sizeof(struct _mbr): %d\n",                     sizeof(struct _mbr));
        printf("sizeof(mbr.partition_table.entry0): %d\n",      sizeof(mbr.par_entry[0]));

        printf("mbr.partition_entry0: 0x%x\n",  mbr.par_entry[0].sectcnt);
        printf("mbr.partition_entry1: 0x%x\n",  mbr.par_entry[1].sectcnt);
        printf("mbr.partition_entry2: 0x%x\n",  mbr.par_entry[2].sectcnt);
        printf("mbr.partition_entry3: 0x%x\n",  mbr.par_entry[3].sectcnt);

        struct _chs_t chs;
        printf("sizeof(struct _chs_t): %d\n",   sizeof(chs));
        printf("sizeof(cylinder): %d\n",        sizeof(chs.cylinder));
        printf("sizeof(head): %d\n",            sizeof(chs.head));
        printf("sizeof(sector): %d\n",          sizeof(chs.sector));

        chs.sector = 0xC1;
        printf("chs.sector: 0x%x\n",            chs.sector);
        printf("chs._sect: 0x%x\n",             chs._sect);
        printf("chs._cylinder_h: 0x%x\n",       chs._cylinder_h);
        printf("\n");
}

void mbr_print_raw(const struct _disk_access *accs)
{
        int i, j;
        
        /* Print BOOTSTRAP info */
        print_ch(61, '-');
        printf("\nBOOTSTRAP:\n");

        print_ch(12, ' ');
        for (i = 0; i < 16; i++) {
                printf("%02x ", i);
                print_cond(i == 7, "  ");
        }
        printf("\n");

        for (j = 0; j < 32; j++) {
                for (i = 0; i < 16; i++) {

                        if ((j * 16 + i) >= 446)
                                continue;

                        print_cond(i == 0, "0x%08x: ", (u32)(accs->lba * 512 + j * 16)); 
                        printf("%02x ", ((unsigned char *)accs->buf)[j * 16 + i]);
                        print_cond(i == 7, "  ");
                }
                if ((j * 16 + i) >= 446)
                        continue;

                printf("\n");
        }
        printf("\n");
        
        /* Print PARTITION */
        print_ch(61, '-');
        printf("\nPARTITION:\n");

        print_ch(12, ' ');
        for (i = 0; i < 16; i++) {
                printf("%02x ", i);
                print_cond(i == 7, "  ");
        }
        printf("\n");

        for (j = 0; j < 32; j++) {
                for (i = 0; i < 16; i++) {
                        
                        if ((j * 16 + i) < 432 || (j * 16 + i) >= 510)
                                continue;
                        
                        print_cond(i == 0, "0x%08x: ", (u32)(accs->lba + j * 16));
                        print_cond(((j * 16 + i) < 446), "   ");
                        print_cond(((j * 16 + i) >= 446), "%02x ", ((unsigned char *)accs->buf)[j * 16 + i]);
                        print_cond(i == 7, "  ");
                }
                if ((j * 16 + i) <= 446 || (j * 16 + i) >= 510)
                    continue;
                printf("\n");
        }
        printf("\n");

        /* Print SIGNATURE */
        print_ch(61, '-');
        printf("\nSIGNATURE:\n");

        print_ch(12, ' ');
        for (i = 0; i < 16; i++) {
                printf("%02x ", i);
                print_cond(i == 7, "  ");
        }
        printf("\n");


        for (j = 0; j < 32; j++) {
                for (i = 0; i < 16; i++) {
                        
                        if ((j * 16 + i) < 496)
                                continue;
                        
                        print_cond(i == 0, "0x%08x: ", (u32)(accs->lba + j * 16));
                        print_cond(((j * 16 + i) < 510), "   ");
                        print_cond(((j * 16 + i) >= 510), "%02x ", ((unsigned char *)accs->buf)[j * 16 + i]);
                        print_cond(i == 7, "  ");
                }
                
                if ((j * 16 + i) <= 496)
                        continue;
                
                printf("\n");
        }
        printf("\n");
}

void mbr_par_print_par_entry_all(const struct _disk_access *accs)
{
        int i, j;
        unsigned char *temp;

        for (j = 0; j < 4; j++) {
                print_ch(61, '-');
                printf("\nPARITION_ENTRY %d:\n", j);

                print_ch(12, ' ');
                for (i = 0; i < 16; i++) {
                        printf("%02x ", i);
                        print_cond(i == 7, "  ");
                }
                printf("\n");

                temp = ((unsigned char *)((struct _mbr *)accs->buf)->par_entry) + sizeof(struct _mbr_par_entry) * j;
                for (i = 0; i < 16; i++) {
                        print_cond (i == 0, "0x%08x: ", 446 + j * 16);
                        printf("%02x ", temp[i]);
                        print_cond(i == 7, "  ");
                }
                printf("\n\n");
        }
}

void mbr_par_print_par_entry_detail(const struct _disk_access *accs)
{
        int i;
        unsigned char par_entry_idx = accs->par_entry_idx;
        unsigned char *temp = ((unsigned char *)((struct _mbr *)accs->buf)->par_entry) + sizeof(struct _mbr_par_entry) * par_entry_idx;
        const struct _mbr_par_entry *par_entry = ((struct _mbr *)accs->buf)->par_entry + par_entry_idx;

        if (par_entry_idx >= 4)
                return;

        print_ch(61, '-');
        printf("\nPARITION_ENTRY %d:\n", par_entry_idx);

        /* Print header */
        print_ch(12, ' ');
        for (i = 0; i < 16; i++) {
                printf("%02x ", i);
                print_cond(i == 7, "  ");
        }
        printf("\n");

        for (i = 0; i < 16; i++) {
                /* Print leading address */
                print_cond(i == 0, "0x%08x: ", 446 + par_entry_idx * 16);

                /* Print data */
                printf("%02x ", temp[i]);
                print_cond(i == 7, "  ");
        }

        printf("\n");
        printf("disk_status: 0x%02x\n",         par_entry->disk_status);
        printf("chs_start: 0x%06x\n",           par_entry->chs_start);
        printf(".head: 0x%02x\n",               par_entry->chs_start.head);
        printf(".cylinder: 0x%02x\n",           par_entry->chs_start._cylinder_h << 8 | par_entry->chs_start.cylinder);
        printf(".sector: 0x%02x\n",             par_entry->chs_start._sect);
        printf("patition_type: 0x%02x\n",       par_entry->par_type);
        printf("chs_end: 0x%06x\n",             par_entry->chs_end);
        printf(".head: 0x%02x\n",               par_entry->chs_end.head);
        printf(".cylinder: 0x%02x\n",           par_entry->chs_end._cylinder_h << 8 | par_entry->chs_end.cylinder);
        printf(".sector: 0x%02x\n",             par_entry->chs_end._sect);
        printf("lba: 0x%08x\n",                 par_entry->lba);
        printf("sectcnt: 0x%08x\n\n",           par_entry->sectcnt);
}

void mbr_par_print_all_par(void)
{
        struct _mbr_par_list *par;
        char *prefix;
        u32 i = 0;

        if (mbr_disk_mgr == NULL)
                mbr_par_read();

        print_ch(112, '-');
        printf("\nPrimary partition (%d):\n", mbr_disk_mgr->ppar_count);
        list_for_each_entry(par, &mbr_disk_mgr->ppar_head, list) {

                if (par->_type == 0)
                        prefix = "(f)";
                else if (par->par_type == 0x05 || par->par_type == 0x0f)
                        prefix = "(e)";
                else if (par->_type == LIST_MBR)
                        prefix = "(m)";
                else
                        prefix = "   ";

                printf("%s par: %2d, type: 0x%02x, base: 0x%08x, lba: 0x%08x, sectcnt: 0x%08x, end: 0x%08x (%9.2lf GiB)\n", prefix, i++, par->par_type, par->base, par->lba, par->sectcnt, par->lba + par->sectcnt, (float)par->sectcnt / 2 / 1024 / 1024);
        }

        print_ch(112, '-');
        printf("\nLogical Partition:\n");
        list_for_each_entry(par, &mbr_disk_mgr->lpar_head, list) {

                if (par->_type == 0)
                        prefix = "(f)";
                else if (par->par_type == 0x05 || par->par_type == 0x0f)
                        prefix = "(e)";
                else if (par->_type == LIST_MBR)
                        prefix = "(m)";
                else
                        prefix = "   ";

                printf("%s par: %2d, type: 0x%02x, base: 0x%08x, lba: 0x%08x, sectcnt: 0x%08x, end: 0x%08x (%9.2lf GiB)\n", prefix, i++, par->par_type, par->base, (par->base & MBR_PAR_LBA_BASE_MASK) + par->lba, par->sectcnt, (par->base & MBR_PAR_LBA_BASE_MASK) + par->lba + par->sectcnt, (float)par->sectcnt / 2 / 1024 / 1024);
        }

        printf("\n(m)br/ebr, (e)xtended partition, (f)ree partition\n");
        print_ch(112, '-');
        printf("\n\n");
}

void mbr_par_print_par(void)
{
        struct _mbr_par_list *par;
        char *prefix;
        u32 i = 0;

        print_ch(65, '-');
        printf("\nPrimary partition:\n");
        list_for_each_entry(par, &mbr_disk_mgr->ppar_head, list) {
                if (!par->_type)
                        continue;

                if (!par->_type)
                        prefix = "(f)";
                else if (par->par_type == 0x05 || par->par_type == 0x0f)
                        prefix = "(e)";
                else
                        prefix = "   ";

                printf("%s ppar: %2d, lba: 0x%08x, sectcnt: 0x%08x (%9.2lf GiB)\n", prefix, i++, par->lba, par->sectcnt, (float)par->sectcnt / 2 / 1024 / 1024);
        }

        printf("\n(e)xtended partition, (f)ree space, (l)ocigal disk\n");
        print_ch(65, '-');
        printf("\n\n");
}

void mbr_par_print_free_par(void)
{
        struct _mbr_par_list *par;
        char *prefix;
        u32 i = 0;

        print_ch(65, '-');
        printf("\nPrimary partition:\n");
        list_for_each_entry(par, &mbr_disk_mgr->ppar_head, list) {
                if (par->_type)
                        continue;

                if (!par->_type)
                        prefix = "(f)";
                else if (par->par_type == 0x05 || par->par_type == 0x0f)
                        prefix = "(e)";
                else
                        prefix = "   ";

                printf("%s ppar: %2d, lba: 0x%08x, sectcnt: 0x%08x (%9.2lf GiB)\n", prefix, i++, par->lba, par->sectcnt, (float)par->sectcnt / 2 / 1024 / 1024);
        }

        printf("\n(e)xtended partition, (f)ree space, (l)ocigal disk\n");
        print_ch(65, '-');
        printf("\n\n");
}

void init_mbr_disk(struct _mbr_disk **mbr_disk)
{
        struct _mbr_par_list *par;

        free_mbr_disk(*mbr_disk);        

        /** 
         * allocate memory for mbr_disk_mgr 
         * and initialize ppar & ldisk list 
         */
        (*mbr_disk) = malloc(sizeof(struct _mbr_disk));   
        memset((*mbr_disk), 0, sizeof(struct _mbr_disk));
        
        INIT_LIST_HEAD(&((*mbr_disk)->ppar_head));
        INIT_LIST_HEAD(&((*mbr_disk)->lpar_head));

        /* add a used par(mbr) to ppar list*/
        par = malloc(sizeof(struct _mbr_par_list));
        par->base = LIST_MBR;
        par->lba = 0 | LIST_MBR;
        par->sectcnt = MBR_RESERVED_SIZE;
        list_add_tail(&par->list, &((*mbr_disk)->ppar_head));

        /* add a free par to ppar_list */
        par = malloc(sizeof(struct _mbr_par_list));
        par->base = LIST_FREE;
        par->lba = MBR_RESERVED_SIZE;
        par->sectcnt = DISK_SECTOR_COUNT - MBR_RESERVED_SIZE;
        list_add_tail(&par->list, &((*mbr_disk)->ppar_head));
}

void free_mbr_disk(struct _mbr_disk *mbr_disk)
{
        if (mbr_disk != NULL) {

                struct _mbr_par_list *i, *j;
                
                list_for_each_entry_safe(i, j, &mbr_disk->ppar_head, list) {
                        list_del(&i->list);
                        free(i);
                }

                __fatal_error(!list_empty(&mbr_disk->ppar_head));

                list_for_each_entry_safe(i, j, &mbr_disk->lpar_head, list) {
                        list_del(&i->list);
                        free(i);
                }

                __fatal_error(!list_empty(&mbr_disk->lpar_head));

                free(mbr_disk);
        }
}

/**
 * mbr_par_alloc - allocate a new partition by searching an unused partition.
 * @par_head:   the &struct list_head in which a free partition reside.
 * @req:        the &struct _mbr_par_list contains the information of the required partition size.
 */
bool mbr_par_alloc(struct _mbr_par_list *req,
                   struct list_head *head)
{
        struct _mbr_par_list *par;
        u32 start, fstart;
        u64 fend;
        u64 end; // for debug

        /* search a valid free par in par_list */
        list_for_each_entry(par, head, list) {

                if (par->_type)
                        continue;

                start = (req->base & MBR_PAR_LBA_BASE_MASK) + req->lba;
                fstart = par->lba;
                end = (u64)start + (u64)req->sectcnt;                                   // for debug
                fend = (u64)fstart + (u64)par->sectcnt;

                printf("#srch (%x) s: %#x, fs: %#x (%x) e: %#llx, fe: %#llx\n", start < fstart, start, fstart, end > fend, end, fend);    // for debug
                
                if (start >= fend)
                        continue;

        #ifdef MBR_DISK_DEBUG_MODE
                //u32 end = start + req->sectcnt;

                if (end > fend || start < fstart) {
                        //printf("#srch (%x) s: %#x, fs: %#x (%x) e: %#llx, fe: %#llx\n", start < fstart, start, fstart, end > fend, end, fend);
                        mbr_par_print_all_par();
                        __fatal_error(1);
                }
        #endif
                printf("\n");

                /* if start is not align with fstart 
                 * split free partition into two parts, 
                 * the one which fstart is not align with start will be inserted
                 * in front of the aligned one. 
                 */
                if (start > fstart) {
                        struct _mbr_par_list *new = malloc(sizeof(struct _mbr_par_list));
                        memset(new, 0, sizeof(struct _mbr_par_list));

                        new->lba = fstart;
                        new->sectcnt = start - fstart;
                        par->lba = start;
                        par->sectcnt = par->sectcnt - new->sectcnt;

                        list_add_tail(&new->list, &par->list);
                }

                /* update the information of an existent free-par */
                par->lba = start + req->sectcnt;
                par->sectcnt = par->sectcnt - req->sectcnt;

                /* insert a new used-par to the par_list. */
                list_add_tail(&req->list, &par->list);

                if (par->sectcnt == 0) {
                        list_del(&par->list);
                        free(par);
                }        
 
                return 0;
        }
        return 1;
}

struct _mbr_par_list *mbr_par_req(struct list_head *head, u32 sectcnt)
{
        struct _mbr_par_list *par;

        /* search a valid free-par in par_list */
        list_for_each_entry(par, head, list) {

        #ifdef MBR_PAR_REQ_DEBUG_CONFIG
                printf("#req(%d %d) sect: 0x%08x, _type: 0x%02x, lba: 0x%08x, sect: 0x%08x\n", par->_type > 0, sectcnt, par->sectcnt < sectcnt, par->_type, par->lba, par->sectcnt);
        #endif
                if (par->_type || par->sectcnt < sectcnt)
                        continue;

                return par;
        }

        return NULL;
}

void mbr_par_read(void)
{
        struct _mbr_par_list *par;
        struct _mbr_par_entry *entry;
        struct _mbr *mbr = (struct _mbr *)sector_buffer;
        u32 i, ebr_lba = 0;

        init_mbr_disk(&mbr_disk_mgr);

        /* phase 1, read lba 0 */
        read_disk(udaccs.disk_idx, 0, 1, mbr);

        /* scan 4 par entries at lba 0(MBR). */
        for (i = 0; i < 4; i++) {

                entry = mbr->par_entry + i;

                /* if we find a valid partition. */
                if (!entry->sectcnt)
                        break;

                /* alloc a primary par to ppar_list */
                par = malloc(sizeof(struct _mbr_par_list));
                memcpy(par, entry, sizeof(struct _mbr_par_entry));
                par->base = LIST_PAR;
        #ifdef MBR_PAR_READ_DEBUG_CONFIG
                printf("[+ppar] type: 0x%02x, base: 0x%08x, lba: 0x%08x, sect: 0x%08x\n", par->par_type, par->base, par->lba, par->sectcnt);
        #endif
                __fatal_error(mbr_par_alloc(par, &mbr_disk_mgr->ppar_head));
                mbr_disk_mgr->ppar_count++;

                /* if we meet an extended par, add a free par to lpar_list for logical par */
                if (entry->par_type == 0x05 || entry->par_type == 0x0f) {
                    
                        struct _mbr_par_list *fpar = malloc(sizeof(struct _mbr_par_list));
                        memcpy(fpar, par, sizeof(struct _mbr_par_list));
                        fpar->base = LIST_FREE;
                        fpar->par_type = 0;
                        list_add(&fpar->list, &mbr_disk_mgr->lpar_head);

                        ebr_lba = entry->lba;
                        mbr_disk_mgr->epar_lba_start = ebr_lba;
                        mbr_disk_mgr->epar_lba_end = (u64)ebr_lba + (u64)entry->sectcnt;
                }
        }
        
        if (!ebr_lba)
                return;

        read_disk(udaccs.disk_idx, ebr_lba, 1, mbr);

        /* phase 2, scan all lpar if it is existent */
        for (i = 0; ; i = i ^ 1) {

                entry = mbr->par_entry + i;

                if (!entry->sectcnt)
                        return;

                if (entry->par_type != 0x05 && entry->par_type != 0x0f) {

                        /* alloc an ebr to lpar_list */
                        par = malloc(sizeof(struct _mbr_par_list));
                        par->base = ebr_lba | LIST_MBR;
                        par->lba = 0;
                        par->sectcnt = MBR_RESERVED_SIZE;
                        par->par_type = entry->par_type;
                #ifdef MBR_PAR_READ_DEBUG_CONFIG
                        printf("[+ ebr] type: 0x%02x, base: 0x%02x, lba: 0x%08x, sect: 0x%08x\n", par->par_type, par->base, par->lba, par->sectcnt);
                #endif
                        __fatal_error(mbr_par_alloc(par, &mbr_disk_mgr->lpar_head));

                        /* alloc a lpar to lpar_list */
                        par = malloc(sizeof(struct _mbr_par_list));
                        par->base = ebr_lba | LIST_PAR;
                        par->lba = entry->lba;
                        par->sectcnt = entry->sectcnt;
                        par->par_type = entry->par_type;
                #ifdef MBR_PAR_READ_DEBUG_CONFIG
                        printf("[+lpar] type: 0x%02x, base: 0x%02x, lba: 0x%08x, sect: 0x%08x\n", par->par_type, par->base, par->lba, par->sectcnt);
                #endif
                        __fatal_error(mbr_par_alloc(par, &mbr_disk_mgr->lpar_head));

                } else {
                        printf("ebr_lba: 0x%x epar_start: 0x%x entry_lba: 0x%x next_ebr: 0x%x\n", ebr_lba, mbr_disk_mgr->epar_lba_start, entry->lba, mbr_disk_mgr->epar_lba_start + entry->lba);
                        ebr_lba = mbr_disk_mgr->epar_lba_start + entry->lba;
                        read_disk(udaccs.disk_idx, ebr_lba, 1, mbr);
                }
        }
}

void mbr_par_add_ext(void)
{
        struct _mbr_par_list *fpar, *par;
        u32 lba, sectcnt;
        bool empty, ebr_exist = false;

        if (udaccs.sectcnt < MBR_RESERVED_SIZE) {
                __fatal_error(1);
                return;
        }

        empty = list_empty(&mbr_disk_mgr->lpar_head);

        /* search the list to see whether we have a valid free par for logical par */
        fpar = mbr_par_req(!empty ? &mbr_disk_mgr->lpar_head : &mbr_disk_mgr->ppar_head,
                           udaccs.sectcnt + MBR_RESERVED_SIZE);

        if (fpar == NULL) {
                __fatal_error(1);
                return;
        }

        lba = fpar->lba;
        sectcnt = fpar->sectcnt;

        /* if we do not have a free par in lpar_list yet */
        if (empty) {

                /* modified current par as an extented par  */
                fpar->base = LIST_PAR;
                fpar->par_type = 0x0f;
                mbr_disk_mgr->ppar_count++;

                /* add a free par to lpar_list */
                struct _mbr_par_list *temp = malloc(sizeof(struct _mbr_par_list));
                temp->base = LIST_FREE;
                temp->lba = lba;
                temp->sectcnt = sectcnt;
                list_add_tail(&temp->list, &mbr_disk_mgr->lpar_head);

                mbr_disk_mgr->epar_lba_start = lba;
                mbr_disk_mgr->epar_lba_end = lba + sectcnt;
        }
        
#ifdef MBR_PAR_ADD_EXT_DEBUG_CONFIG
        printf("#ebr_exist(%d %d)\n", fpar != list_first_entry(&mbr_disk_mgr->lpar_head, struct _mbr_par_list, list), list_prev_entry(fpar, list)->_type == LIST_MBR);
#endif
        if (fpar != list_first_entry(&mbr_disk_mgr->lpar_head, struct _mbr_par_list, list) && 
            list_prev_entry(fpar, list)->_type == LIST_MBR) {
                ebr_exist = true;
        }

#ifdef MBR_PAR_ADD_EXT_DEBUG_CONFIG
        printf("[=fpar] type: 0x%02x, base: 0x%02x, lba: 0x%08x, sect: 0x%08x\n", fpar->par_type, fpar->base, lba, sectcnt);
#endif
        /* alloc an ebr to lpar_list */
        par = malloc(sizeof(struct _mbr_par_list));
        par->base = (ebr_exist ? lba + udaccs.sectcnt : lba) | LIST_MBR;
        par->lba = 0;
        par->sectcnt = MBR_RESERVED_SIZE;
        par->par_type = 0x0c;
#ifdef MBR_PAR_ADD_EXT_DEBUG_CONFIG
        printf("[+ ebr] type: 0x%02x, base: 0x%02x, lba: 0x%08x, sect: 0x%08x\n", par->par_type, par->base, par->lba, par->sectcnt);
#endif
        __fatal_error(mbr_par_alloc(par, &mbr_disk_mgr->lpar_head));

        if (ebr_exist) {

                struct _mbr_par_list *par_next = par;

                list_for_each_entry_continue(par_next, &mbr_disk_mgr->lpar_head, list) {

                        if (par_next->_type != LIST_PAR)
                                continue;

                        if (par->base < par_next->base)
                                __fatal_error(1);

                        u32 diff = (par->_base - par_next->_base) << MBR_PAR_TYPE_SHIFT;
                        par_next->base = (par->base & MBR_PAR_LBA_BASE_MASK) | LIST_PAR;
                        par_next->lba = par_next->lba - diff;
                #ifdef MBR_PAR_ADD_EXT_DEBUG_CONFIG
                        printf("[=lpar] type: 0x%02x, base: 0x%02x, lba: 0x%08x, sect: 0x%08x\n", par_next->par_type, par_next->base, par_next->lba, par_next->sectcnt);
                #endif
                        break;
                }
        }

        /* alloc a par to xpar_list */
        par = malloc(sizeof(struct _mbr_par_list));
        par->base = lba - (ebr_exist ? MBR_RESERVED_SIZE : 0) | LIST_PAR;
        par->lba = MBR_RESERVED_SIZE;
        par->sectcnt = udaccs.sectcnt;
        par->par_type = 0x0c;
#ifdef MBR_PAR_ADD_EXT_DEBUG_CONFIG
        printf("[=fpar] type: 0x%02x, base: 0x%02x, lba: 0x%08x, sect: 0x%08x\n", fpar->par_type, fpar->base, lba, sectcnt);
        printf("[+lpar] type: 0x%02x, base: 0x%02x, lba: 0x%08x, sect: 0x%08x\n", par->par_type, par->base, par->lba, par->sectcnt);
#endif
        __fatal_error(mbr_par_alloc(par, &mbr_disk_mgr->lpar_head));
}

void mbr_par_add(void)
{
        struct _mbr_par_list *fpar, *par;

        if (udaccs.sectcnt < MBR_RESERVED_SIZE) {
                __fatal_error(1);
                return;
        }

        /* search the list to see whether we have a valid free par for primary par */
        fpar = mbr_par_req(&mbr_disk_mgr->ppar_head, udaccs.sectcnt);

        if (fpar == NULL) {
                __fatal_error(1);
                return;
        }

        /* alloc a primary par to ppar_list */
        par = malloc(sizeof(struct _mbr_par_list));
        par->base = LIST_PAR;
        par->lba = fpar->lba;
        par->sectcnt = udaccs.sectcnt;
        par->par_type = 0x0c;

        __fatal_error(mbr_par_alloc(par, &mbr_disk_mgr->ppar_head));
        mbr_disk_mgr->ppar_count++;

        /* clear the first lba of par to zero */
        memset(sector_buffer, 0, 512);
        write_disk(udaccs.disk_idx, par->lba, 1, sector_buffer);
}

void mbr_par_write(void)
{
        struct _mbr_par_list *par;
        struct _mbr *mbr = (struct _mbr *)sector_buffer;
        u32 i = 0;

        /* phase 1 */
        memset(mbr, 0, sizeof(struct _mbr));
        mbr->sig = 0xAA55;

        list_for_each_entry(par, &mbr_disk_mgr->ppar_head, list) {

                if (par->_type != LIST_PAR)
                        continue;

                mbr->par_entry[i].lba = par->lba;
                mbr->par_entry[i].sectcnt = par->sectcnt;
                mbr->par_entry[i].par_type = par->par_type;
                mbr->par_entry[i].chs_start.cylinder = 0xff;
                mbr->par_entry[i].chs_start.sector = 0xff;
                mbr->par_entry[i].chs_start.head = 0xfe;
                mbr->par_entry[i].chs_end.cylinder = 0xff;
                mbr->par_entry[i].chs_end.sector = 0xff;
                mbr->par_entry[i].chs_end.head = 0xfe;

                //printf("[wppa] type: 0x%02x, lba: 0x%08x, sect: 0x%08x\n", par->par_type, par->lba, par->sectcnt);

                if (i++ >= 4) {
                        mbr_par_print_all_par();
                        __fatal_error(1);
                }
        }

        write_disk(udaccs.disk_idx, 0, 1, mbr);

        if (list_empty(&mbr_disk_mgr->lpar_head) || list_is_singular(&mbr_disk_mgr->lpar_head)) {
                //__fatal_error(1);
                return;
        }

        /* phase 2 */
        struct _mbr_par_list *par_next;
        memset(mbr, 0, sizeof(struct _mbr));
        mbr->sig = 0xAA55;
        par = NULL;

        while (1) {

                par = list_prepare_entry(par, &mbr_disk_mgr->lpar_head, list);
                list_for_each_entry_continue(par, &mbr_disk_mgr->lpar_head, list) {

                        if (par->_type != LIST_PAR)
                                continue;

                        //printf("[=par] type: 0x%02x, bas: 0x%08x, lba: 0x%08x, sect: 0x%08x\n", par->par_type, par->base, par->lba, par->sectcnt);

                        mbr->par_entry[0].lba = par->lba;
                        mbr->par_entry[0].sectcnt = par->sectcnt;
                        mbr->par_entry[0].par_type = par->par_type;
                        /*
                        mbr->par_entry[0].chs_start.cylinder = 0xff;
                        mbr->par_entry[0].chs_start.sector = 0xff;
                        mbr->par_entry[0].chs_start.head = 0xfe;
                        mbr->par_entry[0].chs_end.cylinder = 0xff;
                        mbr->par_entry[0].chs_end.sector = 0xff;
                        mbr->par_entry[0].chs_end.head = 0xfe;
                        */
                        //printf("[wepa] type: 0x%02x, lba: 0x%08x, sect: 0x%08x\n", mbr->par_entry[0].par_type, mbr->par_entry[0].lba, mbr->par_entry[0].sectcnt);
                        break;
                }

                par_next = par;
                memset(&mbr->par_entry[1], 0, sizeof(struct _mbr_par_entry));

                list_for_each_entry_continue(par_next, &mbr_disk_mgr->lpar_head, list) {

                        if (par_next->_type != LIST_PAR)
                                continue;

                        printf("[=pnt] type: 0x%02x, bas: 0x%08x, lba: 0x%08x, sect: 0x%08x\n", par_next->par_type, par_next->base, par_next->lba, par_next->sectcnt);

                        if ((par_next->base & MBR_PAR_LBA_BASE_MASK) < mbr_disk_mgr->epar_lba_start)
                                __fatal_error(1);

                        mbr->par_entry[1].lba = (par_next->base & MBR_PAR_LBA_BASE_MASK) - mbr_disk_mgr->epar_lba_start;
                        mbr->par_entry[1].sectcnt = par_next->lba + par_next->sectcnt;
                        mbr->par_entry[1].par_type = 0x5;
                        /*
                        mbr->par_entry[1].chs_start.cylinder = 0xff;
                        mbr->par_entry[1].chs_start.sector = 0xff;
                        mbr->par_entry[1].chs_start.head = 0xfe;
                        mbr->par_entry[1].chs_end.cylinder = 0xff;
                        mbr->par_entry[1].chs_end.sector = 0xff;
                        mbr->par_entry[1].chs_end.head = 0xfe;
                        */
                        //printf("[wnxt] type: 0x%02x, lba: 0x%08x, sect: 0x%08x\n", mbr->par_entry[1].par_type, mbr->par_entry[1].lba, mbr->par_entry[1].sectcnt);
                        break;
                }
                //system("pause");
                //printf("\n");

                //printf("par->base: 0x%08x\n", (par->base & MBR_PAR_LBA_BASE_MASK));
                write_disk(udaccs.disk_idx, (par->base & MBR_PAR_LBA_BASE_MASK), 1, mbr);

                /*
                if (par == list_last_entry(&mbr_disk_mgr->lpar_head, struct _mbr_par_list, list) ||
                    list_next_entry(par, list) == list_last_entry(&mbr_disk_mgr->lpar_head, struct _mbr_par_list, list))
                        break;
                */
                //for debug
                if (par == list_last_entry(&mbr_disk_mgr->lpar_head, struct _mbr_par_list, list))
                        break;

                if (list_next_entry(par, list) == list_last_entry(&mbr_disk_mgr->lpar_head, struct _mbr_par_list, list)) {

                        if (list_next_entry(par, list)->_type != LIST_FREE)
                                __fatal_error(1);
                        
                        break;
                }
        }
}

void mbr_par_clear(void)
{
        memset(sector_buffer, 0, 512);
        write_disk(udaccs.disk_idx, 0, 1, sector_buffer);
}

void mbr_par_del(void)
{
        /* TODO */
}

void mbr_par_del_all(void)
{
        /* TODO */
}

void mbr_par_gc(void)
{
        /* TODO */
}

void mbr_par_cmpr(void)
{
        /* TODO */
}

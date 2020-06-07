#include "utility.h"

unsigned char *sector_buffer;
unsigned char *sector_buffer_back;
const char *disk_prefix = "\\\\.\\PhysicalDrive";
struct _disk_access udaccs = {.lba = 0, .disk_idx = 1, .volume_lable = NULL, .sectcnt = 1, .buf = NULL, .epar = 0, .par_entry_idx = 0};      /* user disk access interface */
struct _disk_access gdaccs = {.lba = 0, .disk_idx = 1, .volume_lable = NULL, .sectcnt = 1, .buf = NULL, .epar = 0, .par_entry_idx = 0};      /* global disk access interface */

void init_sect_buffer(void)
{
        sector_buffer = malloc(SECTOR_BUFFER_SIZE);
        sector_buffer_back = malloc(SECTOR_BUFFER_SIZE);
        udaccs.buf = sector_buffer;
        gdaccs.buf = sector_buffer;
}

void free_sect_buffer(void)
{
        safe_free(sector_buffer);
        safe_free(sector_buffer_back);
        udaccs.buf = NULL;
        gdaccs.buf = NULL;
}

void __safe_free(void **p)
{
        if (p == NULL || *p == NULL)
                return;
    
        free(*p);
        *p = NULL;

        return;
}

int strlen_of(const char *str)
{
        u32 i;

        for (i = 0; i < UINT_MAX; i++) {
                if (str[i] == '\0')
                        return(i);
        }
#ifdef UTILITY_DBG_CONFIG
        __fatal_error(1);
#endif
}

void print_ch(char ch, int count)
{
        int i;
        for (i = 0; i < count; i++)
                printf("%c", ch);
}
#if 0
void __printd_cond(int cond, char *format, ...)
{
        if (cond) {
                va_list args;
                va_start(args, format);
                printf(format, args);
                va_end(args);
        }
}
#endif
unsigned char is_str_to_hex_valid(const char *str) 
{
        u32 i, len;
        char ch;

        len = strlen_of(str);

        if (len > 16)
                return false;

        for (i = 0; i < len; i++) {
                ch = str[i];
                if ((ch >= '0' && ch <= '9') || 
                    (ch >= 'A' && ch <= 'F') ||
                    (ch >= 'a' && ch <= 'f'))
                        continue;

                return false;
        }

        return true;
}

u64 *str_to_hex(const char *str)
{
        static u64 val;
        u32 i, len;
        char ch;
        unsigned char temp;

        if (!is_str_to_hex_valid(str))
                return false;

        for (i = 0, len = strlen_of(str), val = 0; i < len; i++) {

                ch = str[i];

                if (ch >= '0' && ch <= '9')
                        temp = ch - 48;
                else if (ch >= 'A' && ch <= 'F')
                        temp = ch - 55;
                else if (ch >= 'a' && ch <= 'f')
                        temp = ch - 87;
                
                val = (val << 4) + temp;
        }

        return &val;
}

void write_disk(int disk_idx, u32 lba, u32 sectcnt, void *buf)
{
        s64 lba_bytecnt;
        char *fullpath;
        HANDLE handle;
        DWORD sect_bytecnt, bytewritten;
        long sectlow, secthigh;

        sect_bytecnt = sectcnt * 512;

        __fatal_error(disk_idx == 0);

        fullpath = (char *)malloc(strlen_of(disk_prefix) + 2);
        sprintf(fullpath, "%s%d", disk_prefix, disk_idx);
        printf("#Write ::%s, LBA: %#x, SECTCNT: %#x\n", fullpath, lba, sectcnt);

        handle = CreateFile(fullpath,
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL,
                             OPEN_EXISTING,
                             0,
                             NULL);

        if (handle == INVALID_HANDLE_VALUE) {
                printf("#CreateFile failed\n");
                return;
        }

        lba_bytecnt = lba * 512;
        sectlow  = lba_bytecnt & 0xFFFFFFFF;
        secthigh = (lba_bytecnt >> 32) & 0xFFFFFFFF;
        SetFilePointer(handle, sectlow, &secthigh, FILE_BEGIN);

        if (!WriteFile(handle, buf, sect_bytecnt, &bytewritten, NULL))
                printf("#WriteFile failed\n");

        CloseHandle(handle);
        free(fullpath);
}

void read_disk(int disk_idx, u32 lba, u32 sectcnt, void *buf)
{
        s64 lba_bytecnt;
        char *fullpath;
        HANDLE handle;
        DWORD sect_bytecnt, byteread;
        long sectlow, secthigh;

        sect_bytecnt = sectcnt * 512;
#if 0
        if (sect_bytecnt > SECTOR_BUFFER_SIZE) {
                sect_bytecnt = SECTOR_BUFFER_SIZE;
                sectcnt = sect_bytecnt >> 9;
        }
#endif

        fullpath = (char *)malloc(strlen_of(disk_prefix) + 2);
        sprintf(fullpath, "%s%d", disk_prefix, disk_idx);
        printf("#Read ::%s, LBA: %#x, SECTCNT: %#x\n", fullpath, lba, sectcnt);

        handle = CreateFile(fullpath,
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL,
                             OPEN_EXISTING,
                             0,
                             NULL);

        if (handle == INVALID_HANDLE_VALUE) {
                printf("#CreateFile failed\n");
                return;
        }

        lba_bytecnt = lba * 512;
        sectlow  = lba_bytecnt & 0xFFFFFFFF;
        secthigh = (lba_bytecnt >> 32) & 0xFFFFFFFF;
        SetFilePointer(handle, sectlow, &secthigh, FILE_BEGIN);

        if (!ReadFile(handle, buf, sect_bytecnt, &byteread, NULL))
                printf("#ReadFile failed\n");

        CloseHandle(handle);
        free(fullpath);
}

void lba_clear(void)
{
        memset(sector_buffer, 0, 512);
        write_disk(udaccs.disk_idx, udaccs.lba, 1, sector_buffer);
}

void lba_clear_range(void)
{
        u64 lba, lba_end = (u64)udaccs.lba + (u64)udaccs.sectcnt;
        u8 *buf;
        float p;

        buf = malloc(0x8000000);
        memset(buf, 0, 0x8000000);

        for (lba = udaccs.lba; lba < lba_end; lba = lba + 0x40000) {
                write_disk(udaccs.disk_idx, lba, 0x40000, buf);
                
                if ((udaccs.sectcnt > 0x40000 * 100) && ((lba - udaccs.lba) % (0x40000 * (int)((float)(lba_end - udaccs.lba) / 100 / 0x40000)) == 0))
                        printf("lba_clear_range: %0.3f %%\n", 100 * (float)(lba - udaccs.lba)/(float)(lba_end - udaccs.lba));
        }

        free(buf);        
}

void lba_clear_all(void)
{
        u64 lba, lba_end = (u64)udaccs.lba + DISK_SECTOR_COUNT;
        u8 *buf;

        buf = malloc(0x8000000);
        memset(buf, 0, 0x8000000);

        #define CONST_LBA 0x10

        for (lba = udaccs.lba; lba < DISK_SECTOR_COUNT; lba = lba + CONST_LBA) {
                write_disk(udaccs.disk_idx, lba, CONST_LBA, buf);

                if (((lba - udaccs.lba) % (CONST_LBA * (int)((float)(lba_end - udaccs.lba) / 100 / CONST_LBA)) == 0))
                        printf("lba_clear_range: %0.3f %%\n", 100 * (float)(lba - udaccs.lba)/(float)(lba_end - udaccs.lba));
        }

        free(buf);
}

void lba_write(void)
{
#if 1
        write_disk(udaccs.disk_idx, udaccs.lba, 1, sector_buffer);
#else
        //write_disk(udaccs.disk_idx, udaccs.lba, 1, sector_buffer);
        u8 *buf;

        #define BUF_SIZE (1 << 28)

        buf = malloc(BUF_SIZE);
        memset(buf, 0x5A, BUF_SIZE);
        write_disk(udaccs.disk_idx, udaccs.lba, BUF_SIZE >> 9, buf);

        free(buf);
#endif
}

void sb_copy(void)
{
        memcpy(sector_buffer_back, sector_buffer, 512);
}

void sb_paste(void)
{
        memcpy(sector_buffer, sector_buffer_back, 512);
}

void list_disk(void)
{
        system("wmic diskdrive list");
        return;
}

void print_sector_buffer(u32 lba, u32 sectcnt, u8 *buf)        
{
        int i, j, k;

        for (k = 0; k < sectcnt; k++) {
                print_ch('-', 61);
                printf("\nSECTOR BUFFER: [LBA 0x%08x]\n", lba + k);

                print_ch(' ', 12);
                for (i = 0; i < 16; i++) {
                        printf("%02x ", i);
                        __printd_cond(i == 7, "  ");
                }
                printf("\n");

                for (j = 0; j < 32; j++) {
                        for (i = 0; i < 16; i++) {
                                __printd_cond(i == 0, "0x%08x: ", (u32)((lba + k) * 512 + j * 16)); 
                                printf("%02x ", ((unsigned char *)buf)[k * 512 + j * 16 + i]);
                                __printd_cond(i == 7, "  ");
                        }
                        printf("\n");
                }
                printf("\n");
        }
}

#if 1
void __print_buf(void *buf, u32 size, char *title)
{
    u32 i, addr;
    u32 j, l, p;

    if (title)
        printf("%s\n",title);

    if (size == 0) {
        printf("[%s][%d] SIZE is zero.\n\n", __FUNCTION__, __LINE__);
        return;
    }

    printf("0x%08X: ", addr);

    for (i = 0; i < size; i++) {
        if (i) {
            if (i % 16 == 0) {
                printf("   ");
                for (j = i - 16; j < i; j++) {
                    if (((u8 *)buf)[j] < 0x20 || ((u8 *)buf)[j] > 0x7E)
                        printf(".");
                    else
                        printf("%c", ((u8 *)buf)[j]);
                }
                printf("\n0x%08X: ", addr + i);
                p = 1;
            } else if (i % 8 == 0)
                printf(" ");
        }
        printf("%02X ", ((u8 *)buf)[i]);
        p = 0;
    }

    if (!p) {
        if (i & 0xF) {
            for (j = i; j < i + 16 - (i & 0xF); j++) {
                if (j % 8 == 0)
                    printf(" ");
                    
                printf("   ");
            }
        }

        printf("   ");
        for (j = i - (i & 0xF); j < i; j++)
            printf(".");

        if (i & 0xF) {
            for (j = i; j < i + 16 - (i & 0xF); j++) {
                if (((u8 *)buf)[j] < 0x20 || ((u8 *)buf)[j] > 0x7E)
                    printf(".");
                else
                    printf("%c", ((u8 *)buf)[j]);
            }
        } else {
            for (j = i - 16; j < i; j++) {
                if (((u8 *)buf)[j] < 0x20 || ((u8 *)buf)[j] > 0x7E)
                    printf(".");
                else
                    printf("%c", ((u8 *)buf)[j]);
            }
        }
    }

    printf("\n\n");
}
#else
void __print_buf(void *buf, u32 size, char *title)
{
    u32 i, l, addr, offset;
    u32 j;

    if (title)
        printf("%s:\n",title);

    if (size == 0) {
        printd(WARN,"[%s][%d] SIZE is zero.\n\n", __FUNCTION__, __LINE__);
        return;
    }

    print_ch(' ', 12);

    for (i = 0; i < 16; i++) {
        if (i == 8)
            printf(" ");

        printf("%02X ", i);
    }

    offset = (u32)buf & 0xF;
    addr = (u32)buf - offset;
    printf("\n0x%08X: ", addr);

    for (i = 0; i < offset; i++)
        printf("   ");

    for (i = 0; i < size; i++) {
        l = i + offset;
        if (l) {
            if (l % 16 == 0) {
                printf("    ");
                if (offset && l == 16) {
                    for (j = 0; j < 16; j++) {
                        if (j < offset) {
                            printf(".");
                        } else {
                            if (((u8 *)buf)[j] < 0x20 || ((u8 *)buf)[j] > 0x7E)
                                printf(".");
                            else
                                printf("%c", ((u8 *)buf)[j]);
                        }
                    }

                } else {
                    for (j = i - 16; j < i; j++) {
                        if (((u8 *)buf)[j] < 0x20 || ((u8 *)buf)[j] > 0x7E)
                            printf(".");
                        else
                            printf("%c", ((u8 *)buf)[j]);
                    }
                }

                printf("\n0x%08X: ", addr + l);
            } else if (l % 8 == 0)
                printf(" ");
        }
        printf("%02X ", ((u8 *)buf)[i]);
    }

    printf("\n\n");
}
#endif
/*
 * memcmp - Compare two areas of memory
 * @cs: One area of memory
 * @ct: Another areas of memory
 * @count: The size of the area.
 */
int memcmp(const void *cs, const void *ct, size_t count)
{
        const unsigned char *su1, *su2;
        int res = 0;

        for (su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, --count)
                if ((res = *su1 - *su2) != 0)
                        break;
     
        return res;
}

/*
 *  strlen - Find the length of a string
 *  @s: The string to be sized
 */
size_t strlen(const char *s)
{
        const char *sc;
        for (sc = s; *sc != '\0'; ++sc)
                /* nothing */;
                
        return sc - s;
}

/* linux/lib/string.c
 * strchr - Find the first occurrence of a character in a string
 * @s: The string to be searched
 * @c: The character to search for
 */
char *strchr(const char *s, int c)
{
        for (; *s != (char)c; ++s) {
                if (*s == '\0')
                        return NULL;
        }
        
        return (char *)s;
}

/*
 * strstr - Find the first substring in a %NUL terminated string
 * @s1: The string to be searched
 * @s2: The string to search for
 */
char *strstr(const char *s1, const char *s2)
{
        size_t l1, l2;
        l2 = strlen(s2);
        if (!l2 == 0)
                return (char *)s1;
                
        l1 = strlen(s1);
        
        while (l1 > l2) {
                l1--;
                if (memcmp(s1, s2, l2))
                        return (char *)s1;
                        
                s1++;
        }
        
        return NULL;
}

/*
 * strcmp - Compare two strings
 * @cs: One string
 * @ct: Another string
 */
int strcmp(const char *cs, const char *ct)
{
        unsigned char c1, c2;
        
        while (1) {
                c1 = *cs++;
                c2 = *ct++;
                if (c1 != c2)
                        return c1 < c2 ? -1 : 1;

                if (!c1)
                        break;
        }
        
        return 0;
}

#ifndef _UTILITY_H
#define _UTILITY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <windows.h>
#include <winioctl.h>

#include "types.h"
#include "error.h"

//define debug_filter     [assert_enable] << 1 | [msg_enable] << 0
#define AST(n)            ((n) << 1)
#define MSG(n)            (n)
#define WARN            (AST(1) | MSG(1))
#define EVP                (AST(1) | MSG(1))


#define UTILITY_DBG_CONFIG

#define DISK_LBA_START                  0x00000000
//#define DISK_LBA_END                    0xEF1000
//#define DISK_LBA_END                    0x74706800
#define DISK_LBA_END                    0x100000ULL
#define DISK_SECTOR_COUNT               (DISK_LBA_END - DISK_LBA_START)
#define DISK_RESERVED_LBA_START         (DISK_LBA_END - DISK_RESERVED_SECTOR_COUNT)
#define DISK_RESERVED_SECTOR_COUNT      0x1000
#define SECTOR_SIZE                     (1 << 9)

#define SECTOR_BUFFER_SIZE_SHIFT        (16)
#define SECTOR_BUFFER_SIZE              (1 << SECTOR_BUFFER_SIZE_SHIFT)

#define PAU system("pause");
#define safe_free(p) __safe_free((void **)&(p))

#define break_cond(cond) \
{ \
        if (cond) \
                break; \
}
#if 0
#define __printd_cond(cond, ...) \
{ \
        if (cond) \
                printf(__VA_ARGS__); \
}
#else
static inline void __printd_cond(int cond, char *format, ...)
{
        if (cond) {
                va_list args;
                va_start(args, format);
                vprintf(format, args);
                va_end(args);
        }
}
#endif

#define to_str(s)       __to_str(s)
#define __to_str(s)     #s

#define string_format1  "%s: %x"
#define string_format2  "%s: %x, %s: %x"
#define string_format3  "%s: %x, %s: %x, %s: %x"
#define string_format4  "%s: %x, %s: %x, %s: %x, %s: %x"
#define string_format5  "%s: %x, %s: %x, %s: %x, %s: %x, %s: %x"
#define string_format6  "%s: %x, %s: %x, %s: %x, %s: %x, %s: %x, %s: %x"

#define var1(v, ...)    #v, v
#define var2(v, ...)    #v, v, var1(__VA_ARGS__)
#define var3(v, ...)    #v, v, var2(__VA_ARGS__)
#define var4(v, ...)    #v, v, var3(__VA_ARGS__)
#define var5(v, ...)    #v, v, var4(__VA_ARGS__)
#define var6(v, ...)    #v, v, var5(__VA_ARGS__)
#define var(n, ...)     var##n(__VA_ARGS__)

#define printd_var(n, ...) \
        printd(string_format##n "\n", var(n, __VA_ARGS__))

#define printd_var2(s, n, ...) \
        printd("%s" string_format##n "\n", s, var(n, __VA_ARGS__))

#define printd_var3(s, n, ...) \
        printd("%s" string_format##n "\n", s, __VA_ARGS__)

#define ASSERT(debug_filter, cond) ({ \
    if (((debug_filter) & AST(1)) && !(cond)){ \
        printf("[%s][%d] ASSERT(0)\n", __FUNCTION__, __LINE__); \
        while (1); \
    }  \
    (cond); \
})

#define LITTLE_ENDIAN           (0)
#define BIG_ENDIAN              (1)

struct _disk_access
{
        u32 lba;
        u32 sectcnt;
        int disk_idx;
        u8 *volume_lable;
        u32 epar;
        unsigned char par_entry_idx;
        u32 cluster;
        unsigned char *buf;
};

void __safe_free(void **p);
int strlen_of(const char *str);
void print_ch(char ch, int count);
u64 *str_to_hex(const char *str);
/*void print_cond(int cond, char *format, ...);*/

/* disk related operation */
void list_disk(void);
void print_sector_buffer(u32 lba, u32 sectcnt, u8 *buf);
void write_disk(int disk_idx, u32 lba, u32 sectcnt, void *buf);
void read_disk(int disk_idx, u32 lba, u32 sectcnt, void *buf);
void lba_clear(void);
void lba_clear_range(void);
void lba_clear_all(void);
void sb_copy(void);
void sb_paste(void);
void lba_write(void);
void init_sect_buffer(void);
void free_sect_buffer(void);

extern unsigned char *sector_buffer;
extern unsigned char *sector_buffer_back;
extern const char *disk_prefix;
extern struct _disk_access udaccs;
extern struct _disk_access gdaccs;
extern void __print_buf(void *buf, u32 size, char *title);

/* String */
int str_to_hex2(u32 *hex, const char *str, int len, int endianess);
bool str_match_str_list(const char *str, const char *temp[]);
void str_to_upper(char *str, int len);

#endif

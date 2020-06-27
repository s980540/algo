#ifndef __KSOC_ARRAY_GENERATION_H__
#define __KSOC_ARRAY_GENERATION_H__

typedef struct _AI_WEIGHT_SRAM_DESCRIPTOR {
        char *array_name_string;
        char *array_size_string;
        char *array_base_string;
        char *array_offset_string;
} AI_WEIGHT_SRAM_DESCRIPTOR;

typedef struct _COE_FILE_DESCRIPTOR {
        char *file_name_string;
} COE_FILE_DESCRIPTOR;

typedef struct _SRAM_FILE_DESCRIPTOR {
        char *file_name_string;
        char *location_string;
} SRAM_FILE_DESCRIPTOR;

#define COE_FILE_NUM            (8)
#define GROUP_PER_COE_FILE      (3)

unsigned int str_to_hex2(char *str, int len, int endianess);
int ksoc_coe_to_sram(void);
int ksoc_bin_to_array(const char *bin_name, const char *array_name);


#endif  /* ~ __KSOC_ARRAY_GENERATION_H__ */
#ifndef __FILE_KSOC_ARRAY_GENERATION_H__
#define __FILE_KSOC_ARRAY_GENERATION_H__

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

void ksoc_bin_to_c_array(void);
int ksoc_coe_to_sram(void);
void ksoc_io_script_parser(const char *file_name, const bool enable_debug_verbose);

#endif  /* ~ __FILE_KSOC_ARRAY_GENERATION_H__ */
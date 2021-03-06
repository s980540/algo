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
ret_code ksoc_coe_to_sram(void);
ret_code ksoc_io_script_parser(const char *in_file_name, const bool verbose);
ret_code ksoc_set_bin_name(const char *file_name, const char *version, const char *datetime);

#endif  /* ~ __FILE_KSOC_ARRAY_GENERATION_H__ */
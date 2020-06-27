#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "utility.h"
#include "file.h"
#include "ksoc_array_generation.h"

char *comment_string[] = {
        "memory_initialization_radix=16;",
        "memory_initialization_vector=",
        "y",
        NULL
};

COE_FILE_DESCRIPTOR coe_file_descriptor[] = {
        {"r_sram0.coe"},
        {"r_sram1.coe"},
        {"r_sram2.coe"},
        {"r_sram3.coe"},

        {"l_sram0.coe"},
        {"l_sram1.coe"},
        {"l_sram2.coe"},
        {"l_sram3.coe"},
        {NULL}
};

SRAM_FILE_DESCRIPTOR sram_file_descriptor[] = {
        {"r_sram0.c", "Wloc_R0"},
        {"r_sram1.c", "Wloc_R1"},
        {"r_sram2.c", "Wloc_R2"},
        {"r_sram3.c", "Wloc_R3"},

        {"l_sram0.c", "Wloc_L0"},
        {"l_sram1.c", "Wloc_L1"},
        {"l_sram2.c", "Wloc_L2"},
        {"l_sram3.c", "Wloc_L3"},
        {NULL}
};

AI_WEIGHT_SRAM_DESCRIPTOR ai_weight_sram_descriptor[COE_FILE_NUM * GROUP_PER_COE_FILE] = {
        {"K_AIWloc_R0_G0", "AIW_R_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_R0_G0_OFFSET"},
        {"K_AIWloc_R0_G1", "AIW_R_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_R0_G1_OFFSET"},
        {"K_AIWloc_R0_G2", "AIW_R_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_R0_G2_OFFSET"},

        {"K_AIWloc_R1_G0", "AIW_R_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_R1_G0_OFFSET"},
        {"K_AIWloc_R1_G1", "AIW_R_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_R1_G1_OFFSET"},
        {"K_AIWloc_R1_G2", "AIW_R_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_R1_G2_OFFSET"},

        {"K_AIWloc_R2_G0", "AIW_R_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_R2_G0_OFFSET"},
        {"K_AIWloc_R2_G1", "AIW_R_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_R2_G1_OFFSET"},
        {"K_AIWloc_R2_G2", "AIW_R_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_R2_G2_OFFSET"},

        {"K_AIWloc_R3_G0", "AIW_R_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_R3_G0_OFFSET"},
        {"K_AIWloc_R3_G1", "AIW_R_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_R3_G1_OFFSET"},
        {"K_AIWloc_R3_G2", "AIW_R_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_R3_G2_OFFSET"},

        {"K_AIWloc_L0_G0", "AIW_L_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_L0_G0_OFFSET"},
        {"K_AIWloc_L0_G1", "AIW_L_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_L0_G1_OFFSET"},
        {"K_AIWloc_L0_G2", "AIW_L_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_L0_G2_OFFSET"},

        {"K_AIWloc_L1_G0", "AIW_L_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_L1_G0_OFFSET"},
        {"K_AIWloc_L1_G1", "AIW_L_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_L1_G1_OFFSET"},
        {"K_AIWloc_L1_G2", "AIW_L_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_L1_G2_OFFSET"},

        {"K_AIWloc_L2_G0", "AIW_L_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_L2_G0_OFFSET"},
        {"K_AIWloc_L2_G1", "AIW_L_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_L2_G1_OFFSET"},
        {"K_AIWloc_L2_G2", "AIW_L_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_L2_G2_OFFSET"},

        {"K_AIWloc_L3_G0", "AIW_L_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_L3_G0_OFFSET"},
        {"K_AIWloc_L3_G1", "AIW_L_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_L3_G1_OFFSET"},
        {"K_AIWloc_L3_G2", "AIW_L_RAM_GROUP_ARR_SIZE", "AI_WEIGHT_FLASH_BASE", "AI_Wloc_L3_G2_OFFSET"},
};

int str_match_str_list(char *str, char *temp[])
{
        while (*temp != NULL) {
                if (strcmp(str, *temp) == 0) {
                        return TRUE;
                }
                temp++;
        }

        return FALSE;
}

#define STR_SIZE        (100)
char str[STR_SIZE];

long coe_get_line_num(ALGO_FILE *af)
{
        long i = 0, pos;
        char *loc;

        pos = ftell(af->fp);

        while (fgets(str, STR_SIZE, af->fp) != NULL) {
                loc = strchr(str, '\n');
                *loc = '\0';
                if (str_match_str_list(str, comment_string) == TRUE) {
                        continue;
                }
                i++;
        }
        fseek(af->fp, pos, SEEK_SET);

        return i;
}

int coe_to_sram(int coe_file_index, ALGO_FILE *coe_file, ALGO_FILE *sram_file)
{
        int l, g;
        long line_num;
        unsigned int len;
        char *loc;
        unsigned int *sram[GROUP_PER_COE_FILE];

        fseek(coe_file->fp, 0, SEEK_SET);
        fseek(sram_file->fp, 0, SEEK_SET);
        line_num = coe_get_line_num(coe_file);
        printf("coe: %s sram: %s line: %d\n", coe_file->file_name, sram_file->file_name, line_num);
        sram[0] = (unsigned int *)malloc(sizeof(unsigned int) * line_num);
        sram[1] = (unsigned int *)malloc(sizeof(unsigned int) * line_num);
        sram[2] = (unsigned int *)malloc(sizeof(unsigned int) * line_num);

        l = 0;
        while (fgets(str, STR_SIZE, coe_file->fp) != NULL) {
                loc = strchr(str, '\n');
                *loc = '\0';

                if (str_match_str_list(str, comment_string) == TRUE) {
                        continue;
                }

                loc = strchr(str, ',');
                *loc = '\0';
                len = loc - str;
                //printf("i: %08X str: %s len: %d\n", i * 4, str, len);
                sram[0][l] = str_to_hex2(str + 16, 8, BIG_ENDIAN);
                sram[1][l] = str_to_hex2(str + 8, 8, BIG_ENDIAN);
                sram[2][l] = str_to_hex2(str, 8, BIG_ENDIAN);
                l++;
        }

        fprintf(sram_file->fp, "// %s\n", coe_file->file_name);
        fprintf(sram_file->fp, "// Ai Weight location : %s\n", sram_file_descriptor[coe_file_index].location_string);
        fprintf(sram_file->fp, "#include \"ai_weight_def.h\"\n\n");
        fprintf(sram_file->fp, "#if ENABLE_AI_WEIGHT_INCLUDE\n\n\n");
        for (g = 0; g < GROUP_PER_COE_FILE; g++) {
                fprintf(sram_file->fp, "// Group : G%d\n", g);
                fprintf(sram_file->fp, "const uint32_t %s[%s]  __attribute__((at(%s + %s))) = \n{\n",
                        ai_weight_sram_descriptor[coe_file_index * 3 + g].array_name_string,
                        ai_weight_sram_descriptor[coe_file_index * 3 + g].array_size_string,
                        ai_weight_sram_descriptor[coe_file_index * 3 + g].array_base_string,
                        ai_weight_sram_descriptor[coe_file_index * 3 + g].array_offset_string
                        );

                l = 0;
                while (l < line_num) {
                        printf("\rWrite SRAM %s: %7.3f %%", sram_file->file_name, (((float)l * g) / ((float)line_num * GROUP_PER_COE_FILE)) * 100);
                        fflush(stdout);
                        fprintf(sram_file->fp, "    0x%08X,    /* offset= 0x%08X */\n",
                                sram[g][l], l << 2);
                        l++;
                }
                fprintf(sram_file->fp, "                   /* End = 0x%08X */\n};\n\n", l << 2);
        }
        printf("\rWrite SRAM %s : %7.3f %%\n", sram_file->file_name, (((float)l * g) / ((float)line_num * GROUP_PER_COE_FILE)) * 100);
        fflush(stdout);
        fprintf(sram_file->fp, "#endif\n");

        return 0;
}

int ksoc_coe_to_sram(void)
{
        int i, ret = -1, ferr;
        ALGO_FILE coe_file = {.fp = NULL, .file_name = NULL};
        ALGO_FILE sram_file = {.fp = NULL, .file_name = NULL};

        i = 0;
        while (coe_file_descriptor[i].file_name_string != NULL) {
                if (file_open(&coe_file, coe_file_descriptor[i].file_name_string, "r")) {
                        goto exit;
                }

                if (file_open(&sram_file, sram_file_descriptor[i].file_name_string, "w")) {
                        goto exit;
                }

                if (coe_to_sram(i, &coe_file, &sram_file)) {
                        goto exit;
                }

                if (feof(coe_file.fp)) {
                        printf("feof\n");
                }

                ferr = ferror(coe_file.fp);
                if (ferr) {
                        printf("ferror: %d\n", ferr);
                        goto exit;
                }

                if (file_close(&coe_file)) {
                        return ret;
                }

                if (file_close(&sram_file)) {
                        return ret;
                }

                i++;
        }

        ret = 0;
exit:
        if (coe_file.fp) {
                file_close(&coe_file);
        }

        if (sram_file.fp) {
                file_close(&sram_file);
        }

        return ret;
}

int ksoc_bin_to_array(const char *bin_name, const char *array_name)
{
        int i, c, bin_size, err, ret = -1;
        ALGO_FILE bin_file = {.fp = NULL, .file_name = NULL};
        ALGO_FILE text_file = {.fp = NULL, .file_name = NULL};
        FILE *bfp, *tfp;

        if (file_open(&bin_file, bin_name, "rb")) {
                goto exit;
        }

        if (file_open(&text_file, array_name, "w")) {
                goto exit;
        }

        bfp = bin_file.fp;
        tfp = text_file.fp;
        fseek(bfp, 0, SEEK_SET);
        fseek(tfp, 0, SEEK_SET);
        bin_size = file_get_bin_size(bfp);
        fprintf(tfp, "#define %s\n", array_name, bin_size);
        fprintf(tfp, "static const char %s[%d] = {\n", array_name, bin_size);
        for (i = 0; i < bin_size; i++) {
                c = fgetc(bfp);
                if (c == EOF) {
                        err = ferror(bfp);
                        printf("fgetc %s failed (ferror: %d)\n", bin_file.file_name, err);
                        goto exit;
                }
                if ((i % 16) == 0) {
                        fprintf(tfp, "        ");
                }
                fprintf(tfp, "0x%02X", c);
                if ((i + 1) != bin_size) {
                        fprintf(tfp, ",");
                } else {
                        fprintf(tfp, "\n");
                        break;
                }
                if (((i + 1) % 16) != 0) {
                        fprintf(tfp, " ");
                } else {
                        fprintf(tfp, "\n");
                }
        }
        fprintf(tfp, "};\n");
        ret = 0;

exit:
        if (bin_file.fp) {
                file_close(&bin_file);
        }

        if (text_file.fp) {
                file_close(&text_file);
        }

        return ret;
}

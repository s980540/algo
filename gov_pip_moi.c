#include "gov_pip_moi.h"

void pip_moi_init(void)
{

}

#define STR_BUF_SIZE    (256)

char str_buf[STR_BUF_SIZE];

int pip_moi_str2date(const char *s)
{

}

int pip_moi_modify(void)
{
    FILE *fp = NULL;
    const char fname[] = "1.csv";
    long fline = 0, pos;

    //
    fp = fopen(fname, "r");
    if (fp == NULL) {
        printf("fopen %s failed\n", fname);
        return -1;
    }
    //
    fseek(fp, 0, SEEK_SET);

    //
    pos = ftell(fp);
    while (fgets(str_buf, STR_BUF_SIZE, fp) != NULL)
        fline++;
    fseek(fp, pos, SEEK_SET);

    //
    while (fline--) {
        file_gets(str_buf, STR_BUF_SIZE, fp);
        printf("%s", str_buf);
    }

    fclose(fp);
}
#ifndef __FILE_H__
#define __FILE_H__

typedef struct _ALGO_FILE
{
        FILE *file;
        char *file_name;
} ALGO_FILE, *PALGO_FILE;

void read_file_test(void);
int file_open(ALGO_FILE *fp, const char *name, const char *mode);
int file_close(ALGO_FILE *fp);

long file_get_bin_size(FILE *fp);
long file_get_bin_size2(const char *file_name);

int file_bin_to_array(const char *bin_name, const char *array_name);

#endif  /* ~ __FILE_H__ */
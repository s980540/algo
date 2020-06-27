#ifndef __FILE_H__
#define __FILE_H__

typedef struct _ALGO_FILE
{
        FILE *fp;
        char *file_name;
} ALGO_FILE, *PALGO_FILE;

int file_open(ALGO_FILE *afp, const char *name, const char *mode);
int file_close(ALGO_FILE *afp);

long file_get_bin_size(FILE *fp);
long file_get_bin_size2(const char *file_name);

#endif  /* ~ __FILE_H__ */
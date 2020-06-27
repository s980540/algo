#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "types.h"
#include "utility.h"
#include "file.h"

int file_open(ALGO_FILE *af, const char *name, const char *mode)
{
        af->fp = fopen(name, mode);
        if (af->fp == NULL) {
                printf("fopen %s failed\n", name);
                return ALGO_SYSTEM_ERROR;
        } else {
                int len = (int)strlen(name) + 1;
                char *sname = malloc(len);
                memcpy(sname, name, len);
                af->file_name = sname;
                printf("fopen %s\n", af->file_name);
                return 0;
        }
}

int file_close(ALGO_FILE *af)
{
        int ret;

        ret = fclose(af->fp);
        if (ret == EOF) {
                ret = ferror(af->fp);
                printf("fclose %s failed (ferror %d)\n", af->file_name, ret);
                free(af->file_name);
                return ALGO_SYSTEM_ERROR;
        } else {
                printf("fclose %s\n", af->file_name);
                free(af->file_name);
                af->fp = NULL;
        }

        return 0;
}

/*
 * The fseek function
 *
 * Synopsis
 *      #include <stdio.h>
 *      int fseek(FILE *stream, long int offset, int whence);
 *
 * Description
 *      The fseek function sets the file position indicator for the stream pointed by stream.
 *
 * @whence
 *      SEEK_SET: The beginning of the file
 *      SEEK_CUR: The current value of the file position indicator
 *      SEEK_END: End-of-file
 *
 * Returns
 *      The fseek functions returns nonzero only for a request that cannot be satisfied.
 *
 * Remark
 *      fsetpos, fgetpos
 */
long file_get_bin_size(FILE *fp)
{
        long pos, size;

        // backup the current location of file position indicator
        pos = ftell(fp);
        // move the file position indicator to the end
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        // restore the location of file position indicator
        fseek(fp, pos, SEEK_SET);

        return size;
}

/*
 * #include <sys/stat.h>
 */
long file_get_bin_size2(const char *file_name)
{
        struct stat st;
        stat(file_name, &st);

        return st.st_size;
}

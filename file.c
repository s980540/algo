#include <stdio.h>
#include <sys/stat.h>
#include "types.h"
#include "utility.h"
#include "file.h"

/*
 * Remark
 *      fgetc(), fgets()
 * "r": open text file
 * "rb": open binary file
 */
void read_file_test(void)
{
        int ret, c;
        long i, bin_size, bin_size2;
        char *bin_buf;
        const char *file_name = "KKT_SOC.axf.bin";
        FILE *fp = NULL;

        printf("read_file_test\n");
        printf("sizeof(int): %d, sizeof(long): %d\n", sizeof(int), sizeof(long));

        //fp = fopen(file_name, "r");
        fp = fopen(file_name, "rb");
        if (fp == NULL) {
                printf("Read a file failed.\n");
        } else {
                printf("Read a file passed.\n");
        }

        // calculate file size
        bin_size = file_get_bin_size(fp);
        printf("file size: %d\n", bin_size);

        bin_size2 = file_get_bin_size2(file_name);
        printf("file_size2: %d\n", bin_size2);

        bin_buf = (char *)malloc((size_t)bin_size2);

        // get the content and put them into the buffer
        for (i = 0; i < bin_size2; i++) {
                c = getc(fp);
                if (c == EOF) {
                        ret = ferror(fp);
                        printf("fgetc() failed (ferror: %d)\n", ret);
                        break;
                }
                bin_buf[i] = (char)c;
        }

        printf("i: %d bin_size: %d\n", i, bin_size);

        // print the file content
        __print_buf(bin_buf, bin_size, "KKT_SOC.axf.bin");

        // output a new file base on the file content

        // close the file
        free(bin_buf);
        ret = fclose(fp);
        if (ret == EOF) {
                printf("fclose() failed\n");
        }
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

#include "string_match.h"

void test_string_match(void)
{
        char *src = "detabcgfabdcabc";
        char *tar = "abc";
        u32 rlt;

        if (native_string_match(src, tar, &rlt) > 0)
                printf("String is found.(%d)\n", rlt);
        else
                printf("String is not found\n.");

        return;
}

int native_string_match(const char *src, const char *tar, u32 *rlt)
{
#ifdef EN_STRING_MATCH_DBG_OPT
        if (src == NULL || tar == NULL)
                __fatal_error(1);
#endif
        
        u32 tmp = 0, slen = 0, tlen = 0, i, j;

        slen = strlen_of(src);
        tlen = strlen_of(tar);

        if (!slen || !tlen)
                return(0);

        i = 0;
        while (i + tlen <= slen) {
                j = 0;
                while (1) {
                        if ((j >= tlen) || (src[i + j] != tar[j]))
                                break;

                        j++;
                }

                if (j == tlen) {
                #ifdef EN_STRING_MATCH_DBG_OPT
                        if (tmp == UINT_MAX)
                                __fatal_error(1);
                #endif

                        tmp++;

                        /* Adjust the index to the next character of the current string end. */
                        i = i + tlen;
                } else {
                        i++;
                }
        }

        if (rlt != NULL)
                *rlt = tmp;

        return(tmp > 0);
}


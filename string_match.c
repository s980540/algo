#include "string_match.h"

int native_string_match(const char *src, const char *tar, int *rlt)
{
#if CONFIG_STRING_MATCH_DEBUG_MESSAGE
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
        #if CONFIG_STRING_MATCH_DEBUG_MESSAGE
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


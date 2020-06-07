#ifndef _ERROR_H
#define _ERROR_H

#if 0
enum _error_t
{
        ERROR_UNDEFINED = -2,
        ERROR_OVERFLOW = -1,
        NO_ERROR = 0,
        /* General */
        ERROR_FATAL_ERROR = 1,
        ERROR_MALLOC_FAILED = 2,
        ERROR_INVALID_PTR = 3,
        ERROR_INVALID_ARG = 4,
        ERROR_ZERO_LEN = 5,
        ERROR_ZERO_ARG = 6,
        ERROR_ZERO_BIG_NUM,
        /* SHA2 */
        ERROR_SHA2_UNKNOWN_HASH_ALGORITHM,
        ERROR_SHA2_BITLEN_TOO_LARGE,
        /* RSA */
        ERROR_RSA_MLEN_TOO_LARGE,
        /**/
        ERROR_ZERO_DIVISOR,
        /* STACK */
        ERROR_STACK_OVERFLOW,
        ERROR_STACK_UNDERFLOW,
        /* QUEUE */
        ERROR_QUEUE_OVERFLOW,
        ERROR_QUEUE_UNDERFLOW,
        /* LIST */
        ERROR_LIST_OVERFLOW,
        ERROR_LIST_UNDERFLOW,
        /* OPT */
        ERROR_UNKNOWN_OPT
};
#endif

#define __fatal_error(cond)                                                                           \
    do {                                                                                            \
            if (cond) {                                                                             \
                    printf("[%s][%s][%d] FATAL ERROR.\n", __FILE__, __FUNCTION__, __LINE__);    \
                    while(1);                                                                       \
            }                                                                                       \
    } while(0)

#endif

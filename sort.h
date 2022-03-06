#ifndef _SORT_H
#define _SORT_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "menu.h"
#include "types.h"
#include "utility.h"

#define CONFIG_SORT_LOG_ENABLE      (TRUE)
#define CONFIG_SORT_DEBUG_ENABLE    (TRUE)

#if (CONFIG_SORT_DEBUG_ENABLE)
    #define SORT_ASSERT(cond)   ASSERT(cond)
#else
    #define SORT_ASSERT(cond)
#endif

#if (CONFIG_SORT_LOG_ENABLE)
    #define SORT_PRINT_ARRAY(p, size, digit)    PRINT_ARRAY(p, size, digit)
#else
    #define SORT_PRINT_ARRAY(p, size, digit)
#endif

#define DUMMY_SIZE  (128)   // number of elements of a dummy char array for temporarily sorted values

ret_code menu_func_sort(int argc, char **argv);

#endif
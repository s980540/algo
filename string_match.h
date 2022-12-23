#ifndef _STRING_MATCH_H
#define _STRING_MATCH_H

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "error.h"
#include "utility.h"
#include "menu.h"

#define CONFIG_STRING_MATCH_DEBUG_MESSAGE   (TRUE)

ret_code menu_func_string_match(int argc, char **argv);
void string_match_test(void);
int native_string_match(const char *src, const char *tar, int *rlt);

#endif

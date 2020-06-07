#ifndef _STRING_MATCH_H
#define _STRING_MATCH_H

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "error.h"
#include "utility.h"

#define EN_STRING_MATCH_DBG_OPT

void test_string_match(void);
int native_string_match(const char *src, const char *tar, u32 *rlt);

#endif

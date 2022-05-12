#ifndef _MENU_BIGNUM_H
#define _MENU_BIGNUM_H

#include "types.h"
#include "menu.h"

#include "gmp.h"
#include "expr.h"

#include <stdio.h>
#include <windows.h>

#define CONFIG_ENABLE_LIBEXPR   (1)

ret_code menu_func_bignum(int argc, char **argv);
void bignum_test(char *str);

#endif  // ~ _MENU_BIGNUM_H
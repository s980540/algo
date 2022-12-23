#ifndef _MENU_BIGNUM_H
#define _MENU_BIGNUM_H

#include <stdio.h>
#include "types.h"
#include "menu.h"

#include "gmp.h"
#include "expr.h"


extern const menu_option_t  menu_config_bignum;

ret_code menu_func_bignum(int argc, char **argv);
ret_code demo_bignum(int argc, char **argv);
ret_code bignum_expr(int argc, char **argv);

#endif  // ~ _MENU_BIGNUM_H
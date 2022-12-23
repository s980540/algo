#ifndef _MENU_STRING_MATCH_H
#define _MENU_STRING_MATCH_H

#include <stdio.h>

#include "types.h"
#include "menu.h"
#include "string_match.h"


const menu_option_t menu_config_string_match;

ret_code menu_func_string_match(int argc, char **argv);
ret_code demo_string_match(int argc, char **argv);
ret_code run_native_string_match(int argc, char **argv);


#endif  // ~ _MENU_STRING_MATCH_H

#ifndef _MENU_LIST_H
#define _MENU_LIST_H

#include <stdio.h>

#include "types.h"
#include "menu.h"
#include "list.h"

const menu_option_t menu_config_list;

ret_code menu_func_list(int argc, char **argv);
ret_code demo_list(int argc, char **argv);

#endif
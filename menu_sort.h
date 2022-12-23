#ifndef _MENU_SORT_H
#define _MENU_SORT_H

#include <stdio.h>

#include "types.h"
#include "menu.h"
#include "sort.h"
#include "sort_demo.h"

extern const menu_option_t menu_config_sort;

ret_code menu_func_sort(int argc, char **argv);

ret_code demo_insertion_sort(int argc, char **argv);
ret_code demo_merge_sort(int argc, char **argv);
ret_code demo_heap_sort(int argc, char **argv);
// ret_code demo_max_heap_insert(int argc, char **argv);
ret_code demo_quick_sort(int argc, char **argv);

#endif  // ~ _MENU_SORT_H
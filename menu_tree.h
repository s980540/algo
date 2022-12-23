#ifndef _MENU_TREE_H
#define _MENU_TREE_H

#include <stdio.h>

#include "types.h"
#include "menu.h"
// #include "tree.h"
#include "tree_traverse.h"
#include "tree_derivateve.h"

extern const menu_option_t menu_config_tree;

ret_code menu_func_tree(int argc, char **argv);

// void tree_traverse_demo(void);
// void demo_tree_diff(void);
// void demo_tree_diff_r(void);

ret_code demo_tree(int argc, char **argv);
ret_code demo_tree_diff(int argc, char **argv);
ret_code demo_tree_diff_r(int argc, char **argv);

#endif  // ~ _MENU_TREE_H
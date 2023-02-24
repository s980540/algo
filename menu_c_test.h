#ifndef _MENU_C_TEST_H
#define _MENU_C_TEST_H

#include <stdio.h>

#include "types.h"
#include "menu.h"
#include "c_test.h"

const menu_option_t menu_config_c_test;

ret_code menu_func_c_test(int argc, char **argv);
ret_code run_c_test(int argc, char **argv);
ret_code run_c_test_quanta(int argc, char **argv);
ret_code run_c_test(int argc, char **argv);
ret_code run_songlist2wikitable(int argc, char **argv);
ret_code run_csv2gallerytable(int argc, char ** argv);

#endif  // ~ _MENU_C_TEST_H
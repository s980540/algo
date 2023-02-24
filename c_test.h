#ifndef _C_TEST_H
#define _C_TEST_H

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "utility.h"
#include "menu.h"

#include "sort.h"
#include "search.h"
#include "file.h"

void c_test(void);
void c_test_quanta(int n);
int c_test_inventec(void);
void c_test_elen_1(void);
void c_test_elen_2(void);
ret_code songlist_to_wikitable(void);
ret_code csv_to_gallerytable(void);

#endif
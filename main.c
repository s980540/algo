#include "global.h"
#include "menu.h"
#include "data_struct.h"
#include "string_match.h"
#include "mbr.h"
#include "gpt.h"
#include "fat.h"
#include "evp.h"
#include "file.h"
#include "file_ksoc_array_generation.h"
#include "serial.h"
#include "list.h"
#include "c_test.h"
#include "sort.h"
#include "tree.h"
#include "menu_bignum.h"
#include "menu_tree.h"
#include "menu_sort.h"
#include "menu_c_test.h"
#include "menu_list.h"
#include "menu_string_match.h"

static menu_option_t *_menu_options;

void main_init(void)
{
    menu_register(&_menu_options, &menu_config_bignum);
    menu_register(&_menu_options, &menu_config_tree);
    menu_register(&_menu_options, &menu_config_sort);
    menu_register(&_menu_options, &menu_config_c_test);
    menu_register(&_menu_options, &menu_config_list);
    menu_register(&_menu_options, &menu_config_string_match);
}

int main(int argc, char *argv[])
{
    int ret;
    const char *func_name = menu_get_prog_name(argv[0]);

    main_init();

    /* Initialize menu option module */
    ret = menu_opt_init(argc, argv, 1);
    if (ret != MENU_RET_SUCCESS) {
        printf("menu_opt_init failed (%d)\n", ret);
        goto exit;
    }

    ret = menu_opt_check(_menu_options);
    if (ret != MENU_RET_SUCCESS) {
        printf("menu_opt_check failed (%d)\n", ret);
        goto exit;
    }

    ret = menu_opt_process(argc, argv, func_name, _menu_options);
    if (ret != MENU_RET_SUCCESS) {
        if (ret != MENU_RET_EOF) {
            printf("menu_opt_process failed (%d)\n", ret);
            menu_func_help(func_name, _menu_options);
            goto exit;
        }
    }

exit:
    free(_menu_options);
    return ret;
}

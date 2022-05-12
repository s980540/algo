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

static const struct _MENU_FUNCTION _menu_functions[] =
{
    {"help",    MENU_FUNC_HELP,         menu_func_help,         "Display this summary"},
    {"string",  MENU_FUNC_STRING_MATCH, menu_func_string_match, "Demo string match algorithm"},
    {"list",    MENU_FUNC_LIST,         menu_func_list,         "Demo list algorithm"},
    {"c-test",  MENU_FUNC_C_TEST,       menu_func_c_test,       "Demo c programming language"},
    {"sort",    MENU_FUNC_SORT,         menu_func_sort,         "Demo sort algorithm"},
    {"tree",    MENU_FUNC_TREE,         menu_func_tree,         "Demo tree algorithm"},
    {"bignum",  MENU_FUNC_BIGNUM,       menu_func_bignum,       "Demo big num"},

    {NULL}
};

int main(int argc, char *argv[])
{
    int ret;

    /* Initialize menu option module */
    ret = menu_func_init(argc, argv, _menu_functions);
    if (ret != MENU_RET_SUCCESS) {
        printf("menu_func_init failed (%d)\n", ret);
        goto exit;
    }

    ret = menu_func_process(argc, argv, _menu_functions);
    if (ret != MENU_RET_SUCCESS) {
        if (ret != MENU_RET_EOF) {
            printf("menu_func_process failed (%d)\n", ret);
            printf("%s: Type \"algo help\" to get useful messages.\n", menu_get_prog_name(argv[0]));
        }

        goto exit;
    }

exit:
    return ret;
}

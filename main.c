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

static const struct _MENU_FUNCTION _menu_functions[] = {
    {"help",    MENU_FUNC_HELP,         menu_func_help,         "Display this summary"},
    {"string",  MENU_FUNC_STRING_MATCH, menu_func_string_match, "Demo string match algorithm"},
    {NULL}
};

// bool flag_verbose_debug_message = false;

// void flag_init(void)
// {
//     flag_verbose_debug_message = false;
// }

int main(int argc, char *argv[])
{
    int ret;

    // debug
    printf("argc=%d\n", argc);
    for (int i = 0; ; i++) {
        if (argv[i] == NULL)
            break;

        printf("argv[%d]=%s\n", i, argv[i]);
    }

    /* Initialize menu option module */
    ret = menu_func_init(argc, argv, _menu_functions);
    if (ret != MENU_RET_SUCCESS) {
        printf("menu_func_init failed (%d)\n", ret);
        goto exit;
    }

    ret = menu_func_process(argc, argv, _menu_functions);
    if (ret != MENU_RET_SUCCESS) {
        printf("menu_func_process failed (%d)\n", ret);
        goto exit;
    }

exit:
    return ret;
}

#include "list.h"

typedef enum _OPT_CODE_LIST 
{
    OPT_CODE_LIST_HELP = 0,
    OPT_CODE_LIST_DEMO = 1
} OPT_CODE_LIST;

static const struct _MENU_OPTION list_options[] = 
{
    {"--help", OPT_CODE_LIST_HELP, '-', "Display this summary"},
    {"--demo", OPT_CODE_LIST_DEMO, '-', "Demo list algorithm"},

    {NULL}
};

ret_code menu_func_list(int argc, char **argv)
{
    MENU_RET_CODE ret;
    int opt_code;

    printf("%s\n", __FUNCTION__);

    ret = menu_opt_init(argc, 3, argv, list_options);
    if (ret != MENU_RET_SUCCESS) {
        printf("menu_opt_init failed (%d)\n", ret);

        /* If fail to get an option, we show available options for user */
        menu_opt_help("algo list", list_options);
        return ret;
    }

    while (1) {
        ret = menu_get_opt_code(&opt_code, list_options);
        if (ret != MENU_RET_SUCCESS) {
            if (ret != MENU_RET_EOF) {
                printf("menu_get_opt_code failed (%d)\n", ret);

                /* If fail to get an option, we show available options for user */
                menu_opt_help("algo list", list_options);
                return ret;
            }
            break;
        }

        switch (opt_code) {
        case OPT_CODE_LIST_HELP:
            menu_opt_help("algo list", list_options);
            break;

        case OPT_CODE_LIST_DEMO:
            list_test();
            break;

        default:
            break;
        }
    }

    return ret;
}

struct _foo
{
    int var;
    struct list_head list;
};

void list_test(void)
{
    u32 i;

    printf("%s\n", __FUNCTION__);

    LIST_HEAD(foo_head);

    LIST_ASSERT(list_is_empty(&foo_head));
    
    /* Push 10 elements into a stack */
    for (i = 0; i < 10; i++) {
        struct _foo *foo = malloc(sizeof(struct _foo));
        foo->var = i;
        list_add(&foo->list, &foo_head);
    }

    LIST_ASSERT(!list_is_empty(&foo_head));

    /* Pop 10 element from the stack */
    for (i = 0; i < 10; i++) {
        struct list_head *ptr = foo_head.next;
        list_del(ptr);
        printf("i:%d var=%d\n", i, list_entry(ptr, struct _foo, list)->var);
        free(list_entry(ptr, struct _foo, list));
    }

    LIST_ASSERT(list_is_empty(&foo_head));

    /* Push 10 elements into a queue */
    for (i = 0; i < 10; i++) {
        struct _foo *foo = malloc(sizeof(struct _foo));
        foo->var = i;
        list_add_tail(&foo->list, &foo_head);
    }

    LIST_ASSERT(!list_is_empty(&foo_head));

    /* Pop 10 element from the queue */
    for (i = 0; i < 10; i++) {
        struct list_head *ptr = foo_head.next;
        list_del(ptr);
        printf("i:%d var=%d\n", i, list_entry(ptr, struct _foo, list)->var);
        free(list_entry(ptr, struct _foo, list));
    }

    LIST_ASSERT(list_is_empty(&foo_head));

    return;
}
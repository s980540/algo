#include "menu_list.h"

const menu_option_t menu_config_list =
    {"list", 's', menu_func_list, "List related implementations."};

static const menu_option_t _menu_options[] =
{
    {"--help", '-', NULL, "Display this summary"},
    {"--demo", '-', demo_list, "Demo list algorithm"},

    {NULL}
};

ret_code menu_func_list(int argc, char **argv)
{
    int ret;
    const char *func_name = menu_config_list.name;

    ret = menu_opt_check(_menu_options);
    if (ret != MENU_RET_SUCCESS) {
        printf("menu_opt_check failed (%d)\n", ret);
        exit(ret);
    }

    ret = menu_opt_process(argc, argv, func_name, _menu_options);
    if (ret != MENU_RET_SUCCESS) {
        if (ret != MENU_RET_EOF) {
            printf("menu_opt_process failed (%d)\n", ret);
            menu_func_help(func_name, _menu_options);
            exit(ret);
        }
    }

    return ret;
}

ret_code demo_list(int argc, char **argv)
{

    struct _foo
    {
        int var;
        struct list_head list;
    };

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

    return MENU_RET_SUCCESS;
}
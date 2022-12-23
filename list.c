#include "list.h"

void list_test(void)
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

    return;
}
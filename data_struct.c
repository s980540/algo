#include "data_struct.h"

#define ENTRY_NUM 10

struct _foo
{
        u32 bar;
        struct list_head list;  
};

void test_list(void)
{
        u32 i;
        
        /* create a empty list/ list head. */
        LIST_HEAD(foo_head);
        #if 0
        if (!list_empty(&foo_head))
                __fatal_error(1);

        /* add elements to a stack. */
        for (i = 0; i < ENTRY_NUM; i++) {
                struct _foo *foo = malloc(sizeof(*foo));
                list_add(&foo->list, &foo_head);
        }

        /* delete elements from the stack. */
        for (i = 0; !list_empty(&foo_head); i++) {
                struct list_head *del = foo_head.next;
                list_del(del);
                free(list_entry(del, struct _foo, list));
        }

        if (i != ENTRY_NUM)
                __fatal_error(1);

        /* add elements to a queue. */
        for (i = 0; i < ENTRY_NUM; i++) {
                struct _foo *foo = malloc(sizeof(*foo));
                list_add_tail(&foo->list, &foo_head);
        }

        /* delete elements from the queue. */
        for (i = 0; !list_empty(&foo_head); i++) {
                struct list_head *del = foo_head.next;
                list_del(del);
                free(list_entry(del, struct _foo, list));
        }

        if (i != ENTRY_NUM)
                __fatal_error(1);
        #endif
/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

        /* add elements to a stack. */
        for (i = 0; i < ENTRY_NUM; i++) {
                struct _foo *foo = malloc(sizeof(*foo));
                foo->bar = i;
                printf("1_%x %x %x %x %x %x\n", foo_head, foo_head.prev, foo_head.next, foo->list, foo->list.prev, foo->list.next);
                list_add(&foo->list, &foo_head);
                printf("2_%x %x %x %x %x %x\n", foo_head, foo_head.prev, foo_head.next, foo->list, foo->list.prev, foo->list.next);
        }
        #if 0
        struct list_head *j;
        struct _foo *k;

        /**
         * list_for_each - iterate over a list
         * @pos:        the &struct list_head to use as a loop cursor.
         * @head:       the head for your list.
         */
        printf("list_for_each\n");
        list_for_each(j, &foo_head) {
                printf("bar = %d\n", list_entry(j, struct _foo, list)->bar);
        }

        /**
         * list_for_each_prev - iterate over a list backwards
         * @pos:        the &struct list_head to use as a loop cursor.
         * @head:       the head for your list.
         */
        printf("list_for_each_prev\n");
        list_for_each_prev(j, &foo_head) {
                printf("bar = %d\n", list_entry(j, struct _foo, list)->bar);
        }

        /**
         * list_for_each_entry  - iterate over list of given type
         * @pos:        the type * to use as a loop cursor.
         * @head:       the head for your list.
         * @member:     the name of the list_head within the struct.
         */
        printf("list_for_each_entry\n");
        list_for_each_entry(k, &foo_head, list) {
                printf("bar = %d\n", k->bar);
        }

        /**
         * list_for_each_entry_reverse - iterate backwards over list of given type.
         * @pos:        the type * to use as a loop cursor.
         * @head:       the head for your list.
         * @member:     the name of the list_head within the struct.
         */
        printf("list_for_each_entry_reverse\n");
        list_for_each_entry_reverse(k, &foo_head, list) {
                printf("bar = %d\n", k->bar);
        }
        #endif
        /* delete elements from the queue. */
        for (i = 0; !list_empty(&foo_head); i++) {
                struct list_head *del = foo_head.next;
                list_del(del);
                free(list_entry(del, struct _foo, list));
        }

        if (i != ENTRY_NUM)
                __fatal_error(1);
}


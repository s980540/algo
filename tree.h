#ifndef _TREE_H
#define _TREE_H

#include "types.h"
#include "menu.h"
#include "list.h"

#include <stdio.h>

// #define tree_add_l  list_add
// #define tree_add_r  list_add_tail
// #define TREE   LIST_HEAD
// #define list tree

// typedef struct list_head TREE;

#define CONFIG_TREE_DEBUG_MESSAGE       (TRUE)
#if (CONFIG_TREE_DEBUG_MESSAGE)
    #define TREE_ASSERT (cond)      ASSERT(cond)
#else
    #define TREE_ASSERT(cond)
#endif

struct tree {
    struct tree *llink, *rlink;
};

#define TREE_INIT(name)    {&(name), &(name)}

#define TREE(name) \
    struct tree name = TREE_INIT(name)

#define tree_entry(ptr, type, member) \
    ((type *)( (char *)(ptr) - ((size_t)&((type *)0)->member) ))

static inline void INIT_TREE(struct tree *tree)
{
    tree->llink = NULL;
    tree->rlink = NULL;
}

static inline void tree_add_l(
    struct tree *child,
    struct tree *parent)
{
    parent->llink = child;
}

static inline void tree_add_r(
    struct tree *child,
    struct tree *parent)
{
    parent->rlink = child;
}

ret_code menu_func_tree(int argc, char **argv);

void tree_test(void);

#endif
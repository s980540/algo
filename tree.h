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

struct threaded_tree {
    struct {
        u8 ltag : 1;
        u8 rtag : 1;
    };
    struct threaded_tree *llink, *rlink;
};

// #define TTREE_HEAD_INIT(name)    {.ltag = 1, .rtag = 0, .llink = &(name), .rlink = &(name)}
#define TTREE_HEAD_INIT(name)    {1, 0, &(name), &(name)}

#define TTREE_HEAD(name) \
    struct threaded_tree name = TTREE_HEAD_INIT(name)

#define tree_entry(ptr, type, member) \
    ((type *)( (char *)(ptr) - ((size_t)&((type *)0)->member) ))

static inline void INIT_TREE_NODE(struct tree *tree)
{
    tree->llink = NULL;
    tree->rlink = NULL;
}

static inline void INIT_TTREE_NODE(struct threaded_tree *tree)
{
    tree->ltag = 0;
    tree->rtag = 0;
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

static inline void ttree_add_l(
    struct threaded_tree *child,
    struct threaded_tree *parent,
    u8 tag)
{
    parent->llink = child;
    parent->ltag = tag;
}

static inline void ttree_add_r(
    struct threaded_tree *child,
    struct threaded_tree *parent,
    u8 tag)
{
    parent->rlink = child;
    parent->rtag = tag;
}

ret_code menu_func_tree(int argc, char **argv);

void tree_test(void);

#endif
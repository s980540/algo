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

struct right_threaded_tree {
    u8 rtag;
    struct right_threaded_tree *llink, *rlink;
};

#define TTREE_HEAD_INIT(name)    {.ltag = 1, .rtag = 0, .llink = &(name), .rlink = &(name)}

#define TTREE_HEAD(name) \
    struct threaded_tree name = TTREE_HEAD_INIT(name)

#define RTTREE_HEAD_INIT(name)  {.rtag = 0, .llink = &(name), .rlink = &(name)}

#define RTTREE_HEAD(name) \
    struct right_threaded_tree name = RTTREE_HEAD_INIT(name)

#define tree_entry(ptr, type, member) \
    ((type *)( (char *)(ptr) - ((size_t)&((type *)0)->member) ))

#define PRINT_TTO_NODE(name) \
    _PRINT_TTREE_NODE_INFO(&ttree_head, tto, name, struct _tto, ttree, a)

#define PRINT_RTTO_NODE(name) \
    _PRINT_RTTREE_NODE_INFO(&rttree_head, rtto, name, struct _rtto, rttree, a)

#define _PRINT_TTREE_NODE_INFO(head, p, index, type, member, data) \
    printf("\n" #index "->ltag %d\n", ((p)[index]).member.ltag); \
    printf(#index "->llink %c\n", ((p)[index]).member.llink == (head) ? 'H' : tree_entry(((p)[index]).member.llink, type, member)->data); \
    printf(#index "->rtag %d\n", ((p)[index]).member.rtag); \
    printf(#index "->rlink %c\n", ((p)[index]).member.rlink == (head) ? 'H' : tree_entry(((p)[index]).member.rlink, type, member)->data);

#define _PRINT_RTTREE_NODE_INFO(head, p, index, type, member, data) \
    printf("\n" #index "->llink %c\n", (((p)[index]).member.llink == (head)) ? 'H' : (((p)[index]).member.llink == NULL) ? 'N' : tree_entry(((p)[index]).member.llink, type, member)->data); \
    printf(#index "->rtag %d\n", ((p)[index]).member.rtag); \
    printf(#index "->rlink %c\n", ((p)[index]).member.rlink == (head) ? 'H' : tree_entry(((p)[index]).member.rlink, type, member)->data);

static inline void INIT_TTREE_HEAD(struct threaded_tree *ttree)
{
    ttree->llink = ttree;
    ttree->ltag = 1;
    ttree->rlink = ttree;
    ttree->rtag = 0;
}

static inline void INIT_RTTREE_HEAD(struct right_threaded_tree *rttree)
{
    rttree->llink = rttree;
    rttree->rlink = rttree;
    rttree->rtag = 0;
}

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

static inline void INIT_RTTREE_NODE(struct right_threaded_tree *tree)
{
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

static inline void ttree_set_l(
    struct threaded_tree *child,
    struct threaded_tree *parent,
    u8 tag)
{
    parent->llink = child;
    parent->ltag = tag;
}

static inline void ttree_set_r(
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
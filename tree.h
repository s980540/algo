#ifndef _TREE_H
#define _TREE_H

#include "types.h"
#include "list.h"

#include <stdio.h>

#define CONFIG_TREE_DEBUG_MESSAGE       (TRUE)
#if (CONFIG_TREE_DEBUG_MESSAGE)
    #define TREE_ASSERT (cond)      ASSERT(cond)
#else
    #define TREE_ASSERT(cond)
#endif

struct _tree {
    struct _tree *llink, *rlink;
};

typedef struct _threaded_tree_t {
    struct _threaded_tree_t *llink, *rlink;
    struct {
        u8 ltag : 1;
        u8 rtag : 1;
    };
} ttree_t;

typedef struct _right_threaded_tree_t {
    struct _right_threaded_tree_t *llink, *rlink;
    u8 rtag;
} rttree_t;

#define TTREE_HEAD_INIT(name)    {.ltag = 1, .rtag = 0, .llink = &(name), .rlink = &(name)}

#define TTREE_HEAD(name) \
    ttree_t name = TTREE_HEAD_INIT(name)

#define RTTREE_HEAD_INIT(name)  {.rtag = 0, .llink = &(name), .rlink = &(name)}

#define RTTREE_HEAD(name) \
    rttree_t name = RTTREE_HEAD_INIT(name)

#define tree_entry(ptr, type, member) \
    ((type *)( (char *)(ptr) - ((size_t)&((type *)0)->member) ))

// #define PRINT_TTO_NODE(name) \
//     _PRINT_TTREE_NODE_INFO(&ttree_head, tto, name, struct _tto, ttree, a)

// #define PRINT_RTTO_NODE(name) \
//     _PRINT_RTTREE_NODE_INFO(&rttree_head, rtto, name, struct _rtto, rttree, a)

// #define PRINT_RTTO_NODE2(p) \
//     _PRINT_RTTREE_NODE_INFO2(&rttree_head, p, p, struct _rtto, rttree, a)

// #define _PRINT_TTREE_NODE_INFO(head, p, index, type, member, data) \
//     printf("\n" #index "->ltag %d\n", ((p)[index]).member.ltag); \
//     printf(#index "->llink %c\n", ((p)[index]).member.llink == (head) ? 'H' : tree_entry(((p)[index]).member.llink, type, member)->data); \
//     printf(#index "->rtag %d\n", ((p)[index]).member.rtag); \
//     printf(#index "->rlink %c\n", ((p)[index]).member.rlink == (head) ? 'H' : tree_entry(((p)[index]).member.rlink, type, member)->data);

// #define _PRINT_RTTREE_NODE_INFO(head, p, index, type, member, data) \
//     printf("\n" #index "->llink %c\n", (((p)[index]).member.llink == (head)) ? 'H' : (((p)[index]).member.llink == NULL) ? 'N' : tree_entry(((p)[index]).member.llink, type, member)->data); \
//     printf(#index "->rtag %d\n", ((p)[index]).member.rtag); \
//     printf(#index "->rlink %c\n", (((p)[index]).member.rlink == (head)) ? 'H' : (((p)[index]).member.rlink == NULL) ? 'N' : tree_entry(((p)[index]).member.rlink, type, member)->data);

// #define _PRINT_RTTREE_NODE_INFO2(head, p, name, type, member, data) \
//     printf("\n" #name "->llink %c\n", ((p)->llink == (head)) ? 'H' : ((p)->llink == NULL) ? 'N' : tree_entry((p)->llink, type, member)->data); \
//     printf(#name "->rtag %d\n", (p)->rtag); \
//     printf(#name "->rlink %c\n", ((p)->rlink == (head)) ? 'H' : ((p)->rlink == NULL) ? 'N' : tree_entry((p)->rlink, type, member)->data);

static inline void INIT_TTREE_HEAD(struct _threaded_tree_t *ttree)
{
    ttree->llink = ttree;
    ttree->ltag = 1;
    ttree->rlink = ttree;
    ttree->rtag = 1;
}

static inline void INIT_RTTREE_HEAD(rttree_t *rttree)
{
    rttree->llink = NULL;
    rttree->rlink = rttree;
    rttree->rtag = 1;
}

static inline void INIT_RTTREE_NODE(rttree_t *tree)
{
    tree->rtag = 1;
    tree->llink = NULL;
    tree->rlink = NULL;
}

static inline void INIT_TREE_NODE(struct _tree *tree)
{
    tree->llink = NULL;
    tree->rlink = NULL;
}

static inline void INIT_TTREE_NODE(ttree_t *tree)
{
    tree->ltag = 1;
    tree->rtag = 1;
    tree->llink = NULL;
    tree->rlink = NULL;
}

static inline void tree_add_l(
    struct _tree *child,
    struct _tree *parent)
{
    parent->llink = child;
}

static inline void tree_add_r(
    struct _tree *child,
    struct _tree *parent)
{
    parent->rlink = child;
}

static inline void ttree_set_l(
    ttree_t *child,
    ttree_t *parent,
    u8 tag)
{
    parent->llink = child;
    parent->ltag = tag;
}

static inline void ttree_set_r(
    ttree_t *child,
    ttree_t *parent,
    u8 tag)
{
    parent->rlink = child;
    parent->rtag = tag;
}

typedef void (*tree_info_copy_func_t)(void *, const void *);
typedef void *(*tree_alloc_func_t)(const void *, void *, void *);
typedef void *(*tree_copy_func_t)(void *, void *, tree_info_copy_func_t, tree_alloc_func_t);
typedef void (*tree_node_oper_func_t)(void *, void *);


void tree_preorder_traverse_test(struct _tree *t); // TBD
void tree_inorder_traverse_test(struct _tree *t); // TBD
void tree_level_order_traverse_test(struct _tree *t); // TBD

ttree_t *ttree_preorder_successor(ttree_t **, ttree_t *);
ttree_t *ttree_preorder_predecessor(ttree_t **, ttree_t *);
ttree_t *ttree_inorder_successor(ttree_t **, ttree_t *);
ttree_t *ttree_inorder_predecessor(ttree_t **, ttree_t *);
ttree_t *ttree_postorder_successor(ttree_t **, ttree_t *);
ttree_t *ttree_postorder_predecessor(ttree_t **, ttree_t *);
void ttree_add_l(ttree_t *, ttree_t *);
void ttree_add_r(ttree_t *, ttree_t *);
void *ttree_copy(void *, void *, tree_info_copy_func_t, tree_alloc_func_t);

void ttree_copy2(void *t2, void *t1); // TBD
void ttree_preorder_traverse_test(ttree_t *t); // TBD
void ttree_inorder_traverse_test(ttree_t *t); // TBD
void tree_postorder_traverse_test(struct _tree *t); // TBD

rttree_t *rttree_preorder_successor(rttree_t **, rttree_t *);
rttree_t *rttree_inorder_successor(rttree_t **, rttree_t *);
rttree_t *rttree_inodrer_predecessor(rttree_t **, rttree_t *);
void rttree_add_l(rttree_t *, rttree_t *);
void rttree_add_r(rttree_t *, rttree_t *);
void *rttree_copy(void *, void *, tree_info_copy_func_t, tree_alloc_func_t);
void rttree_preorder_traverse(rttree_t *, tree_node_oper_func_t);
void rttree_inorder_traverse(rttree_t *, tree_node_oper_func_t);

#endif

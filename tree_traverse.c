#include "tree_traverse.h"

struct _too
{
    char a;
    struct _tree tree;
};

struct _tto
{
    char a;
    ttree_t ttree;
};

struct _rtto
{
    char a;
    rttree_t rttree;
};

enum _tree_node_index {
    A = 0,
    B = 1,
    C,
    D,
    E,
    F,
    G,
    H,
    I
};

#define ELE_NUM (9)

void _tto_tree_info_copy(void *q, const void *p)
{
    tree_entry(q, struct _tto, ttree)->a = tree_entry(p, struct _tto, ttree)->a;
}

void _rtto_tree_info_copy(void *q, const void *p)
{
    tree_entry(q, struct _rtto, rttree)->a = tree_entry(p, struct _rtto, rttree)->a;
}

void *_tto_tree_alloc(const void *x, void *u, void *v)
{
    struct _tto *r = malloc(sizeof(struct _tto));
    INIT_TTREE_NODE(&r->ttree);
    return r;
}

void *_rtto_tree_alloc(const void *x, void *u, void *v)
{
    struct _rtto *r = malloc(sizeof(struct _rtto));
    INIT_RTTREE_NODE(&r->rttree);
    return &r->rttree;
}

void _rtto_print_node(void *p, void *t)
{
    printf("%c ", tree_entry(p, struct _rtto, rttree)->a);
}

void tree_traverse_demo(void)
{
    u32 i;

    printf("Hello tree algorithm!\n");

    // TREE(too_root);
    struct _tree *too_root;

    // Allocate memory space for 9 nodes
    struct _too *too;
    too = malloc(sizeof(struct _too) * ELE_NUM);

    // Initialize 9 nodes
    for (i = 0; i < 9; i++) {
        INIT_TREE_NODE(&too[i].tree);
        too[i].a = 'A' + i;
    }

    // debug
    for (i = 0; i < 9; i++) {
        printf("%c ", tree_entry(&too[i].tree, struct _too, tree)->a);
    }
    printf("\n");

    // Create tree and subtree and connect them
    too_root = &too[A].tree;

    // 0A 1B 2C 3D 4E 5F 6G 7H 8J
    // A-B
    tree_add_l(&too[B].tree, &too[A].tree);
    // A-C
    tree_add_r(&too[C].tree, &too[A].tree);

    // B-D
    tree_add_l(&too[D].tree, &too[B].tree);

    // C-E
    tree_add_l(&too[E].tree, &too[C].tree);
    // C-F
    tree_add_r(&too[F].tree, &too[C].tree);

    // E-G
    tree_add_r(&too[G].tree, &too[E].tree);

    // F-H
    tree_add_l(&too[H].tree, &too[F].tree);
    // F-J
    tree_add_r(&too[I].tree, &too[F].tree);

    tree_preorder_traverse_test(too_root);
    tree_inorder_traverse_test(too_root);
    tree_postorder_traverse_test(too_root);
    tree_level_order_traverse_test(too_root);

    /* TEST THREADED BINARY TREE */
    // Allocate memory space for 9 nodes
    struct _tto *tto;
    tto = malloc(sizeof(struct _tto) * ELE_NUM);

    // Initialize 9 nodes
    for (i = 0; i < 9; i++) {
        INIT_TTREE_NODE(&tto[i].ttree);
        tto[i].a = 'A' + i;
    }

    // debug
    for (i = 0; i < 9; i++) {
        printf("%c ", tree_entry(&tto[i].ttree, struct _tto, ttree)->a);
    }
    printf("\n");

    // Create tree and subtree and connect them
    // too_root = &tto[0].tree;
    TTREE_HEAD(ttree_head);
    // T-A ($P)
    ttree_set_l(&tto[A].ttree, &ttree_head, 0);

    // 0A 1B 2C 3D 4E 5F 6G 7H 8J
    // A-B ($P)
    ttree_set_l(&tto[B].ttree, &tto[A].ttree, 0);
    // A-C (P$)
    ttree_set_r(&tto[C].ttree, &tto[A].ttree, 0);

    // B-D ($P)
    ttree_set_l(&tto[D].ttree, &tto[B].ttree, 0);
    // B-A (P$)
    ttree_set_r(&tto[A].ttree, &tto[B].ttree, 1);

    // C-E ($P)
    ttree_set_l(&tto[E].ttree, &tto[C].ttree, 0);
    // C-F (P$)
    ttree_set_r(&tto[F].ttree, &tto[C].ttree, 0);

    // D-T ($P)
    ttree_set_l(&ttree_head, &tto[D].ttree, 1);
    // D-B (P$)
    ttree_set_r(&tto[B].ttree, &tto[D].ttree, 1);

    // E-A ($P)
    ttree_set_l(&tto[A].ttree, &tto[E].ttree, 1);
    // E-G (P$)
    ttree_set_r(&tto[G].ttree, &tto[E].ttree, 0);

    // F-H ($P)
    ttree_set_l(&tto[H].ttree, &tto[F].ttree, 0);
    // F-J (P$)
    ttree_set_r(&tto[I].ttree, &tto[F].ttree, 0);

    // G-E ($P)
    ttree_set_l(&tto[E].ttree, &tto[G].ttree, 1);
    // G-C (P$)
    ttree_set_r(&tto[C].ttree, &tto[G].ttree, 1);

    // H-C ($P)
    ttree_set_l(&tto[C].ttree, &tto[H].ttree, 1);
    // H-F (P$)
    ttree_set_r(&tto[F].ttree, &tto[H].ttree, 1);

    // I-F ($P)
    ttree_set_l(&tto[F].ttree, &tto[I].ttree, 1);
    // I-T (P$)
    ttree_set_r(&ttree_head, &tto[I].ttree, 1);

    ttree_preorder_traverse_test(&ttree_head);
    ttree_inorder_traverse_test(&ttree_head);

    //
    printf("\nthreaded tree insertion 1\n");
    INIT_TTREE_HEAD(&ttree_head);
    memset(tto, 0, sizeof(struct _tto) * ELE_NUM);
    // Initialize 9 nodes
    for (i = 0; i < 9; i++) {
        INIT_TTREE_NODE(&tto[i].ttree);
        tto[i].a = 'A' + i;
    }

    ttree_add_l(&tto[A].ttree, &ttree_head);
    ttree_add_l(&tto[B].ttree, &tto[A].ttree);
    ttree_add_r(&tto[C].ttree, &tto[A].ttree);
    ttree_add_l(&tto[D].ttree, &tto[B].ttree);
    ttree_add_l(&tto[E].ttree, &tto[C].ttree);
    ttree_add_r(&tto[F].ttree, &tto[C].ttree);
    ttree_add_r(&tto[G].ttree, &tto[E].ttree);
    ttree_add_l(&tto[H].ttree, &tto[F].ttree);
    ttree_add_r(&tto[I].ttree, &tto[F].ttree);

    ttree_preorder_traverse_test(&ttree_head);
    ttree_inorder_traverse_test(&ttree_head);

    //
    printf("\nthreaded tree insertion 2\n");
    INIT_TTREE_HEAD(&ttree_head);
    memset(tto, 0, sizeof(struct _tto) * ELE_NUM);
    // Initialize 9 nodes
    for (i = 0; i < 9; i++) {
        INIT_TTREE_NODE(&tto[i].ttree);
        tto[i].a = 'A' + i;
    }

    ttree_add_l(&tto[A].ttree, &ttree_head);
    ttree_add_l(&tto[B].ttree, &tto[A].ttree);
    ttree_add_l(&tto[D].ttree, &tto[B].ttree);
    ttree_add_r(&tto[C].ttree, &tto[A].ttree);
    ttree_add_r(&tto[F].ttree, &tto[C].ttree);
    ttree_add_r(&tto[I].ttree, &tto[F].ttree);
    ttree_add_l(&tto[E].ttree, &tto[C].ttree);
    ttree_add_r(&tto[G].ttree, &tto[E].ttree);
    ttree_add_l(&tto[H].ttree, &tto[F].ttree);

    ttree_preorder_traverse_test(&ttree_head);
    ttree_inorder_traverse_test(&ttree_head);

    //
    printf("\nthreaded tree insertion 3\n");
    INIT_TTREE_HEAD(&ttree_head);
    memset(tto, 0, sizeof(struct _tto) * ELE_NUM);
    // Initialize 9 nodes
    for (i = 0; i < 9; i++) {
        INIT_TTREE_NODE(&tto[i].ttree);
        tto[i].a = 'A' + i;
    }

    ttree_add_l(&tto[A].ttree, &ttree_head);
    ttree_add_l(&tto[D].ttree, &tto[A].ttree);
    ttree_add_r(&tto[I].ttree, &tto[A].ttree);
    ttree_add_r(&tto[F].ttree, &tto[A].ttree);
    ttree_add_l(&tto[H].ttree, &tto[F].ttree);
    ttree_add_r(&tto[C].ttree, &tto[A].ttree);
    ttree_add_l(&tto[E].ttree, &tto[C].ttree);
    ttree_add_r(&tto[G].ttree, &tto[E].ttree);
    ttree_add_l(&tto[B].ttree, &tto[A].ttree);

    ttree_preorder_traverse_test(&ttree_head);
    ttree_inorder_traverse_test(&ttree_head);

    //
    printf("\nthreaded tree insertion 4\n");
    TTREE_HEAD(ttree_head2);
    INIT_TTREE_HEAD(&ttree_head);
    INIT_TTREE_HEAD(&ttree_head2);
    memset(tto, 0, sizeof(struct _tto) * ELE_NUM);
    // Initialize 9 nodes
    for (i = 0; i < 9; i++) {
        INIT_TTREE_NODE(&tto[i].ttree);
        tto[i].a = 'A' + i;
    }

    ttree_add_l(&tto[A].ttree, &ttree_head);
    ttree_add_l(&tto[D].ttree, &tto[A].ttree);
    ttree_add_r(&tto[I].ttree, &tto[A].ttree);
    ttree_add_r(&tto[F].ttree, &tto[A].ttree);
    ttree_add_l(&tto[H].ttree, &tto[F].ttree);
    ttree_add_r(&tto[C].ttree, &tto[A].ttree);
    ttree_add_l(&tto[E].ttree, &tto[C].ttree);
    ttree_add_r(&tto[G].ttree, &tto[E].ttree);
    ttree_add_l(&tto[B].ttree, &tto[A].ttree);

    ttree_copy2(&ttree_head2, &ttree_head);

    ttree_preorder_traverse_test(&ttree_head);
    ttree_preorder_traverse_test(&ttree_head2);

    ttree_inorder_traverse_test(&ttree_head);
    ttree_inorder_traverse_test(&ttree_head2);

    TTREE_HEAD(ttree_head3);
    INIT_TTREE_HEAD(&ttree_head3);
    ttree_copy(&ttree_head3, &ttree_head,
                _tto_tree_info_copy,
                _tto_tree_alloc);
    ttree_inorder_traverse_test(&ttree_head3);
    ttree_preorder_traverse_test(&ttree_head3);

    // Verify exercises 23. [22]
    /* TEST RIGHT-THREADED BINARY TREE */
    printf("\nright-threaded tree insertion 1\n");

    struct _rtto *rtto;

    rtto = malloc(sizeof(struct _rtto) * ELE_NUM);
    memset(rtto, 0, sizeof(struct _rtto) * ELE_NUM);
    for (i = 0; i < 9; i++) {
        INIT_RTTREE_NODE(&rtto[i].rttree);
        rtto[i].a = 'A' + i;
    }

    for (i = 0; i < 9; i++) {
        printf("%c ", tree_entry(&rtto[i].rttree, struct _tto, ttree)->a);
    }
    printf("\n");

    rttree_t rttree_head;
    INIT_RTTREE_HEAD(&rttree_head);

    // PRINT_RTTO_NODE(A);
    rttree_add_l(&rtto[A].rttree, &rttree_head);
    // PRINT_RTTO_NODE(A);
    rttree_add_l(&rtto[B].rttree, &rtto[A].rttree);
    rttree_add_r(&rtto[C].rttree, &rtto[A].rttree);
    rttree_add_l(&rtto[D].rttree, &rtto[B].rttree);
    rttree_add_l(&rtto[E].rttree, &rtto[C].rttree);
    rttree_add_r(&rtto[F].rttree, &rtto[C].rttree);
    rttree_add_r(&rtto[G].rttree, &rtto[E].rttree);
    rttree_add_l(&rtto[H].rttree, &rtto[F].rttree);
    rttree_add_r(&rtto[I].rttree, &rtto[F].rttree);
    // PRINT_RTTO_NODE(D);
    // PRINT_RTTO_NODE(I);

    rttree_inorder_traverse(&rttree_head, _rtto_print_node);
    rttree_preorder_traverse(&rttree_head, _rtto_print_node);

    // RTTREE_HEAD(rttree_head4);
    // INIT_RTTREE_HEAD(&rttree_head4);
    rttree_t *rttree_head4 = malloc(sizeof(rttree_t));
    INIT_RTTREE_NODE(rttree_head4);
    // PRINT_RTTO_NODE2(&rttree_head);
    // PRINT_RTTO_NODE(I);
    // PRINT_RTTO_NODE(D);
    // PRINT_RTTO_NODE(A);
    // rttree_head4 = rttree_copy(0, &rttree_head,
    //                             _rtto_tree_info_copy,
    //                             _rtto_tree_alloc);
    rttree_head4->llink = rttree_copy(0, rttree_head.llink,
                                _rtto_tree_info_copy,
                                _rtto_tree_alloc);
    // ttree_copy2(&ttree_head3, &ttree_head);
    rttree_inorder_traverse(rttree_head4, _rtto_print_node);
    rttree_preorder_traverse(rttree_head4, _rtto_print_node);
    if (rttree_head4 != NULL)
        free(rttree_head4);

    return;

    //
    printf("\nthreaded tree insertion 2\n");
    INIT_RTTREE_HEAD(&rttree_head);
    memset(rtto, 0, sizeof(struct _rtto) * ELE_NUM);
    // Initialize 9 nodes
    for (i = 0; i < 9; i++) {
        INIT_RTTREE_NODE(&rtto[i].rttree);
        rtto[i].a = 'A' + i;
    }

    rttree_add_l(&rtto[A].rttree, &rttree_head);
    rttree_add_l(&rtto[B].rttree, &rtto[A].rttree);
    rttree_add_l(&rtto[D].rttree, &rtto[B].rttree);
    rttree_add_r(&rtto[C].rttree, &rtto[A].rttree);
    rttree_add_r(&rtto[F].rttree, &rtto[C].rttree);
    rttree_add_r(&rtto[I].rttree, &rtto[F].rttree);
    rttree_add_l(&rtto[E].rttree, &rtto[C].rttree);
    rttree_add_r(&rtto[G].rttree, &rtto[E].rttree);
    rttree_add_l(&rtto[H].rttree, &rtto[F].rttree);

    rttree_inorder_traverse(&rttree_head, _rtto_print_node);

    //
    printf("\nthreaded tree insertion 3\n");
    INIT_RTTREE_HEAD(&rttree_head);
    memset(rtto, 0, sizeof(struct _rtto) * ELE_NUM);
    // Initialize 9 nodes
    for (i = 0; i < 9; i++) {
        INIT_RTTREE_HEAD(&rtto[i].rttree);
        rtto[i].a = 'A' + i;
    }

    rttree_add_l(&rtto[A].rttree, &rttree_head);
    rttree_add_l(&rtto[D].rttree, &rtto[A].rttree);
    rttree_add_r(&rtto[I].rttree, &rtto[A].rttree);
    rttree_add_r(&rtto[F].rttree, &rtto[A].rttree);
    rttree_add_l(&rtto[H].rttree, &rtto[F].rttree);
    rttree_add_r(&rtto[C].rttree, &rtto[A].rttree);
    rttree_add_l(&rtto[E].rttree, &rtto[C].rttree);
    rttree_add_r(&rtto[G].rttree, &rtto[E].rttree);
    rttree_add_l(&rtto[B].rttree, &rtto[A].rttree);

    rttree_inorder_traverse(&rttree_head, _rtto_print_node);

    free(too);
    free(tto);
    free(rtto);

    return;
}
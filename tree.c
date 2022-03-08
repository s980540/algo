#include "tree.h"

typedef enum _OPT_CODE_TREE
{
    OPT_CODE_TREE_HELP = 0,
    OPT_CODE_TREE_DEMO = 1
} OPT_CODE_TREE;

static const struct _MENU_OPTION tree_options[] =
{
    {"--help", OPT_CODE_TREE_HELP, '-', "Display this summary"},
    {"--demo", OPT_CODE_TREE_DEMO, '-', "Demo tree algorithm"},

    {NULL}
};

ret_code menu_func_tree(int argc, char **argv)
{
    MENU_RET_CODE ret;
    int opt_code;

    printf("%s\n", __FUNCTION__);

    ret = menu_opt_init(argc, 3, argv, tree_options);
    if (ret != MENU_RET_SUCCESS) {
        printf("menu_opt_init failed (%d)\n", ret);

        /* If fail to get an option, we show available options for user */
        menu_opt_help("algo tree", tree_options);
        return ret;
    }

    while (1) {
        ret = menu_get_opt_code(&opt_code, tree_options);
        if (ret != MENU_RET_SUCCESS) {
            if (ret != MENU_RET_EOF) {
                printf("menu_get_opt_code failed (%d)\n", ret);

                /* If fail to get an option, we show available options for user */
                menu_opt_help("algo tree", tree_options);
                return ret;
            }
            break;
        }

        switch (opt_code) {
        case OPT_CODE_TREE_HELP:
            menu_opt_help("algo tree", tree_options);
            break;

        case OPT_CODE_TREE_DEMO:
            tree_test();
            break;

        default:
            break;
        }
    }

    return ret;
}

struct _too
{
    char a;
    struct tree tree;
};

struct _tto
{
    char a;
    struct threaded_tree ttree;
};


#define ELE_NUM (9)
#define STACK_SIZE (256)


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


static struct tree *_lstack[STACK_SIZE];
static struct tree *_rstack[STACK_SIZE];

void tree_preorder_traverse_test(struct tree *t)
{
    // T1. [Initialize]
    int lsp = 0;
    struct tree *p = t;

    printf("\npreorder traverse start >>>\n");

    while (1) {
    T2:
        // T2. [p = EMPTY?]
        if (p == NULL) {
            goto T4;
        }

    T3:
        // T3. [Visit p and stack p]
        printf("%c ", tree_entry(p, struct _too, tree)->a);
        _lstack[lsp++]= p;
        p = p->llink;
        goto T2;

    T4:
        // T4. [p <= STACK]
        if (lsp == 0) {
            break;
        }
        p = _lstack[--lsp];
        p = p->rlink;
        goto T2;
    }

    printf("\npreorder traverse end <<<\n");
}

void tree_inorder_traverse_test(struct tree *t)
{
    // T1. [Initialize] Set stack A empty, and set the link variable P <- T.
    int lsp = 0;
    struct tree *p = t;


    printf("\ninorder traverse start >>>\n");

    while (1) {
    T2:
        // T2. [P = EMPTY?] If P is empty, go to step T4.
        if (p == NULL) {
            // printf("p = NULL\n");
            goto T4;
        }

        // T3. [STACK <= P] Set A <= P. Then set P <- LLINK(P) and return to step T2.
        _lstack[lsp++] = p;

        // debug
        // printf("push %c\n", tree_entry(p, struct _too, tree)->a);
        // printf("LLINK(%c): %c\n",
        //     tree_entry(p, struct _too, tree)->a,
        //     ((p->llink == NULL) ? 'n' : (tree_entry(p->llink, struct _too, tree)->a)));

        // P <- LLINK(P)
        p = p->llink;
        goto T2;

    T4:
        // T4. [P <= STACK] If stack A is empty, the algorithm terminates; otherwise set P <= A.
        if (lsp == 0) {
            // debug
            // printf("STACK IS EMPTY\n");
            break;
        }

        // debug
        // printf("lsp: %d\n", lsp);
        // printf("_lstack(%d): %c\n", lsp - 1,
        //     tree_entry(_lstack[lsp - 1], struct _too, tree)->a);

        p = _lstack[--lsp];

        // debug
        // printf("RLINK(%c): %c\n",
        //     tree_entry(p, struct _too, tree)->a,
        //     ((p->rlink == NULL) ? 'n' : (tree_entry(p->rlink, struct _too, tree)->a)));

        // T5. [Visit P] Visit NODE(P). Then set P <- RLINK(P) and return to step T2
        printf("%c ", tree_entry(p, struct _too, tree)->a);

        p = p->rlink;
        goto T2;
    }

    printf("\ninorder traverse end <<<\n");
}

void tree_postorder_traverse_test(struct tree *t)
{
    // T1. [Initialize]
    int lsp = 0, rsp = 0;
    struct tree *p = t;

    printf("\npostorder traverse start >>>\n");

    while (1) {
    T2:
        // T2. [p = EMPTY?]
        if (p == NULL) {
            goto T4;
        }

    T3:
        // T3. [STACK <= P]
        _lstack[lsp++] = p;
        p = p->llink;
        goto T2;

    T4:
        // T4. [p <= STACK]
        if (lsp == 0) {
            break;
        }
        p = _lstack[--lsp];

        // T5. [RLINK(P) = EMPTY?]
        if (p->rlink == NULL) {
        T6:
            // T6. [Visit P]
            printf("%c ", tree_entry(p, struct _too, tree)->a);

        T8:
            // T8. [RSTACK = EMPTY?]
            if (rsp == 0) {
                goto T4;
            } else {
                // T9.
                if (_rstack[rsp - 1]->rlink == p) {
                    // T10.
                    p = _rstack[--rsp];
                    printf("%c ", tree_entry(p, struct _too, tree)->a);
                    goto T8;
                } else {
                    goto T4;
                }
            }
        } else {
            // T7. [STACK <= P, P <- RLINK(P)]
            _rstack[rsp++] = p;
            p = p->rlink;
            goto T2;
        }
    }

    printf("\npostorder traverse end <<<\n");
}

#if 1
void threaded_tree_inorder_traverse_test(struct threaded_tree *t)
{
    // S0. [Initialize.] Set P <- HEAD (Q <- HEAD, also)
    struct threaded_tree *p = t, *q;

    printf("\nthreaded tree inorder traverse start >>>\n");

    while (1) {
    S2:
        // S2. [Search to left.] Set P <- Q, Q <- LLINK(Q)
        q = p->llink;
        if (p->ltag == 0) {
            p = q;
            goto S2;
        }

    S3:
        // S3. [Visit P.] Visit unless P = HEAD
        if (p == t) {
            break;
        }

        printf("%c ", tree_entry(p, struct _tto, ttree)->a);

    S1:
        // S1. [RLINK(P) a thread?]
        q = p->rlink;
        if (p->rtag) {
            p = q;
            goto S3;
        } else {
            p = q;
        }
    }

    printf("\nthreaded tree  inorder traverse end <<<\n");
}
#elif 0
void threaded_tree_inorder_traverse_test(struct threaded_tree *t)
{
    // S0. [Initialize.] Set P <- HEAD (Q <- HEAD, also)
    struct threaded_tree *p = t, *q = t;

    printf("\nthreaded tree inorder traverse start >>>\n");

    while (1) {
    S2:
        // S2. [Search to left.] Set P <- Q, Q <- LLINK(Q)
        p = q;
        q = p->llink;
        if (p->ltag == 0) {
            goto S2;
        }

    S3:
        // S3. [Visit P.] Visit unless P = HEAD
        if (p == t) {
            break;
        }

        printf("%c ", tree_entry(p, struct _tto, ttree)->a);

    S1:
        // S1. [RLINK(P) a thread?]
        q = p->rlink;
        if (p->rtag) {
            p = q;
            goto S3;
        }
    }

    printf("\nthreaded tree  inorder traverse end <<<\n");
}
#elif 0
void threaded_tree_inorder_traverse_test(struct threaded_tree *t)
{
    // S0. [Initialize.] Set P <- HEAD (Q <- HEAD, also)
    struct threaded_tree *p = t;

    printf("\nthreaded tree inorder traverse start >>>\n");

    while (1) {

        // S2. [Search to left.] if LTAG(P) = 0, set P <- LLINK(P)
        while (p->ltag == 0)
            p = p->llink;

    VISIT:
        // S3. [Visit P.] Visit unless P = HEAD
        if (p == t)
            break;

        printf("%c ", tree_entry(p, struct _tto, ttree)->a);

        // S1. [RLINK(P) a thread?]
        if (p->rtag) {
            p = p->rlink;
            goto VISIT;
        } else {
            p = p->rlink;
        }
    }

    printf("\nthreaded tree  inorder traverse end <<<\n");
}
#endif

#if 1
struct threaded_tree *ttree_inorder_successor(
    struct threaded_tree **q,
    struct threaded_tree *p)
{
    struct threaded_tree *s;

    if (p == NULL)
        goto EXIT;

    // S1. [RLINK(P) a threaded?]
    s = p->rlink;
    if (p->rtag)
        goto EXIT;

    // S2. [Search to left.]
    while (s->ltag == 0)
        s = s->llink;

EXIT:
    if (q != NULL)
        *q = s;
    return s;
}

struct threaded_tree *ttree_inorder_predecessor(
    struct threaded_tree **q,
    struct threaded_tree *p)
{
    struct threaded_tree *s;

    if (p == NULL)
        goto EXIT;

    // S1. [LLINK(P) a threaded?]
    s = p->llink;
    if (p->ltag)
        goto EXIT;

    // S2. [Search to right.]
    while (s->rtag == 0)
        s = s->rlink;

EXIT:
    // printf("(*q)->a :%c\n", tree_entry((*q), struct _tto, ttree)->a);
    if (q != NULL)
        *q = s;
    return s;
}

void threaded_tree_add_l(
    struct threaded_tree *q,
    struct threaded_tree *p)
{
    // I1. [Adjust tags and links]
    q->llink = p->llink;
    q->ltag = p->ltag;

    q->rlink = p;
    q->rtag = 1;

    p->llink = q;
    p->ltag = 0;

    // I2. [Was LLINK(P) a thread?]
    if (q->ltag == 0)
        ttree_inorder_predecessor(0, q)->rlink = q;
}

void threaded_tree_add_r(
    struct threaded_tree *q,
    struct threaded_tree *p)
{
    // I1. [Adjust tags and links]
    q->llink = p;
    q->ltag = 1;
    // printf("q->llink->a :%c\n", tree_entry(q->llink, struct _tto, ttree)->a);

    q->rlink = p->rlink;
    q->rtag = p->rtag;
    // printf("q->rlink->a :%c\n", tree_entry(q->rlink, struct _tto, ttree)->a);

    p->rlink = q;
    p->rtag = 0;
    // printf("p->rlink->a :%c\n", tree_entry(p->rlink, struct _tto, ttree)->a);

    // I2. [Was RLINK(P) a thread?]
    if (q->rtag == 0)
        ttree_inorder_successor(0, q)->llink = q;
}
#endif

void tree_test(void)
{
    u32 i;

    printf("Hello tree algorithm!\n");

    // TREE(too_root);
    struct tree *too_root;

    /* Allocate memory space for 9 nodes */
    struct _too *too;
    too = malloc(sizeof(struct _too) * ELE_NUM);

    /* Initialize 9 nodes */
    for (i = 0; i < 9; i++) {
        INIT_TREE_NODE(&too[i].tree);
        too[i].a = 'A' + i;
    }

    // debug
    for (i = 0; i < 9; i++) {
        printf("%c ", tree_entry(&too[i].tree, struct _too, tree)->a);
    }
    printf("\n");

    /* Create tree and subtree and connect them */
    too_root = &too[0].tree;

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
#if 1
    /* Allocate memory space for 9 nodes */
    struct _tto *tto;
    tto = malloc(sizeof(struct _tto) * ELE_NUM);

    /* Initialize 9 nodes */
    for (i = 0; i < 9; i++) {
        INIT_TTREE_NODE(&tto[i].ttree);
        tto[i].a = 'A' + i;
    }

    // debug
    for (i = 0; i < 9; i++) {
        printf("%c ", tree_entry(&tto[i].ttree, struct _tto, ttree)->a);
    }
    printf("\n");

    /* Create tree and subtree and connect them */
    // too_root = &tto[0].tree;
    TTREE_HEAD(ttree_head);
    // T-A ($P)
    threaded_tree_set_l(&tto[A].ttree, &ttree_head, 0);

    // 0A 1B 2C 3D 4E 5F 6G 7H 8J
    // A-B ($P)
    threaded_tree_set_l(&tto[B].ttree, &tto[A].ttree, 0);
    // A-C (P$)
    threaded_tree_set_r(&tto[C].ttree, &tto[A].ttree, 0);

    // B-D ($P)
    threaded_tree_set_l(&tto[D].ttree, &tto[B].ttree, 0);
    // B-A (P$)
    threaded_tree_set_r(&tto[A].ttree, &tto[B].ttree, 1);

    // C-E ($P)
    threaded_tree_set_l(&tto[E].ttree, &tto[C].ttree, 0);
    // C-F (P$)
    threaded_tree_set_r(&tto[F].ttree, &tto[C].ttree, 0);

    // D-T ($P)
    threaded_tree_set_l(&ttree_head, &tto[D].ttree, 1);
    // D-B (P$)
    threaded_tree_set_r(&tto[B].ttree, &tto[D].ttree, 1);

    // E-A ($P)
    threaded_tree_set_l(&tto[A].ttree, &tto[E].ttree, 1);
    // E-G (P$)
    threaded_tree_set_r(&tto[G].ttree, &tto[E].ttree, 0);

    // F-H ($P)
    threaded_tree_set_l(&tto[H].ttree, &tto[F].ttree, 0);
    // F-J (P$)
    threaded_tree_set_r(&tto[I].ttree, &tto[F].ttree, 0);

    // G-E ($P)
    threaded_tree_set_l(&tto[E].ttree, &tto[G].ttree, 1);
    // G-C (P$)
    threaded_tree_set_r(&tto[C].ttree, &tto[G].ttree, 1);

    // H-C ($P)
    threaded_tree_set_l(&tto[C].ttree, &tto[H].ttree, 1);
    // H-F (P$)
    threaded_tree_set_r(&tto[F].ttree, &tto[H].ttree, 1);

    // I-F ($P)
    threaded_tree_set_l(&tto[F].ttree, &tto[I].ttree, 1);
    // I-T (P$)
    threaded_tree_set_r(&ttree_head, &tto[I].ttree, 1);

    threaded_tree_inorder_traverse_test(&ttree_head);

    //
    printf("threaded tree insertion 1\n");
    INIT_TTREE_HEAD(&ttree_head);
    memset(tto, 0, sizeof(struct _tto) * ELE_NUM);
    /* Initialize 9 nodes */
    for (i = 0; i < 9; i++) {
        INIT_TTREE_NODE(&tto[i].ttree);
        tto[i].a = 'A' + i;
    }

    threaded_tree_add_l(&tto[A].ttree, &ttree_head);
    threaded_tree_add_l(&tto[B].ttree, &tto[A].ttree);
    threaded_tree_add_r(&tto[C].ttree, &tto[A].ttree);

    threaded_tree_add_l(&tto[D].ttree, &tto[B].ttree);

    threaded_tree_add_l(&tto[E].ttree, &tto[C].ttree);
    threaded_tree_add_r(&tto[F].ttree, &tto[C].ttree);

    threaded_tree_add_r(&tto[G].ttree, &tto[E].ttree);

    threaded_tree_add_l(&tto[H].ttree, &tto[F].ttree);
    threaded_tree_add_r(&tto[I].ttree, &tto[F].ttree);

    threaded_tree_inorder_traverse_test(&ttree_head);

    //
    printf("threaded tree insertion 2\n");
    INIT_TTREE_HEAD(&ttree_head);
    memset(tto, 0, sizeof(struct _tto) * ELE_NUM);
    /* Initialize 9 nodes */
    for (i = 0; i < 9; i++) {
        INIT_TTREE_NODE(&tto[i].ttree);
        tto[i].a = 'A' + i;
    }

    threaded_tree_add_l(&tto[A].ttree, &ttree_head);
    threaded_tree_add_l(&tto[B].ttree, &tto[A].ttree);
    threaded_tree_add_l(&tto[D].ttree, &tto[B].ttree);

    threaded_tree_add_r(&tto[C].ttree, &tto[A].ttree);
    threaded_tree_add_r(&tto[F].ttree, &tto[C].ttree);
    threaded_tree_add_r(&tto[I].ttree, &tto[F].ttree);

    threaded_tree_add_l(&tto[E].ttree, &tto[C].ttree);
    threaded_tree_add_r(&tto[G].ttree, &tto[E].ttree);

    threaded_tree_add_l(&tto[H].ttree, &tto[F].ttree);

    threaded_tree_inorder_traverse_test(&ttree_head);

    //
    printf("threaded tree insertion 3\n");
    INIT_TTREE_HEAD(&ttree_head);
    memset(tto, 0, sizeof(struct _tto) * ELE_NUM);
    /* Initialize 9 nodes */
    for (i = 0; i < 9; i++) {
        INIT_TTREE_NODE(&tto[i].ttree);
        tto[i].a = 'A' + i;
    }

    // printf("\nAdd A\n");
    threaded_tree_add_l(&tto[A].ttree, &ttree_head);
    // PRINT_TTO_NODE(A);

    // printf("\nAdd D\n");
    threaded_tree_add_l(&tto[D].ttree, &tto[A].ttree);
    // PRINT_TTO_NODE(A);
    // PRINT_TTO_NODE(D);

    // printf("\nAdd I\n");
    threaded_tree_add_r(&tto[I].ttree, &tto[A].ttree);
    // PRINT_TTO_NODE(A);
    // PRINT_TTO_NODE(D);
    // PRINT_TTO_NODE(I);

    // printf("\nAdd F\n");
    threaded_tree_add_r(&tto[F].ttree, &tto[A].ttree);
    // PRINT_TTO_NODE(A);
    // PRINT_TTO_NODE(D);
    // PRINT_TTO_NODE(F);
    // PRINT_TTO_NODE(I);

    threaded_tree_add_l(&tto[H].ttree, &tto[F].ttree);

    threaded_tree_add_r(&tto[C].ttree, &tto[A].ttree);

    threaded_tree_add_l(&tto[E].ttree, &tto[C].ttree);
    threaded_tree_add_r(&tto[G].ttree, &tto[E].ttree);

    threaded_tree_add_l(&tto[B].ttree, &tto[A].ttree);

    threaded_tree_inorder_traverse_test(&ttree_head);

#endif

    free(too);
    free(tto);

    return;
}
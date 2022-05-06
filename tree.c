#include "tree.h"

typedef enum _OPT_CODE_TREE
{
    OPT_CODE_TREE_HELP = 0,
    OPT_CODE_TREE_DEMO = 1,
    OPT_CODE_TREE_DIFF,
    OPT_CODE_TREE_DIFFR
} OPT_CODE_TREE;

static const struct _MENU_OPTION tree_options[] =
{
    {"--help", OPT_CODE_TREE_HELP, '-', "Display this summary"},
    {"--demo", OPT_CODE_TREE_DEMO, '-', "Demo tree algorithm"},
    {"--diff", OPT_CODE_TREE_DIFF, '-', "Demo derivative of formula"},
    {"--diffr", OPT_CODE_TREE_DIFFR, '-', "Demo derivative of formula"},

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

        case OPT_CODE_TREE_DIFF:
            tree_differentiation_test();
            break;

        case OPT_CODE_TREE_DIFFR:
            tree_differentiation_r_test();
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
    struct _tree tree;
};

struct _tto
{
    char a;
    struct _threaded_tree ttree;
};

struct _rtto
{
    char a;
    struct _right_threaded_tree rttree;
};

typedef struct _algebra_symbol
{
    char info[6];
    u8 type;
    struct _right_threaded_tree tree;
} ALGEBRA_SYMBOL, *PALGEBRA_SYMBOL;

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

static struct _tree *_lstack[STACK_SIZE];
static struct _tree *_rstack[STACK_SIZE];
static struct _tree *_queue[STACK_SIZE];

typedef void (*PTREE_INFO_COPY_FUNC)(void *, const void *);
typedef void *(*PTREE_ALLOC_FUNC)(const void *, void *, void *);
typedef void *(*PTREE_COPY_FUNC)(void *, void *, PTREE_INFO_COPY_FUNC, PTREE_ALLOC_FUNC);
typedef void (*PTREE_NODE_OPERATION_FUNC)(void *, void *);

void alge_tree_info_copy(void *q, const void *p);
void *alge_tree_alloc2(const void *x, void *u, void *v);
void *alge_tree_alloc1(const void *x, void *u, void *v);
void *alge_tree_alloc(const void *x, void *u, void *v);
void alge_print_symbol(void *q, void *p);
PALGEBRA_SYMBOL _differentiation_r(
    PALGEBRA_SYMBOL y,
    PTREE_INFO_COPY_FUNC _tree_info_copy,
    PTREE_ALLOC_FUNC _tree_alloc,
    PTREE_COPY_FUNC _tree_copy);
PALGEBRA_SYMBOL _differentiation2(
    int type,
    PALGEBRA_SYMBOL p1,
    PALGEBRA_SYMBOL p2,
    PTREE_INFO_COPY_FUNC _tree_info_copy,
    PTREE_ALLOC_FUNC _tree_alloc,
    PTREE_COPY_FUNC _tree_copy);

void rttree_preorder_traverse_test(
    struct _right_threaded_tree *t,
    PTREE_NODE_OPERATION_FUNC _operation);

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

// 2.3.1. Algorithm T, exercise 12. [22]
void tree_preorder_traverse_test(struct _tree *t)
{
    // T1. [Initialize]
    int lsp = 0;
    struct _tree *p = t;

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

void tree_level_order_traverse_test(struct _tree *t)
{
    int qf = 0, qr = 0, sz;
    struct _tree *p = t;

    printf("\nlevel-order traverse start >>>\n");

    _queue[qr] = t;
    qr = (qr + 1) % STACK_SIZE;

    while (qr != qf) {
        sz = qr > qf ? qr - qf : qf + (STACK_SIZE - qr);
        while (sz--) {
            p = _queue[qf];
            qf = (qf + 1) % STACK_SIZE;
            printf("%c ", tree_entry(p, struct _too, tree)->a);

            if (p->llink != NULL)
                _queue[qr++] = p->llink;

            if (p->rlink != NULL)
                _queue[qr++] = p->rlink;

            qr = qr % STACK_SIZE;
        }
    }

    printf("\nlevel-order traverse end <<<\n");
}

// 2.3.1. Algorithm T
void tree_inorder_traverse_test(struct _tree *t)
{
    // T1. [Initialize] Set stack A empty, and set the link variable P <- T.
    int lsp = 0;
    struct _tree *p = t;

    printf("\ninorder traverse start >>>\n");

    while (1) {
    T2:
        // T2. [P = EMPTY?] If P is empty, go to step T4.
        if (p == NULL)
            goto T4;

        // T3. [STACK <= P] Set A <= P. Then set P <- LLINK(P) and return to step T2.
        _lstack[lsp++] = p;

        // P <- LLINK(P)
        p = p->llink;
        goto T2;

    T4:
        // T4. [P <= STACK] If stack A is empty, the algorithm terminates; otherwise set P <= A.
        if (lsp == 0)
            break;

        p = _lstack[--lsp];

        // T5. [Visit P] Visit NODE(P). Then set P <- RLINK(P) and return to step T2
        printf("%c ", tree_entry(p, struct _too, tree)->a);

        p = p->rlink;
        goto T2;
    }

    printf("\ninorder traverse end <<<\n");
}

// 2.3.1. Algorithm T, exercise 13. [24]
void tree_postorder_traverse_test(struct _tree *t)
{
    // T1. [Initialize]
    int lsp = 0, rsp = 0;
    struct _tree *p = t;

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
void ttree_inorder_traverse_test(struct _threaded_tree *t)
{
    // S0. [Initialize.] Set P <- HEAD (Q <- HEAD, also)
    struct _threaded_tree *p = t, *q;

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
        // if (p == t) {
        if (p == t || p == NULL) {
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

    printf("\nthreaded tree inorder traverse end <<<\n");
}
#elif 0
void ttree_inorder_traverse_test(struct _threaded_tree *t)
{
    // S0. [Initialize.] Set P <- HEAD (Q <- HEAD, also)
    struct _threaded_tree *p = t, *q = t;

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
        // if (p == t) {
        if (p == t || p == NULL) {
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
void ttree_inorder_traverse_test(struct _threaded_tree *t)
{
    // S0. [Initialize.] Set P <- HEAD (Q <- HEAD, also)
    struct _threaded_tree *p = t;

    printf("\nthreaded tree inorder traverse start >>>\n");

    while (1) {

        // S2. [Search to left.] if LTAG(P) = 0, set P <- LLINK(P)
        while (p->ltag == 0)
            p = p->llink;

    VISIT:
        // S3. [Visit P.] Visit unless P = HEAD
        // if (p == t)
        if (p == t || p == NULL)
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

// 2.3.1. Algorithm Sm, exercise 17.[22]
struct _threaded_tree *ttree_preorder_successor(
    struct _threaded_tree **q,
    struct _threaded_tree *p)
{
    if (p == NULL)
        goto EXIT;

    // S1. [If LINKK(P) not a thread]
    if (p->ltag == 0) {
        p = p->llink;
        goto EXIT;
    }

    // S2. [Search to right through thread]
    while ((p->rtag) && (p->rlink != NULL))
        p = p->rlink;

    p = p->rlink;

EXIT:
    if (q != NULL)
        *q = p;

    return p;
}

// NOT CHECK YET
struct _threaded_tree *ttree_preorder_predecessor(
    struct _threaded_tree **q,
    struct _threaded_tree *p)
{
    struct _threaded_tree *s = NULL;

    if (p == NULL)
        goto EXIT;

    // S1.
    s = p;
    while (s->rtag == 0)
        s = s->rlink;

    if ((s->rlink != NULL) && (s->rlink->llink == p)) {
        s = s->rlink;
        goto EXIT;
    }

    // S2.
    s = p;
    while (s->ltag == 0)
        s = s->llink;

    s = s->llink;
    if (s == NULL)
        goto EXIT;

    // S3. [Find the rightmost leaf of the left subtree of the root]
    while (s->ltag == 0) {
        s = s->llink;

        while (s->rtag == 0)
            s = s->rlink;
    }

EXIT:
    if (q != NULL)
        *q = s;

    return s;
}

// 2.3.1. Algorithm S
struct _threaded_tree *ttree_inorder_successor(
    struct _threaded_tree **q,
    struct _threaded_tree *p)
{
    struct _threaded_tree *s = NULL;

    if (p == NULL)
        goto EXIT;

    // S1. [RLINK(P) a thread?]
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

struct _threaded_tree *ttree_inorder_predecessor(
    struct _threaded_tree **q,
    struct _threaded_tree *p)
{
    struct _threaded_tree *s = NULL;

    if (p == NULL)
        goto EXIT;

    // S1. [LLINK(P) a thread?]
    s = p->llink;
    if (p->ltag)
        goto EXIT;

    // S2. [Search to right.]
    while (s->rtag == 0)
        s = s->rlink;

EXIT:
    if (q != NULL)
        *q = s;

    return s;
}

// NOT CHECK YET
struct _threaded_tree *ttree_postorder_successor(
    struct _threaded_tree **q,
    struct _threaded_tree *p)
{
    struct _threaded_tree *s = NULL;

    if (p == NULL)
        goto EXIT;

    // S1.
    s = p;
    while (s->ltag == 0)
        s = s->llink;

    if (s->llink != NULL && s->llink->rlink == p) {
        s = s->llink;
        goto EXIT;
    }

    // S2.
    s = p;
    while (s->rtag == 0)
        s = s->rlink;

    s = s->rlink;
    if (s == NULL)
        goto EXIT;

    // S3. [Find the rightmost leaf of the left subtree of the root]
    while (s->rtag == 0) {
        s = s->rlink;

        while (s->ltag == 0)
            s = s->llink;
    }

EXIT:
    if (q != NULL)
        *q = s;

    return s;
}

struct _threaded_tree *ttree_postorder_predecessor(
    struct _threaded_tree **q,
    struct _threaded_tree *p)
{
    if (p->rtag == 0) {
        p = p->rlink;
        goto EXIT;
    }

    while (p->ltag && p->llink != NULL)
        p = p->llink;

    p = p->llink;

EXIT:
    if (q != NULL)
        *q = p;

    return p;
}

void ttree_add_l(
    struct _threaded_tree *q,
    struct _threaded_tree *p)
{
    // I1. [Adjust tags and links]
    q->llink = p->llink;
    q->ltag = p->ltag;

    q->rlink = p;
    if (p == p->rlink)
        q->rlink = NULL;
    q->rtag = 1;

    p->llink = q;
    p->ltag = 0;

    /**
     * This step is necessary only when inserting into
     * the midst of a threaded tree instead of merely
     * inserting a new leaf.
     */
    // I2. [Was LLINK(P) a thread?]
    if (q->ltag == 0)
        ttree_inorder_predecessor(0, q)->rlink = q;
}

// 2.3.1. Algorithm I
void ttree_add_r(
    struct _threaded_tree *q,
    struct _threaded_tree *p)
{
    // I1. [Adjust tags and links]
    q->llink = p;
    if (p == p->llink)
        q->llink == NULL;
    q->ltag = 1;

    q->rlink = p->rlink;
    q->rtag = p->rtag;

    p->rlink = q;
    p->rtag = 0;

    /**
     * This step is necessary only when inserting into
     * the midst of a threaded tree instead of merely
     * inserting a new leaf.
     */
    // I2. [Was RLINK(P) a thread?]
    if (q->rtag == 0)
        ttree_inorder_successor(0, q)->llink = q;
}

// 2.3.1. Algorithm C
void *ttree_copy(
        void *t2,
        void *t1,
        PTREE_INFO_COPY_FUNC _tree_info_copy,
        PTREE_ALLOC_FUNC _tree_alloc)
{
    struct _threaded_tree *p, *q;
    // C1. [Initiailize]
    if (t2 == NULL) {
        t2 = malloc(sizeof(struct _threaded_tree));
        INIT_TTREE_HEAD(t2);
    }
    q = (struct _threaded_tree *)t2;
    p = (struct _threaded_tree *)t1;

    printf("\nttree_copy start >>>\n");

    while (1) {
        // C4. [Anything to left?]
        if (p->ltag == 0) {
            // struct _tto *r = malloc(sizeof(struct _tto));
            // INIT_TTREE_NODE(&r->ttree);
            // ttree_add_l(&r->ttree, q);
            struct _threaded_tree *r = _tree_alloc(0, 0, 0);
            ttree_add_l(r, q);
        }

        // C5. [Advance.]
        p = ttree_preorder_successor(0, p);
        q = ttree_preorder_successor(0, q);

        // C6. [Test if complete.]
        // if (q == t2->rlink)
        if (p == NULL)
            break;

        // C2. [Anything to right?]
        if (p->rtag == 0) {
            // struct _tto *r = malloc(sizeof(struct _tto));
            // INIT_TTREE_NODE(&r->ttree);
            // ttree_add_r(&r->ttree, q);
            struct _threaded_tree *r = _tree_alloc(0, 0, 0);
            ttree_add_r(r, q);
        }

        // C3. [Copy INFO.]
        // tree_entry(q, struct _tto, ttree)->a = tree_entry(p, struct _tto, ttree)->a;
        _tree_info_copy(q, p);
    }

    printf("\nttree_copy end <<<\n");

    return t2;
}

// 2.3.1. Algorithm C
void ttree_copy2(void *t2, void *t1)
{
    struct _threaded_tree *p, *q;
    // C1. [Initiailize]
    q = (struct _threaded_tree *)t2;
    p = (struct _threaded_tree *)t1;

    while (1) {
        // C4. [Anything to left?]
        if (p->ltag == 0) {
            struct _tto *r = malloc(sizeof(struct _tto));
            INIT_TTREE_NODE(&r->ttree);
            ttree_add_l(&r->ttree, q);
        }

        // C5. [Advance.]
        p = ttree_preorder_successor(0, p);
        q = ttree_preorder_successor(0, q);

        // C6. [Test if complete.]
        // if (q == t2->rlink)
        if (p == NULL)
            break;

        // C2. [Anything to right?]
        if (p->rtag == 0) {
            struct _tto *r = malloc(sizeof(struct _tto));
            INIT_TTREE_NODE(&r->ttree);
            ttree_add_r(&r->ttree, q);
        }

        // C3. [Copy INFO.]
        tree_entry(q, struct _tto, ttree)->a = tree_entry(p, struct _tto, ttree)->a;
    }
}

struct _right_threaded_tree *rttree_preorder_successor(
    struct _right_threaded_tree **q,
    struct _right_threaded_tree *p)
{
    if (p == NULL)
        goto EXIT;

    // S1. [If LLINK(P) not a thread.]
    if (p->llink != NULL) {
        p = p->llink;
        goto EXIT;
    }

    // S2. [Search to right through thread.]
    while (p->rtag && (p->rlink != NULL))
        p = p->rlink;

    p = p->rlink;

EXIT:
    if (q != NULL)
        *q = p;

    return p;
}

struct _right_threaded_tree *rttree_preorder_predecessor(
    struct _right_threaded_tree **q,
    struct _right_threaded_tree *p)
{
    // CAN NOT GUARENTEE TO FIND A PREDECESSOR IN ALL CASES
    return NULL;
}

struct _right_threaded_tree *rttree_inorder_successor(
    struct _right_threaded_tree **q,
    struct _right_threaded_tree *p)
{
    struct _right_threaded_tree *s = NULL;

    if (p == NULL)
        goto EXIT;

    s = p->rlink;
    if (p->rtag)
        goto EXIT;

    while (s->llink != NULL)
        s = s->llink;

EXIT:
    if (q != NULL)
        *q = s;

    return s;
}

struct _right_threaded_tree *rttree_inodrer_predecessor(
    struct _right_threaded_tree **q,
    struct _right_threaded_tree *p)
{
    struct _right_threaded_tree *s = NULL;

    if (p == NULL)
        goto EXIT;

    s = p->llink;
    if (p->llink == NULL)
        goto EXIT;

    while (s->rtag == 0)
        s = s->rlink;

EXIT:
    if (q != NULL)
        *q = s;

    return s;
}

struct _right_threaded_tree *rttree_postorder_successor(
    struct _right_threaded_tree **q,
    struct _right_threaded_tree *p)
{
    // CAN NOT GUARENTEE TO FIND A PREDECESSOR IN ALL CASES
    return NULL;
}

struct _right_threaded_tree *rttree_postorder_predecessor(
    struct _right_threaded_tree **q,
    struct _right_threaded_tree *p)
{
    // CAN NOT GUARENTEE TO FIND A PREDECESSOR IN ALL CASES
    return p;
}

// Algorithm Im, 2.3.1. exercises 23. [22]
void rttree_add_l(
    struct _right_threaded_tree *q,
    struct _right_threaded_tree *p)
{
    // I1. [Adjust tags and links]
    q->llink = p->llink;
    // // SC. If llink point to head, llink will be NULL.
    // if (q->llink == p)
    //     q->llink = NULL;

    q->rlink = p;
    if (p == p->rlink)
        q->rlink = NULL;
    q->rtag = 1;

    p->llink = q;

    /**
     * This step is necessary only when inserting into
     * the midst of a threaded tree instead of merely
     * inserting a new leaf.
     */
    // I2. [Was LLINK(P) a thread?]
    if (q->llink != NULL)
        rttree_inodrer_predecessor(0, q)->rlink = q;
}

// Algorithm Im, 2.3.1. exercises 23. [22]
void rttree_add_r(
    struct _right_threaded_tree *q,
    struct _right_threaded_tree *p)
{
    // I1. [Adjust tags and links]
    // Since we do not have lthread, llink will be NULL here.
    q->llink = NULL;

    q->rlink = p->rlink;
    q->rtag = p->rtag;

    p->rlink = q;
    p->rtag = 0;

    /**
     * This step is necessary only when inserting into
     * the midst of a threaded tree instead of merely
     * inserting a new leaf.
     */
    // // I2. [Was RLINK(P) a thread?]
    // And we do not need to set the llink of the successor of q.
    // if (q->rtag == 0)
    //     rttree_inorder_successor(0, q)->llink = q;
}

void _rtto_print_node(void *p, void *t)
{
    printf("%c ", tree_entry(p, struct _rtto, rttree)->a);
}

void *rttree_copy(
        void *t2,
        void *t1,
        PTREE_INFO_COPY_FUNC _tree_info_copy,
        PTREE_ALLOC_FUNC _tree_alloc)
{
    struct _right_threaded_tree *p, *q;
    // C1. [Initiailize]
#if 1
    struct _right_threaded_tree *q1 = malloc(sizeof(struct _right_threaded_tree));
    struct _right_threaded_tree *p1 = malloc(sizeof(struct _right_threaded_tree));
    INIT_RTTREE_HEAD(q1);
    INIT_RTTREE_HEAD(p1);
    q = q1;
    p = p1;
    p->llink = (struct _right_threaded_tree *)t1;
#else
    // if (t2 == NULL) {
    //     t2 = _tree_alloc(0, 0, 0);
    //     // INIT_TTREE_HEAD(t2);
    // }
    if (t2 == NULL) {
        t2 = malloc(sizeof(struct _threaded_tree));
        INIT_TTREE_NODE(t2);
    }

    q = t2;
    p = t1;
#endif

    // rttree_preorder_traverse_test(p,  alge_print_symbol);

    // printf("\nrttree_copy start >>>\n");
    int type = -1;
    while (1) {
        // C4. [Anything to left?]
        // if (p != NULL) {
        //     printf("p1: %08X\n", p);
        //     printf("p1: %08X\n", p->llink);
        //     printf("p1: %08X\n\n", p->rlink);
        // }
        if (p->llink != NULL) {
            struct _right_threaded_tree *r = _tree_alloc(0, 0, 0);
            // printf("add_l\n");
            rttree_add_l(r, q);
            // if (q != NULL) {
            //     printf("q1: %08X\n", q);
            //     printf("q1: %08X\n", q->llink);
            //     printf("q1: %08X\n\n", q->rlink);
            // }
            // if (r != NULL) {
            //     printf("r1: %08X\n", r);
            //     printf("r1: %08X\n", r->llink);
            //     printf("r1: %08X\n\n", r->rlink);
            // }
        }

        // C5. [Advance.]
        p = rttree_preorder_successor(0, p);
        // if (p != NULL) {
        //     printf("p2: %08X\n", p);
        //     printf("p2: %08X\n", p->llink);
        //     printf("p2: %08X\n\n", p->rlink);
        // }
        // alge_print_symbol(p, &type);
        // alge_print_symbol(p->llink, &type);
        q = rttree_preorder_successor(0, q);

        // C6. [Test if complete.]
        // if (q == ((struct _right_threaded_tree *)t2)->rlink)
        if (p == NULL || q == NULL)
        // if (p == NULL)
            break;

        // C2. [Anything to right?]
        if (p->rtag == 0) {
            struct _right_threaded_tree *r = _tree_alloc(0, 0, 0);
            // printf("add_r\n");
            rttree_add_r(r, q);
        }

        // C3. [Copy INFO.]
        _tree_info_copy(q, p);
        // if (q != NULL) {
        //     printf("q2: %08X\n", q);
        //     printf("q2: %08X\n", q->llink);
        //     printf("q2: %08X\n\n", q->rlink);
        // }
        // printf("copy\n");
    }

    // printf("\nrttree_copy end <<<\n");
#if 1
    q = q1->llink;
    free(q1);
    free(p1);
    return q;
#else
    q = ((struct _right_threaded_tree *)t2)->llink;
    return q;
    // return t2;
#endif
}

void ttree_preorder_traverse_test(struct _threaded_tree *t)
{
    struct _threaded_tree *p = t;

    printf("\nthreaded tree preorder traverse start >>>\n");

    while (1) {
        p = ttree_preorder_successor(0, p);
        // if (p == t)
        if ((p == t) || p == NULL)
            break;
        else
            printf("%c ", tree_entry(p, struct _tto, ttree)->a);
    }
    printf("\nthreaded tree preorder traverse end <<<\n");
}

void rttree_preorder_traverse_test(
    struct _right_threaded_tree *t,
    PTREE_NODE_OPERATION_FUNC _operation)
{
    struct _right_threaded_tree *p = t;

    printf("\nright-threaded tree preorder traverse start >>>\n");

    while (1) {
        p = rttree_preorder_successor(0, p);
        // if (p == t)
        if ((p == t) || p == NULL)
            break;
        else
            _operation(p, 0);
    }
    printf("\nright-threaded tree preorder traverse end <<<\n");
}

void rttree_inorder_traverse_test(
    struct _right_threaded_tree *t,
    PTREE_NODE_OPERATION_FUNC _operation)
{
    // S0. [Initialize.] Set P <- HEAD (Q <- HEAD, also)
    struct _right_threaded_tree *p = t, *q;

    printf("\nright-threaded tree inorder traverse start >>>\n");

    while (1) {
    S2:
        // S2. [Search to left.] Set P <- Q, Q <- LLINK(Q)
        q = p->llink;
        // if (p->ltag == 0) {
        if (p->llink != NULL) {
            p = q;
            goto S2;
        }

    S3:
        // S3. [Visit P.] Visit unless P = HEAD
        // if (p == t) {
        if (p == NULL) {
            break;
        }

        _operation(p, 0);

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

    printf("\nright-threaded tree inorder traverse end <<<\n");
}

void tree_test(void)
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

    struct _right_threaded_tree rttree_head;
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

    rttree_inorder_traverse_test(&rttree_head, _rtto_print_node);
    rttree_preorder_traverse_test(&rttree_head, _rtto_print_node);

    // RTTREE_HEAD(rttree_head4);
    // INIT_RTTREE_HEAD(&rttree_head4);
    struct _right_threaded_tree *rttree_head4 = malloc(sizeof(struct _right_threaded_tree));
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
    rttree_inorder_traverse_test(rttree_head4, _rtto_print_node);
    rttree_preorder_traverse_test(rttree_head4, _rtto_print_node);
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

    rttree_inorder_traverse_test(&rttree_head, _rtto_print_node);

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

    rttree_inorder_traverse_test(&rttree_head, _rtto_print_node);

    free(too);
    free(tto);
    free(rtto);

    return;
}

#define ALGE_SYMBOL_NUM     (13)
#define ALGE_SYMBOL_SIZE    (sizeof(struct _algebra) * ALGE_SYMBOL_NUM)

static const struct _algebra_symbol alge_symbols[] = {
    {.type = 0, .info = {1, 0, 0, 0, 0, 0}, .tree = {.rtag = 1, .llink = NULL, .rlink = NULL}},
    {.type = 1, .info = "x",                .tree = {.rtag = 1, .llink = NULL, .rlink = NULL}},
    {.type = 2, .info = "ln",               .tree = {.rtag = 1, .llink = NULL, .rlink = NULL}},
    {.type = 3, .info = "neg",              .tree = {.rtag = 1, .llink = NULL, .rlink = NULL}},
    {.type = 4, .info = "+",                .tree = {.rtag = 1, .llink = NULL, .rlink = NULL}},
    {.type = 5, .info = "-",                .tree = {.rtag = 1, .llink = NULL, .rlink = NULL}},
    {.type = 6, .info = "*",                .tree = {.rtag = 1, .llink = NULL, .rlink = NULL}},
    {.type = 7, .info = "/",                .tree = {.rtag = 1, .llink = NULL, .rlink = NULL}},
    {.type = 8, .info = "**",               .tree = {.rtag = 1, .llink = NULL, .rlink = NULL}},
    {.type = 0, .info = {0, 0, 0, 0, 0, 0}, .tree = {.rtag = 1, .llink = NULL, .rlink = NULL}},
    {.type = 0, .info = {2, 0, 0, 0, 0, 0}, .tree = {.rtag = 1, .llink = NULL, .rlink = NULL}},
    {.type = 0, .info = {3, 0, 0, 0, 0, 0}, .tree = {.rtag = 1, .llink = NULL, .rlink = NULL}},
    {.type = 0, .info = "a",                .tree = {.rtag = 1, .llink = NULL, .rlink = NULL}},
    {.type = 1, .info = "y",                .tree = {.rtag = 1, .llink = NULL, .rlink = NULL}},
};

#define ALGE_ONE    (alge_symbols)
#define ALGE_VARX   (alge_symbols + 1)
#define ALGE_LN     (alge_symbols + 2)
#define ALGE_NEG    (alge_symbols + 3)
#define ALGE_ADD    (alge_symbols + 4)
#define ALGE_SUB    (alge_symbols + 5)
#define ALGE_MUL    (alge_symbols + 6)
#define ALGE_DIV    (alge_symbols + 7)
#define ALGE_POW    (alge_symbols + 8)
#define ALGE_ZERO   (alge_symbols + 9)
#define ALGE_TWO    (alge_symbols + 10)
#define ALGE_THREE  (alge_symbols + 11)
#define ALGE_A      (alge_symbols + 12)
#define ALGE_VARY   (alge_symbols + 13)

#define DIFF(x)                     _differentiation_r(x, _tree_info_copy, _tree_alloc, _tree_copy)
#define MUL(u, v)                   multiple(u, v, _tree_alloc)
#define ALGE_COPY(s)                tree_entry(_tree_copy(0, &(s->tree), _tree_info_copy, _tree_alloc), ALGEBRA_SYMBOL, tree)
#define ALGES(x)                    tree_entry(x, ALGEBRA_SYMBOL, tree)
#define ALGE0(x)                    tree_entry(_tree_alloc(x, 0, 0), ALGEBRA_SYMBOL, tree)
#define ALGE1(x, u)                 tree_entry(_tree_alloc(x, &(u)->tree, 0), ALGEBRA_SYMBOL, tree)
#define ALGE2(x, u, v)              tree_entry(_tree_alloc(x, &(u)->tree, &(v)->tree), ALGEBRA_SYMBOL, tree)
#define ALGE(N, ...)                ALGE##N(__VA_ARGS__)


void alge_tree_info_copy(void *q, const void *p)
{
    PALGEBRA_SYMBOL q1 = tree_entry(q, ALGEBRA_SYMBOL, tree);
    PALGEBRA_SYMBOL p1 = tree_entry(p, ALGEBRA_SYMBOL, tree);
    memcpy(q1->info, p1->info, 6);
    q1->type = p1->type;
}

void *alge_tree_alloc2(const void *x, void *u, void *v)
{
    PALGEBRA_SYMBOL w = malloc(sizeof(ALGEBRA_SYMBOL));
    if (x != NULL)
        memcpy(w, x, sizeof(ALGEBRA_SYMBOL));
    else
        INIT_RTTREE_NODE(&w->tree);

    if (u != NULL && v != NULL) {
        w->tree.llink = u;
        ((struct _right_threaded_tree *)u)->rlink = v;
        ((struct _right_threaded_tree *)u)->rtag = 0;
        ((struct _right_threaded_tree *)v)->rlink = &w->tree;
        ((struct _right_threaded_tree *)v)->rtag = 1;
    } else if (u != NULL) {
        w->tree.llink = u;
        ((struct _right_threaded_tree *)u)->rlink = &w->tree;
        ((struct _right_threaded_tree *)u)->rtag = 1;
    }
    return &w->tree;
}

void *alge_tree_alloc1(const void *x, void *u, void *v)
{
    PALGEBRA_SYMBOL w = malloc(sizeof(ALGEBRA_SYMBOL));
    if (x != NULL)
        memcpy(w, x, sizeof(ALGEBRA_SYMBOL));
    else
        INIT_RTTREE_NODE(&w->tree);

    if (u != NULL) {
        w->tree.llink = u;
        ((struct _right_threaded_tree *)u)->rlink = &w->tree;
        ((struct _right_threaded_tree *)u)->rtag = 1;
    }
    return &w->tree;
}

void *alge_tree_alloc(const void *x, void *u, void *v)
{
    PALGEBRA_SYMBOL w = malloc(sizeof(ALGEBRA_SYMBOL));
    if (x != NULL)
        memcpy(w, x, sizeof(ALGEBRA_SYMBOL));
    else {
        INIT_RTTREE_NODE(&w->tree);
        w->type = 0;
        memset(w->info, 0, 6);
    }
    return &w->tree;
}

char alge_zero[6] = {0, 0, 0, 0, 0, 0};
char alge_one[6] = {1, 0, 0, 0, 0, 0};

PALGEBRA_SYMBOL multiple(
    PALGEBRA_SYMBOL u,
    PALGEBRA_SYMBOL v,
    PTREE_ALLOC_FUNC _tree_alloc)
{
    if ((u->type == 0) && memcmp(u->info, alge_one, 6) == 0) {
        free(u);
        return v;
    }

    if ((v->type == 0) && memcmp(v->info, alge_one, 6) == 0) {
        free(v);
        return u;
    }

    // return _tree_alloc(ALGE_MUL, &u->tree, &v->tree);
    return ALGE(2, ALGE_MUL, u, v);
}

// Algorithm D
// Differentiation
PALGEBRA_SYMBOL differentiation_r(
    PALGEBRA_SYMBOL y,
    PTREE_INFO_COPY_FUNC _tree_info_copy,
    PTREE_ALLOC_FUNC _tree_alloc,
    PTREE_COPY_FUNC _tree_copy)
{
    PALGEBRA_SYMBOL dy, p, p2;

    if (y == NULL)
        return NULL;

    // D1. [Initialize.]
    p  = ALGE(S, rttree_preorder_successor(0, &y->tree));

    // D2. [Differentiate.]
    dy = ALGE(1, ALGE_VARX, _differentiation_r(p, _tree_info_copy, _tree_alloc, _tree_copy));

    return dy;
}

PALGEBRA_SYMBOL _differentiation_r(
    PALGEBRA_SYMBOL p,
    PTREE_INFO_COPY_FUNC _tree_info_copy,
    PTREE_ALLOC_FUNC _tree_alloc,
    PTREE_COPY_FUNC _tree_copy)
{
    // p1 <- u, p2 <-v, q1 <- D(u), q <- D(v)
    PALGEBRA_SYMBOL q, q1, p1, p2, c1, c2;
    // for debug
    // int type = -1;

    if (p == NULL)
        return NULL;

    // printf("DIFF[%d]\n", p->type);
    // alge_print_symbol(&p->tree, &type);

    switch (p->type) {
    case 0:
        // q = _tree_alloc(ALGE_ZERO, 0, 0);
        q = ALGE(0, ALGE_ZERO);
        break;

    case 1:
        // q = _tree_alloc(strcmp(p->info, "x") == 0 ? ALGE_ONE : ALGE_ZERO, 0, 0);
        // q = _tree_alloc(p->type == 1 ? ALGE_ONE : ALGE_ZERO, 0, 0);
        q = ALGE(0, p->type == 1 ? ALGE_ONE : ALGE_ZERO);
        break;

    case 2:
        // p1 <- u, q <- D(u)
        // info(q) != 0 ? q' <- D(u) / u = q / copy(p1)
        p1 = tree_entry(p->tree.llink, ALGEBRA_SYMBOL, tree);
        q  = DIFF(p1);

        if (memcmp(q->info, alge_zero, 6))
            q = ALGE(2, ALGE_DIV, q, ALGE_COPY(p1));

        break;

    case 3:
        // p1 <- u, q <- D(u)
        // info(q) != 0 ? q' <- -D(u) = -q
        p1 = tree_entry(p->tree.llink, ALGEBRA_SYMBOL, tree);
        q = DIFF(p1);

        if (memcmp(q->info, alge_zero, 6))
            // q = _tree_alloc(ALGE_NEG, &q->tree, 0);
            q = ALGE(1, ALGE_NEG, q);

        break;

    case 4:
        // q' <- D(u + v) = D(u) + D(v)
        // p1 <- u, p2 <-v, q1 <- D(u), q <- D(v)
        p1 = tree_entry(p->tree.llink, ALGEBRA_SYMBOL, tree);
        p2 = tree_entry(p->tree.llink->rlink, ALGEBRA_SYMBOL, tree);
        q1 = DIFF(p1);
        q  = DIFF(p2);

        // info(q1) = 0 ? q' <- 0 + D(v) = D(q)
        // info(q)  = 0 ? q' <- D(u) + 0 = D(q1)
        // q' <- D(u) + D(v) = D(q1) + D(q)
        if (memcmp(q1->info, alge_zero, 6) == 0) {
            free(q1);

        } else if (memcmp(q->info, alge_zero, 6) == 0) {
            free(q);
            q = q1;
        } else {
            // q = _tree_alloc(ALGE_ADD, &q1->tree, &q->tree);
            q = ALGE(2, ALGE_ADD, q1, q);
        }

        break;

    case 5:
        // q' <- D(u - v) = D(u) - D(v)
        // p1 <- u, p2 <-v, q1 <- D(u), q <- D(v)
        p1 = tree_entry(p->tree.llink, ALGEBRA_SYMBOL, tree);
        p2 = tree_entry(p->tree.llink->rlink, ALGEBRA_SYMBOL, tree);
        q1 = DIFF(p1);
        q  = DIFF(p2);

        // info(q)  = 0 ? q' <- D(u) - 0 = D(q1)
        // info(q1) = 0 ? q' <- 0 - D(v) = -D(q)
        // q' <- D(u) + D(v) = D(q1) - D(q)
        if (memcmp(q->info, alge_zero, 6) == 0) {
            free(q);
            q = q1;
        } else if (memcmp(q1->info, alge_zero, 6) == 0) {
            free(q1);
            // q = _tree_alloc(ALGE_NEG, &q->tree, 0);
            q = ALGE(1, ALGE_NEG, q);
        } else
            // q = _tree_alloc(ALGE_SUB, &q1->tree, &q->tree);
            q = ALGE(2, ALGE_SUB, q1, q);

        break;

    case 6:
        // q' <- D(u * v) = D(u) * v + u * D(v)
        // p1 <- u, p2 <-v, q1 <- D(u), q <- D(v)
        p1 = tree_entry(p->tree.llink, ALGEBRA_SYMBOL, tree);
        p2 = tree_entry(p->tree.llink->rlink, ALGEBRA_SYMBOL, tree);
        q1 = DIFF(p1);
        q  = DIFF(p2);

        // info(q1) = 0 ? q1' <- D(u) * v
        // info(q)  = 0 ? q' <- u * D(v)
        if (memcmp(q1->info, alge_zero, 6)) {
            // c1 = tree_entry(_tree_copy(0, &p2->tree, _tree_info_copy, _tree_alloc), ALGEBRA_SYMBOL, tree);
            c1 = ALGE_COPY(p2);
            q1 = multiple(q1, c1, _tree_alloc);
        }

        if (memcmp(q->info, alge_zero, 6)) {
            // c1 = tree_entry(_tree_copy(0, &p1->tree, _tree_info_copy, _tree_alloc), ALGEBRA_SYMBOL, tree);
            c1 = ALGE_COPY(p1);
            q = multiple(c1, q, _tree_alloc);
        }

        // info(q1) = 0 ? q' <- 0 + u * D(v)
        // info(q)  = 0 ? q' <- D(u) * v + 0
        // q' <- D(u) * v + u * D(v)
        if (memcmp(q1->info, alge_zero, 6) == 0) {
            free(q1);
        } else if (memcmp(q->info, alge_zero, 6) == 0) {
            free(q);
            q = q1;
        } else {
            // q = _tree_alloc(ALGE_ADD, &q1->tree, &q->tree);
            q = ALGE(2, ALGE_ADD, q1, q);
        }

        break;

    case 7:
        // q' <- D(u / v) = D(u) / v - (u * D(v)) / (v ^ 2)
        // p1 <- u, p2 <-v, q1 <- D(u), q <- D(v)
        p1 = tree_entry(p->tree.llink, ALGEBRA_SYMBOL, tree);
        p2 = tree_entry(p->tree.llink->rlink, ALGEBRA_SYMBOL, tree);
        q1 = DIFF(p1);
        q  = DIFF(p2);

        // info(q1) != 0 ? q1' <- D(u) / v = q1 / c1(p2)
        // info(q) != 0 ? <- (u * D(v)) / (v ^ 2)
        if (memcmp(q1->info, alge_zero, 6)) {
            q1 = ALGE(2, ALGE_DIV, q1, ALGE_COPY(p2));
        }

        if (memcmp(q->info, alge_zero, 6)) {
            q = MUL(ALGE_COPY(p1), q);
            c1 = ALGE(2, ALGE_POW, ALGE_COPY(p2), ALGE(0, ALGE_TWO));
            q = ALGE(2, ALGE_DIV, q, c1);
        }

        // info(q)  = 0 ? q' <- D(u) - 0 = D(q1)
        // info(q1) = 0 ? q' <- 0 - D(v) = -D(q)
        // q' <- D(u) + D(v) = D(q1) - D(q)
        if (memcmp(q->info, alge_zero, 6) == 0) {
            free(q);
            q = q1;
        } else if (memcmp(q1->info, alge_zero, 6) == 0) {
            free(q1);
            // q = _tree_alloc(ALGE_NEG, &q->tree, 0);
            q = ALGE(1, ALGE_NEG, q);
        } else
            // q = _tree_alloc(ALGE_SUB, &q1->tree, &q->tree);
            q = ALGE(2, ALGE_SUB, q1, q);

        break;

    case 8:
        // q' <- D(u ** v) = D(u) * (v * (u ** (v - 1))) + ((ln u) * D(v)) x (u ** v)
        // p1 <- u, p2 <-v, q1 <- D(u), q <- D(v)
        p1 = tree_entry(p->tree.llink, ALGEBRA_SYMBOL, tree);
        p2 = tree_entry(p->tree.llink->rlink, ALGEBRA_SYMBOL, tree);
        q1 = DIFF(p1);
        q  = DIFF(p2);

        // q1 = D(u) * c1 = D(u) * (v * (u ** (v - 1)))
        c1 = ALGE(2, ALGE_SUB, ALGE_COPY(p2), ALGE(0, ALGE_ONE));
        c1 = ALGE(2, ALGE_POW, ALGE_COPY(p1), c1);
        c1 = ALGE(2, ALGE_MUL, ALGE_COPY(p2), c1);
        q1 = ALGE(2, ALGE_MUL, q1, c1);

        // q = c2 * (u ** v) = (ln u * D(v)) * (u ** v)
        c2 = ALGE(1, ALGE_LN, ALGE_COPY(p1));
        c2 = ALGE(2, ALGE_MUL, c2, q);
        q = ALGE(2, ALGE_MUL, c2, ALGE(2, ALGE_POW, ALGE_COPY(p1), ALGE_COPY(p2)));

        // info(q1) = 0 ? q' <- 0 + D(v) = D(q)
        // info(q)  = 0 ? q' <- D(u) + 0 = D(q1)
        // q' <- D(u) + D(v) = D(q1) + D(q)
        if (memcmp(q1->info, alge_zero, 6) == 0) {
            free(q1);
        } else if (memcmp(q->info, alge_zero, 6) == 0) {
            free(q);
            q = q1;
        } else {
            // q = _tree_alloc(ALGE_ADD, &q1->tree, &q->tree);
            q = ALGE(2, ALGE_ADD, q1, q);
        }

        break;

    default:
        break;
    }

    // alge_print_symbol(&q->tree, &type);
    return q;
}

PALGEBRA_SYMBOL _differentiation(
    int type,
    PALGEBRA_SYMBOL p1,
    PALGEBRA_SYMBOL p2,
    PALGEBRA_SYMBOL q1,
    PALGEBRA_SYMBOL q,
    PTREE_INFO_COPY_FUNC _tree_info_copy,
    PTREE_ALLOC_FUNC _tree_alloc,
    PTREE_COPY_FUNC _tree_copy)
{
    PALGEBRA_SYMBOL c1, c2;
    // DIFF[TYPE(P)]
    // printf("DIFF[%d]\n", type);
    switch (type) {
    case 0:
        q = ALGE0(ALGE_ZERO);
        break;

    case 1:
        q = ALGE0(type == 1 ? ALGE_ONE : ALGE_ZERO);
        break;

    case 2:
        // p1 <- u, q <- D(u)
        // info(q) != 0 ? q' <- D(u) / u = q / copy(p1)
        if (memcmp(q->info, alge_zero, 6))
            q = ALGE(2, ALGE_DIV, q, ALGE_COPY(p1));

        break;

    case 3:
        // p1 <- u, q <- D(u)
        // info(q) != 0 ? q' <- -D(u) = -q
        if (memcmp(q->info, alge_zero, 6))
            q = ALGE(1, ALGE_NEG, q);

        break;

    case 4:
        // q' <- D(u + v) = D(u) + D(v)
        // q1 <- D(u), q <- D(v)
        // info(q1) = 0 ? q' <- 0 + D(v) = D(q)
        // info(q)  = 0 ? q' <- D(u) + 0 = D(q1)
        // q' <- D(u) + D(v) = D(q1) + D(q)
        if (memcmp(q1->info, alge_zero, 6) == 0) {
            free(q1);
        } else if (memcmp(q->info, alge_zero, 6) == 0) {
            free(q);
            q = q1;
        } else
            q = ALGE2(ALGE_ADD, q1, q);

        break;

    case 5:
        // q' <- D(u - v) = D(u) - D(v)
        // p1 <- u, p2 <-v, q1 <- D(u), q <- D(v)
        if (memcmp(q->info, alge_zero, 6) == 0) {
            free(q);
            q = q1;
        } else if (memcmp(q1->info, alge_zero, 6) == 0) {
            free(q1);
            q = ALGE(1, ALGE_NEG, q);
        } else
            q = ALGE(2, ALGE_SUB, q1, q);

        break;

    case 6:
        // q' <- D(u * v) = D(u) * v + u * D(v)
        // p1 <- u, p2 <-v, q1 <- D(u), q <- D(v)

        // info(q1) = 0 ? q1' <- D(u) * v
        // info(q)  = 0 ? q' <- u * D(v)
        if (memcmp(q1->info, alge_zero, 6)) {
            // c1 = tree_entry(_tree_copy(0, &p2->tree, _tree_info_copy, _tree_alloc), ALGEBRA_SYMBOL, tree);
            c1 = ALGE_COPY(p2);
            q1 = multiple(q1, c1, _tree_alloc);
            // Since q1 may be released in multiple(...), we set p1->rlink to null to avoid following copy fic
            p1->tree.rlink = NULL;
        }

        if (memcmp(q->info, alge_zero, 6)) {
            // c1 = tree_entry(_tree_copy(0, &p1->tree, _tree_info_copy, _tree_alloc), ALGEBRA_SYMBOL, tree);
            c1 = ALGE_COPY(p1);
            q = multiple(c1, q, _tree_alloc);
        }

        // info(q1) = 0 ? q' <- 0 + u * D(v)
        // info(q)  = 0 ? q' <- D(u) * v + 0
        // q' <- D(u) * v + u * D(v)
        if (memcmp(q1->info, alge_zero, 6) == 0) {
            free(q1);
        } else if (memcmp(q->info, alge_zero, 6) == 0) {
            free(q);
            q = q1;
        } else {
            q = ALGE(2, ALGE_ADD, q1, q);
        }

        break;

    case 7:
        // q' <- D(u / v) = D(u) / v - (u * D(v)) / (v ^ 2)
        // p1 <- u, p2 <-v, q1 <- D(u), q <- D(v)

        // info(q1) != 0 ? q1' <- D(u) / v = q1 / c1(p2)
        // info(q) != 0 ? <- (u * D(v)) / (v ^ 2)
        if (memcmp(q1->info, alge_zero, 6)) {
            q1 = ALGE(2, ALGE_DIV, q1, ALGE_COPY(p2));
        }

        if (memcmp(q->info, alge_zero, 6)) {
            q = MUL(ALGE_COPY(p1), q);
            c1 = ALGE(2, ALGE_POW, ALGE_COPY(p2), ALGE(0, ALGE_TWO));
            q = ALGE(2, ALGE_DIV, q, c1);
        }

        // info(q)  = 0 ? q' <- D(u) - 0 = D(q1)
        // info(q1) = 0 ? q' <- 0 - D(v) = -D(q)
        // q' <- D(u) + D(v) = D(q1) - D(q)
        if (memcmp(q->info, alge_zero, 6) == 0) {
            free(q);
            q = q1;
        } else if (memcmp(q1->info, alge_zero, 6) == 0) {
            free(q1);
            // q = _tree_alloc(ALGE_NEG, &q->tree, 0);
            q = ALGE(1, ALGE_NEG, q);
        } else
            // q = _tree_alloc(ALGE_SUB, &q1->tree, &q->tree);
            q = ALGE(2, ALGE_SUB, q1, q);

        break;

    case 8:
        // q' <- D(u ** v) = D(u) * (v * (u ** (v - 1))) + ((ln u) * D(v)) x (u ** v)
        // p1 <- u, p2 <-v, q1 <- D(u), q <- D(v)

        // q1 = D(u) * c1 = D(u) * (v * (u ** (v - 1)))
        c1 = ALGE(2, ALGE_SUB, ALGE_COPY(p2), ALGE(0, ALGE_ONE));
        c1 = ALGE(2, ALGE_POW, ALGE_COPY(p1), c1);
        c1 = ALGE(2, ALGE_MUL, ALGE_COPY(p2), c1);
        q1 = ALGE(2, ALGE_MUL, q1, c1);

        // q = c2 * (u ** v) = (ln u * D(v)) * (u ** v)
        c2 = ALGE(1, ALGE_LN, ALGE_COPY(p1));
        c2 = ALGE(2, ALGE_MUL, c2, q);
        q = ALGE(2, ALGE_MUL, c2, ALGE(2, ALGE_POW, ALGE_COPY(p1), ALGE_COPY(p2)));

        // info(q1) = 0 ? q' <- 0 + D(v) = D(q)
        // info(q)  = 0 ? q' <- D(u) + 0 = D(q1)
        // q' <- D(u) + D(v) = D(q1) + D(q)
        if (memcmp(q1->info, alge_zero, 6) == 0) {
            free(q1);
        } else if (memcmp(q->info, alge_zero, 6) == 0) {
            free(q);
            q = q1;
        } else {
            // q = _tree_alloc(ALGE_ADD, &q1->tree, &q->tree);
            q = ALGE(2, ALGE_ADD, q1, q);
        }

        break;
    }

    return q;
}

// Algorithm D
// Differentiation
PALGEBRA_SYMBOL differentiation(
    PALGEBRA_SYMBOL y,
    PTREE_INFO_COPY_FUNC _tree_info_copy,
    PTREE_ALLOC_FUNC _tree_alloc,
    PTREE_COPY_FUNC _tree_copy)
{
    PALGEBRA_SYMBOL dy, p, p1, p2, q1, q;

    if (y == NULL)
        return NULL;

    dy = ALGE0(ALGE_VARY);

    while (1) {
    D1:
        // Find Y$
        // D1. [Initialize.]
        p = tree_entry(y->tree.llink, ALGEBRA_SYMBOL, tree);
    H2:
        while (p->tree.llink != NULL)
            p = tree_entry(p->tree.llink, ALGEBRA_SYMBOL, tree);
    D2:
        // D2. [Differentiate.]
        q = _differentiation(p->type, p1, p2, q1, q, _tree_info_copy, _tree_alloc, _tree_copy);
    D3:
        // D3. [Restore link]
        if (p->type >= 4)
            p1->tree.rlink = &p2->tree;
    D4:
        // D4. [Advance to P$]
        p2 = p;
        p = tree_entry(p->tree.rlink, ALGEBRA_SYMBOL, tree);
        if (p2->tree.rtag == 0) {
            p2->tree.rlink = &q->tree;
            goto H2;
        }
    D5:
        // D5. [Done ?]
        // Prepare for step D2 (binary operators)
        if (p->tree.llink != NULL)
            p1 = tree_entry(p->tree.llink, ALGEBRA_SYMBOL, tree);

        if (p1->tree.rlink != NULL)
            q1 = tree_entry(p1->tree.rlink, ALGEBRA_SYMBOL, tree);

        if (p != y)
            goto D2;
        else {
            dy->tree.llink = &q->tree;
            q->tree.rlink = &dy->tree;
            q->tree.rtag = 1;
            break;
        }
    }

    return dy;
}

void alge_print_symbol(void *q, void *p)
{
    ALGEBRA_SYMBOL *s;
    int type;

    if (q != NULL) {
        s = tree_entry(q, ALGEBRA_SYMBOL, tree);
        type = s->type;
    } else
        return;

    if (p != NULL)
        type = *(int *)p;

    switch (type) {
    case 0:
    case 9:
        if (s->info[0] >= 'a' && s->info[0] <= 'z')
            printf("%c ", s->info[0]);
        else
            printf("%d ", s->info[0]);

        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        printf("%s ", s->info);
        break;
    default:
        printf("\nType: %2d\nInfo: \n", s->type);
        for (int i = 0; i < 6; i++) {
            printf("%02X ", s->info[i]);
        }
        printf("\n");
        for (int i = 0; i < 6; i++) {
            printf("%2c ", s->info[i]);
        }
        printf("\n");
    }
}

void tree_differentiation_test(void)
{
    int type = -1;
    PTREE_INFO_COPY_FUNC _tree_info_copy = alge_tree_info_copy;
    PTREE_ALLOC_FUNC _tree_alloc = alge_tree_alloc2;
    PTREE_COPY_FUNC _tree_copy = rttree_copy;
    ALGEBRA_SYMBOL *x;
    ALGEBRA_SYMBOL *a;
    ALGEBRA_SYMBOL *op;
    ALGEBRA_SYMBOL *y;
    ALGEBRA_SYMBOL *p;
    ALGEBRA_SYMBOL *dy;

    // y = x + 1
    x  = ALGE(0, ALGE_VARX);
    a  = ALGE(0, ALGE_ONE);
    op = ALGE(2, ALGE_ADD, x, a);
    y  = ALGE(1, ALGE_VARX, op);

    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    // alge_print_symbol(&x->tree, &type);
    // alge_print_symbol(&a->tree, &type);
    dy = differentiation(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse_test(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);

    // y = 1 - x
    x  = ALGE(0, ALGE_VARX);
    a  = ALGE(0, ALGE_ONE);
    op = ALGE(2, ALGE_SUB, a, x);
    y  = ALGE(1, ALGE_VARX, op);

    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    dy = differentiation(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse_test(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);

    // y = ln x
    x  = ALGE(0, ALGE_VARX);
    a  = ALGE(0, ALGE_ONE);
    op = ALGE(1, ALGE_LN, x);
    y  = ALGE(1, ALGE_VARY, op);

    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    dy = differentiation(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse_test(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);

    // y = x * x
    x  = ALGE(0, ALGE_VARX);
    a  = ALGE(0, ALGE_VARX);
    op = ALGE(2, ALGE_MUL, x, a);
    y  = ALGE(1, ALGE_VARY, op);

    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    dy = differentiation(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse_test(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);

    // y = x / x
    x  = ALGE(0, ALGE_VARX);
    a  = ALGE(0, ALGE_VARX);
    op = ALGE(2, ALGE_DIV, x, a);
    y  = ALGE(1, ALGE_VARX, op);

    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    dy = differentiation(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse_test(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);

    // y = ln x / x
    x  = ALGE(1, ALGE_LN, ALGE(0, ALGE_VARX));
    a  = ALGE(0, ALGE_VARX);
    op = ALGE(2, ALGE_DIV, x, a);
    y  = ALGE(1, ALGE_VARY, op);

    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    dy = differentiation(y, _tree_info_copy, _tree_alloc, _tree_copy);
    // rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    rttree_preorder_traverse_test(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);

    // y = x ** x
    x  = ALGE(0, ALGE_VARX);
    a  = ALGE(0, ALGE_VARX);
    op = ALGE(2, ALGE_POW, x, a);
    y  = ALGE(1, ALGE_VARX, op);

    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    dy = differentiation(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse_test(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);

    // y = 3 ln (x + 1) - a / x^2
    x  = ALGE(2, ALGE_MUL, ALGE(0, ALGE_THREE), ALGE(1, ALGE_LN, ALGE(2, ALGE_ADD, ALGE(0, ALGE_VARX), ALGE(0, ALGE_ONE))));
    a  = ALGE(2, ALGE_DIV, ALGE(0, ALGE_A), ALGE(2, ALGE_POW, ALGE(0, ALGE_VARX), ALGE(0, ALGE_TWO)));
    op = ALGE(2, ALGE_SUB, x, a);
    y  = ALGE(1, ALGE_VARX, op);

    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    dy = differentiation(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    rttree_preorder_traverse_test(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);
}

// Test recursive version
void tree_differentiation_r_test(void)
{
    int type = -1;
    PTREE_INFO_COPY_FUNC _tree_info_copy = alge_tree_info_copy;
    PTREE_ALLOC_FUNC _tree_alloc = alge_tree_alloc2;
    PTREE_COPY_FUNC _tree_copy = rttree_copy;
    ALGEBRA_SYMBOL *x;
    ALGEBRA_SYMBOL *a;
    ALGEBRA_SYMBOL *op;
    ALGEBRA_SYMBOL *y;
    ALGEBRA_SYMBOL *p;
    ALGEBRA_SYMBOL *dy;

    // y = x + 1
    x  = ALGE(0, ALGE_VARX);
    a  = ALGE(0, ALGE_ONE);
    op = ALGE(2, ALGE_ADD, x, a);
    y  = ALGE(1, ALGE_VARX, op);

    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    // alge_print_symbol(&x->tree, &type);
    // alge_print_symbol(&a->tree, &type);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse_test(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);

    // y = x + 1
    x  = ALGE(0, ALGE_VARX);
    a  = ALGE(0, ALGE_ONE);
    op = ALGE(2, ALGE_ADD, x, a);
    y  = ALGE(1, ALGE_VARX, op);

    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse_test(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);

    // y = 1 - x
    x  = ALGE(0, ALGE_VARX);
    a  = ALGE(0, ALGE_ONE);
    op = ALGE(2, ALGE_SUB, a, x);
    y  = ALGE(1, ALGE_VARX, op);

    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse_test(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);

    // y = ln x
    x  = ALGE(0, ALGE_VARY);
    a  = ALGE(0, ALGE_ONE);
    op = ALGE(1, ALGE_LN, x);
    y  = ALGE(1, ALGE_VARX, op);

    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse_test(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);

    // y = x * x
    x  = ALGE(0, ALGE_VARX);
    a  = ALGE(0, ALGE_VARX);
    op = ALGE(2, ALGE_MUL, x, a);
    y  = ALGE(1, ALGE_VARX, op);

    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse_test(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);

    // y = x * x
    x  = ALGE(0, ALGE_VARX);
    a  = ALGE(0, ALGE_VARX);
    op = ALGE(2, ALGE_DIV, x, a);
    y  = ALGE(1, ALGE_VARX, op);

    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse_test(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);

    // y = ln x / x
    x  = ALGE(1, ALGE_LN, ALGE(0, ALGE_VARX));
    a  = ALGE(0, ALGE_VARX);
    op = ALGE(2, ALGE_DIV, x, a);
    y  = ALGE(1, ALGE_VARX, op);

    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse_test(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);

    // y = x ** x
    x  = ALGE(0, ALGE_VARX);
    a  = ALGE(0, ALGE_VARX);
    op = ALGE(2, ALGE_POW, x, a);
    y  = ALGE(1, ALGE_VARX, op);

    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse_test(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);

    // y = 3 ln (x + 1) - a / x^2
    x  = ALGE(2, ALGE_MUL, ALGE(0, ALGE_THREE), ALGE(1, ALGE_LN, ALGE(2, ALGE_ADD, ALGE(0, ALGE_VARX), ALGE(0, ALGE_ONE))));
    a  = ALGE(2, ALGE_DIV, ALGE(0, ALGE_A), ALGE(2, ALGE_POW, ALGE(0, ALGE_VARX), ALGE(0, ALGE_TWO)));
    op = ALGE(2, ALGE_SUB, x, a);
    y  = ALGE(1, ALGE_VARX, op);

    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse_test(&y->tree, alge_print_symbol);
    rttree_preorder_traverse_test(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);
}
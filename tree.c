#include "tree.h"

// 2.3.1. Algorithm T, exercise 12. [22]
void tree_preorder_traverse_test(struct _tree *t)
{
    // // T1. [Initialize]
    // int lsp = 0;
    // struct _tree *p = t;

    // printf("\npreorder traverse start >>>\n");

    // while (1) {
    // T2:
    //     // T2. [p = EMPTY?]
    //     if (p == NULL) {
    //         goto T4;
    //     }

    // T3:
    //     // T3. [Visit p and stack p]
    //     printf("%c ", tree_entry(p, struct _too, tree)->a);
    //     _lstack[lsp++]= p;
    //     p = p->llink;
    //     goto T2;

    // T4:
    //     // T4. [p <= STACK]
    //     if (lsp == 0) {
    //         break;
    //     }
    //     p = _lstack[--lsp];
    //     p = p->rlink;
    //     goto T2;
    // }

    // printf("\npreorder traverse end <<<\n");
}

// 2.3.1. Algorithm T
void tree_inorder_traverse_test(struct _tree *t)
{
    // // T1. [Initialize] Set stack A empty, and set the link variable P <- T.
    // int lsp = 0;
    // struct _tree *p = t;

    // printf("\ninorder traverse start >>>\n");

    // while (1) {
    // T2:
    //     // T2. [P = EMPTY?] If P is empty, go to step T4.
    //     if (p == NULL)
    //         goto T4;

    //     // T3. [STACK <= P] Set A <= P. Then set P <- LLINK(P) and return to step T2.
    //     _lstack[lsp++] = p;

    //     // P <- LLINK(P)
    //     p = p->llink;
    //     goto T2;

    // T4:
    //     // T4. [P <= STACK] If stack A is empty, the algorithm terminates; otherwise set P <= A.
    //     if (lsp == 0)
    //         break;

    //     p = _lstack[--lsp];

    //     // T5. [Visit P] Visit NODE(P). Then set P <- RLINK(P) and return to step T2
    //     printf("%c ", tree_entry(p, struct _too, tree)->a);

    //     p = p->rlink;
    //     goto T2;
    // }

    // printf("\ninorder traverse end <<<\n");
}

void tree_level_order_traverse_test(struct _tree *t)
{
    // int qf = 0, qr = 0, sz;
    // struct _tree *p = t;

    // printf("\nlevel-order traverse start >>>\n");

    // _queue[qr] = t;
    // qr = (qr + 1) % STACK_SIZE;

    // while (qr != qf) {
    //     sz = qr > qf ? qr - qf : qf + (STACK_SIZE - qr);
    //     while (sz--) {
    //         p = _queue[qf];
    //         qf = (qf + 1) % STACK_SIZE;
    //         printf("%c ", tree_entry(p, struct _too, tree)->a);

    //         if (p->llink != NULL)
    //             _queue[qr++] = p->llink;

    //         if (p->rlink != NULL)
    //             _queue[qr++] = p->rlink;

    //         qr = qr % STACK_SIZE;
    //     }
    // }

    // printf("\nlevel-order traverse end <<<\n");
}

// 2.3.1. Algorithm Sm, exercise 17.[22]
ttree_t *ttree_preorder_successor(ttree_t **q, ttree_t *p)
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
ttree_t *ttree_preorder_predecessor(ttree_t **q, ttree_t *p)
{
    ttree_t *s = NULL;

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
ttree_t *ttree_inorder_successor(ttree_t **q, ttree_t *p)
{
    ttree_t *s = NULL;

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

ttree_t *ttree_inorder_predecessor(ttree_t **q, ttree_t *p)
{
    ttree_t *s = NULL;

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
ttree_t *ttree_postorder_successor(ttree_t **q, ttree_t *p)
{
    ttree_t *s = NULL;

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

ttree_t *ttree_postorder_predecessor(ttree_t **q, ttree_t *p)
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

void ttree_add_l(ttree_t *q, ttree_t *p)
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
void ttree_add_r(ttree_t *q, ttree_t *p)
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
void *ttree_copy(void *t2, void *t1,
        tree_info_copy_func_t _tree_info_copy,
        tree_alloc_func_t _tree_alloc)
{
    ttree_t *p, *q;
    // C1. [Initiailize]
    if (t2 == NULL) {
        t2 = malloc(sizeof(ttree_t));
        INIT_TTREE_HEAD(t2);
    }
    q = (ttree_t *)t2;
    p = (ttree_t *)t1;

    printf("\nttree_copy start >>>\n");

    while (1) {
        // C4. [Anything to left?]
        if (p->ltag == 0) {
            // struct _tto *r = malloc(sizeof(struct _tto));
            // INIT_TTREE_NODE(&r->ttree);
            // ttree_add_l(&r->ttree, q);
            ttree_t *r = _tree_alloc(0, 0, 0);
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
            ttree_t *r = _tree_alloc(0, 0, 0);
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
    // ttree_t *p, *q;
    // // C1. [Initiailize]
    // q = (ttree_t *)t2;
    // p = (ttree_t *)t1;

    // while (1) {
    //     // C4. [Anything to left?]
    //     if (p->ltag == 0) {
    //         struct _tto *r = malloc(sizeof(struct _tto));
    //         INIT_TTREE_NODE(&r->ttree);
    //         ttree_add_l(&r->ttree, q);
    //     }

    //     // C5. [Advance.]
    //     p = ttree_preorder_successor(0, p);
    //     q = ttree_preorder_successor(0, q);

    //     // C6. [Test if complete.]
    //     // if (q == t2->rlink)
    //     if (p == NULL)
    //         break;

    //     // C2. [Anything to right?]
    //     if (p->rtag == 0) {
    //         struct _tto *r = malloc(sizeof(struct _tto));
    //         INIT_TTREE_NODE(&r->ttree);
    //         ttree_add_r(&r->ttree, q);
    //     }

    //     // C3. [Copy INFO.]
    //     tree_entry(q, struct _tto, ttree)->a = tree_entry(p, struct _tto, ttree)->a;
    // }
}

void ttree_preorder_traverse_test(ttree_t *t)
{
    // ttree_t *p = t;

    // printf("\nthreaded tree preorder traverse start >>>\n");

    // while (1) {
    //     p = ttree_preorder_successor(0, p);
    //     // if (p == t)
    //     if ((p == t) || p == NULL)
    //         break;
    //     else
    //         printf("%c ", tree_entry(p, struct _tto, ttree)->a);
    // }
    // printf("\nthreaded tree preorder traverse end <<<\n");
}

#if 1
void ttree_inorder_traverse_test(ttree_t *t)
{
    // // S0. [Initialize.] Set P <- HEAD (Q <- HEAD, also)
    // ttree_t *p = t, *q;

    // printf("\nthreaded tree inorder traverse start >>>\n");

    // while (1) {
    // S2:
    //     // S2. [Search to left.] Set P <- Q, Q <- LLINK(Q)
    //     q = p->llink;
    //     if (p->ltag == 0) {
    //         p = q;
    //         goto S2;
    //     }

    // S3:
    //     // S3. [Visit P.] Visit unless P = HEAD
    //     // if (p == t) {
    //     if (p == t || p == NULL) {
    //         break;
    //     }

    //     printf("%c ", tree_entry(p, struct _tto, ttree)->a);

    // S1:
    //     // S1. [RLINK(P) a thread?]
    //     q = p->rlink;
    //     if (p->rtag) {
    //         p = q;
    //         goto S3;
    //     } else {
    //         p = q;
    //     }
    // }

    // printf("\nthreaded tree inorder traverse end <<<\n");
}
#elif 0
void ttree_inorder_traverse_test(ttree_t *t)
{
    // S0. [Initialize.] Set P <- HEAD (Q <- HEAD, also)
    ttree_t *p = t, *q = t;

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
void ttree_inorder_traverse_test(ttree_t *t)
{
    // S0. [Initialize.] Set P <- HEAD (Q <- HEAD, also)
    ttree_t *p = t;

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

// 2.3.1. Algorithm T, exercise 13. [24]
void tree_postorder_traverse_test(struct _tree *t)
{
    // // T1. [Initialize]
    // int lsp = 0, rsp = 0;
    // struct _tree *p = t;

    // printf("\npostorder traverse start >>>\n");

    // while (1) {
    // T2:
    //     // T2. [p = EMPTY?]
    //     if (p == NULL) {
    //         goto T4;
    //     }

    // T3:
    //     // T3. [STACK <= P]
    //     _lstack[lsp++] = p;
    //     p = p->llink;
    //     goto T2;

    // T4:
    //     // T4. [p <= STACK]
    //     if (lsp == 0) {
    //         break;
    //     }
    //     p = _lstack[--lsp];

    //     // T5. [RLINK(P) = EMPTY?]
    //     if (p->rlink == NULL) {
    //     T6:
    //         // T6. [Visit P]
    //         printf("%c ", tree_entry(p, struct _too, tree)->a);

    //     T8:
    //         // T8. [RSTACK = EMPTY?]
    //         if (rsp == 0) {
    //             goto T4;
    //         } else {
    //             // T9.
    //             if (_rstack[rsp - 1]->rlink == p) {
    //                 // T10.
    //                 p = _rstack[--rsp];
    //                 printf("%c ", tree_entry(p, struct _too, tree)->a);
    //                 goto T8;
    //             } else {
    //                 goto T4;
    //             }
    //         }
    //     } else {
    //         // T7. [STACK <= P, P <- RLINK(P)]
    //         _rstack[rsp++] = p;
    //         p = p->rlink;
    //         goto T2;
    //     }
    // }

    // printf("\npostorder traverse end <<<\n");
}

rttree_t *rttree_preorder_successor(
    rttree_t **q,
    rttree_t *p)
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

rttree_t *rttree_preorder_predecessor(
    rttree_t **q,
    rttree_t *p)
{
    // CAN NOT GUARENTEE TO FIND A PREDECESSOR IN ALL CASES
    return NULL;
}

rttree_t *rttree_inorder_successor(
    rttree_t **q,
    rttree_t *p)
{
    rttree_t *s = NULL;

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

rttree_t *rttree_inodrer_predecessor(
    rttree_t **q,
    rttree_t *p)
{
    rttree_t *s = NULL;

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

rttree_t *rttree_postorder_successor(
    rttree_t **q,
    rttree_t *p)
{
    // CAN NOT GUARENTEE TO FIND A PREDECESSOR IN ALL CASES
    return NULL;
}

rttree_t *rttree_postorder_predecessor(
    rttree_t **q,
    rttree_t *p)
{
    // CAN NOT GUARENTEE TO FIND A PREDECESSOR IN ALL CASES
    return NULL;
}

// Algorithm Im, 2.3.1. exercises 23. [22]
void rttree_add_l(rttree_t *q, rttree_t *p)
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
void rttree_add_r(rttree_t *q, rttree_t *p)
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

void *rttree_copy(
        void *t2,
        void *t1,
        tree_info_copy_func_t _tree_info_copy,
        tree_alloc_func_t _tree_alloc)
{
    rttree_t *p, *q;
    // C1. [Initiailize]
#if 1
    rttree_t *q1 = malloc(sizeof(rttree_t));
    rttree_t *p1 = malloc(sizeof(rttree_t));
    INIT_RTTREE_HEAD(q1);
    INIT_RTTREE_HEAD(p1);
    q = q1;
    p = p1;
    p->llink = (rttree_t *)t1;
#else
    // if (t2 == NULL) {
    //     t2 = _tree_alloc(0, 0, 0);
    //     // INIT_TTREE_HEAD(t2);
    // }
    if (t2 == NULL) {
        t2 = malloc(sizeof(ttree_t));
        INIT_TTREE_NODE(t2);
    }

    q = t2;
    p = t1;
#endif

    // rttree_preorder_traverse(p,  alge_print_symbol);

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
            rttree_t *r = _tree_alloc(0, 0, 0);
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
        // if (q == ((rttree_t *)t2)->rlink)
        if (p == NULL || q == NULL)
        // if (p == NULL)
            break;

        // C2. [Anything to right?]
        if (p->rtag == 0) {
            rttree_t *r = _tree_alloc(0, 0, 0);
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
    q = ((rttree_t *)t2)->llink;
    return q;
    // return t2;
#endif
}

void rttree_preorder_traverse(
    rttree_t *t,
    tree_node_oper_func_t _operation)
{
    rttree_t *p = t;

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

void rttree_inorder_traverse(
    rttree_t *t,
    tree_node_oper_func_t _operation)
{
    // S0. [Initialize.] Set P <- HEAD (Q <- HEAD, also)
    rttree_t *p = t, *q;

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

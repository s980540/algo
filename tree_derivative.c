#include "tree_derivateve.h"


typedef struct _algebra_symbol_t
{
    char info[6];
    u8 type;
    rttree_t tree;
} algebra_symbol_t;

#define ALGE_SYMBOL_NUM     (13)
#define ALGE_SYMBOL_SIZE    (sizeof(struct _algebra) * ALGE_SYMBOL_NUM)

static const struct _algebra_symbol_t alge_symbols[] = {
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
#define ALGE_COPY(s)                tree_entry(_tree_copy(0, &(s->tree), _tree_info_copy, _tree_alloc), algebra_symbol_t, tree)
#define ALGES(x)                    tree_entry(x, algebra_symbol_t, tree)
#define ALGE0(x)                    tree_entry(_tree_alloc(x, 0, 0), algebra_symbol_t, tree)
#define ALGE1(x, u)                 tree_entry(_tree_alloc(x, &(u)->tree, 0), algebra_symbol_t, tree)
#define ALGE2(x, u, v)              tree_entry(_tree_alloc(x, &(u)->tree, &(v)->tree), algebra_symbol_t, tree)
#define ALGE(N, ...)                ALGE##N(__VA_ARGS__)


void alge_tree_info_copy(void *q, const void *p)
{
    algebra_symbol_t * q1 = tree_entry(q, algebra_symbol_t, tree);
    algebra_symbol_t * p1 = tree_entry(p, algebra_symbol_t, tree);
    memcpy(q1->info, p1->info, 6);
    q1->type = p1->type;
}

void *alge_tree_alloc2(const void *x, void *u, void *v)
{
    algebra_symbol_t * w = malloc(sizeof(algebra_symbol_t));
    if (x != NULL)
        memcpy(w, x, sizeof(algebra_symbol_t));
    else
        INIT_RTTREE_NODE(&w->tree);

    if (u != NULL && v != NULL) {
        w->tree.llink = u;
        ((rttree_t *)u)->rlink = v;
        ((rttree_t *)u)->rtag = 0;
        ((rttree_t *)v)->rlink = &w->tree;
        ((rttree_t *)v)->rtag = 1;
    } else if (u != NULL) {
        w->tree.llink = u;
        ((rttree_t *)u)->rlink = &w->tree;
        ((rttree_t *)u)->rtag = 1;
    }
    return &w->tree;
}

void *alge_tree_alloc1(const void *x, void *u, void *v)
{
    algebra_symbol_t * w = malloc(sizeof(algebra_symbol_t));
    if (x != NULL)
        memcpy(w, x, sizeof(algebra_symbol_t));
    else
        INIT_RTTREE_NODE(&w->tree);

    if (u != NULL) {
        w->tree.llink = u;
        ((rttree_t *)u)->rlink = &w->tree;
        ((rttree_t *)u)->rtag = 1;
    }
    return &w->tree;
}

void *alge_tree_alloc(const void *x, void *u, void *v)
{
    algebra_symbol_t * w = malloc(sizeof(algebra_symbol_t));
    if (x != NULL)
        memcpy(w, x, sizeof(algebra_symbol_t));
    else {
        INIT_RTTREE_NODE(&w->tree);
        w->type = 0;
        memset(w->info, 0, 6);
    }
    return &w->tree;
}

char alge_zero[6] = {0, 0, 0, 0, 0, 0};
char alge_one[6] = {1, 0, 0, 0, 0, 0};

algebra_symbol_t *multiple(
    algebra_symbol_t *u,
    algebra_symbol_t *v,
    tree_alloc_func_t _tree_alloc)
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

algebra_symbol_t *_differentiation_r(
    algebra_symbol_t *p,
    tree_info_copy_func_t _tree_info_copy,
    tree_alloc_func_t _tree_alloc,
    tree_copy_func_t _tree_copy)
{
    // p1 <- u, p2 <-v, q1 <- D(u), q <- D(v)
    algebra_symbol_t *q, *q1, *p1, *p2, *c1, *c2;
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
        p1 = tree_entry(p->tree.llink, algebra_symbol_t, tree);
        q  = DIFF(p1);

        if (memcmp(q->info, alge_zero, 6))
            q = ALGE(2, ALGE_DIV, q, ALGE_COPY(p1));

        break;

    case 3:
        // p1 <- u, q <- D(u)
        // info(q) != 0 ? q' <- -D(u) = -q
        p1 = tree_entry(p->tree.llink, algebra_symbol_t, tree);
        q = DIFF(p1);

        if (memcmp(q->info, alge_zero, 6))
            // q = _tree_alloc(ALGE_NEG, &q->tree, 0);
            q = ALGE(1, ALGE_NEG, q);

        break;

    case 4:
        // q' <- D(u + v) = D(u) + D(v)
        // p1 <- u, p2 <-v, q1 <- D(u), q <- D(v)
        p1 = tree_entry(p->tree.llink, algebra_symbol_t, tree);
        p2 = tree_entry(p->tree.llink->rlink, algebra_symbol_t, tree);
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
        p1 = tree_entry(p->tree.llink, algebra_symbol_t, tree);
        p2 = tree_entry(p->tree.llink->rlink, algebra_symbol_t, tree);
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
        p1 = tree_entry(p->tree.llink, algebra_symbol_t, tree);
        p2 = tree_entry(p->tree.llink->rlink, algebra_symbol_t, tree);
        q1 = DIFF(p1);
        q  = DIFF(p2);

        // info(q1) = 0 ? q1' <- D(u) * v
        // info(q)  = 0 ? q' <- u * D(v)
        if (memcmp(q1->info, alge_zero, 6)) {
            // c1 = tree_entry(_tree_copy(0, &p2->tree, _tree_info_copy, _tree_alloc), algebra_symbol_t, tree);
            c1 = ALGE_COPY(p2);
            q1 = multiple(q1, c1, _tree_alloc);
        }

        if (memcmp(q->info, alge_zero, 6)) {
            // c1 = tree_entry(_tree_copy(0, &p1->tree, _tree_info_copy, _tree_alloc), algebra_symbol_t, tree);
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
        p1 = tree_entry(p->tree.llink, algebra_symbol_t, tree);
        p2 = tree_entry(p->tree.llink->rlink, algebra_symbol_t, tree);
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
        p1 = tree_entry(p->tree.llink, algebra_symbol_t, tree);
        p2 = tree_entry(p->tree.llink->rlink, algebra_symbol_t, tree);
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

// Algorithm D
// Differentiation
algebra_symbol_t *differentiation_r(
    algebra_symbol_t *y,
    tree_info_copy_func_t _tree_info_copy,
    tree_alloc_func_t _tree_alloc,
    tree_copy_func_t _tree_copy)
{
    algebra_symbol_t *dy, *p, *p2;

    if (y == NULL)
        return NULL;

    // D1. [Initialize.]
    p  = ALGE(S, rttree_preorder_successor(0, &y->tree));

    // D2. [Differentiate.]
    dy = ALGE(1, ALGE_VARX, _differentiation_r(p, _tree_info_copy, _tree_alloc, _tree_copy));

    return dy;
}

algebra_symbol_t *_differentiation(
    int type,
    algebra_symbol_t *p1,
    algebra_symbol_t *p2,
    algebra_symbol_t *q1,
    algebra_symbol_t *q,
    tree_info_copy_func_t _tree_info_copy,
    tree_alloc_func_t _tree_alloc,
    tree_copy_func_t _tree_copy)
{
    algebra_symbol_t *c1, *c2;
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
            // c1 = tree_entry(_tree_copy(0, &p2->tree, _tree_info_copy, _tree_alloc), algebra_symbol_t, tree);
            c1 = ALGE_COPY(p2);
            q1 = multiple(q1, c1, _tree_alloc);
            // Since q1 may be released in multiple(...), we set p1->rlink to null to avoid following copy fic
            p1->tree.rlink = NULL;
        }

        if (memcmp(q->info, alge_zero, 6)) {
            // c1 = tree_entry(_tree_copy(0, &p1->tree, _tree_info_copy, _tree_alloc), algebra_symbol_t, tree);
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
algebra_symbol_t *differentiation(
    algebra_symbol_t *y,
    tree_info_copy_func_t _tree_info_copy,
    tree_alloc_func_t _tree_alloc,
    tree_copy_func_t _tree_copy)
{
    algebra_symbol_t *dy, *p, *p1, *p2, *q1, *q;

    if (y == NULL)
        return NULL;

    dy = ALGE0(ALGE_VARY);

    while (1) {
    D1:
        // Find Y$
        // D1. [Initialize.]
        p = tree_entry(y->tree.llink, algebra_symbol_t, tree);
    H2:
        while (p->tree.llink != NULL)
            p = tree_entry(p->tree.llink, algebra_symbol_t, tree);
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
        p = tree_entry(p->tree.rlink, algebra_symbol_t, tree);
        if (p2->tree.rtag == 0) {
            p2->tree.rlink = &q->tree;
            goto H2;
        }
    D5:
        // D5. [Done ?]
        // Prepare for step D2 (binary operators)
        if (p->tree.llink != NULL)
            p1 = tree_entry(p->tree.llink, algebra_symbol_t, tree);

        if (p1->tree.rlink != NULL)
            q1 = tree_entry(p1->tree.rlink, algebra_symbol_t, tree);

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
    algebra_symbol_t *s;
    int type;

    if (q != NULL) {
        s = tree_entry(q, algebra_symbol_t, tree);
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

void tree_differentiation_demo(void)
{
    int type = -1;
    tree_info_copy_func_t _tree_info_copy = alge_tree_info_copy;
    tree_alloc_func_t _tree_alloc = alge_tree_alloc2;
    tree_copy_func_t _tree_copy = rttree_copy;
    algebra_symbol_t *x;
    algebra_symbol_t *a;
    algebra_symbol_t *op;
    algebra_symbol_t *y;
    algebra_symbol_t *p;
    algebra_symbol_t *dy;

    // y = x + 1
    x  = ALGE(0, ALGE_VARX);
    a  = ALGE(0, ALGE_ONE);
    op = ALGE(2, ALGE_ADD, x, a);
    y  = ALGE(1, ALGE_VARX, op);

    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    // alge_print_symbol(&x->tree, &type);
    // alge_print_symbol(&a->tree, &type);
    dy = differentiation(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse(&dy->tree, alge_print_symbol);

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

    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    dy = differentiation(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse(&dy->tree, alge_print_symbol);

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

    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    dy = differentiation(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse(&dy->tree, alge_print_symbol);

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

    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    dy = differentiation(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse(&dy->tree, alge_print_symbol);

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

    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    dy = differentiation(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse(&dy->tree, alge_print_symbol);

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

    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    dy = differentiation(y, _tree_info_copy, _tree_alloc, _tree_copy);
    // rttree_preorder_traverse(&y->tree, alge_print_symbol);
    rttree_preorder_traverse(&dy->tree, alge_print_symbol);

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

    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    dy = differentiation(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse(&dy->tree, alge_print_symbol);

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

    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    dy = differentiation(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    rttree_preorder_traverse(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);
}

// Test recursive version
void tree_differentiation_r_demo(void)
{
    int type = -1;
    tree_info_copy_func_t _tree_info_copy = alge_tree_info_copy;
    tree_alloc_func_t _tree_alloc = alge_tree_alloc2;
    tree_copy_func_t _tree_copy = rttree_copy;
    algebra_symbol_t *x;
    algebra_symbol_t *a;
    algebra_symbol_t *op;
    algebra_symbol_t *y;
    algebra_symbol_t *p;
    algebra_symbol_t *dy;

    // y = x + 1
    x  = ALGE(0, ALGE_VARX);
    a  = ALGE(0, ALGE_ONE);
    op = ALGE(2, ALGE_ADD, x, a);
    y  = ALGE(1, ALGE_VARX, op);

    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    // alge_print_symbol(&x->tree, &type);
    // alge_print_symbol(&a->tree, &type);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse(&dy->tree, alge_print_symbol);

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

    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse(&dy->tree, alge_print_symbol);

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

    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse(&dy->tree, alge_print_symbol);

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

    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse(&dy->tree, alge_print_symbol);

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

    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse(&dy->tree, alge_print_symbol);

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

    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse(&dy->tree, alge_print_symbol);

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

    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse(&dy->tree, alge_print_symbol);

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

    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse(&dy->tree, alge_print_symbol);

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

    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    dy = differentiation_r(y, _tree_info_copy, _tree_alloc, _tree_copy);
    rttree_preorder_traverse(&y->tree, alge_print_symbol);
    rttree_preorder_traverse(&dy->tree, alge_print_symbol);

    free(x);
    free(a);
    free(op);
    free(y);
    free(dy);
}
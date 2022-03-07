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

#define ELE_NUM (9)
#define STACK_SIZE (256)

static struct tree *_stack[STACK_SIZE];

void tree_inorder_traverse_test(struct tree *t)
{
    // T1. [Initialize] Set stack A empty, and set the link variable P <- T.
    int sp = 0;
    struct tree *p = t;


    printf("inorder traverse start >>>\n");

    while (1) {
    T2:
        // T2. [P = EMPTY?] If P is empty, go to step T4.
        if (p == NULL) {
            // printf("p = NULL\n");
            goto T4;
        }

        // T3. [STACK <= P] Set A <= P. Then set P <- LLINK(P) and return to step T2.
        _stack[sp++] = p;

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
        if (sp == 0) {
            // debug
            // printf("STACK IS EMPTY\n");
            break;
        }

        // debug
        // printf("sp: %d\n", sp);
        // printf("_stack(%d): %c\n", sp - 1,
        //     tree_entry(_stack[sp - 1], struct _too, tree)->a);

        p = _stack[--sp];

        // debug
        // printf("RLINK(%c): %c\n", 
        //     tree_entry(p, struct _too, tree)->a,
        //     ((p->rlink == NULL) ? 'n' : (tree_entry(p->rlink, struct _too, tree)->a)));

        // T5. [Visit P] Visit NODE(P). Then set P <- RLINK(P) and return to step T2
        printf("%c ", tree_entry(p, struct _too, tree)->a);

        p = p->rlink;
        goto T2;
    }

    printf("\n<<< inorder traverse end\n");
}

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
        INIT_TREE(&too[i].tree);
        too[i].a = 'A' + i;
    }

    // debug
    for (i = 0; i < 9; i++) {
        printf("%c ", tree_entry(&too[i].tree, struct _too, tree)->a);
    }
    printf("\n");

    too_root = &too[0].tree;

    /* Create tree and subtree and connect them */
    // 0A 1B 2C 3D 4E 5F 6G 7H 8J
    // A-B
    tree_add_l(&too[1].tree, &too[0].tree);
    // A-C
    tree_add_r(&too[2].tree, &too[0].tree);

    // // debug
    // for (i = 0; i < 9; i++) {
    //     printf("%c ", tree_entry(&too[i].tree, struct _too, tree)->a);
    // }
    // printf("\n");

    // printf("%c %c %c\n", 
    //     tree_entry(&too[0].tree, struct _too, tree)->a, 
    //     tree_entry(too[0].tree.llink, struct _too, tree)->a,
    //     tree_entry(too[0].tree.rlink, struct _too, tree)->a);

    // B-D
    tree_add_l(&too[3].tree, &too[1].tree);

    // C-E
    tree_add_l(&too[4].tree, &too[2].tree);
    // C-F
    tree_add_r(&too[5].tree, &too[2].tree);

    // printf("%c %c %c\n", 
    //     tree_entry(&too[2].tree, struct _too, tree)->a, 
    //     tree_entry(too[2].tree.llink, struct _too, tree)->a,
    //     tree_entry(too[2].tree.rlink, struct _too, tree)->a);

    // E-G
    tree_add_r(&too[6].tree, &too[4].tree);

    // F-H
    tree_add_l(&too[7].tree, &too[5].tree);
    // F-J
    tree_add_r(&too[8].tree, &too[5].tree);

    tree_inorder_traverse_test(too_root);

    free(too);

    return;
}
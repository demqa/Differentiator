#include "bin_tree.h"

#define CATCH_TREE_ERRORS    \
do{                           \
    if (TreeVerify(tree))      \
    {                           \
        return TreeDump(tree);   \
    }                             \
}while(0);

#define CATCH_NODE_ERRORS          \
do{                                 \
    if (NodeVerify(node))            \
    {                                 \
        return NodeVerify(node);       \
    }                                   \
}while(0);

Node_t    *DEAD_PTR    = (Node_t *) (1000 - 7);
size_t     DEAD_SIZE   = 1000 - 7;
TreeStatus DEAD_STATUS = (TreeStatus) (1000 - 7);

TreeStatus TreeCtor      (Tree_t *tree)
{
    if (tree == nullptr)
        return TREE_IS_NULL;

    if (TreeIsEmpty(tree) == TREE_IS_EMPTY)
        return TREE_IS_OK;

    if (TreeIsDtored(tree) != TREE_IS_DTORED)
        return TREE_IS_NOT_EMPTY;

    tree->root   = nullptr;
    tree->size   = 0;
    tree->status = TREE_IS_OK;

    return tree->status;
}

TreeStatus NodesDtor     (Node_t *node)
{
    if (node == nullptr)
        return NODE_PTR_IS_NULL;

    int status = TREE_IS_OK;

    node->parent = DEAD_PTR;
    
    if (node->left  != nullptr) status |= NodesDtor(node->left);
    if (node->right != nullptr) status |= NodesDtor(node->right);

    node->left  = DEAD_PTR;
    node->right = DEAD_PTR;
    
    node->value = DEAD_VAL;
    // CHANGE      ^     SOMEDAY
    // ON SOMETHING BETTER

    free(node);

    return (TreeStatus) (status | TREE_IS_DTORED);
}

TreeStatus TreeDtor      (Tree_t *tree)
{
    if (tree == nullptr)
        return TREE_IS_NULL;

    int status   = NodesDtor(tree->root);

    tree->root   = nullptr;
    tree->size   = DEAD_SIZE;
    tree->status = DEAD_STATUS;

    return (TreeStatus) status;
}

TreeStatus TreeIsDtored  (Tree_t *tree)
{
    if (tree == nullptr)
        return TREE_IS_NULL;

    if (tree->root   == nullptr     &&
        tree->size   == DEAD_SIZE   &&
        tree->status == DEAD_STATUS)
        return TREE_IS_DTORED;
    
    return RESULT_IS_UNKNOWN;
}

TreeStatus TreeIsEmpty   (Tree_t *tree)
{
    if (tree == nullptr)
        return TREE_IS_NULL;

    if (tree->root   == nullptr &&
        tree->size   == 0       && 
        tree->status == 0)
        return TREE_IS_EMPTY;

    return RESULT_IS_UNKNOWN;
}

TreeStatus NodeInsert    (Tree_t *tree, Node_t *node, const ChildNumeration n_child, const Value_t value)
{
    CATCH_TREE_ERRORS;

    if (node == nullptr && node != tree->root)
        return NODE_PTR_IS_NULL;

    Node_t *new_node = (Node_t *) calloc(1, sizeof(Node_t));
    if (new_node == nullptr)
        return BAD_ALLOC;

    new_node->value = value;

    if (node == tree->root && tree->size == 0)
    {
        tree->root = new_node;
        tree->size++;

        return TREE_IS_OK;
    }

    if (n_child == L_CHILD)
    {
        new_node->parent = node;
        node->left = new_node;
    }
    else
    if (n_child == R_CHILD)
    {
        new_node->parent = node;
        node->right = new_node;
    }
    else
    {
        // TODO )))
        // BUT TODAY:
        return TREE_CANT_HAVE_THIS_CHILD; 
    }

    tree->size++;

    return NODE_IS_OK;
}

TreeStatus NodeRemove    (Tree_t *tree, Node_t *node)
{
    CATCH_TREE_ERRORS;
    CATCH_NODE_ERRORS;

    if (NodeIsTerminal(node) == NODE_IS_TERMINAL)
    {
        if (node->parent != nullptr)
        {
            if (node->parent->left  == node) node->parent->left  = nullptr;
            if (node->parent->right == node) node->parent->right = nullptr;
        }
        else
        {
            assert(node == tree->root);

            tree->root = DEAD_PTR;
        }

        free(node);

        tree->size--;
    }
    else
    if (node == tree->root)
    {
        return CANT_REMOVE_TREE_ROOT;
    }
    else
    {
        // CANT REMOVE NON_TERMINAL NODES YET
        // I HAVE TO ADD COUNTER IN DTORS, THEN I CAN DO THIS
        // BUT NOW I CANT COUNT TREE->SIZE NORMALLY
        return CANT_REMOVE_NON_TERMINAL_NODE;
    }

    return UNBELIEVABLE_CASE;
}

TreeStatus NodeIsTerminal(const Node_t *node)
{
    if (node == nullptr)
        return NODE_PTR_IS_NULL;

    if (node->left == nullptr && node->right == nullptr)
        return NODE_IS_TERMINAL;

    return RESULT_IS_UNKNOWN;
}

TreeStatus NodeIsDtored  (const Node_t *node)
{
    if (node == nullptr)
        return NODE_PTR_IS_NULL;

    if (node == DEAD_PTR)
        return NODE_IS_DTORED;

    return RESULT_IS_UNKNOWN;
}

TreeStatus NodeVerify    (const Node_t *node)
{
    if (NodeIsDtored(node) == NODE_IS_DTORED)
        return NODE_IS_DTORED;

    if (node == nullptr)
        return NODE_PTR_IS_NULL;

    int status = NODE_IS_OK;

    if (node->parent        != nullptr &&
        node->parent->left  != node    &&
        node->parent->right != node)
        status |= NODE_PARENT_IS_INVALID;

    if (node->left          != nullptr &&
        node->left->parent  != node)
        status |= NODE_LEFT___IS_INVALID;
    
    if (node->right         != nullptr &&
        node->right->parent != node)
        status |= NODE_RIGHT__IS_INVALID;

    return (TreeStatus) status;   
}

static TreeStatus NodesVerify(Node_t *node)
{
    CATCH_NODE_ERRORS;
    
    int status = TREE_IS_OK;

    status |= NodeVerify(node);

    if (node->left  != nullptr) status |= NodesVerify(node->left);
    if (node->right != nullptr) status |= NodesVerify(node->right);

    return (TreeStatus) status;
}

TreeStatus TreeVerify    (Tree_t *tree)
{
    if (tree == nullptr)
        return TREE_IS_NULL;

    if (TreeIsEmpty(tree) == TREE_IS_EMPTY)
        return TREE_IS_EMPTY;

    if (TreeIsDtored(tree) == TREE_IS_DTORED)
        return TREE_IS_DTORED;

    if (tree->root == nullptr)
        return TREE_ROOT_IS_NULL;

    int status = TREE_IS_OK;
    if (tree->root->parent != nullptr)
    {
        status |= TREE_ROOT_IS_RUINED;
    }

    if (tree->root->left  != nullptr)
        status |= NodesVerify(tree->root->left);

    if (tree->root->right != nullptr)
        status |= NodesVerify(tree->root->right);

    tree->status = (TreeStatus) status;
    return tree->status;
}

#define PRINT_ERROR(error_code)                  \
do{                                               \
    if (status & error_code)                       \
    {                                               \
        fprintf(TREE_LOG_FILE_DEFAULT, #error_code); \
        fprintf(TREE_LOG_FILE_DEFAULT, "\n");         \
    }                                                  \
}while(0);

FILE *TREE_LOG_FILE_DEFAULT = stderr;

static void PrintNode     (const Node_t *node, const size_t *number_of_node, const char color[COLOR_MAX_LEN], FILE *dump_file)
{
    if (node == nullptr || number_of_node == nullptr || color == nullptr || dump_file == nullptr)
        return;
    
    fprintf(dump_file, "    node%lu [fillcolor=\"%s\", ", *number_of_node, color);

    fprintf(dump_file, "    label=\"");

    fprintf(dump_file, "%x", node->value);
    // TODO
    // FUNC PRINT_VALUE_DUMP
    // IT CAN BE STORED IN TREE

    fprintf(dump_file, "\"");

    fprintf(dump_file, "];\n");

    return;
}

static void PrintEdge     (const size_t from, const size_t to, const char color[COLOR_MAX_LEN], FILE *dump_file)
{
    if (color == nullptr || dump_file == nullptr)
        return;
    
    fprintf(dump_file, "    edge [color=\"%s\"]; node%lu -> node%lu;\n", color, from, to);

    return;
}

static TreeStatus NodeDump(const Node_t *node, size_t *number_of_node, FILE *dump_file)
{
    CATCH_NODE_ERRORS;
    if (number_of_node == nullptr)
        return NUMBER_OF_NODE_NULL;
    if (dump_file == nullptr)
        return DUMP_FILE_IS_NULL;

    int status = NODE_IS_OK;

    if (*number_of_node == 0)
    {
        PrintNode(node, number_of_node, root_color, dump_file);
    }
    else
    if (NodeIsTerminal(node) == NODE_IS_TERMINAL)
    {
        PrintNode(node, number_of_node, terminal_color, dump_file);
    }
    else
    {
        PrintNode(node, number_of_node, default_color, dump_file);
    }

    size_t current_number_of_node = *number_of_node;

    if (node->left)
    {
        PrintEdge(current_number_of_node, ++(*number_of_node), left_color, dump_file);

        status |= NodeDump(node->left, number_of_node, dump_file);
    }

    if (node->right)
    {
        PrintEdge(current_number_of_node, ++(*number_of_node), right_color, dump_file);

        status |= NodeDump(node->right, number_of_node, dump_file);
    }

    return (TreeStatus) status;
}

TreeStatus TreeDump(Tree_t *tree)
{
    int status = TreeVerify(tree);
    if (status && (status & (1 << 9 - 1)) == 0)
    {
        fprintf(TREE_LOG_FILE_DEFAULT, "CANT DUMP, TREE IS RUINED\n");
        
        PRINT_ERROR(BAD_ALLOC);

        PRINT_ERROR(TREE_IS_DTORED);

        PRINT_ERROR(TREE_IS_NULL);
        PRINT_ERROR(TREE_ROOT_IS_NULL);
        PRINT_ERROR(NODE_PTR_IS_NULL);

        PRINT_ERROR(NODE_PARENT_IS_INVALID);
        PRINT_ERROR(NODE_LEFT___IS_INVALID);
        PRINT_ERROR(NODE_RIGHT__IS_INVALID);

        PRINT_ERROR(TREE_ROOT_IS_RUINED);
        PRINT_ERROR(TREE_CANT_HAVE_THIS_CHILD);

        PRINT_ERROR(NUMBER_OF_NODE_NULL);
        PRINT_ERROR(DUMP_FILE_IS_NULL);

        PRINT_ERROR(CANT_REMOVE_TREE_ROOT);
        PRINT_ERROR(CANT_REMOVE_NON_TERMINAL_NODE);

        PRINT_ERROR(UNBELIEVABLE_CASE);
        PRINT_ERROR(RESULT_IS_UNKNOWN);

        return (TreeStatus) status;
    }

    size_t number_of_node = 0;
    FILE *dump_file = fopen("dump", "w");
    
    fputs("digraph structs {\n", dump_file);
    fputs("    node [color=black, shape=box, style=\"filled\"];\n", dump_file);

    status = NodeDump(tree->root, &number_of_node, dump_file);

    fputs("}\n", dump_file);

    fclose(dump_file);

    system("dot dump -T png -o dump.png");
    system("eog dump.png");

    return (TreeStatus) status;
}

#undef CATCH_NODE_ERRORS
#undef CATCH_TREE_ERRORS

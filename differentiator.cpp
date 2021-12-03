#include "differentiator.h"
#include "debug_lib.h"

int Filesize  (FILE *stream, size_t *filesize)
{
    if (stream == nullptr)
        return -1;

    struct stat buff = {};
        
    fstat(fileno(stream), &buff);
    
    *filesize = buff.st_size;

    return 0;
}

int ReadBuffer(char **buffer, FILE *stream)
{
    if (stream == nullptr)
        return -1;

    size_t filesize = 0;
    if (Filesize(stream, &filesize))
    {
        fclose(stream);
        return -2;
    }

    *buffer = (char *) calloc(filesize + 1, sizeof(char));
    if (buffer == nullptr)
    {
        fclose(stream);
        return -3;
    }

    size_t count = fread(*buffer, sizeof(char), filesize, stream);
    if (count != filesize)
    {
        fclose(stream);
        return -4;
    }

    (*buffer)[filesize] = '\0';

    fclose(stream);

    return 0;
}

int GetString (char **ptr, char *string)
{
    if (ptr == nullptr || *ptr == nullptr || string == nullptr) return PTR_IS_NULL;

    if (**ptr == ')')
    {
        string[0] = '\0';
        return FUNC_IS_OK;
    }

    assert(**ptr != '(');

    string[0] = **ptr;

    size_t num = 1;

    while (*(*ptr + 1) != ')' && *(*ptr + 1) != '(')
    {
        if (*(*ptr + 1) == '\0')          return TILT;

        if (num + 1 >= MAX_EXPR_ELEM_LEN) return MAX_EXPR_ELEM_LEN_REACHED;

        (*ptr)++;

        string[num++] = **ptr;
    }

    assert(num < MAX_EXPR_ELEM_LEN);

    string[num] = '\0';

    return DEAD_INSIDE;
}

int IsCharOper(const char c)
{
    if (c == SUB || c == ADD || c == MUL || c == DIV || c == POW)
        return 1;

    return 0;
}

int GetArg    (char *string, RT *arg)
{
    if (string == nullptr || arg == nullptr) return PTR_IS_NULL;

    size_t number = 0;

    size_t length = strlen(string);

    if (length == 0) return INVALID_STRING;

    double num = NAN;

    if (sscanf(string, "%lf", &num) == 1)
    {           
        arg->type = NUM_TYPE;
        arg->num  = num;
        // arg->subtree_status = CONST;
    }
    else
    if (length == 1)
    {
        if (IsCharOper(string[0]))
        {
            arg->type = OPER_TYPE;
            arg->oper = string[0];
            // arg->subtree_status = NOT_CALCULATED;
        }
        else
        if ('a' < string[0] && string[0] < 'z')
        {
            arg->type = VAR_TYPE;
            arg->var  = string[0];
            // arg->subtree_status = NOT_CALCULATED;
            // we cant know which parameter is const for now
        }
        else
        {
            fprintf(stderr, "UNKNOWN EXPRESSION %s\n", string);
            return UNKNOWN_EXPRESSION;
        }
    }
    else
    if (length == 2)
    {
        arg->type = OPER_TYPE;
        if(strcmp(string, "ln") == 0)
            arg->oper = LN;
        else
        if (strcmp(string, "lg") == 0)
            arg->oper = LG;
        else
        if (strcmp(string, "sh") == 0)
            arg->oper = SH;
        else
        if (strcmp(string, "ch") == 0)
            arg->oper = CH;
        else
        {
            fprintf(stderr, "UNKNOWN EXPRESSION %s\n", string);
            return UNKNOWN_EXPRESSION;
        }
    }
    else
    if (length == 3)
    {   
        arg->type = OPER_TYPE;
        if (strcmp(string, "sin") == 0)
            arg->oper = SIN;
        else
        if (strcmp(string, "cos") == 0)
            arg->oper = COS;
        else
        {
            fprintf(stderr, "UNKNOWN EXPRESSION %s\n", string);
            return UNKNOWN_EXPRESSION;
        }

    }
    else
    {
        fprintf(stderr, "UNKNOWN EXPRESSION %s\n", string);
        return UNKNOWN_EXPRESSION;
    }

    return FUNC_IS_OK;
}

int ProceedNodeValue  (char **ptr, Node_t *node, MemoryDefender *def)
{
    int status = FUNC_IS_OK;

    char *string = (char *) calloc(MAX_EXPR_ELEM_LEN + 1, sizeof(char));
    if (string == nullptr) status |= BAD_ALLOC;

    status |= GetString(ptr, string);
    if (status) return status;

    RT *arg = (RT *) calloc(1, sizeof(RT));
    if (arg == nullptr)    status |= BAD_ALLOC;

    DefenderPush(def, (char *)arg);

    status |= GetArg(string, arg);
    if (status) return status;

    free(string);

    if (status) return status;

    node->value = arg;
    
    return status;
}

int TreeReadProcessing(Tree_t *tree, Node_t *node, char **ptr, char *end_ptr, MemoryDefender *def)
{
    if (tree == nullptr)   return TREE_IS_NULL;
    
    if (ptr  == nullptr || *ptr == nullptr || end_ptr == nullptr) return PTR_IS_NULL;

    if (*ptr > end_ptr)    return PTR_BIGGER_BUFF_END;

    int status = FUNC_IS_OK;

    while (*ptr <= end_ptr)
    {
        if (**ptr == '(')
        {
            (*ptr)++;

            if (node == nullptr)
            {
                NodeInsert(tree, node, L_CHILD, nullptr);
                node = tree->root;
            }
            else
            if (node->left == nullptr)
            {
                NodeInsert(tree, node, L_CHILD, nullptr);
                node = node->left;
            }
            else
            {
                NodeInsert(tree, node, R_CHILD, nullptr);
                node = node->right;
            }

            if (**ptr == '(')
                continue;

            assert(**ptr != ')' && "You've posted cringe!");
            
            // number or variable
            status |= ProceedNodeValue(ptr, node, def);
            if (status) return status;

            (*ptr)++;
        }
        else
        if (**ptr == ')')
        {
            if (node == nullptr) return FUCK_MY_LIFE;

            if (node == tree->root)
                break;

            (*ptr)++;

            node = node->parent;
        }
        else
        {
            if (node == nullptr) return FUCK_MY_LIFE;

            // operator
            status |= ProceedNodeValue(ptr, node, def);
            if (status) return status;

            (*ptr)++;
        }
    }
    
    return status;
}

int TreeRead          (Tree_t *tree, char *buffer, size_t buff_size, MemoryDefender *def)
{
    if (tree == nullptr)   return TREE_IS_NULL;

    if (buffer == nullptr) return BUFFER_IS_NULL;

    if (buff_size == 0)    return FILESIZE_IS_ZERO;

    char *ptr     = buffer;
    char *end_ptr = buffer + buff_size;

    int counter = 0;
    while (ptr <= end_ptr)
    {
        if (*ptr == '(')
            counter++;

        if (*ptr == ')')
            counter--;
        
        if (counter < 0)
        {
            return EXPR_IS_INVALID;
        }

        ptr++;
    }

    ptr = buffer;
    int status = TreeReadProcessing(tree, tree->root, &ptr, end_ptr, def);
    if (status)
        return status;
    
    return FUNC_IS_OK;
}

int TreeFill          (Tree_t *tree, FILE *stream, char **buff, MemoryDefender *def)
{
    if (tree == nullptr)   return TREE_IS_NULL;

    if (stream == nullptr) return STREAM_IS_NULL;

    if (buff == nullptr || *buff != nullptr) return PTR_IS_NULL;
    
    size_t buff_size = 0;
    if (Filesize(stream, &buff_size)) return FILESIZE_IS_ZERO;

    char *buffer = nullptr;

    if (ReadBuffer(&buffer, stream))  return BUFFER_CANT_BE_READ;

    *buff = buffer;

    DefenderPush(def, buffer);
    
    int status = TreeRead(tree, buffer, buff_size, def);

    if (status)
        return status;

    return FUNC_IS_OK;
}

/*
optimising funcs

these funcs gives
0 if ans is no
1 if ans is yes
other - if there was an error
*/

int IsConst(Node_t *node, const char variable) /*Is Subtree Const*/
{
    if (node == nullptr) return 0;

    if (node->value == nullptr)
    {
        return CONST;
    }

    RT *arg = node->value;

    if (arg->subtree_status != NOT_CALCULATED)
        return arg->subtree_status;
    else
    {
        if (NodeIsTerminal(node) == NODE_IS_TERMINAL)
        {
            if (arg->type == NUM_TYPE || arg->type == OPER_TYPE && arg->oper != variable)
            {
                arg->subtree_status = CONST;
                return CONST;
            }
            else
            {
                arg->subtree_status = VARIABLE;
                return VARIABLE;
            }
        }
        else
        {
            int res = (IsConst(node->left, variable) | IsConst(node->right, variable));
            if (res & VARIABLE)
                res = VARIABLE;
            else
                assert(res == CONST);

            arg->subtree_status = res;
            return res;
        }
    }

    return UNBELIEVABLE_CASE;
}

int IsZeroNum(double x)
{
    if (!isfinite(x)) return 0;

    return abs(x) < 0.00001;
}

int IsEqualNum(double x, double num)
{
    if (!isfinite(x) || !isfinite(num)) return 0;

    return IsZeroNum(x - num);
}

int IsZero (Node_t *node) /*Is Subtree Zero*/
{
    if (node == nullptr || node->value == nullptr) return 0;

    RT *arg = node->value;
    if (NodeIsTerminal(node) == NODE_IS_TERMINAL)
    {
        assert(arg->type == NUM_TYPE);
        if (IsZeroNum(arg->num))
            return 1;
        else
            return 0;
    }

    assert(arg->type == OPER_TYPE);
    
    if (arg->oper == MUL)
    {   
        assert(node->left && node->right);
        if (node->left->value  && node->left->value->type  == NUM_TYPE && IsZeroNum(node->left->value->num) ||
            node->right->value && node->right->value->type == NUM_TYPE && IsZeroNum(node->right->value->num))
            return 1;
    }
    else
    if (arg->oper == LN || arg->oper == LG)
    {
        assert(node->left);
        if (node->left->value && node->left->value->type == NUM_TYPE && IsEqualNum(node->left->value->num, 1))
            return 1;
    }
    else
    if (arg->oper == DIV)
    {
        assert(node->left);
        if (node->left->value && node->left->value->type == NUM_TYPE && IsZeroNum(node->left->value->num))
            return 1;
    }

    int res = IsZero(node->left) + IsZero(node->right);
    if (res) return 1;

    return 0;
}

#define OPER_INIT(ARG, OPER)                    \
do                                               \
{                                                 \
    assert(ARG->type == 0 && ARG->oper == 0 && ARG->num == 0 && ARG->var == 0); \
    ARG->type = OPER_TYPE;                          \
    ARG->oper = OPER;                                \
                                                      \
}while (0);

#define NODE_INIT(NAME, PARENT)                          \
    Node_t *NAME = (Node_t *) calloc(1, sizeof(Node_t));  \
    if (NAME == nullptr) return BAD_ALLOC;                 \
    NAME->parent = PARENT;

#define ARG__INIT(NAME)                                       \
    RT *NAME = (RT *) calloc(1, sizeof(RT));                   \
    if (NAME == nullptr) return BAD_ALLOC;                      \
    DefenderPush(def, (char *)NAME);

#define ARG_ASSIGN(NODE, ARG)                                     \
    NODE->value = ARG;

#define NUM__INIT(ARG, NUM) \
do                           \
{                             \
    assert(ARG->type == 0 && ARG->oper == 0 && ARG->num == 0 && ARG->var == 0); \
    ARG->type = NUM_TYPE;       \
    ARG->num  = NUM;             \
} while (0);                      \

#define ZERO_INIT(ARG) NUM__INIT(ARG, 0);
#define ONE__INIT(ARG) NUM__INIT(ARG, 1);

#define LEFT  node->left
#define RIGHT node->right

#define OP_NODE_INIT(OP, AP, PAR, SIDE)  \
    NODE_INIT(new_ ## AP, new_ ## PAR);   \
    ARG__INIT(arg_ ## AP);                 \
    OPER_INIT(arg_ ## AP, OP);              \
    ARG_ASSIGN(new_ ## AP, arg_ ## AP);      \
    (new_ ## PAR)->SIDE  = (new_ ## AP);          

#define NUM_NODE_INIT(NUM, AP, PAR, SIDE)       \
    NODE_INIT(new_ ## AP, new_ ## PAR);          \
    ARG__INIT(arg_ ## AP);                        \
    NUM__INIT(arg_ ## AP, NUM);                    \
    ARG_ASSIGN(new_ ## AP, arg_ ## AP);             \
    (new_ ## PAR)->SIDE  = (new_ ## AP);

#define DIFF(FROM, DEST, SIDE)                         \
    status |= DiffNodes(FROM, &(new_ ## DEST)->SIDE, new_ ## DEST, variable, def);

#define COPY(FROM, DEST, SIDE)                            \
    status |= CopyNodes(FROM, &(new_ ## DEST)->SIDE, new_ ## DEST, def);

int CopyNodes(Node_t *node, Node_t **copy, Node_t *parent, MemoryDefender *def);

int DiffNodes(Node_t *node, Node_t **diff, Node_t *parent, const char variable, MemoryDefender *def)
{
    if (node == nullptr)                     return NODE_PTR_IS_NULL;

    if (diff == nullptr || *diff != nullptr) return PTR_IS_NULL;

    NODE_INIT(new_node, parent);

    if (node->value == nullptr)              return NODE_VALUE_IS_NULL;
        
    RT *arg = node->value;

    int status = FUNC_IS_OK;

    ARG__INIT(new_arg);

    ARG_ASSIGN(new_node, new_arg);

    *diff = new_node;

    if (IsConst(node, variable) == CONST)
    {
        ZERO_INIT(new_arg);
        return FUNC_IS_OK;
    }

    if (arg->type == OPER_TYPE)
    {
        assert(NodeIsTerminal(node) != NODE_IS_TERMINAL);

        assert(arg->num == 0 && arg->var == 0);

        if (arg->oper == ADD || arg->oper == SUB)
        {
            OPER_INIT(new_arg, arg->oper);
            
            DIFF(LEFT,  node, left);
            DIFF(RIGHT, node, right);
        }

        if (arg->oper == MUL)
        {
            OPER_INIT(new_arg, ADD);

            OP_NODE_INIT(MUL, L, node, left);

            DIFF(LEFT,  L, left);
            COPY(RIGHT, L, right);

            OP_NODE_INIT(MUL, R, node, right);
            
            COPY(LEFT,  R, left);
            DIFF(RIGHT, R, right);
        }

        if (arg->oper == DIV)
        {
            OPER_INIT(new_arg, arg->oper);

            OP_NODE_INIT(SUB, L, node, left);

            OP_NODE_INIT(MUL, LL, L, left);
            DIFF(LEFT,  LL, left);
            COPY(RIGHT, LL, right);

            OP_NODE_INIT(MUL, LR, L, right);
            COPY(LEFT,  LR, left);
            DIFF(RIGHT, LR, right);

            OP_NODE_INIT(POW, R, node, right);
            COPY(RIGHT, R, left);

            NUM_NODE_INIT(2, RR, R, right);
        }

        if (arg->oper == POW)
        {
            if (IsConst(node->left,  variable) == VARIABLE &&
                IsConst(node->right, variable) == CONST)
            {
                OPER_INIT(new_arg, MUL);

                OP_NODE_INIT(MUL, L, node, left);

                COPY(RIGHT, L, left);

                OP_NODE_INIT(POW, LR, L, right);

                COPY(LEFT, LR, left);

                OP_NODE_INIT(SUB, LRR, LR, right);
                COPY(RIGHT, LRR, left);
                NUM_NODE_INIT(1, LRRR, LRR, right);

                DIFF(LEFT, node, right);
            }
            else
            if (IsConst(node->left,  variable) == CONST &&
                IsConst(node->right, variable) == VARIABLE)
            {
                OPER_INIT(new_arg, MUL);

                OP_NODE_INIT(POW, L, node, left);
                COPY(LEFT,  L, left);
                COPY(RIGHT, L, right);

                OP_NODE_INIT(MUL, R, node, right);
                OP_NODE_INIT(LN, RL, R, left);
                COPY(LEFT, RL, left);

                DIFF(RIGHT, R, right);
            }
            else
            if (IsConst(node->left,  variable) == VARIABLE &&
                IsConst(node->right, variable) == VARIABLE)
            {
                OPER_INIT(new_arg, MUL);

                OP_NODE_INIT(POW, L, node, left);
                COPY(LEFT,  L, left);
                COPY(RIGHT, L, right);

                OP_NODE_INIT(ADD, R, node, right);

                OP_NODE_INIT(MUL, RL, R, left);
                DIFF(RIGHT, RL, left);
                OP_NODE_INIT(LN, RLR, RL, right);
                COPY(LEFT, RLR, left);

                OP_NODE_INIT(DIV, RR, R, right);

                OP_NODE_INIT(MUL, RRL, RR, left);
                DIFF(LEFT, RRL, left);
                COPY(RIGHT, RRL, right);
                
                COPY(LEFT, RR, right);
            }
            else
            {
                assert(false && "WHAT THE HELL???");
            }
            
        }

        if (arg->oper == SIN)
        {
            OPER_INIT(new_arg, MUL);

            OP_NODE_INIT(COS, L, node, left);
            COPY(LEFT, L, left);

            DIFF(LEFT, node, right);
        }

        if (arg->oper == COS)
        {
            OPER_INIT(new_arg, MUL);

            OP_NODE_INIT(MUL, L, node, left);

            NUM_NODE_INIT(-1, LL, L, left);

            OP_NODE_INIT(SIN, LR, L, right);
            COPY(LEFT, LR, left);

            DIFF(LEFT, node, right);
        }

        if (arg->oper == LN)
        {
            OPER_INIT(new_arg, DIV);

            DIFF(LEFT, node, left);
            
            COPY(LEFT, node, right);
        }

        if (arg->oper == LG)
        {
            OPER_INIT(new_arg, DIV);

            DIFF(LEFT, node, left);
            
            OP_NODE_INIT(MUL, R, node, right);
            COPY(LEFT, R, left);
            OP_NODE_INIT(LN, RR, R, right);
            NUM_NODE_INIT(10, RRL, RR, left);
        }

        if (arg->oper == SH)
        {
            OPER_INIT(new_arg, MUL);

            OP_NODE_INIT(CH, L, node, left);
            COPY(LEFT, L, left);

            DIFF(LEFT, node, right);
        }

        if (arg->oper == CH)
        {
            OPER_INIT(new_arg, MUL);

            OP_NODE_INIT(SH, L, node, left);
            COPY(LEFT, L, left);

            DIFF(LEFT, node, right);
        }
    }
    else
    if (arg->type == NUM_TYPE)
    {
        assert(NodeIsTerminal(node) == NODE_IS_TERMINAL);

        assert(arg->oper == 0 && arg->var == 0);

        ZERO_INIT(new_arg);
    }
    else
    if (arg->type == VAR_TYPE)
    {
        assert(NodeIsTerminal(node) == NODE_IS_TERMINAL);

        assert(arg->oper == 0 && arg->num == 0);

        if (arg->var == variable)
        {
            ONE__INIT(new_arg);
        }
        else
        {
            ZERO_INIT(new_arg);
        }
    }   
    else
        return EXCEPTION_UNEXPECTED_VALUE_TYPE;

    return status;
}

int CopyNodes(Node_t *node, Node_t **copy, Node_t *parent, MemoryDefender *def)
{
    if (node == nullptr)                     return NODE_PTR_IS_NULL;

    if (copy == nullptr || *copy != nullptr) return PTR_IS_NULL;

    // i have to save these ptr's or 
    // do some func that will free
    // this memory when it won't be used no more
    Node_t *new_node = (Node_t *) calloc(1, sizeof(Node_t));
    if (new_node == nullptr) return BAD_ALLOC;
    new_node->parent = parent;

    RT *arg = (RT *) calloc(1, sizeof(RT));
    if (arg == nullptr)      return BAD_ALLOC;

    DefenderPush(def, (char *)arg);

    RT *check = (RT *) memcpy(arg, node->value, sizeof(RT));
    if (check != arg)        return MEMCPY_CRASH;

    new_node->value = arg;

    int status = FUNC_IS_OK;
    if (node->left  != nullptr)
        status |= CopyNodes(node->left,  &new_node->left,  new_node, def);

    if (node->right != nullptr)
        status |= CopyNodes(node->right, &new_node->right, new_node, def);

    *copy = new_node;

    return status;
}

int Differentiate(Tree_t *tree, Tree_t **tree_res, const char variable, MemoryDefender *def)
{
    if (tree == nullptr)  return TREE_IS_NULL;

    if (TreeVerify(tree)) return TreeDump(tree);

    if (tree_res == nullptr || *tree_res != nullptr)
        return PTR_IS_NULL;

    Tree_t *new_tree = (Tree_t *) calloc(1, sizeof(Tree_t));
    if (new_tree == nullptr) return BAD_ALLOC;

    DefenderPush(def, (char *)new_tree);

    TreeCtor(new_tree);

    int status = FUNC_IS_OK;

    // tree->size doesnt work now
    status |= DiffNodes(tree->root, &new_tree->root, nullptr, variable, def);
    
    *tree_res = new_tree;

    return status;
}

int main()
{
    int status = 0;
    Tree_t tree = {};

    status |= TreeCtor(&tree);
    if (status) fprintf(stderr, "status = %d\n", status);

    FILE *stream = fopen("expr1", "r");
    if (stream == nullptr)
    {
        fprintf(stderr, "cringe\n");
        return -1;
    }

    char *buffer = nullptr;

    MemoryDefender def = {};
    DefenderCtor(&def);

    status |= TreeFill(&tree, stream, &buffer, &def);
    if (status) PRINT_D(status);

    status |= TreeDump(&tree);
    if (status) PRINT_D(status);

    Tree_t *differentiated_tree = nullptr;

    status |= Differentiate(&tree, &differentiated_tree, 'x', &def);
    if (status) PRINT_D(status);
    
    // i haven't got any idea about diff_tree->size ))))))

    status |= TreeDump(differentiated_tree);
    if (status) PRINT_D(status);

    TreeDtor(&tree);

    TreeDtor(differentiated_tree);

    DefenderClear(&def);

    DefenderDtor(&def);

    return 0;
}


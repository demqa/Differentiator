#include "differentiator.h"
#include "tex.h"
#include "debug_lib.h"

int Filesize  (FILE *stream, size_t *filesize)  
{
    if (stream == nullptr) return STREAM_IS_NULL;

    struct stat buff = {};
        
    fstat(fileno(stream), &buff);
    
    *filesize = buff.st_size;

    return FUNC_IS_OK;
}

int ReadBuffer(char **buffer, FILE *stream)
{
    if ( stream == nullptr) return STREAM_IS_NULL;
    if ( buffer == nullptr) return BUFFER_IS_NULL;
    if (*buffer != nullptr) return PTR_IS_NOT_NULL;

    size_t filesize = 0;

    int status = Filesize(stream, &filesize);
    if (status)
    {
        fclose(stream);
        return status;
    }

    *buffer = (char *) calloc(filesize + 1, sizeof(char));
    if (buffer == nullptr)
    {
        fclose(stream);
        return BAD_ALLOC;
    }

    size_t count = fread(*buffer, sizeof(char), filesize, stream);
    if (count != filesize)
    {
        fclose(stream);
        return READ_WAS_UNSUCCESSFUL;
    }

    (*buffer)[filesize] = '\0';

    fclose(stream);

    return FUNC_IS_OK;
}

int SyntaxError(char **ptr)
{
    if (ptr == nullptr || *ptr == nullptr) return PTR_IS_NULL;

    fprintf(stderr, "INVALID_INPUT_FORMAT\n");
    fprintf(stderr, "EXPECTED NUMBER, INPUT: %c\n", **ptr);

    return FUNC_IS_OK;
}

int Require    (char **ptr, const char c)
{
    if (ptr == nullptr || *ptr == nullptr) return PTR_IS_NULL;

    if (**ptr != c)
    {
        fprintf(stderr, "INVALID_INPUT_FORMAT\n");
        fprintf(stderr, "EXPECTED: %c\n INPUT:    %c\n", c, **ptr);
        return 1;
    }                                                          
    else                                                                   
        (*ptr)++;
    
    return 0;
}

int StrEqual(const char *l, const char *r)
{
    if (l == nullptr || r == nullptr) return 0;

    for (int i = 0; l[i] != '\0' && r[i] != '\0'; i++)
        if (l[i] != r[i]) return 0;

    return 1;
}

int IsMathConst(char op)
{
    if (op == EXP || op == PI)
        return op;

    return FUNC_IS_OK;
}

int ReadFunc(char **ptr, int *n)
{
    if (ptr == nullptr || *ptr == nullptr) return PTR_IS_NULL;
    if (n   == nullptr)                    return PTR_IS_NULL;
    
    int status = FUNC_IS_OK;

    int check  = 0;

    char *str  = *ptr;

    if (StrEqual(*ptr, "pi"))
    {
        (*ptr) += 2;
        *n     = PI;
    }
    else
    if (StrEqual(*ptr, "e"))
    {
        (*ptr) += 1;
        *n    = EXP;
    }
    else
    if (StrEqual(str, "sin"))
    {
        (*ptr) += 3;
        *n    = SIN;
    }
    else
    if (StrEqual(str, "cos"))
    {
        (*ptr) += 3;
        *n    = COS;
    }
    else
    if (StrEqual(str, "ln"))
    {
        (*ptr) += 2;
        *n     = LN;
    }
    else
    if (StrEqual(str, "lg"))
    {
        (*ptr) += 2;
        *n     = LG;
    }
    else
    if (StrEqual(str, "sh"))
    {
        (*ptr) += 2;
        *n     = SH;
    }
    else
    if (StrEqual(str, "ch"))
    {
        (*ptr) += 2;
        *n     = CH;
    }
    else
    {
        *n = 0;
        status = OPER_NOT_FOUND;
    }

    return status;
}

int ReadVar (char **ptr, char *c)
{
    if (ptr == nullptr || *ptr == nullptr) return 0;
    if (c == nullptr)                      return 0;

    if ('a' <= **ptr && **ptr <= 'z')
    {
        *c = **ptr;
        (*ptr)++;
    }

    return 0;
}

int ReadNum (char **ptr, double *num)
{
    if (ptr == nullptr || *ptr == nullptr) return 0;
    if (num == nullptr)                    return 0;

    int check = 0;
    sscanf(*ptr, "%lf%n", num, &check);

    (*ptr) += check;

    if (check == 0)
        return 0;

    return 1;
}

Node_t *GetG(char **ptr, MemoryDefender *def)
{
    if (ptr == nullptr || *ptr == nullptr) return nullptr;
    if (def == nullptr)                    return nullptr;

    Node_t *val = GetE(ptr, def);
    assert(val != nullptr);

    if (Require(ptr, '$'))                 return nullptr;

    return val;
}

Node_t *GetW(char **ptr, MemoryDefender *def)
{
    if (ptr == nullptr || *ptr == nullptr) return nullptr;
    if (def == nullptr)                    return nullptr;

    Node_t *val = GetP(ptr, def);
    assert(val != nullptr);

    if (**ptr == '^')
    {
        (*ptr)++;
        Node_t *val2 = GetW(ptr, def);
        assert(val2 != nullptr);

        NEW_NODE(node);
        NEW_ARG (node, arg);
        OPER_ARG(arg,  POW);

        CONNECT(node, val,  left);
        CONNECT(node, val2, right);

        val = node;
    }
    
    return val;
}

Node_t *GetT(char **ptr, MemoryDefender *def)
{
    if (ptr == nullptr || *ptr == nullptr) return nullptr;
    if (def == nullptr)                    return nullptr;

    Node_t *val = GetW(ptr, def);
    assert(val != nullptr);
    
    while (**ptr == '*' || **ptr == '/')
    {
        int op = **ptr;
        (*ptr)++;

        Node_t *val2 = GetW(ptr, def);
        assert(val2 != nullptr);

        NEW_NODE(node);
        NEW_ARG (node, arg);
        OPER_ARG (arg,  op);

        CONNECT(node, val,  left);
        CONNECT(node, val2, right);
        
        val = node;
    }

    return val;
}

Node_t *GetE(char **ptr, MemoryDefender *def)
{
    if (ptr == nullptr || *ptr == nullptr) return nullptr;
    if (def == nullptr)                    return nullptr;

    Node_t *val = GetT(ptr, def);
    assert(val != nullptr);

    while (**ptr == '+' || **ptr == '-')
    {
        char op = **ptr;
        (*ptr)++;

        Node_t *val2 = GetT(ptr, def);
        assert(val2 != nullptr);

        NEW_NODE(node);
        NEW_ARG (node, arg);
        OPER_ARG(arg,   op);

        CONNECT(node, val,  left);
        CONNECT(node, val2, right);
        
        val = node;
    }

    return val;
}

Node_t *GetN(char **ptr, MemoryDefender *def)
{
    if (ptr == nullptr || *ptr == nullptr) return nullptr;
    if (def == nullptr)                    return nullptr;

    double num = NAN;
    
    if (ReadNum(ptr, &num) != 1)
    {
        SyntaxError(ptr);
        return nullptr;
    }
    
    NEW_NODE(val);
    NEW_ARG (val, arg);
    NUM_ARG (arg, num);

    return val;
}

Node_t *GetP(char **ptr, MemoryDefender *def)
{
    if (ptr == nullptr || *ptr == nullptr) return nullptr;
    if (def == nullptr)                    return nullptr;

    int multiplier = 1;

    while (**ptr == '+' || **ptr == '-')
        if (*((*ptr)++) == '-')
            multiplier *= -1;
    
    if (multiplier == -1)
    {
        NEW_NODE(node);
        NEW_ARG (node, arg);
        OPER_ARG(arg,  SUB);

        NEW_NODE(L);
        NEW_ARG (L, AL);
        NUM_ARG (AL, 0);
        CONNECT(node, L, left);

        Node_t *R = GetP(ptr, def);
        assert(R != nullptr);

        CONNECT(node, R, right);

        return node;
    }

    if (**ptr == '(')
    {
        (*ptr)++;

        Node_t *val = GetE(ptr, def);
        assert(val != nullptr);

        if (Require(ptr, ')'))
            return nullptr;

        return val;
    }
    else
        return GetF(ptr, def);
}

Node_t *GetV(char **ptr, MemoryDefender *def)
{
    if (ptr == nullptr || *ptr == nullptr) return nullptr;
    if (def == nullptr)                    return nullptr;

    char var = 0;

    ReadVar(ptr, &var);

    if (var != 0)
    {
        NEW_NODE(val);
        NEW_ARG (val, arg);
        VAR_ARG (arg, var);

        return val;
    }
    else
        return GetN(ptr, def);
}

Node_t *GetF(char **ptr, MemoryDefender *def)
{
    if (ptr == nullptr || *ptr == nullptr) return nullptr;
    if (def == nullptr)                    return nullptr;

    int oper = 0;

    ReadFunc(ptr, &oper);

    if (IsMathConst(oper))
    {
        NEW_NODE(node);
        NEW_ARG (node, arg);
        OPER_ARG(arg, IsMathConst(oper));

        return node;
    }
    else
    if (oper != 0)
    {
        Node_t *val = GetP(ptr, def);
        assert(val != nullptr);

        NEW_NODE(node);
        NEW_ARG (node, arg);
        OPER_ARG(arg, oper);

        CONNECT(node, val, left);
        
        return node;
    }
    else
        return GetV(ptr, def);
}

int TreeReadProcessing(Tree_t *tree, char **ptr,                     MemoryDefender *def)
{
    if (tree == nullptr)                   return TREE_IS_NULL;
    if (TreeVerify(tree))                  return TreeDump(tree);
    if (ptr == nullptr || *ptr == nullptr) return PTR_IS_NULL;
    if (def == nullptr)                    return DEF_IS_NULL;

    tree->root = GetG(ptr, def);

    if (tree->root == nullptr)
        return TREE_IS_EMPTY;

    return FUNC_IS_OK;
}

int TreeRead          (Tree_t *tree, char *buffer, size_t buff_size, MemoryDefender *def)
{
    if (tree   == nullptr) return TREE_IS_NULL;
    if (TreeVerify(tree))  return TreeDump(tree);
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
    int status = TreeReadProcessing(tree, &ptr, def);
    if (status) return status;
    
    return FUNC_IS_OK;
}

int TreeFill          (Tree_t *tree, FILE *stream, char **buff,      MemoryDefender *def)
{
    if (tree   == nullptr)                     return TREE_IS_NULL;
    if (stream == nullptr)                     return STREAM_IS_NULL;
    if (buff   == nullptr || *buff != nullptr) return PTR_IS_NULL;
    
    size_t buff_size = 0;
    if (Filesize(stream, &buff_size))          return FILESIZE_IS_ZERO;
         
    char *buffer = nullptr;         
         
    if (ReadBuffer(&buffer, stream))           return BUFFER_CANT_BE_READ;

    *buff = buffer;

    DefenderPush(def, buffer);
    
    int status = TreeRead(tree, buffer, buff_size, def);
    if (status) return status;

    return FUNC_IS_OK;
}


int IsConst    (Node_t *node, const char variable) /*Is Subtree Const*/
{
    if (node        == nullptr) return 0;
    if (node->value == nullptr) return NODE_VALUE_IS_NULL;

    RT *arg = node->value;

    if (arg->subtree_status != NOT_CALCULATED)
        return arg->subtree_status;
    else
    {
        if (NodeIsTerminal(node) == NODE_IS_TERMINAL)
        {
            if (arg->type == NUM_TYPE || arg->type == OPER_TYPE && arg->oper != variable)
            {
                arg->subtree_status |= CONST;
                return CONST;
            }
            else
            {
                arg->subtree_status |= VARIABLE;
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

            arg->subtree_status |= res;
            return res;
        }
    }

    assert(false);
    return UNBELIEVABLE_CASE;
}

int ClearStatus(Node_t *node)
{
    if (node == nullptr)        return 0;
    if (node->value == nullptr) return NODE_VALUE_IS_NULL;

    node->value->subtree_status = 0;

    int status = FUNC_IS_OK;

    status |= ClearStatus(LEFT);
    status |= ClearStatus(RIGHT);

    return status;
}

int IsZeroNum (double x)
{
    if (!isfinite(x)) return 0;

    return abs(x) < 0.00001;
}

int IsEqualNum(double x, double num)
{
    if (!isfinite(x) || !isfinite(num)) return 0;

    return IsZeroNum(x - num);
}

int CopyNodes(Node_t *node, Node_t **copy, Node_t *parent, MemoryDefender *def)
{
    if (node == nullptr)                     return NODE_PTR_IS_NULL;
    if (copy == nullptr || *copy != nullptr) return PTR_IS_NULL;

    // i have to save these ptr's or 
    // do some func that will free
    // this memory when it won't be used no more
    Node_t *new_node = (Node_t *) calloc(1, sizeof(Node_t));
    if (new_node == nullptr)                 return BAD_ALLOC;

    new_node->parent = parent;

    RT *arg = (RT *) calloc(1, sizeof(RT));
    if (arg == nullptr)                      return BAD_ALLOC;

    DefenderPush(def, (char *)arg);

    RT *check = (RT *) memcpy(arg, node->value, sizeof(RT));
    if (check != arg)                        return MEMCPY_CRASH;

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
    if (tree == nullptr)                             return TREE_IS_NULL;
    if (TreeVerify(tree))                            return TreeDump(tree);
    if (tree_res == nullptr || *tree_res != nullptr) return PTR_IS_NULL;

    Tree_t *new_tree = (Tree_t *) calloc(1, sizeof(Tree_t));
    if (new_tree == nullptr)                         return BAD_ALLOC;

    DefenderPush(def, (char *)new_tree);

    TreeCtor(new_tree);

    int status = FUNC_IS_OK;

    // TODO
    // tree->size doesnt work now

    status |= DiffNodes  (tree->root, &new_tree->root, nullptr, variable, def);

    status |= ClearStatus(tree->root);

    status |= Simplify   (new_tree, def);

    *tree_res = new_tree;

    return status;
}


int AssignValue(RT *arg, Node_t *left, Node_t *right, int num, int *flag)
{
    if (arg  == nullptr) return NODE_VALUE_IS_NULL;

    arg->type = NUM_TYPE;
    arg->num  = num;

    NodesDtor(left);
    NodesDtor(right);

    *flag = YE_CHANGES;

    return 0;
}

int Reconnect  (Node_t *save, Node_t *kill, Node_t** node_, int *flag, MemoryDefender *def)
{
    if (save  == nullptr ||  kill  == nullptr ||
        node_ == nullptr || *node_ == nullptr ||
        flag  == nullptr ||   def  == nullptr) return PTR_IS_NULL;

    Node_t *node = *node_;

    save->parent = node->parent;
    if (save->parent)
    {
        if (node->parent->left  == node) node->parent->left  = save;
        if (node->parent->right == node) node->parent->right = save;
    }

    DefenderPush(def, (char *) node);

    NodesDtor(kill);

    *flag  = YE_CHANGES;
    *node_ = save;

    return 0;
}

double Eval    (const char SIGN, double x, double y)
{
    switch (SIGN)
    {
        case ADD:
            return x + y;
        case MUL:
            return x * y;
        case SUB:
            return x - y;
        case POW:
            return pow(x, y);
        case DIV:
            return x / y;
        
        default:
            return NAN;
    }

    return NAN;
}

int SimplifyNodesNum (Node_t  *node,  int *flag)
{
    if (node        == nullptr) return NODE_PTR_IS_NULL; 
    if (flag        == nullptr) return FLAG_IS_NULL;
    if (node->value == nullptr) return NODE_VALUE_IS_NULL;
    int status = FUNC_IS_OK;

    RT *arg = node->value;

    if (arg->type            == OPER_TYPE &&
        LEFT  && LEFT->TYPE  == NUM_TYPE  &&
        RIGHT && RIGHT->TYPE == NUM_TYPE)
    {
        ASSIGN_VALUE(Eval(OP, LEFT->NUM, RIGHT->NUM));
        if (status) PRINT_D(status);
        return status;
    }

    // if (status) PRINT_D(status);

    if (status) PRINT_D(status);
    // PRINT_PTR(LEFT); // PRINT_PTR(RIGHT);
    if (LEFT)   status |= SimplifyNodesNum(LEFT,  flag);
    if (status) PRINT_D(status);

    // PRINT_PTR(LEFT); PRINT_PTR(RIGHT);
    if (RIGHT)  status |= SimplifyNodesNum(RIGHT, flag);
    if (status) PRINT_D(status);

    return status;
}

int SimplifyNodesUniq(Node_t **node_, int *flag, MemoryDefender *def)
{
    if (node_ == nullptr || node_ == nullptr) return NODE_PTR_IS_NULL;
    if (flag  == nullptr)                     return FLAG_IS_NULL;
    if (def   == nullptr)                     return DEF_IS_NULL;

    Node_t *node = *node_;

    if (node->value == nullptr)               return NODE_VALUE_IS_NULL;

    int status = FUNC_IS_OK;

    RT *arg = node->value;

    if (node->TYPE == OPER_TYPE)
    {
        switch (OP)
        {
            case MUL:   
                if (LEFT->TYPE  == NUM_TYPE && IsZeroNum (LEFT->NUM) ||
                    RIGHT->TYPE == NUM_TYPE && IsZeroNum (RIGHT->NUM))
                {
                    ASSIGN_VALUE(0);

                    return status;
                }
                if (LEFT->TYPE  == NUM_TYPE && IsEqualNum(LEFT->NUM,  1))
                {
                    RECONNECT(RIGHT, LEFT);

                    return status;
                }
                if (RIGHT->TYPE == NUM_TYPE && IsEqualNum(RIGHT->NUM, 1))
                {
                    RECONNECT(LEFT, RIGHT);

                    return status;
                }
                break;

            case ADD:
                if (LEFT->TYPE  == NUM_TYPE && IsZeroNum(LEFT->NUM))
                {
                    RECONNECT(RIGHT, LEFT);

                    return status;
                }
                if (RIGHT->TYPE == NUM_TYPE && IsZeroNum(RIGHT->NUM))
                {
                    RECONNECT(LEFT, RIGHT);

                    return status;
                }
                break;

            case SUB:
                if (LEFT->TYPE  == VAR_TYPE && RIGHT->TYPE == VAR_TYPE &&
                    RIGHT->VAR  == LEFT->VAR)
                {
                    ASSIGN_VALUE(0);

                    return status;
                }
                if (LEFT->TYPE  == NUM_TYPE && RIGHT->TYPE == NUM_TYPE &&
                    IsEqualNum(LEFT->NUM, RIGHT->NUM))
                {
                    ASSIGN_VALUE(0);

                    return status;
                }
                if (RIGHT->TYPE == NUM_TYPE && IsZeroNum (RIGHT->NUM))
                {
                    RECONNECT(LEFT, RIGHT);

                    return status;
                }
                break;

            case LG:
                if (LEFT->TYPE  == NUM_TYPE && IsEqualNum(LEFT->NUM,  1))
                {
                    ASSIGN_VALUE(0);
                
                    return status;
                }
                if (LEFT->TYPE  == NUM_TYPE && IsEqualNum(LEFT->NUM, 10))
                {
                    ASSIGN_VALUE(1);
                
                    return status;
                }
                break;
            case LN:
                if (LEFT->TYPE  == NUM_TYPE && IsEqualNum(LEFT->NUM,  1))
                {
                    ASSIGN_VALUE(0);
                
                    return status;
                }
                if (LEFT->TYPE  == OPER_TYPE && LEFT->OPER == EXP)
                {
                    ASSIGN_VALUE(1);
                
                    return status;
                }
                break;

            case DIV:
                if (LEFT->TYPE  == NUM_TYPE && IsZeroNum (LEFT->NUM))
                {
                    ASSIGN_VALUE(0);
                
                    return status;
                }
                if (RIGHT->TYPE == NUM_TYPE && IsEqualNum(RIGHT->NUM, 1))
                {
                    RECONNECT(LEFT, RIGHT);
                
                    return status;
                }
                if (LEFT->TYPE  == VAR_TYPE && RIGHT->TYPE == VAR_TYPE && 
                    LEFT->VAR == RIGHT->VAR)
                {
                    ASSIGN_VALUE(1);
                
                    return status;
                }
                break;

            case POW:
                if (RIGHT->TYPE == NUM_TYPE && IsZeroNum (RIGHT->NUM) ||
                    LEFT->TYPE  == NUM_TYPE && IsEqualNum(LEFT->NUM,  1))
                {
                    ASSIGN_VALUE(1);
                
                    return status;
                }
                if (RIGHT->TYPE == NUM_TYPE && IsEqualNum(RIGHT->NUM, 1))
                {
                    RECONNECT(LEFT, RIGHT);

                    return status;
                }
                break;

            default:
                break;
        }
    }

    if (LEFT)  status |= SimplifyNodesUniq(&LEFT,  flag, def);
    if (RIGHT) status |= SimplifyNodesUniq(&RIGHT, flag, def);

    return status;
}

int SimplifyNodes    (Node_t **node,  int *flag, MemoryDefender *def)
{
    if (node == nullptr || *node == nullptr) return NODE_PTR_IS_NULL;
    if (flag == nullptr)                     return FLAG_IS_NULL;
    if ((*node)->value == nullptr)           return NODE_VALUE_IS_NULL;
 
    int status = FUNC_IS_OK;

    PRINT_D(status);
    status |= SimplifyNodesNum(*node, flag);
    if (status) PRINT_D(status);
    // if (status) return status;

    status |= SimplifyNodesUniq(node, flag, def);
    if (status) PRINT_D(status);
    // if (status) return status;

    return status;
}

int Simplify         (Tree_t *tree, MemoryDefender *def)
{
    if (tree == nullptr)  return TREE_IS_NULL;
    if (TreeVerify(tree)) return TreeDump(tree);

    int status = FUNC_IS_OK;

    int flag   = NO_CHANGES;

    do
    {
        flag = NO_CHANGES;

        status |= SimplifyNodes(&tree->root, &flag, def);
        if (status) return status;
    }
    while (flag != NO_CHANGES);

    return status;
}


int main()
{
    int status = 0;
    Tree_t tree = {};

    status |= TreeCtor(&tree);
    if (status) PRINT_D(status);

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

    Tree_t *differentiated_tree = nullptr;

    // i haven't got any idea about diff_tree->size ))))))


    TexInit();

    status |= Differentiate(&tree, &differentiated_tree, 'x', &def);
    if (status) PRINT_D(status);

    TexPrint("В общем");

    OpenFormula();
    status |= TexOut(tree.root, 1, 1);
    if (status) PRINT_D(status);

    PrintOper(EQ);

    status |= TexOut(differentiated_tree->root, 1, 0);
    if (status) PRINT_D(status);

    ClosFormula();

    TexDestroy();

    status |= TreeDump(differentiated_tree);
    if (status) PRINT_D(status);

    TreeDtor(&tree);

    TreeDtor(differentiated_tree);

    DefenderClear(&def);

    DefenderDtor(&def);

    return 0;
}

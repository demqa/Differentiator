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
    if (c == '-' || c == '+' || c == '*' || c == '/' || c == '^')
        return 1;

    return 0;
}

int GetArg    (char *string, RT *arg)
{
    if (string == nullptr || arg == nullptr) return PTR_IS_NULL;

    size_t number = 0;

    size_t length = strlen(string);

    if (length == 0) return INVALID_STRING;

    if (length == 1 && IsCharOper(string[0]))
    {
        arg->type = OPER_TYPE;
        arg->oper = string[0];
        // arg->subtree_status = NOT_CALCULATED;
    }
    else
    if (length == 1 && 'a' < string[0] && string[0] < 'z')
    {
        arg->type = VAR_TYPE;
        arg->var  = string[0];
        // arg->subtree_status = VARIABLE;
    }
    else
    if (length == 2 && strcmp(string, "ln") == 0)
    {
        fprintf(stderr, "I can't do this yet %s\n", string);
        return IM_LITTLE_PROGRAM_DONT_SCARE_ME_WITH_THIS_MATH;
    }
    else
    if (length == 2 && strcmp(string, "lg") == 0)
    {
        fprintf(stderr, "I can't do this yet %s\n", string);
        return IM_LITTLE_PROGRAM_DONT_SCARE_ME_WITH_THIS_MATH;
    }
    else
    if (length == 2 && strcmp(string, "sh") == 0)
    {
        fprintf(stderr, "I can't do this yet %s\n", string);
        return IM_LITTLE_PROGRAM_DONT_SCARE_ME_WITH_THIS_MATH;
    }
    else
    if (length == 2 && strcmp(string, "ch") == 0)
    {
        fprintf(stderr, "I can't do this yet %s\n", string);
        return IM_LITTLE_PROGRAM_DONT_SCARE_ME_WITH_THIS_MATH;
    }
    else
    if (length == 3 && strcmp(string, "sin") == 0)
    {
        fprintf(stderr, "I can't do this yet %s\n", string);
        return IM_LITTLE_PROGRAM_DONT_SCARE_ME_WITH_THIS_MATH;
    }
    else
    if (length == 3 && strcmp(string, "cos") == 0)
    {
        fprintf(stderr, "I can't do this yet %s\n", string);
        return IM_LITTLE_PROGRAM_DONT_SCARE_ME_WITH_THIS_MATH;
    }
    else
    {
        int flag = 1;
        for (size_t i = 0; i < length; i++)
            if ((string[i] > '9' || string[i] < '0') && string[i] != '.')
                flag = 0;

        assert('0' < '9');

        if (flag)
        {
            double num = NAN;
            if (sscanf(string, "%lf", &num) == 1)
            {           
                arg->type = NUM_TYPE;
                arg->num  = num;
                // arg->subtree_status = CONST;
            }
            else
                return NUMBER_READING_ERROR;
        }
        else
        {
            fprintf(stderr, "UNKNOWN EXPRESSION %s\n", string);
            return UNKNOWN_EXPRESSION;
        }
    }

    return FUNC_IS_OK;
}

int ProceedNodeValue  (char **ptr, Node_t *node)
{
    int status = FUNC_IS_OK;

    char *string = (char *) calloc(MAX_EXPR_ELEM_LEN + 1, sizeof(char));
    if (string == nullptr) status |= BAD_ALLOC;

    PRINT_C(**ptr);
    status |= GetString(ptr, string);
    if (status) return status;

    RT *arg = (RT *) calloc(1, sizeof(RT));
    if (arg == nullptr)    status |= BAD_ALLOC;

    status |= GetArg(string, arg);
    if (status) return status;

    free(string);

    if (status) return status;

    node->value = arg;
    
    return status;
}

int TreeReadProcessing(Tree_t *tree, Node_t *node, char **ptr, char *end_ptr)
{
    if (tree == nullptr)   return TREE_IS_NULL;
    
    if (ptr  == nullptr || *ptr == nullptr || end_ptr == nullptr) return PTR_IS_NULL;

    if (*ptr > end_ptr)    return PTR_BIGGER_BUFF_END;

    int status = FUNC_IS_OK;

    while (*ptr <= end_ptr)
    {
        PRINT_C(**ptr);
        if (**ptr == '(')
        {
            (*ptr)++;

            PRINT_PTR(node);

            if (node == nullptr)
            {
                NodeInsert(tree, node, L_CHILD, nullptr);
                node = tree->root;
                PRINT_PTR(node);
            }
            else
            if (node->left == nullptr)
            {
                NodeInsert(tree, node, L_CHILD, nullptr);
                node = node->left;
                PRINT_PTR(node);
            }
            else
            {
                NodeInsert(tree, node, R_CHILD, nullptr);
                node = node->right;
                PRINT_PTR(node);
            }

            if (**ptr == '(' || **ptr == ')')
                continue;
            
            // number or variable
            status |= ProceedNodeValue(ptr, node);
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
            status |= ProceedNodeValue(ptr, node);
            if (status) return status;

            (*ptr)++;
        }
    }
    
    return status;
}

int TreeRead          (Tree_t *tree, char *buffer, size_t buff_size)
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
    int status = TreeReadProcessing(tree, tree->root, &ptr, end_ptr);
    if (status)
        return status;
    
    return FUNC_IS_OK;
}

int TreeFill          (Tree_t *tree, FILE *stream, char **buff)
{
    if (tree == nullptr)   return TREE_IS_NULL;

    if (stream == nullptr) return STREAM_IS_NULL;

    if (buff == nullptr || *buff != nullptr) return PTR_IS_NULL;
    
    size_t buff_size = 0;
    if (Filesize(stream, &buff_size)) return FILESIZE_IS_ZERO;

    char *buffer = nullptr;

    if (ReadBuffer(&buffer, stream))  return BUFFER_CANT_BE_READ;

    *buff = buffer;
    
    int status = TreeRead(tree, buffer, buff_size);

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

int IsConst(Node_t *node) /*Is Subtree Const*/
{
    return 0;
}

int IsZero (Node_t *node) /*Is Subtree Zero*/
{
    return 0;
}

int DiffNodes(Node_t *node, Node_t **diff, const char variable)
{
    if (node == nullptr)                     return NODE_PTR_IS_NULL;

    if (diff == nullptr || *diff != nullptr) return PTR_IS_NULL;

    Node_t *new_node = (Node_t *) calloc(1, sizeof(Node_t));
    if (new_node == nullptr) return BAD_ALLOC;

    RT *arg = node->value;

    int status = FUNC_IS_OK;
    if (arg->type == OPER_TYPE)
    {

    }
    else
    if (arg->type == NUM_TYPE)
    {

    }
    else
    if (arg->type == VAR_TYPE)
    {

    }
    else
        return EXCEPTION_UNEXPECTED_VALUE_TYPE;

    return status;
}

int CopyNodes(Node_t *node, Node_t **copy)
{
    if (node == nullptr)                     return NODE_PTR_IS_NULL;

    if (copy == nullptr || *copy != nullptr) return PTR_IS_NULL;

    // i have to save these ptr's or 
    // do some func that will free
    // this memory when it won't be used no more
    Node_t *new_node = (Node_t *) calloc(1, sizeof(node));
    if (new_node == nullptr) return BAD_ALLOC;

    RT *arg = (RT *) calloc(1, sizeof(RT));
    if (arg == nullptr)      return BAD_ALLOC;

    RT *check = (RT *) memcpy(arg, node->value, sizeof(RT));
    if (check != arg)        return MEMCPY_CRASH;

    new_node->value = arg;

    int status = FUNC_IS_OK;
    if (node->left  != nullptr)
        status |= CopyNodes(node->left,  &new_node->left);

    if (node->right != nullptr)
        status |= CopyNodes(node->right, &new_node->right);

    *copy = new_node;

    return status;
}

int Differentiate(Tree_t *tree, Tree_t **tree_res)
{
    if (tree == nullptr)  return TREE_IS_NULL;

    if (TreeVerify(tree)) return TreeDump(tree);

    if (tree_res == nullptr || *tree_res != nullptr)
        return PTR_IS_NULL;

    Tree_t *new_tree = (Tree_t *) calloc(1, sizeof(Tree_t));
    if (new_tree == nullptr) return BAD_ALLOC;

    // tree->size doesnt work now


    return 0;
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

    status |= TreeFill(&tree, stream, &buffer);
    if (status) fprintf(stderr, "status = %d\n", status);

    status |= TreeDump(&tree);
    if (status) fprintf(stderr, "status = %d\n", status);
    
    // i lose so many allocated memory...

    TreeDtor(&tree);

    return 0;
}


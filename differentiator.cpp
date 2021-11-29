#include "differentiator.h"
#include "BinaryTree/bin_tree.h"
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

int GetString (char **ptr, char *end_ptr, char **string)
{
    if (ptr == nullptr || *ptr == nullptr || end_ptr == nullptr) return PTR_IS_NULL;

    if (*ptr > end_ptr) return PTR_BIGGER_BUFF_END;

    *string    = *ptr;
    int number =  0;

    assert(**ptr != ')');

    while (*(*ptr + 1) != ')' && *(*ptr + 1) != '(')
    {
        if (*ptr + 1 >= end_ptr)          return TILT;

        if (number + 1 >= MAX_EXPR_ELEM_LEN) return MAX_EXPR_ELEM_LEN_REACHED;

        (*string)[number++] = **ptr;

        (*ptr)++;
    }

    assert(number < MAX_EXPR_ELEM_LEN);

    if (number == 0) return EXPR_ELEM_IS_EMPTY;

    if ((*string)[number] != ' ')
        fprintf(stderr, "WHY YOU CANT JUST PUT SPACE AFTER NAME\n");

    (*string)[number] = '\0';

    assert(*ptr < end_ptr);
    
    return DEAD_INSIDE;
}

int EvalString        (char *string, RT *arg)
{
    if (string == nullptr || arg == nullptr) return PTR_IS_NULL;

    // TODO

    return 0;
}

int TreeReadProcessing(Tree_t *tree, Node_t *node, char **ptr, char *end_ptr)
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

            char *string = nullptr;
            status |= GetString(ptr, end_ptr, &string);
            if (status) return status;

            RT arg = {};
            EvalString(string, &arg);
            
            (*ptr)++;

            if (node == nullptr)
            {
                NodeInsert(tree, node, L_CHILD, arg);
                node = tree->root;
            }
            else
            if (node->left == nullptr)
            {
                NodeInsert(tree, node, L_CHILD, arg);
                node = node->left;
            }
            else
            {
                NodeInsert(tree, node, R_CHILD, arg);
                node = node->right;
            }
        }
        else
        if (**ptr == ')')
        {
            if (node == nullptr) return FUCK_MY_LIFE;

            (*ptr)++;

            if (node == tree->root)
                break;

            node = node->parent;
        }
        else
        {
            assert(**ptr == '\n');
            break;
        }
    }
    
    return DEAD_INSIDE;
}

int TreeRead          (Tree_t *tree, char *buffer, size_t buff_size)
{
    if (tree == nullptr)   return TREE_IS_NULL;

    if (buffer == nullptr) return BUFFER_IS_NULL;

    if (buff_size == 0)    return FILESIZE_IS_ZERO;

    char *ptr     = buffer;
    char *end_ptr = buffer + buff_size - 1;

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

int ReadExpression    (Tree_t *tree)
{

    return 0;
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

int DiffNodes    (Node_t *node)
{
    if (node == nullptr) return NODE_PTR_IS_NULL;

    return 0;
}

int Differentiate(Tree_t *tree)
{
    if (tree == nullptr)  return TREE_IS_NULL;

    if (TreeVerify(tree)) return TreeDump(tree);

    return 0;
}

int main()
{
    return 0;
}


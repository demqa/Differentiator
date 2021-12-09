#include "tex.h"

static FILE *tex_out = nullptr;

int TexVerify()
{
    if (tex_out == nullptr)
        return 1;

    return 0;
}

int TexInit()
{
    if (tex_out != nullptr)
    {
        fprintf(stderr, "tex is already opened\n");
        return 0;
    }

    tex_out = fopen("out.tex", "w");
    if (tex_out == nullptr)
    {
        fprintf(stderr, "tex cringe\n");
        return -1;
    }

    fputs("\\usepackage [utf8x] {inputenc}\n", tex_out);
    fputs("\\documentclass{article}\n",        tex_out);
    fputs("\\begin{document}       \n",        tex_out);
    fputs("\n",                                tex_out);

}

int TexPrint(const char *str)
{
    if (str == nullptr) return -1;

    if (TexVerify())    return -1;

    fprintf(tex_out, "\\text{%s}\n", str);

    return 0;
}

int TexDestroy()
{
    if (TexVerify())
        return -1;

    fputs("\n",              tex_out);
    fputs("\n",              tex_out);
    fputs("\\end{document}", tex_out);

    fclose(tex_out);
    tex_out = nullptr;

    return 0;
}

int PrintOrd (Node_t *node, int prior_prev, int prior, int oper, Order order)
{
    if (node == nullptr)        return NODE_PTR_IS_NULL;

    if (order < 0 || order > 1) return WRONG_ORDER;

    if (order == PRE_ORDER) PrintOper(oper);

    if (prior < prior_prev) fputc('(',  tex_out);

    int status = FUNC_IS_OK;

    fputc('{',  tex_out);
    status |= TexOut(LEFT, prior,  0);
    fputc('}',  tex_out);

    if (order == IN__ORDER) PrintOper(oper);

    fputc('{',  tex_out);
    status |= TexOut(RIGHT, prior, 0);
    fputc('}',  tex_out);

    if (prior < prior_prev) fputc(')',  tex_out);

    return status;
}

int PrintFunc(Node_t *node, int oper)
{
    if (node == nullptr) return NODE_PTR_IS_NULL;

    int status = FUNC_IS_OK;

    PrintOper(oper);

    if (NodeIsTerminal(LEFT) != NODE_IS_TERMINAL) fputc('(', tex_out);

    status |= TexOut(LEFT, ADD_P, 0);

    if (NodeIsTerminal(LEFT) != NODE_IS_TERMINAL) fputc(')', tex_out);

    return status;
}   

int OpenBr()
{
    if (tex_out) fprintf(tex_out, "\\[");
}

int ClosBr()
{
    if (tex_out) fprintf(tex_out, "\\]\n");
}

int OpenF()
{
    if (tex_out) fprintf(tex_out, "{");
}

int ClosF()
{
    if (tex_out) fprintf(tex_out, "}");
}

int PrintOper(int oper)
{
    if (TexVerify()) return -1;

    switch (oper)
    {
        case MUL: fputs("\\cdot", tex_out); break;
        case ADD: fputs("+",      tex_out); break;
        case SUB: fputs("-",      tex_out); break;
        case POW: fputs("^",      tex_out); break;
        case DIV: fputs("\\frac", tex_out); break;
        case SIN: fputs("\\sin",  tex_out); break;
        case COS: fputs("\\cos",  tex_out); break;
        case LN:  fputs("\\ln",   tex_out); break;
        case LG:  fputs("\\lg",   tex_out); break;
        case SH:  fputs("\\sh",   tex_out); break;
        case CH:  fputs("\\ch",   tex_out); break;
        case PI:  fputs("\\pi",   tex_out); break;
        case EXP: fputs("e",      tex_out); break;
        case EQ:  fputs("=",      tex_out); break;

        default:
            fprintf(stderr, "WHAT THE HELL OPERATOR YOU WANT\n");
            return -1;
    }

    return 0;
}

int PrintNum (int num)
{
    if (TexVerify()) return -1;

    fprintf(tex_out, "%d", num);

    return 0;
}

int TexOut   (Node_t *node, int prior, int diff)
{
    if (TexVerify()) return -1;

    if (node == nullptr)        return NODE_PTR_IS_NULL;

    if (node->value == nullptr) return NODE_VALUE_IS_NULL;

    if (prior == 0)
        OpenBr();
    
    if (diff == 1) fprintf(tex_out, "(");

    int status = FUNC_IS_OK;

    if (node->TYPE == OPER_TYPE)
    {
        switch (node->OPER)
        {
            // PRINT, EXIT
            case PI:  PrintOper(node->OPER); break;
            case EXP: PrintOper(node->OPER); break;

            // PRINT LEFT, PRINT, PRINT RIGHT, EXIT
            case POW: status |= PrintOrd(node, prior, POW_P, POW, IN__ORDER); break; 
            case MUL: status |= PrintOrd(node, prior, MUL_P, MUL, IN__ORDER); break;
            case ADD: status |= PrintOrd(node, prior, ADD_P, ADD, IN__ORDER); break;
            case SUB: status |= PrintOrd(node, prior, ADD_P, SUB, IN__ORDER); break;

            // PRINT, PRINT LEFT, PRINT RIGHT, EXIT
            case DIV: status |= PrintOrd(node, prior, MUL_P, DIV, PRE_ORDER); break;
            
            // PRINT, PRINT LEFT, EXIT
            case SIN: status |= PrintFunc(node,              SIN            ); break;
            case COS: status |= PrintFunc(node,              COS            ); break;
            case LN:  status |= PrintFunc(node,              LN             ); break;
            case LG:  status |= PrintFunc(node,              LG             ); break;
            case SH:  status |= PrintFunc(node,              SH             ); break;
            case CH:  status |= PrintFunc(node,              CH             ); break;
            
            default:
                PRINT("gg, wp");
                return INVALID_OPERATOR;
        }
    }
    else
    if (node->TYPE == NUM_TYPE)
    {
        if (!(IsZeroNum(node->NUM) && node->parent && node->parent->OPER == SUB))
            fprintf(tex_out, " %lg ", node->NUM);
    }
    else
    if (node->TYPE == VAR_TYPE)
    {
        fprintf(tex_out, " %c ",  node->VAR);
    }
    else
    {
        fprintf(tex_out, " FATAL_ERROR\n ");
    }

    if (prior == 0)
        ClosBr();
    
    if (diff == 1) fprintf(tex_out, ")\'");

    return FUNC_IS_OK;
}

int TexDump  (Tree_t *tree)
{
    if (tree == nullptr)  return TREE_IS_NULL;

    if (TreeVerify(tree)) return TreeDump(tree);

    int status = FUNC_IS_OK;

    status |= TexOut(tree->root, 0, 0);

    return status;
}

int NumPhrase()
{
    if (Phrases == nullptr)
        return 0;

    return rand()%(sizeof(Phrases) / sizeof(Phrases[0]));
}

int Phrase()
{
    if (TexVerify() || Phrases == nullptr)
        return -1;
    
    return 0;

    fprintf(tex_out, "\n\\text{%s}\n", Phrases[NumPhrase()]);
    fputs("\n\n", tex_out);

    return 0;
}

// int TexStoryOut(Node_t *node, Node_t *diff)
// {
//     if (node == nullptr) return NODE_PTR_IS_NULL;

//     if (diff == nullptr) return NODE_PTR_IS_NULL;

//     int status = FUNC_IS_OK;

//     OpenBr();
//     status |= TexOut(node, 1, 1);

//     PrintOper(EQ);

//     // if (rand() < 5000)
//     // {
//     //     Phrase();
//     //     status |= TexOut(diff, 1, 0);
//     //     return status;
//     // }

//     if (node->TYPE == OPER_TYPE)
//     {
//         switch (node->OPER)
//         {
//             case ADD:
//             case SUB:
//                 status |= TexOut(LEFT,  1, 1);

//                 PrintOper(node->OPER);

//                 status |= TexOut(RIGHT, 1, 1);

//                 ClosBr();

//                 Phrase();
//                 status |= TexStoryOut(LEFT,  diff->left);
//                 status |= TexStoryOut(RIGHT, diff->right);
//                 break;
            
//             case MUL:
//                 status |= TexOut(LEFT,  2, 1);
//                 PrintOper(MUL);
//                 status |= TexOut(RIGHT, 2, 0);

//                 PrintOper(ADD);

//                 status |= TexOut(LEFT,  2, 0);
//                 PrintOper(MUL);
//                 status |= TexOut(RIGHT, 2, 1);
//                 ClosBr();

//                 Phrase();
//                 status |= TexStoryOut(LEFT,  diff->left->left);
//                 status |= TexStoryOut(RIGHT, diff->right->right);
//                 break;

//             case DIV:
//                 PrintOper(DIV);

//                 OpenF();

//                 status |= TexOut(LEFT,  2, 1);
//                 PrintOper(MUL);
//                 status |= TexOut(RIGHT, 2, 0);

//                 PrintOper(SUB);

//                 status |= TexOut(LEFT,  2, 0);
//                 PrintOper(MUL);
//                 status |= TexOut(RIGHT, 2, 1);

//                 ClosF();

//                 OpenF();

//                 OpenF();
//                 status |= TexOut(LEFT, 1, 0);
//                 ClosF();

//                 PrintOper(POW);

//                 PrintNum(2);

//                 ClosF();

//                 ClosBr();

//                 Phrase();
//                 status |= TexStoryOut(LEFT,  diff->left->left->left);
//                 status |= TexStoryOut(RIGHT, diff->left->right->right);
//                 break;

//             default:
//                 Phrase();
//                 status |= TexOut(diff, 1, 0);
//                 ClosBr();
//                 break;
//         }
//     }
//     else
//     if (node->TYPE == NUM_TYPE)
//     {
//         Phrase();
//         PrintNum(0);
//         //status |= TexOut(diff, 1, 0);
//         ClosBr();
//     }
//     else
//     if (node->TYPE == VAR_TYPE)
//     {
        
//         Phrase();
//         PrintNum(1);
//         // status |= TexOut(diff, 1, 0);
//         ClosBr();
//     }
//     else
//         return EXCEPTION_UNEXPECTED_VALUE_TYPE;

//     return status;
// }

// int TexStory (Tree_t *tree, MemoryDefender *def)
// {
//     if (tree == nullptr)       return TREE_IS_NULL;
    
//     if (TreeVerify(tree))      return TreeDump(tree);

//     if (tree->root == nullptr) return TREE_ROOT_IS_NULL;

//     if (def == nullptr)        return PTR_IS_NULL;

//     int status = FUNC_IS_OK;

//     TexInit();

//     OpenBr();

//     status |= TexOut(tree->root, 1, 1);

//     PrintOper(EQ);

//     Tree_t *diff = nullptr;

//     status |= Differentiate(tree, &diff, 'x', def);

//     // fputs("\\text{Предельно понятно, что это примерно}\n", tex_out);

//     status |= TexOut(diff->root, 1, 0);

//     ClosBr();

//     // fputs("\\text{Это конечно всё очень хорошо, но давайте по-порядку...}\n"
//     //       "\\text{Для начала:}\n", tex_out);
    
//     status |= TexStoryOut(tree->root, diff->root);

//     TexDestroy();

//     return status;
// }

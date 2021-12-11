#include "tex.h"


static FILE *tex_out = nullptr;

static int PRINT_CONST[3] = {};

enum PrintConst
{
    NUM_IND   = 0,
    VAR_IND_1 = 1,
    VAR_IND_2 = 2,
};

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

    


    fputs("\\input{header.tex}     \n", tex_out);
    fputs("\\usepackage{upgreek} \n\n", tex_out);
    
    fputs("\\begin{document}     \n\n", tex_out);

}

int TexPrint(const char *str)
{
    if (str == nullptr) return -1;

    if (TexVerify())    return -1;

    fprintf(tex_out, "%s\n", str);

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

    system("pdflatex out.tex > /dev/null");

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

int OpenFormula()
{
    if (tex_out) fprintf(tex_out, "\\begin{dmath}\n");
    // if (tex_out) fprintf(tex_out, "\\[");
    
}

int ClosFormula()
{
    if (tex_out) fprintf(tex_out, "\n\\end{dmath}\n");
    // if (tex_out) fprintf(tex_out, "\\]\n");
}

int OpenFig()
{
    if (tex_out) fprintf(tex_out, "{");
}

int ClosFig()
{
    if (tex_out) fprintf(tex_out, "}");
}

int OpenBr()
{
    if (tex_out) fprintf(tex_out, "(");
}

int ClosBr()
{
    if (tex_out) fprintf(tex_out, ")");
}

int PrintOper(int oper)
{
    if (TexVerify()) return -1;

    switch (oper)
    {
        case MUL: fputs(" \\cdot ",  tex_out); break;
        case ADD: fputs(" + ",       tex_out); break;
        case SUB: fputs(" - ",       tex_out); break;
        case POW: fputs(" ^ ",       tex_out); break;
        case DIV: fputs(" \\cfrac ", tex_out); break;
        case SIN: fputs(" \\sin ",   tex_out); break;
        case COS: fputs(" \\cos ",   tex_out); break;
        case LN:  fputs(" \\ln ",    tex_out); break;
        case LG:  fputs(" \\lg ",    tex_out); break;
        case SH:  fputs(" \\sh ",    tex_out); break;
        case CH:  fputs(" \\ch ",    tex_out); break;
        case PI:  fputs(" \\pi ",    tex_out); break;
        case EXP: fputs(" e ",       tex_out); break;
        case EQ:  fputs(" = ",       tex_out); break;

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
        OpenFormula();
    
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
        ClosFormula();
    
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

    fprintf(tex_out, "\n\n%s\n", Phrases[NumPhrase()]);
    fputs  ("\n\n", tex_out);

    return 0;
}

int DiffNodes(Node_t *node, Node_t **diff, Node_t *parent, const char variable, MemoryDefender *def)
{
    if (node == nullptr)                     return NODE_PTR_IS_NULL;

    if (diff == nullptr || *diff != nullptr) return PTR_IS_NULL;

    NODE_INIT(new_node, parent);

    if (node->value == nullptr)              return NODE_VALUE_IS_NULL;
        
    RT *arg = node->value;

    int status = FUNC_IS_OK;

    ARG__INIT (new_arg);

    ARG_ASSIGN(new_node, new_arg);

    // if (arg->type ==  NUM_TYPE &&  PRINT_CONST[NUM_IND]   || 
    //     arg->type ==  VAR_TYPE && (PRINT_CONST[VAR_IND_1] || PRINT_CONST[VAR_IND_2]))
    // {
        Phrase();

        OpenFormula();
        status |= TexOut(node, 1, 1);
        
        PrintOper(EQ);
    // }

    // if (rand() < 5000)
    // {
    //     Phrase();
    //     status |= TexOut(node, 1, 0);
    //     return status;
    // }

    *diff = new_node;
    Tree_t tr = {};

    if (arg->type == OPER_TYPE)
    {
        assert(NodeIsTerminal(node) != NODE_IS_TERMINAL || IsMathConst(OP));

        assert(arg->num == 0 && arg->var == 0);

        if (IsMathConst(OP))
        {
            PrintNum(0);
            ClosFormula();

            ZERO_INIT(new_arg);
        }

        if (arg->oper == ADD || arg->oper == SUB)
        {
            status |= TexOut(LEFT,  ADD_P, 1);

            PrintOper(OP);

            status |= TexOut(RIGHT, ADD_P, 1);

            ClosFormula();

            OPER_INIT(new_arg, arg->oper);
            
            DIFF(LEFT,  node, left);
            DIFF(RIGHT, node, right);
        }

        if (arg->oper == MUL)
        {
            status |= TexOut(LEFT,  MUL_P, 1);
            PrintOper(MUL);
            status |= TexOut(RIGHT, MUL_P, 0);

            PrintOper(ADD);

            status |= TexOut(LEFT,  MUL_P, 0);
            PrintOper(MUL);
            status |= TexOut(RIGHT, MUL_P, 1);

            ClosFormula();

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
            PrintOper(DIV);

            OpenFig();

            status |= TexOut(LEFT,  MUL_P, 1);
            PrintOper(MUL);
            status |= TexOut(RIGHT, MUL_P, 0);

            PrintOper(SUB);

            status |= TexOut(LEFT,  MUL_P, 0);
            PrintOper(MUL);
            status |= TexOut(RIGHT, MUL_P, 1);

            ClosFig();

            OpenFig();

            OpenFig();
            status |= TexOut(RIGHT, 1, 0);
            ClosFig();

            PrintOper(POW);

            PrintNum(2);

            ClosFig();

            ClosFormula();

            OPER_INIT(new_arg, DIV);

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
                OpenFig();

                OpenBr();

                status |= TexOut(RIGHT, ADD_P, 0);
                PrintOper(SUB);
                PrintNum (1);

                ClosBr();

                PrintOper(MUL);

                status |= TexOut(LEFT,  POW_P, 0);

                PrintOper(POW);

                OpenFig();

                status |= TexOut(RIGHT,  ADD_P, 0);
                PrintOper(SUB);
                PrintNum (1);

                ClosFig();

                PrintOper(MUL);

                status |= TexOut(LEFT,  MUL_P, 1);

                ClosFig();

                ClosFormula();


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
                OpenFig();

                status |= TexOut(LEFT,  POW_P, 0);

                ClosFig();

                PrintOper(POW);

                OpenFig();

                status |= TexOut(RIGHT, POW_P, 0);

                ClosFig();

                PrintOper(MUL);

                PrintOper(LN);
                status |= TexOut(LEFT,  MUL, 0);

                PrintOper(MUL);

                status |= TexOut(RIGHT, ADD_P, 1);

                ClosFormula();


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
                OpenFig();
                status |= TexOut(LEFT,  POW_P, 0);
                ClosFig();

                PrintOper(POW);

                OpenFig();
                status |= TexOut(RIGHT, POW_P, 0);
                ClosFig();

                PrintOper(MUL);

                OpenBr();

                PrintOper(LN);
                status |= TexOut(LEFT,  POW_P, 0);
                
                PrintOper(MUL);

                status |= TexOut(RIGHT, ADD_P, 1);

                PrintOper(ADD);

                PrintOper(DIV);

                OpenFig();
                status |= TexOut(LEFT,  ADD_P, 1);
                PrintOper(MUL);
                status |= TexOut(RIGHT, MUL_P, 0);
                ClosFig();

                OpenFig();
                status |= TexOut(LEFT,  MUL_P, 0);
                ClosFig();

                ClosBr();

                ClosFormula();

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
                PrintNum(0);
                ClosFormula();

                NUM__INIT(new_arg, 0);
                return status;
            }
            
        }

        if (arg->oper == SIN)
        {
            PrintOper(COS);
            status |= TexOut(LEFT,  POW_P, 0);

            PrintOper(MUL);

            status |= TexOut(LEFT,  ADD_P, 1);

            ClosFormula();

            OPER_INIT(new_arg, MUL);

            OP_NODE_INIT(COS, L, node, left);
            COPY(LEFT, L, left);

            DIFF(LEFT, node, right);
        }

        if (arg->oper == COS)
        {
            PrintOper(SUB);
            PrintOper(SIN);
            status |= TexOut(LEFT,  POW_P, 0);

            PrintOper(MUL);

            status |= TexOut(LEFT,  ADD_P, 1);

            ClosFormula();

            OPER_INIT(new_arg, MUL);

            OP_NODE_INIT(SUB, L, node, left);

            NUM_NODE_INIT(0, LL, L, left);

            OP_NODE_INIT(SIN, LR, L, right);
            COPY(LEFT, LR, left);

            DIFF(LEFT, node, right);
        }

        if (arg->oper == LN)
        {
            PrintOper(DIV);

            OpenFig();
            status |= TexOut(LEFT,  ADD_P, 1);
            ClosFig();

            OpenFig();
            status |= TexOut(LEFT,  ADD_P, 0);
            ClosFig();

            ClosFormula();

            OPER_INIT(new_arg, DIV);

            DIFF(LEFT, node, left);
            
            COPY(LEFT, node, right);
        }

        if (arg->oper == LG)
        {
            PrintOper(DIV);

            OpenFig();
            status |= TexOut(LEFT,  ADD_P, 1);
            ClosFig();

            OpenFig();
            status |= TexOut(LEFT,  MUL_P, 0);
            PrintOper(MUL);
            PrintOper(LN);
            PrintNum (10);
            ClosFig();

            ClosFormula();

            OPER_INIT(new_arg, DIV);

            DIFF(LEFT, node, left);
            
            OP_NODE_INIT(MUL, R, node, right);
            COPY(LEFT, R, left);
            OP_NODE_INIT(LN, RR, R, right);
            NUM_NODE_INIT(10, RRL, RR, left);
        }

        if (arg->oper == SH)
        {
            PrintOper(CH);
            status |= TexOut(LEFT,  POW_P, 0);

            PrintOper(MUL);

            status |= TexOut(LEFT,  ADD_P, 1);

            ClosFormula();

            OPER_INIT(new_arg, MUL);

            OP_NODE_INIT(CH, L, node, left);
            COPY(LEFT, L, left);

            DIFF(LEFT, node, right);
        }

        if (arg->oper == CH)
        {
            PrintOper(SH);
            status |= TexOut(LEFT,  POW_P, 0);

            PrintOper(MUL);

            status |= TexOut(LEFT,  ADD_P, 1);

            ClosFormula();

            OPER_INIT(new_arg, MUL);

            OP_NODE_INIT(SH, L, node, left);
            COPY(LEFT, L, left);

            DIFF(LEFT, node, right);
        }
    }
    else
    if (arg->type ==  NUM_TYPE)
    {
        assert(NodeIsTerminal(node) == NODE_IS_TERMINAL);

        assert(arg->oper == 0 && arg->var == 0);

        // if (PRINT_CONST[0]++ == 0)
        // {
            PrintNum   (0);
            ClosFormula();
        // }

        ZERO_INIT(new_arg);
    }
    else
    if (arg->type ==  VAR_TYPE)
    {
        assert(NodeIsTerminal(node) == NODE_IS_TERMINAL);

        assert(arg->oper == 0 && arg->num == 0);

        if (arg->var == variable)
        {
            // if (PRINT_CONST[1]++ == 0)
            // {
                PrintNum(1);
                ClosFormula();
            // }

            ONE__INIT(new_arg);
        }
        else
        {
            // if (PRINT_CONST[2]++ == 0)
            // {
                PrintNum(0);
                ClosFormula();
            // }

            ZERO_INIT(new_arg);
        }
    }   
    else
        return EXCEPTION_UNEXPECTED_VALUE_TYPE;

    return status;
}

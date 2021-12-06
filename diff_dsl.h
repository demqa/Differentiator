#ifndef DIFF_DSL_H

#define DIFF_DSL_H

#define OP arg->oper

#define OPER_INIT(ARG, OPER)                    \
do                                               \
{                                                 \
    assert(ARG->type == 0 && ARG->oper == 0 && ARG->num == 0 && ARG->var == 0); \
    ARG->type = OPER_TYPE;                          \
    ARG->oper = OPER;                                \
}                                                     \
while (0)

#define NODE_INIT(NAME, PARENT)                          \
    Node_t *NAME = (Node_t *) calloc(1, sizeof(Node_t));  \
    if (NAME == nullptr) return BAD_ALLOC;                 \
    NAME->parent = PARENT;

#define ARG__INIT(NAME)                                       \
    RT *NAME = (RT *) calloc(1, sizeof(RT));                   \
    if (NAME == nullptr) return BAD_ALLOC;                      \
    DefenderPush(def, (char *)NAME);

#define ARG_ASSIGN(NODE, ARG)                                      \
    NODE->value = ARG;

#define NUM__INIT(ARG, NUM)  \
do                            \
{                              \
    assert(ARG->type == 0 && ARG->oper == 0 && ARG->num == 0 && ARG->var == 0); \
    ARG->type = NUM_TYPE;        \
    ARG->num  = NUM;              \
}                                  \
while (0)

#define ZERO_INIT(ARG) NUM__INIT(ARG, 0);
#define ONE__INIT(ARG) NUM__INIT(ARG, 1);

#define LEFT  node->left
#define RIGHT node->right

#define OP_NODE_INIT(OP_, AP, PAR, SIDE)   \
    NODE_INIT(new_ ## AP, new_ ## PAR);     \
    ARG__INIT(arg_ ## AP);                   \
    OPER_INIT(arg_ ## AP, OP_);               \
    ARG_ASSIGN(new_ ## AP, arg_ ## AP);        \
    (new_ ## PAR)->SIDE  = (new_ ## AP);          

#define NUM_NODE_INIT(NUM, AP, PAR, SIDE)         \
    NODE_INIT(new_ ## AP, new_ ## PAR);            \
    ARG__INIT(arg_ ## AP);                          \
    NUM__INIT(arg_ ## AP, NUM);                      \
    ARG_ASSIGN(new_ ## AP, arg_ ## AP);               \
    (new_ ## PAR)->SIDE  = (new_ ## AP);

#define DIFF(FROM, DEST, SIDE)                           \
    status |= DiffNodes(FROM, &(new_ ## DEST)->SIDE, new_ ## DEST, variable, def);

#define COPY(FROM, DEST, SIDE)                              \
    status |= CopyNodes(FROM, &(new_ ## DEST)->SIDE, new_ ## DEST, def);

#endif // DIFF_DSL_H

#ifndef DIFF_DSL_H

#define DIFF_DSL_H

#define OP   arg->oper

#define TYPE value->type

#define NUM  value->num

#define VAR  value->var

#define OPER value->oper

#define NEW_NODE(NAME)                                   \
    Node_t *NAME = (Node_t *) calloc(1, sizeof(Node_t));  \
    if (NAME == nullptr) return nullptr;

#define NEW_ARG(NODE, NAME)                                  \
    RT *NAME = (RT *) calloc(1, sizeof(RT));                  \
    if (NAME == nullptr) return nullptr;                       \
    DefenderPush(def, (char *)NAME);                            \
    NODE->value = NAME;
    

#define OPER_ARG(ARG, OPER_)  \
    ARG->type = OPER_TYPE;     \
    ARG->oper = OPER_;

#define NUM_ARG(ARG, NUM_)        \
    ARG->type = NUM_TYPE;          \
    ARG->num  = NUM_;

#define VAR_ARG(ARG, VAR)             \
    ARG->type = VAR_TYPE;              \
    ARG->var  = VAR;

#define CONNECT(PAR, CHI, SIDE)           \
    PAR->SIDE   = CHI;                     \
    CHI->parent = PAR;


#define OPER_INIT(ARG, OPER_)                   \
do                                               \
{                                                 \
    assert(ARG->type == 0 && ARG->oper == 0 && ARG->num == 0 && ARG->var == 0); \
    ARG->type = OPER_TYPE;                          \
    ARG->oper = OPER_;                               \
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

#define NUM__INIT(ARG, NUM_) \
do                            \
{                              \
    assert(ARG->type == 0 && ARG->oper == 0 && ARG->num == 0 && ARG->var == 0); \
    ARG->type = NUM_TYPE;        \
    ARG->num  = NUM_;             \
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

#define NUM_NODE_INIT(NUM_, AP, PAR, SIDE)        \
    NODE_INIT(new_ ## AP, new_ ## PAR);            \
    ARG__INIT(arg_ ## AP);                          \
    NUM__INIT(arg_ ## AP, NUM_);                     \
    ARG_ASSIGN(new_ ## AP, arg_ ## AP);               \
    (new_ ## PAR)->SIDE  = (new_ ## AP);

#define DIFF(FROM, DEST, SIDE)                           \
    status |= DiffNodes(FROM, &(new_ ## DEST)->SIDE, new_ ## DEST, variable, def);

#define COPY(FROM, DEST, SIDE)                              \
    status |= CopyNodes(FROM, &(new_ ## DEST)->SIDE, new_ ## DEST, def);

#define RECONNECT(SAVE, KILL)                                  \
    status |= Reconnect(SAVE, KILL, node_, flag, def);

#define ASSIGN_VALUE(NUMBER)                                      \
    status |= AssignValue(arg, LEFT, RIGHT, NUMBER, flag);        

// do                                                      
// {                                                    
//     arg->type = NUM_TYPE;                             
//     arg->num  = NUMBER;                               
//                                                         
//     NodesDtor(LEFT);                                     
//     NodesDtor(RIGHT);                                     
//                                                            
//     LEFT  = nullptr;                                        
//     RIGHT = nullptr;                                         
//                                                               
//     *flag = YE_CHANGES;                                        
// }                                                               

#endif // DIFF_DSL_H

#ifndef TEX_H

#define TEX_H

#include "differentiator.h"

enum Priority
{
    ADD_P = 1,
    MUL_P = 2,
    POW_P = 3,
};

enum Order
{
    PRE_ORDER = 0,
    IN__ORDER = 1,
};

const int PHRASE_MAX_LEN = 100;

const char Phrases[][PHRASE_MAX_LEN] = 
{
    "А пошли они на хутор, бабочек ловить.",
    "Пошёл ты нахуй, мусор, Я drum&bass продюсер",
    "Эники-беники, ели вареники...",
    "Как же хочется простого человеческого...",
    "А ведь папа говорил, что нужно учиться...",
    "Для любого эпсилон известно, что...",
    "Сим-салабим",
    "Совершенно очевидно, что",
    "Об этом даже не стоит упоминать, но",
    "Ну так чего же мы сидим?",
    "Это всё понятно, а что более интересно, так это",
    "Как же я ненавижу дифференцировать..."

};

int PrintOper(int oper);


int TexInit();
int TexDestroy();

int TexPrint(const char *);

int TexOut (Node_t *node, int prior, int diff);
int TexDump(Tree_t *tree);
// int TexStory (Tree_t *tree, MemoryDefender *def);

#endif // TEX_H

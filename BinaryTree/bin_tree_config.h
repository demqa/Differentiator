#ifndef BIN_TREE_CONFIG_H

#define BIN_TREE_CONFIG_H

#include <assert.h>

#include "../differentiator.h"

enum ValueType
{
    OPER_TYPE = 1,
    NUM_TYPE  = 2,
    VAR_TYPE  = 3,
};

struct RT
{
    int type;

    char   oper;
    double num;
    char   var;

    int subtree_status;
};

typedef RT *Value_t;

const RT DEAD_ELEM     = {1000, '-', 7, (char )'RT', 228};

const Value_t DEAD_VAL = nullptr;

// FILE *TREE_LOG_FILE_DEFAULT = stderr;

const size_t COLOR_MAX_LEN = 7 + 1;

const char left_color    [COLOR_MAX_LEN] = "#7100c7";
const char right_color   [COLOR_MAX_LEN] = "#7100c7";

const char colors[][COLOR_MAX_LEN] =
{
    "#ff002f",
    "#f6fa00",
    "#14d4ff",
    "#00ed14",
};

#endif
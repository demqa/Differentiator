#ifndef BIN_TREE_CONFIG_H

#define BIN_TREE_CONFIG_H

#include <assert.h>

#include "../differentiator.h"
#include "../debug_lib.h"

enum ValueType
{
    VAR_TYPE  = 1,
    NUM_TYPE  = 2,
    OPER_TYPE = 3,
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

const RT DEAD_ELEM     = {1000, '-', 7, (char )'R', 228};

const Value_t DEAD_VAL = nullptr;

// FILE *TREE_LOG_FILE_DEFAULT = stderr;

const size_t COLOR_MAX_LEN = 10 + 1;
const size_t SHAPE_MAX_LEN = 14 + 1;

const char left_color    [COLOR_MAX_LEN] = "#7100c7";
const char right_color   [COLOR_MAX_LEN] = "#7100c7";

const char colors[][COLOR_MAX_LEN] =
{
    "#ff002f",
    "#f6fa00",
    "#14d4ff",
    "#00ed14",
    "#ff8b1f",
};

const char shapes[][SHAPE_MAX_LEN] = 
{
    "octagon",
    "hexagon",
    "invtriangle",
    "parallelogram",
    "ellipse",
};

#endif
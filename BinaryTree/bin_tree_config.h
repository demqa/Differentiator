#ifndef BIN_TREE_CONFIG_H

#define BIN_TREE_CONFIG_H

#include <assert.h>

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

const char root_color    [COLOR_MAX_LEN] = "#f4a1f7";
const char terminal_color[COLOR_MAX_LEN] = "#5ebfff";
const char default_color [COLOR_MAX_LEN] = "#63e69e";

const char left_color    [COLOR_MAX_LEN] = "#3cf71b";
const char right_color   [COLOR_MAX_LEN] = "#ff0000";

#endif
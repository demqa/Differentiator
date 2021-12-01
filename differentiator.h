#ifndef DIFFERENTIATOR_H

#define DIFFERENTIATOR_H

#include <math.h>

#include "BinaryTree/bin_tree.h"

#include "differentiator_config.h"

enum ValueType
{
    OPER_TYPE = 1,
    NUM_TYPE  = 2,
    VAR_TYPE  = 3,
};

enum IsConstStatus
{
    NOT_CALCULATED   = 0,
    CONST            = 1,
    VARIABLE         = 2,
};

enum DifferentiatorStatus
{
    // statuses
    DEAD_INSIDE = 0,
    FUNC_IS_OK  = 0,
    TILT        = 1,

    // errors
    EXPR_IS_INVALID,
    BUFFER_IS_NULL,
    STREAM_IS_NULL,
    PTR_IS_NULL,
    FILESIZE_IS_ZERO,
    BUFFER_CANT_BE_READ,
    FUCK_MY_LIFE,
    PTR_BIGGER_BUFF_END,
    MAX_EXPR_ELEM_LEN_REACHED,
    EXPR_ELEM_IS_EMPTY,
    MEMCPY_CRASH,
    EXCEPTION_UNEXPECTED_VALUE_TYPE,
    UNKNOWN_EXPRESSION,
    NUMBER_READING_ERROR,
    INVALID_STRING,
    IM_LITTLE_PROGRAM_DONT_SCARE_ME_WITH_THIS_MATH,

};

#endif


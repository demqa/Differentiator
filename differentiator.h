#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include <math.h>
#include <ctype.h>

#include "BinaryTree/bin_tree.h"
#include "MemoryDefender/memory_defender.h"

#include "differentiator_config.h"

#include "diff_dsl.h"

// enum ValueType
// {
//     VAR_TYPE  = 1,
//     NUM_TYPE  = 2,
//     OPER_TYPE = 3,
// };

enum SubtreeStatus
{
    NOT_CALCULATED   = 0,
    VARIABLE         = 1,

    CONST            = 1 << 4,
};

enum Changes
{
    NO_CHANGES = 0,
    YE_CHANGES = 1,
};

const char ADD = '+';
const char SUB = '-';
const char MUL = '*';
const char DIV = '/';
const char POW = '^';

const char SIN = 's';
const char COS = 'c';
const char LN  = 'l';
const char LG  = 'g';
const char SH  = 'w';
const char CH  = 'h';

enum DifferentiatorStatus
{
    // statuses
    DEAD_INSIDE      = 0,
    FUNC_IS_OK       = 0,
    TILT             = 1,

    // errors
    EXPR_IS_INVALID                                  = 1 << 2,
    BUFFER_IS_NULL                                   = 1 << 3,
    STREAM_IS_NULL                                   = 1 << 4,
    PTR_IS_NULL                                      = 1 << 5,
    FILESIZE_IS_ZERO                                 = 1 << 6,
    BUFFER_CANT_BE_READ                              = 1 << 7,
    FUCK_MY_LIFE                                     = 1 << 8,
    PTR_BIGGER_BUFF_END                              = 1 << 9,
    MAX_EXPR_ELEM_LEN_REACHED                        = 1 << 10,
    EXPR_ELEM_IS_EMPTY                               = 1 << 11,
    MEMCPY_CRASH                                     = 1 << 12,
    EXCEPTION_UNEXPECTED_VALUE_TYPE                  = 1 << 13,
    UNKNOWN_EXPRESSION                               = 1 << 14,
    NUMBER_READING_ERROR                             = 1 << 15,
    INVALID_STRING                                   = 1 << 16,
    IM_LITTLE_PROGRAM_DONT_SCARE_ME_WITH_THIS_MATH   = 1 << 17,
    PARENT_IS_NULL                                   = 1 << 18,
    INVALID_OPERATOR                                 = 1 << 19,
    NODE_VALUE_IS_NULL                               = 1 << 20,
    FLAG_IS_NULL                                     = 1 << 21,

};

#endif // DIFFERENTIATOR_H

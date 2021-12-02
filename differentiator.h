#ifndef DIFFERENTIATOR_H

#define DIFFERENTIATOR_H

#include <math.h>

#include "BinaryTree/bin_tree.h"

#include "differentiator_config.h"

// enum ValueType
// {
//     OPER_TYPE = 1,
//     NUM_TYPE  = 2,
//     VAR_TYPE  = 3,
// };

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
    EXPR_IS_INVALID                                = 1 << 2,
    BUFFER_IS_NULL                                 = 1 << 3,
    STREAM_IS_NULL                                 = 1 << 4,
    PTR_IS_NULL                                    = 1 << 5,
    FILESIZE_IS_ZERO                               = 1 << 6,
    BUFFER_CANT_BE_READ                            = 1 << 7,
    FUCK_MY_LIFE                                   = 1 << 8,
    PTR_BIGGER_BUFF_END                            = 1 << 9,
    MAX_EXPR_ELEM_LEN_REACHED                      = 1 << 10,
    EXPR_ELEM_IS_EMPTY                             = 1 << 11,
    MEMCPY_CRASH                                   = 1 << 12,
    EXCEPTION_UNEXPECTED_VALUE_TYPE                = 1 << 13,
    UNKNOWN_EXPRESSION                             = 1 << 14,
    NUMBER_READING_ERROR                           = 1 << 15,
    INVALID_STRING                                 = 1 << 16,
    IM_LITTLE_PROGRAM_DONT_SCARE_ME_WITH_THIS_MATH = 1 << 17,

};

#endif


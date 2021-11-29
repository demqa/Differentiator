#ifndef DIFFERENTIATOR_H

#define DIFFERENTIATOR_H

#include <math.h>

#include "differentiator_config.h"

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
};

#endif


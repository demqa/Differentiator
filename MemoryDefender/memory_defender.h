#ifndef MEMORY_DEFENDER_H

#define MEMORY_DEFENDER_H

#include <stdlib.h>

#include "memory_defender_config.h"

struct MemoryDefender
{
    size_t size;
    size_t capacity;
    
    int    status;

    char **allocs;
};

namespace Defender
{
    enum Error
    {
        OK             = 0,

        PTR_IS_NULL    = 1 << 0,
        ALLOCS_NULL    = 1 << 1,
        CANT_PUSH_NULL = 1 << 2,
        BAD_ALLOC      = 1 << 3,
        ARRAY_IS_FULL  = 1 << 4,

    };
}

int DefenderCtor  (MemoryDefender *defender);
int DefenderPush  (MemoryDefender *defender, char *memory);
int DefenderIsFull(MemoryDefender *defender);
int DefenderClear (MemoryDefender *defender);
int DefenderDtor  (MemoryDefender *defender);

#endif // MEMORY_DEFENDER_H
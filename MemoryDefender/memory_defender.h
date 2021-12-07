#ifndef MEMORY_DEFENDER_H

#define MEMORY_DEFENDER_H

#include <stdlib.h>

#include "memory_defender_config.h"

struct MemoryDefender
{
    size_t size;
    size_t capacity;
    char **allocs;
};

int DefenderCtor  (MemoryDefender *defender);
int DefenderPush  (MemoryDefender *defender, char *memory);
int DefenderIsFull(MemoryDefender *defender);
int DefenderClear (MemoryDefender *defender);
int DefenderDtor  (MemoryDefender *defender);

#endif // MEMORY_DEFENDER_H
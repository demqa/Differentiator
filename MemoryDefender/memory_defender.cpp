#include "memory_defender.h"

int DefenderCtor  (MemoryDefender *defender)
{
    if (defender == nullptr)         return -1;

    defender->allocs   = (char **) calloc(MAX_ALLOCS, sizeof(char *));

    defender->size     = 0;
    defender->capacity = MAX_ALLOCS;

    return 0;
}

int DefenderPush  (MemoryDefender *defender, char *memory)
{
    if (defender == nullptr)         return -1;
           
    if (memory   == nullptr)         return -2;

    if (defender->size + 1 >= defender->capacity)
        return -3;
    
    defender->allocs[defender->size++] = memory;

    return 0;
}

int DefenderDtor  (MemoryDefender *defender)
{
    if (defender == nullptr)         return -1;

    defender->size     = 0;
    defender->capacity = 0;
    free(defender->allocs);

    return 0;
}

int DefenderClear (MemoryDefender *defender)
{
    if (defender == nullptr)         return -1;

    if (defender->allocs == nullptr) return -2;

    for (size_t iter = 0; iter < defender->size; iter++)
    {
        free(defender->allocs[iter]);
        defender->allocs[iter] = nullptr;
    }
    defender->size = 0;

    return 0;
}

int DefenderIsFull(MemoryDefender *defender)
{
    if (defender == nullptr)         return -1;

    return (defender->size + 1 >= defender->capacity) ? 1 : 0;
}
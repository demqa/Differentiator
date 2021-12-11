#include "memory_defender.h"

int DefenderCtor  (MemoryDefender *defender)
{
    if (defender == nullptr)         return Defender::PTR_IS_NULL;

    defender->allocs     = (char **) calloc(Defender::MAX_ALLOCS, sizeof(char *));
    if (defender->allocs == nullptr)
    {
        defender->status = Defender::BAD_ALLOC;
        return defender->status;
    }

    defender->size     = 0;
    defender->capacity = Defender::MAX_ALLOCS;

    return 0;
}

int DefenderPush  (MemoryDefender *defender, char *memory)
{
    if (defender == nullptr)         return Defender::PTR_IS_NULL;
           
    if (memory   == nullptr)         return Defender::CANT_PUSH_NULL;

    if (defender->size + 1 >= defender->capacity)
    {
        defender->status = Defender::ARRAY_IS_FULL;
        return defender->status;
    }
    
    defender->allocs[defender->size++] = memory;

    return 0;
}

int DefenderDtor  (MemoryDefender *defender)
{
    if (defender == nullptr)         return Defender::PTR_IS_NULL;

    defender->size     = 0;
    defender->capacity = 0;
    free(defender->allocs);

    return 0;
}

int DefenderClear (MemoryDefender *defender)
{
    if (defender         == nullptr) return Defender::PTR_IS_NULL;

    if (defender->allocs == nullptr) return Defender::ALLOCS_NULL;

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
    if (defender == nullptr)         return Defender::PTR_IS_NULL;

    return (defender->size + 1 >= defender->capacity) ? Defender::ARRAY_IS_FULL : 0;
}
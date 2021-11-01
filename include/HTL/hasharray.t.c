#ifdef HTL_PARAM
#include <string.h>

HTL_DEF bool HTL_MEMBER(Create)(HTL_P(NAME)* hasharray)
{
    memset(hasharray, 0, sizeof *hasharray);
    return 1;
}

HTL_DEF void HTL_MEMBER(Destroy)(HTL_P(NAME)* hasharray)
{
    HTL_FREE(hasharray->table);
}


HTL_DEF HTL_P(ENTRY_TYPE)* HTL_MEMBER(Locate)(HTL_P(NAME)* hasharray, HTL_P(KEY_TYPE) key)
{
#if HTL_HASHARRAY_CACHE_LAST
    if(hasharray->cached_entry && HTL_P(COMP_FUNC)(key, HTL_P(KEY_FUNC)(hasharray->cached_entry))==0)
        return hasharray->cached_entry;
#endif

    size_t hashid = HTL_P(HASH_FUNC)(key)&(hasharray->capacity-1);
    size_t i;
    HTL_P(ENTRY_TYPE)* entry;

    for(i = hashid; i < hasharray->capacity; ++i)
    {
        entry = &hasharray->table[i];
        if(!HTL_MEMBER(IsEntry)(hasharray, entry))           goto EMPTY_ENTRY_FOUND;
        if(HTL_P(COMP_FUNC)(key, HTL_P(KEY_FUNC)(entry))==0) goto ENTRY_FOUND;
    }
    for(i = 0; /*i < hashid*/; ++i)
    {
        entry = &hasharray->table[i];
        if(!HTL_MEMBER(IsEntry)(hasharray, entry))           goto EMPTY_ENTRY_FOUND;
        if(HTL_P(COMP_FUNC)(key, HTL_P(KEY_FUNC)(entry))==0) goto ENTRY_FOUND;
    }

ENTRY_FOUND:
#if HTL_HASHARRAY_CACHE_LAST
    hasharray->cached_entry = entry;
#endif
EMPTY_ENTRY_FOUND:
    return entry;
}


HTL_DEF HTL_P(ENTRY_TYPE)* HTL_MEMBER(PutAlloc)(HTL_P(NAME)* hasharray, HTL_P(KEY_TYPE) key)
{
    if(hasharray->size * HTL_HASHARRAY_LOAD_DEN >= hasharray->capacity * HTL_HASHARRAY_LOAD_NUM)
    {
        if(hasharray->capacity != 0)
        {
            HTL_P(ENTRY_TYPE)* entry = HTL_MEMBER(Locate)(hasharray, key);
            if(HTL_MEMBER(IsEntry)(hasharray, entry)) return entry;
        }

        size_t old_capacity, new_capacity;
        HTL_P(ENTRY_TYPE)* old_table;
        HTL_P(ENTRY_TYPE)* new_table;
        new_capacity = (hasharray->capacity == 0) ? HTL_HASHARRAY_INITIAL_CAPACITY : hasharray->capacity * HTL_HASHARRAY_GROW_FACTOR;
        /*printf("A " HTL_STR(HTL_P(NAME)) " was resized to a capacity of %zu, with current size %zu\n",
               new_capacity, hasharray->size);*/
        new_table = HTL_CALLOC(new_capacity, (sizeof *new_table));
        if(new_table == NULL) return NULL;

        old_table = hasharray->table;
        old_capacity = hasharray->capacity;

        hasharray->table = new_table;
        hasharray->capacity = new_capacity;
        //hasharray->size = hasharray->size;

#if HTL_HASHARRAY_CACHE_LAST
        hasharray->cached_entry = NULL;
#endif

        for(size_t i = 0; i < old_capacity; ++i)
        {
            if(HTL_MEMBER(IsEntry)(hasharray, &old_table[i]))
            {
                HTL_P(ENTRY_TYPE)* e = HTL_MEMBER(Locate)(hasharray, HTL_P(KEY_FUNC)(&old_table[i]));
                *e = old_table[i]; // memcpy
            }
        }
        HTL_FREE(old_table);
    }

    HTL_P(ENTRY_TYPE)* entry = HTL_MEMBER(Locate)(hasharray, key);
    if(!HTL_MEMBER(IsEntry)(hasharray, entry))
        ++hasharray->size;
    return entry;
}


HTL_DEF HTL_P(ENTRY_TYPE)* HTL_MEMBER(Get)(HTL_P(NAME)* hasharray, HTL_P(KEY_TYPE) key)
{
    if(hasharray->capacity)
    {
        HTL_P(ENTRY_TYPE)* entry = HTL_MEMBER(Locate)(hasharray, key);
        if(HTL_MEMBER(IsEntry)(hasharray, entry)) return entry;
    }
    return NULL;
}

HTL_DEF void HTL_MEMBER(RemoveEntry)(HTL_P(NAME)* hasharray, HTL_P(ENTRY_TYPE)* entry)
{
    size_t i = entry-hasharray->table;

    --hasharray->size;

    size_t empty_pos = i;
    for(;;)
    {
        ++i;
        if(i == hasharray->capacity) i = 0;
        if(!HTL_MEMBER(IsEntry)(hasharray, &hasharray->table[i])) break;

        size_t best_index = HTL_P(HASH_FUNC)(HTL_P(KEY_FUNC)(&hasharray->table[i])) % hasharray->capacity;
        if(best_index==i)
            continue;
        size_t relative_a = (hasharray->capacity+best_index-i) % hasharray->capacity;
        size_t relative_b = (hasharray->capacity+empty_pos-i) % hasharray->capacity;
        if(relative_a > relative_b)
            continue; // keep entry here

        hasharray->table[empty_pos] = hasharray->table[i]; // memcpy
        empty_pos = i;
    }
    memset(&hasharray->table[empty_pos], 0, sizeof *entry);
#if HTL_HASHARRAY_CACHE_LAST
    hasharray->cached_entry = NULL;
#endif
}

HTL_DEF void HTL_MEMBER(Remove)(HTL_P(NAME)* hasharray, HTL_P(KEY_TYPE) key)
{
    HTL_P(ENTRY_TYPE)* entry = HTL_MEMBER(Locate)(hasharray, key);
    if(entry != NULL && HTL_MEMBER(IsEntry)(hasharray, entry))
        HTL_MEMBER(RemoveEntry)(hasharray, entry);
}


HTL_DEF bool HTL_MEMBER(IsEntry)(HTL_P(NAME)* hasharray, HTL_P(ENTRY_TYPE)* entry)
{
    return !(HTL_P(EMPTY_FUNC)(entry));
}

HTL_DEF void HTL_MEMBER(Discard)(HTL_P(NAME)* hasharray, HTL_P(ENTRY_TYPE)* entry)
{
    --hasharray->size;
}


HTL_DEF void HTL_MEMBER(Clear)(HTL_P(NAME)* hasharray)
{
    if(hasharray->table)
        memset(hasharray->table, 0, hasharray->capacity * sizeof *hasharray->table);

    hasharray->size = 0;
}
#endif // HTL_PARAM
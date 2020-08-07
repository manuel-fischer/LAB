#ifdef HTL_PARAM
//#include "HTL_hashmap.t.h"
#include <string.h>

HTL_DEF void HTL_MEMBER(Construct)(HTL_P(NAME)* hashmap)
{
    memset(hashmap, 0, sizeof *hashmap);
}

HTL_DEF void HTL_MEMBER(Destruct)(HTL_P(NAME)* hashmap)
{
    HTL_P(FREE)(hashmap->table);
}

HTL_DEF HTL_MEMBER(Entry)* HTL_MEMBER(PutKey)(HTL_P(NAME)* hashmap, HTL_P(KEY_TYPE) key)
{
    if(hashmap->size * HTL_P(LOAD_DEN) >= hashmap->capacity * HTL_P(LOAD_NUM))
    {
        if(hashmap->capacity != 0)
        {
            HTL_MEMBER(Entry)* entry = HTL_MEMBER(Locate)(hashmap, key);
            if(HTL_MEMBER(IsEntry)(hashmap, entry)) return entry;
        }

        size_t old_capacity, new_capacity;
        HTL_MEMBER(Entry)* old_table,* new_table;
        new_capacity = (hashmap->capacity == 0) ? HTL_P(INITIAL_CAPACITY) : hashmap->capacity * HTL_P(GROW_FACTOR);
        new_table = HTL_P(CALLOC)(new_capacity, (sizeof *new_table));
        if(new_table == NULL) return NULL;

        old_table = hashmap->table;
        old_capacity = hashmap->capacity;

        hashmap->table = new_table;
        hashmap->capacity = new_capacity;
        hashmap->size = 0;

        for(int i = 0; i < old_capacity; ++i)
        {
            if(HTL_MEMBER(IsEntry)(hashmap, &old_table[i]))
                HTL_MEMBER(Put)(hashmap, old_table[i].key, old_table[i].value);
        }
        HTL_P(FREE)(old_table);
    }

    HTL_MEMBER(Entry)* entry = HTL_MEMBER(Locate)(hashmap, key);
    if(!HTL_MEMBER(IsEntry)(hashmap, entry))
    {
        entry->key = key;
        ++hashmap->size;
    }
    return entry;
}

HTL_DEF HTL_MEMBER(Entry)* HTL_MEMBER(Put)(HTL_P(NAME)* hashmap, HTL_P(KEY_TYPE) key, HTL_P(VALUE_TYPE) value)
{
    HTL_MEMBER(Entry)* entry = HTL_MEMBER(PutKey)(hashmap, key) ;
    if(entry)
    {
        entry->value = value;
    }
    return entry;
}


HTL_DEF HTL_MEMBER(Entry)* HTL_MEMBER(Get)(HTL_P(NAME)* hashmap, HTL_P(KEY_TYPE) key)
{
    if(hashmap->capacity)
    {
        HTL_MEMBER(Entry)* entry = HTL_MEMBER(Locate)(hashmap, key);
        if(HTL_MEMBER(IsEntry)(hashmap, entry)) return entry;
    }
    return NULL;
}

HTL_DEF void HTL_MEMBER(RemoveEntry)(HTL_P(NAME)* hashmap, HTL_MEMBER(Entry)* entry)
{
    int i = entry-hashmap->table;

    memset(entry, 0, sizeof *entry);
    --hashmap->size;

    int empty_pos = i;
    for(;;)
    {
        ++i;
        if(i == hashmap->capacity) i = 0;
        if(!HTL_MEMBER(IsEntry)(hashmap, &hashmap->table[i])) break;

        int best_index = HTL_P(HASH_FUNC)(hashmap->table[i].key);
        if(best_index==i)
            continue;
        int relative_a = (hashmap->capacity+best_index-i) % hashmap->capacity;
        int relative_b = (hashmap->capacity+empty_pos-i) % hashmap->capacity;
        if(relative_a > relative_b)
            continue; // Lasse Objekt hier

        hashmap->table[empty_pos] = hashmap->table[i];
        memset(&hashmap->table[i], 0, sizeof *entry);
        empty_pos = i;
    }
}

HTL_DEF void HTL_MEMBER(Remove)(HTL_P(NAME)* hashmap, HTL_P(KEY_TYPE) key)
{
    HTL_MEMBER(Entry)* entry = HTL_MEMBER(Locate)(hashmap, key);
    if(entry != NULL && HTL_MEMBER(IsEntry)(hashmap, entry))
        HTL_MEMBER(RemoveEntry)(hashmap, entry);
}

HTL_DEF int HTL_MEMBER(IsEntry)(HTL_P(NAME)* hashmap, HTL_MEMBER(Entry)* entry)
{
    return !(HTL_P(NULL_REPR));
}

HTL_DEF HTL_MEMBER(Entry)* HTL_MEMBER(Locate)(HTL_P(NAME)* hashmap, HTL_P(KEY_TYPE) key)
{
    int hashid = HTL_P(HASH_FUNC)(key)&(hashmap->capacity-1);
    //return hashmap->table+hashid;
    int i;

    for(i = hashid; i < hashmap->capacity; ++i)
    {
        if(!HTL_MEMBER(IsEntry)(hashmap, &hashmap->table[i])) goto ENTRY_FOUND;
        if(HTL_P(COMP_FUNC)(key, hashmap->table[i].key) == 0) goto ENTRY_FOUND;
    }
    for(i = 0; /*i < hashid*/; ++i)
    {
        if(!HTL_MEMBER(IsEntry)(hashmap, &hashmap->table[i])) goto ENTRY_FOUND;
        if(HTL_P(COMP_FUNC)(key, hashmap->table[i].key) == 0) goto ENTRY_FOUND;
    }

ENTRY_FOUND:

    return &hashmap->table[i];
}

#endif // HTL_PARAM

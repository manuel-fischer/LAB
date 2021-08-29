#include "HTL_hasharray_config.h"
#include "HTL_pp.h"
#include <stdlib.h>
#include <stdbool.h>

#include "HTL_hasharray_util.h"

// Implementation Constants
#define HTL_HASHARRAY_LOAD_NUM          3
#define HTL_HASHARRAY_LOAD_DEN          4
#define HTL_HASHARRAY_GROW_FACTOR       2
#define HTL_HASHARRAY_INITIAL_CAPACITY 16
#define HTL_HASHARRAY_CACHE_LAST        0

typedef struct HTL_P(NAME)
{
    size_t capacity;
    size_t size;
    HTL_P(ENTRY_TYPE)* table;
#if HTL_HASHARRAY_CACHE_LAST
    HTL_P(ENTRY_TYPE)* cached_entry; // last requested entry, not thread safe
#endif
} HTL_P(NAME);


/********* LIFETIME *********/
bool HTL_MEMBER(Create)(HTL_P(NAME)* hasharray);
void HTL_MEMBER(Destroy)(HTL_P(NAME)* hasharray);


/********* ENTRY ACCESS *********/
HTL_P(ENTRY_TYPE)* HTL_MEMBER(Locate)(HTL_P(NAME)* hasharray, HTL_P(KEY_TYPE) key);
HTL_P(ENTRY_TYPE)* HTL_MEMBER(PutAlloc)(HTL_P(NAME)* hasharray, HTL_P(KEY_TYPE) key);
HTL_P(ENTRY_TYPE)* HTL_MEMBER(Get)(HTL_P(NAME)* hasharray, HTL_P(KEY_TYPE) key);
void HTL_MEMBER(RemoveEntry)(HTL_P(NAME)* hasharray, HTL_P(ENTRY_TYPE)* entry);
void HTL_MEMBER(Remove)(HTL_P(NAME)* hasharray, HTL_P(KEY_TYPE) key);
bool HTL_MEMBER(IsEntry)(HTL_P(NAME)* hasharray, HTL_P(ENTRY_TYPE)* entry);
void HTL_MEMBER(Discard)(HTL_P(NAME)* hasharray, HTL_P(ENTRY_TYPE)* entry);

void HTL_MEMBER(Clear)(HTL_P(NAME)* hasharray);
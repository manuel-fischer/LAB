// OLD
#error OLD
/**
 * Hashmap with linear forward probing
 *
 * PARAMS:
 *    NAME       name of the generated type, prefix of all functions
 *    KEY_TYPE   key type
 *    VALUE_TYPE value type
 *    HASH_FUNC  hash function
 *    COMP_FUNC  comparison function
 *
 *    CALLOC
 *    FREE
 *
 *    LOAD_NUM   load factor numerator   eg. 3 (lesser than load denominator)
 *    LOAD_DEN   load factor denominator eg. 4
 *
 *    INITIAL_CAPACITY
 *    NULL_REPR
 */

#include "HTL_pp.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct HTL_P(NAME) HTL_P(NAME);
typedef struct HTL_MEMBER(Entry) HTL_MEMBER(Entry);

typedef struct HTL_P(NAME)
{
    size_t capacity;
    size_t size;
    HTL_MEMBER(Entry)* table;
} HTL_P(NAME);

typedef struct HTL_MEMBER(Entry)
{
    HTL_P(KEY_TYPE) key;
    HTL_P(VALUE_TYPE) value;
} HTL_MEMBER(Entry);


void HTL_MEMBER(Construct)(HTL_P(NAME)* hashmap);
void HTL_MEMBER(Destruct)(HTL_P(NAME)* hashmap);

HTL_MEMBER(Entry)* HTL_MEMBER(Locate)(HTL_P(NAME)* hashmap, HTL_P(KEY_TYPE) key);
HTL_MEMBER(Entry)* HTL_MEMBER(PutKey)(HTL_P(NAME)* hashmap, HTL_P(KEY_TYPE) key);
HTL_MEMBER(Entry)* HTL_MEMBER(Put)(HTL_P(NAME)* hashmap, HTL_P(KEY_TYPE) key, HTL_P(VALUE_TYPE) value);
HTL_MEMBER(Entry)* HTL_MEMBER(Get)(HTL_P(NAME)* hashmap, HTL_P(KEY_TYPE) key);
void HTL_MEMBER(RemoveEntry)(HTL_P(NAME)* hashmap, HTL_MEMBER(Entry)* entry);
void HTL_MEMBER(Remove)(HTL_P(NAME)* hashmap, HTL_P(KEY_TYPE) key);




bool HTL_MEMBER(IsEntry)(HTL_P(NAME)* hashmap, HTL_MEMBER(Entry)* entry);

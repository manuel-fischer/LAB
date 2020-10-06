/**
 * Hasharray with linear forward probing
 *
 * PARAMS:
 *    NAME               name of the generated type, prefix of all functions
 *    KEY_TYPE           key type
 *    ENTRY_TYPE         entry type
 *    KEY_FUNC(e)        key function, takes pointer to entry
 *    HASH_FUNC(k)       hash function, takes key
 *    COMP_FUNC(k1,k2)   comparison function, takes key, return 0 if equal
 *
 *    EMPTY_FUNC(e)      function to check if entry is empty,
 *                       all bytes of the entry set to 0 should be an representation of an empty entry
 *                       the function could check if a part of the entry is 0
 *
 *    CALLOC(num, size)
 *    FREE(mem)
 *
 *    LOAD_NUM          load factor numerator   eg. 3 (lesser than load denominator)
 *    LOAD_DEN          load factor denominator eg. 4
 *    GROW_FACTOR       the grow factor, with which the array gets resized, eg. 2
 *    INITIAL_CAPACITY  the initial capacity of the array, when the first element gets pushed
 *
 *    CACHE_LAST        0 or 1, used in #if-#else to enable fast cached check (NOT multithreading compatible)
 */

#include "HTL_pp.h"
#include <stdlib.h>
#include <stdbool.h>

#include "HTL_hasharray_util.h"

typedef struct HTL_P(NAME)
{
    size_t capacity;
    size_t size;
    HTL_P(ENTRY_TYPE)* table;
#if HTL_P(CACHE_LAST)
    HTL_P(ENTRY_TYPE)* cached_entry; // last requested entry, not thread safe
#endif
} HTL_P(NAME);


/********* LIFETIME *********/

/**
 *  Create an empty hasharray at the referenced location
 *  Return 1 on success, 0 on failure
 *
 *  PRE:  hasharray was not already constructed
 *
 *  POST: the hasharray is valid to be used with the other functions
 *
 *  Alternatively the bytes could be set to 0
 */
bool HTL_MEMBER(Create)(HTL_P(NAME)* hasharray);

/**
 *  Destroy a hasharray at the referenced location
 *  Dynamic memory gets cleaned up
 *
 *  POST: All references to entries get invalidated, because
 *        the array gets freed, Construct could be called
 *        again
 */
void HTL_MEMBER(Destroy)(HTL_P(NAME)* hasharray);


/********* ENTRY ACCESS *********/

/**
 *  Locate the element with the given key or where it should be
 *
 *  PRE:  The element is available in the array
 *        or there is enough room for the new element
 *
 *  POST: Return value points to a valid memory location
 *
 *  You should use Put to insert elements
 */
HTL_P(ENTRY_TYPE)* HTL_MEMBER(Locate)(HTL_P(NAME)* hasharray, HTL_P(KEY_TYPE) key);

/**
 *  Create room for the entry with the given key, if it is not already present
 *
 *  The return value points to NULL, when allocation failed
 *  If it was already present, the return value points to that entry
 *
 *  The entry itself is not inserted.
 *  To insert an entry, the caller should change the given entry to a
 *  representation, that is not an empty entry,
 *  When the entry was not inserted, the caller should call Discard
 *
 *  POST: All references to entries get invalidated, because the array
 *        might have been relocated
 */
HTL_P(ENTRY_TYPE)* HTL_MEMBER(PutAlloc)(HTL_P(NAME)* hasharray, HTL_P(KEY_TYPE) key);

/**
 *  Locate the element with the given key, if it is present
 *  Return NULL if it was not found
 *
 *  POST: The function does not change the array
 */
HTL_P(ENTRY_TYPE)* HTL_MEMBER(Get)(HTL_P(NAME)* hasharray, HTL_P(KEY_TYPE) key);

/**
 *  Remove an entry by the given reference to the entry
 *
 *  PRE:  entry is a valid entry that is not an empty entry
 *
 *  POST: All references to entries get invalidated but keep dereferenceable,
 *        because some entries might get relocated but not reallocated
 *
 *  INFO: This function can be used in a loop that iterates over all the
 *        entries linearly from [0] to [hasharray->capacity-1] in hasharray->table
 *        and removes elements conditionally. After this function has been called,
 *        the memory location of entry might contain another valid entry that needs
 *        to be tested. Then it is guaranteed that all elements are visited.
 */
void HTL_MEMBER(RemoveEntry)(HTL_P(NAME)* hasharray, HTL_P(ENTRY_TYPE)* entry);

/**
 *  Remove the entry with the given key, if it is present
 *
 *  POST: All references to entries get invalidated but keep dereferenceable,
 *        because some entries might get relocated but not reallocated
 */
void HTL_MEMBER(Remove)(HTL_P(NAME)* hasharray, HTL_P(KEY_TYPE) key);

/**
 *  Function to check if an entry is empty
 *
 *  INFO: this function could be used when iterating through hasharray->table,
 */
bool HTL_MEMBER(IsEntry)(HTL_P(NAME)* hasharray, HTL_P(ENTRY_TYPE)* entry);

/**
 *  When an entry has been allocated, this function allows to do a fast
 *  return of the memory. This should only be called when the last call
 *  to the map was Put (or Put followed by one or more IsEntry calls)
 *  and the entry itself was kept empty, either not written to or changed
 *  to an empty representation. If the entry is kept fully 0, it is
 *  guaranteed that PutAlloc always returns entries that are fully 0,
 *  when no element was found
 *
 *  TODO should locate/get be allowed in between?
 */
void HTL_MEMBER(Discard)(HTL_P(NAME)* hasharray, HTL_P(ENTRY_TYPE)* entry);

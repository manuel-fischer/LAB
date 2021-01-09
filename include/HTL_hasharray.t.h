/** \file HTL_hasharray.t.h
 *
 *  Hasharray with linear forward probing
 *
 *  Template header (multiple inclusion)
 *
 *  @param NAME               name of the generated type, prefix of all functions
 *  @param KEY_TYPE           key type (\c KeyType)
 *  @param ENTRY_TYPE         entry type (\c EntryType)
 *  @param KEY_FUNC(e)        key function, takes pointer to entry
 *  @param HASH_FUNC(k)       hash function, takes key, return size_t
 *  @param COMP_FUNC(k1,k2)   comparison function, takes key, return 0 if equal
 *
 *  @param EMPTY_FUNC(e)      function to check if entry is empty,
 *                            all bytes of the entry set to 0 should be an representation of an empty entry
 *                            the function could check if a part of the entry is 0
 *
 *  @param CALLOC(num, size)
 *  @param FREE(mem)
 *
 *  @param LOAD_NUM          load factor numerator   eg. 3 (lesser than load denominator)
 *  @param LOAD_DEN          load factor denominator eg. 4
 *  @param GROW_FACTOR       the grow factor, with which the array gets resized, eg. 2
 *  @param INITIAL_CAPACITY  the initial capacity of the array, when the first element gets pushed
 *
 *  @param CACHE_LAST        0 or 1, used in #if-#else to enable fast cached check (NOT multithreading compatible)
 */

#include "HTL_pp.h"
#include <stdlib.h>
#include <stdbool.h>

#include "HTL_hasharray_util.h"

#ifndef DOXYGEN

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

#else /* DOXYGEN SECTION */

typedef struct HashArray
{
    size_t capacity;
    size_t size;
    EntryType* table;
    EntryType* cached_entry; // last requested entry, not thread safe
} HashArray;

/********* LIFETIME *********/

/**
 *  Create an empty hasharray at the referenced location
 *  Return 1 on success, 0 on failure
 *
 *  \pre  hasharray was not already constructed
 *
 *  \post the hasharray is valid to be used with the other functions
 *
 *  Alternatively the bytes could be set to 0
 */
bool HashArray_Create(HashArray* hasharray);

/**
 *  Destroy a hasharray at the referenced location
 *  Dynamic memory gets cleaned up
 *
 *  \post All references to entries get invalidated, because
 *        the array gets freed, Construct could be called
 *        again
 */
void HashArray_Destroy(HashArray* hasharray);


/********* ENTRY ACCESS *********/

/**
 *  Locate the element with the given key or where it should be
 *
 *  \pre  The element is available in the array
 *        or there is enough room for the new element
 *
 *  \post Return value points to a valid memory location
 *
 *  You should use Put to insert elements
 */
EntryType* HashArray_Locate(HashArray* hasharray, KeyType key);

/**
 *  Create room for the entry with the given key, if it is not already present
 *
 *  The return value points to NULL, when allocation failed.
 *  If it was already present, the return value points to that entry
 *
 *  The entry itself is not inserted.
 *  To insert an entry, the caller should change the given entry to a
 *  representation, that is not an empty entry,
 *  When the entry was not inserted, the caller should call Discard
 *
 *  \post All references to entries get invalidated, because the array
 *        might have been relocated
 *
 *  \see HashArray_Discard
 */
EntryType* HashArray_PutAlloc(HashArray* hasharray, KeyType key);

/**
 *  Locate the element with the given key, if it is present.
 *  Return NULL if it was not found
 *
 *  \post The function does not change the array
 */
EntryType* HashArray_Get(HashArray* hasharray, KeyType key);

/**
 *  Remove an entry by the given reference to the entry
 *
 *  \pre  entry is a valid entry that is not an empty entry
 *
 *  \post All references to entries get invalidated but keep dereferenceable,
 *        because some entries might get relocated but not reallocated
 *
 *  INFO: This function can be used in a loop that iterates over all the
 *        entries linearly from [0] to [hasharray->capacity-1] in hasharray->table
 *        and removes elements conditionally. After this function has been called,
 *        the memory location of entry might contain another valid entry that needs
 *        to be tested. Then it is guaranteed that all elements are visited.
 */
void HashArray_RemoveEntry(HashArray* hasharray, EntryType* entry);

/**
 *  Remove the entry with the given key, if it is present
 *
 *  \post All references to entries get invalidated but keep dereferenceable,
 *        because some entries might get relocated but not reallocated
 */
void HashArray_Remove(HashArray* hasharray, KeyType key);

/**
 *  Function to check if an entry is empty
 *
 *  INFO: this function could be used when iterating through hasharray->table,
 */
bool HashArray_IsEntry(HashArray* hasharray, EntryType* entry);

/**
 *  When an entry has been allocated, this function allows to do a fast
 *  return of the memory. This should only be called when the last call
 *  to the map was Put (or Put followed by one or more IsEntry calls)
 *  and the entry itself was kept empty, either not written to or changed
 *  to an empty representation. If the entry is kept fully 0, it is
 *  guaranteed that PutAlloc always returns entries that are fully 0,
 *  when no element was found
 *
 *  \todo should locate/get be allowed in between?
 *
 *  \see HashArray_PutAlloc
 */
void HashArray_Discard(HashArray* hasharray, EntryType* entry);
#endif

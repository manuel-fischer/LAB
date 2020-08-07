/**
 * Hasharray with linear forward probing
 *
 * PARAMS:
 *    NAME        name of the generated type, prefix of all functions
 *    KEY_TYPE    key type
 *    ENTRY_TYPE  entry type
 *    KEY_FUNC    key function, takes entry
 *    HASH_FUNC   hash function, takes key
 *    COMP_FUNC   comparison function, takes key, return 0 if equal
 *
 *    EMPTY_FUNC  function to check if entry is empty,
 *                all bytes of the entry set to 0 should be an representation of an empty entry
 *                the function could check if a part of the entry is 0
 *
 *    CALLOC
 *    FREE
 *
 *    LOAD_NUM   load factor numerator   eg. 3 (lesser than load denominator)
 *    LOAD_DEN   load factor denominator eg. 4
 *    GROW_FACTOR    the grow factor, with which the array gets resized, eg. 2
 *    INITIAL_CAPACITY  the initial capacity of the array, when the first element gets pushed
 */

#include "HTL_pp.h"
#include <stdlib.h>

typedef struct HTL_P(NAME)
{
    size_t capacity;
    size_t size;
    HTL_MEMBER(Entry)* table;
} HTL_P(NAME);


/********* LIFETIME *********/

/**
 *  Construct an empty hasharray at the referenced location
 *  Return 1 on success, 0 on failure
 *
 *  PRE:  hasharray was not already constructed
 *
 *  POST: the hasharray is valid to be used with the other functions
 *
 *  Alternatively the bytes could be set to 0
 */
int  HTL_MEMBER(Construct)(HTL_P(NAME)* hasharray);

/**
 *  Destruct a hasharray at the referenced location
 *  Dynamic memory gets cleaned up
 *
 *  POST: All references to entries get invalidated, because
 *        the array gets freed, Construct could be called
 *        again
 */
void HTL_MEMBER(Destruct)(HTL_P(NAME)* hasharray);


/********* ENTRY ACCESS *********/

/**
 *  Locate the element with the given key or where it should be
 *
 *  PRE:  The element is available in the array
 *        or there is enough room for the new element
 *
 *  POST: Return value points to a valid memory location
 */
HTL_MEMBER(Entry)* HTL_MEMBER(Locate)(HTL_P(NAME)* hasharray, HTL_P(KEY_TYPE) key);

/**
 *  Create room for the entry with the given key, if it is not already present
 *
 *  The return value points to NULL, when allocation failed
 *  If it was already present, the return value points to that entry
 *
 *  The entry itself is not inserted.
 *  To insert an entry, the caller should change the given entry to a
 *  representation, that is not an empty entry
 *
 *  POST: All references to entries get invalidated, because the array
 *        might have been relocated
 */
HTL_MEMBER(Entry)* HTL_MEMBER(Put)(HTL_P(NAME)* hasharray, HTL_P(KEY_TYPE) key);

/**
 *  Locate the element with the given key, if it is present
 *  Return NULL if it was not found
 *
 *  POST: The function does not change the array
 */
HTL_MEMBER(Entry)* HTL_MEMBER(Get)(HTL_P(NAME)* hasharray, HTL_P(KEY_TYPE) key);

/**
 *  Remove an entry by the given reference to the entry
 *
 *  PRE:  entry is a valid entry that is not an empty entry
 *
 *  POST: All references to entries get invalidated, because some entries might get relocated
 */
void HTL_MEMBER(RemoveEntry)(HTL_P(NAME)* hasharray, HTL_MEMBER(Entry)* entry);

/**
 *  Remove the entry with the given key, if it is present
 *
 *  POST: All references to entries get invalidated, because some entries might get relocated
 */
void HTL_MEMBER(Remove)(HTL_P(NAME)* hasharray, HTL_P(KEY_TYPE) key);

/**
 *  Function to check if an entry is empty
 */
int HTL_MEMBER(IsEntry)(HTL_P(NAME)* hasharray, HTL_MEMBER(Entry)* entry);


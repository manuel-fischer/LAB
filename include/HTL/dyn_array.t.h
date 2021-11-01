#ifndef HTL_PARAM
#error HTL_PARAM should be defined
#endif

/**
 * PARAMS
 * @param NAME       name of the generated type, prefix of all functions
 * @param TYPE       content type
 * @param CAPACITY   maximum amount of elements
 */

typedef struct HTL_P(NAME)
{
    HTL_P(TYPE)* data;
    size_t size;
} HTL_P(NAME);

/********* LIFETIME *********/

bool HTL_MEMBER(Create)(HTL_P(NAME)* q);
void HTL_MEMBER(Destroy)(HTL_P(NAME)* q);
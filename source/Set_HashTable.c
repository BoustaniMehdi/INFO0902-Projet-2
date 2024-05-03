/* ========================================================================= *
 * HashTable
 *
 * Implementation of HashTable.h based on linked lists.
 *
 * ========================================================================= */

#include "Set.h"
#include <stdlib.h>
#include <string.h>

#define INIT_CAPACITY 200013

/* Structures */

typedef struct LLElement_t
{
    char *key;
    struct LLElement_t *next;
} LLElement;

struct Set_t
{
    LLElement **table;
    size_t tableSize;
    size_t numElements;
};

/* Prototypes */

static LLElement *findElement(const Set *set, const char *key);
static void freeLLElement(LLElement *element);
static size_t hashFunction(const char *key);
static char *duplicate_string(const char *str);

/* static functions */

/**
 * @brief Find a pointer to the list node that contains the key or
 *        NULL if the key is not in the hash table.
 *
 * @param set              A pointer to a hash table
 * @param key              A valid string
 * @return LLElement*      The list node or NULL
 */
static LLElement *findElement(const Set *set, const char *key)
{
    if (!set)
        return NULL;

    size_t index = hashFunction(key) % set->tableSize;
    LLElement *element = set->table[index];
    while (element != NULL)
    {
        if (strcmp(key, element->key) == 0)
            return element;
        element = element->next;
    }
    return NULL;
}

/**
 * @brief Free a linked list, including its keys.
 *
 * @param element     The start of the linked list to be freed.
 */
static void freeLLElement(LLElement *element)
{
    while (element != NULL)
    {
        LLElement *nextElement = element->next;
        free(element->key);
        free(element);
        element = nextElement;
    }
}

/**
 * @brief Computing an encoding (in base 26) of the key
 *
 * @param key
 * @return size_t
 */
static size_t hashFunction(const char *key)
{
    size_t count = 0;

    for (size_t i = 0; i < strlen(key); i++)
    {
        count *= 26;
        count += key[i] - 'a';
    }

    return count;
}

/**
 * @brief Duplicate a string
 *
 * @param str
 * @return char*
 */
static char *duplicate_string(const char *str)
{
    char *copy = malloc(strlen(str) + 1);
    if (!copy)
        return NULL;
    memcpy(copy, str, strlen(str) + 1);
    return copy;
}

/* header functions */

Set *setCreateEmpty(void)
{
    Set *res = malloc(sizeof(Set));
    if (!res)
        return NULL;

    res->tableSize = INIT_CAPACITY;
    res->numElements = 0;

    res->table = malloc(sizeof(LLElement *) * res->tableSize);
    if (!res->table)
    {
        free(res);
        return NULL;
    }

    for (size_t i = 0; i < res->tableSize; ++i)
        res->table[i] = NULL;

    return res;
}

void setFree(Set *set)
{
    if (!set)
        return;

    for (size_t i = 0; i < set->tableSize; ++i)
        freeLLElement(set->table[i]);

    free(set->table);
    free(set);
}

int setInsert(Set *set, const char *key)
{
    if (!set)
        return -1;

    size_t index = hashFunction(key) % set->tableSize;

    LLElement *element = set->table[index];
    while (element != NULL)
    {
        if (strcmp(key, element->key) == 0)
        {
            return 0;
        }
        element = element->next;
    }

    element = malloc(sizeof(LLElement));
    if (!element)
        return -1;

    element->key = duplicate_string(key);
    element->next = set->table[index];
    set->table[index] = element;
    set->numElements++;

    return 1;
}

size_t setNbKeys(const Set *set)
{
    if (!set)
        return (size_t)-1;
    return set->numElements;
}

bool setContains(const Set *set, const char *key)
{
    return findElement(set, key) ? true : false;
}


/* student code starts here */

/* Prototypes */
static bool isPrefix(const char *str, const char *word);
/* static functions */

/**
 * @brief Checks if a given word is a prefix of a string
 *
 * @param str a string
 * @param word the word to be checked
 * 
 * @return bool : true if word is a prefix of str
 *                false otherwise
 */
static bool isPrefix(const char *str, const char *word){

    return strncmp(str, word, strlen(word)) == 0;
}

List *setGetAllStringPrefixes(const Set *set, const char *str)
{
    List *foundPrefixes = listNew();
    if (!foundPrefixes){
        return NULL;
    }

    size_t strSize = strlen(str);

    LLElement *element = set->table[0];
    
    size_t count = 0;
    for (size_t i = 0; i < strSize ; i++){
        // same strategy as hashFunction
        count *= 26;
        count += str[i] - 'a';
        element = set->table[count % set->tableSize];

        while (element != NULL){

            if (isPrefix(str, element->key)){
                
                char *copy = duplicate_string(element->key);
                if (!copy){
                    listFree(foundPrefixes, true);
                    return NULL;
                }

                if (!listInsertLast(foundPrefixes, copy)){
                    free(copy);
                    listFree(foundPrefixes, true);
                    return NULL;
                }
            }
            element = element->next;
        }
    }
    return foundPrefixes;
}

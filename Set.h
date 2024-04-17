#ifndef _SET_H_
#define _SET_H_

#include <stdbool.h>
#include <stddef.h>

#include<stdio.h> // A ENLEVER


#include "List.h"

/** Set (opaque) structure */
typedef struct Set_t Set;
typedef struct RNode_t RNode;

typedef struct BNode_t BNode;

/**
 * @brief Create an empty set. The returned set needs to be freed
 *        with setFree.
 *
 * @return Set*     a pointer to an empty set
 */
Set *setCreateEmpty(void);

/**
 * @brief Free the set, including all stored keys.
 *
 * @param set
 */
void setFree(Set *set);

/**
 * @brief Returns the number of distinct keys present in the set.
 *
 * @param set         A pointer to a set
 * @return size_t     the number of keys
 */
size_t setNbKeys(const Set *set);

/**
 * @brief Returns true if the key appears in the set, false otherwise.
 *
 * PARAMETERS
 * @param dict         A pointer to a set
 * @param key          A key
 * @return bool
 */
bool setContains(const Set *set, const char *key);

/**
 * @brief Insert the key into the set. A copy of the key will be done.
 *
 * @param dict         A pointer to a set
 * @param key          A key
 * @return int         1 if the key was inserted, 0 if it already appears in the set,
 *                     -1 in case of allocation error
 */
int setInsert(Set *set, const char *key);

/**
 * @brief Return a list of all prefixes of the string that appears in the set.
 *        The list and all the keys it contains need to be freed by the user.
 *
 * @param set          A pointer to a set
 * @param string       A valid string (ending with a \0 character)
 * @return List*       A list of strings
 */
List *setGetAllStringPrefixes(const Set *set, const char *string);

void getRoot(Set *set);
void generateRadixTreeDOT(Set *radixTree, const char *filename);
void writeRadixTreeDOT(FILE *file, RNode *node);

void generateDotFile(BNode *node, FILE *dotFile);
void generateDot(Set *bst);
#endif // !_SET_H_

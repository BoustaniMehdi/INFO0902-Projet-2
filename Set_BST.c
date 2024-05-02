/* ========================================================================= *
 * BST definition
 * ========================================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "List.h"
#include "Set.h"

/* Opaque Structure */
typedef struct BNode_t BNode;

struct BNode_t
{
    BNode *parent;
    BNode *left;
    BNode *right;
    char *key;
};

struct Set_t
{
    BNode *root;
    size_t size;
};

typedef struct Pair_t
{
    long sum;
    size_t n;
} Pair;

/* Prototypes of static functions */

static void bstFreeRec(BNode *n);
static BNode *bnNew(const char *key);
static char *duplicate_string(const char *str);


/* static functions */

/**
 * @brief Create a new tree node with its associated key.
 *
 * @param key
 * @return BNode*
 */
static BNode *bnNew(const char *key)
{
    BNode *n = malloc(sizeof(BNode));
    if (n == NULL)
    {
        printf("bnNew: allocation error\n");
        return NULL;
    }
    n->left = NULL;
    n->right = NULL;
    n->key = duplicate_string(key);
    return n;
}

/**
 * @brief free the tree recursively, including its keys
 *
 * @param n   the root of the tree to be freed.
 */
static void bstFreeRec(BNode *n)
{
    if (n == NULL)
        return;
    bstFreeRec(n->left);
    bstFreeRec(n->right);
    free(n->key);
    free(n);
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
    Set *bst = malloc(sizeof(Set));
    if (bst == NULL)
    {
        printf("bestNew: allocation error");
        return NULL;
    }
    bst->root = NULL;
    bst->size = 0;
    return bst;
}

void setFree(Set *bst)
{
    bstFreeRec(bst->root);
    free(bst);
}

size_t setNbKeys(const Set *bst)
{
    if (bst == NULL)
        return (size_t)-1;
    return bst->size;
}

int setInsert(Set *bst, const char *key)
{
    if (bst->root == NULL)
    {
        bst->root = bnNew(key);
        if (bst->root == NULL)
        {
            return false;
        }
        bst->size++;
        return true;
    }
    BNode *prev = NULL;
    BNode *n = bst->root;
    while (n != NULL)
    {
        prev = n;
        int cmp = strcmp(key, n->key);
        if (cmp == 0)
            return 0;
        else if (cmp < 0)
            n = n->left;
        else
            n = n->right;
    }
    BNode *new = bnNew(key);
    if (new == NULL)
    {
        return -1;
    }
    new->parent = prev;
    if (strcmp(key, prev->key) <= 0)
    {
        prev->left = new;
    }
    else
    {
        prev->right = new;
    }
    bst->size++;
    return 1;
}

bool setContains(const Set *bst, const char *key)
{
    BNode *n = bst->root;
    while (n != NULL)
    {
        int cmp = strcmp(key, n->key);
        if (cmp < 0)
        {
            n = n->left;
        }
        else if (cmp > 0)
        {
            n = n->right;
        }
        else
        {
            return true;
        }
    }
    return false;
}


/* student code starts here */

#define MINSIZE 1 // minimum size of a word in the lexicon

/* Prototypes of static functions */

static bool isPrefix(const char *str, const char *word);
static bool fillPrefixes(List *l, const char *str, size_t size, BNode *n, char *wordMin);


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


/**
 * @brief Gets the common prefix of 2 strings
 *
 * @param str1 a string
 * @param str2 a string
 * 
 * @return char* the common prefix between str1 and str2
 *                
 */
static char *getCommonPref(const char *str1, const char *str2){
    size_t size1 = strlen(str1);
    size_t size2 = strlen(str2);

    char *commonPref = malloc(sizeof(char) * (size1 + 1));
    if (!commonPref){
        printf("Allocation Error : Failed to allocate commonPref\n");
        return NULL;
    }

    bool match = true;
    unsigned int j = 0;
    size_t i = 0;
    while (i < size1 && i < size2 && match){
        if (str1[i] == str2[i]){
            commonPref[j] = str1[i];
            j++;
        }
        else 
            match = false;

        i++;
    }
    commonPref[j] = '\0';
    return commonPref;
}


/**
 * @brief fills recursively a list with found prefixes of a string in the set (bst)
 *
 * @param l  the list to fill
 * @param str the string which prefixes have to be filled
 * @param size size of str
 * @param n the root of the tree
 * @param wordMin the smallest word (smallest prefix possible)
 * 
 * @return true if list was filled successfully
 *         false in case of error 
 *               
 */
static bool fillPrefixes(List *l, const char *str, size_t size, BNode *n, char *wordMin){
    if (listSize(l) == size){
        return true;
    }
    if (n != NULL){

        int cmp1 = strcmp(n->key, wordMin); // compare the node's key with the minimum prefix
        int cmp2 = strcmp(n->key, str); // compare the node's key with the maximum prefix

        if (isPrefix(str, n->key)){
          char *copy = duplicate_string(n->key);
          if (!copy){
            printf("Error : Duplication failed\n");
            return false;
          }
          if (!listInsertLast(l, copy)){
                printf("Failed to fill prefixes into list\n");
                free(copy);
                return false;
            }
            
            if (cmp1 == 0 && cmp2 == 0){ // we only have 1 prefix
                return true; 
            }

            else if (cmp1 == 0 || cmp2 == 0){
                if (cmp1 == 0){ // the minimum (smallest) prefix has been found -> continue search in right sub-tree
                    return fillPrefixes(l, str, size, n->right, wordMin); 
                }

                else if (cmp2 == 0) // the maximum (largest) prefix has been found -> continue search in left sub-tree
                    return fillPrefixes(l, str, size, n->left, wordMin);
            }

            else { // prefix found is between smallest and largest prefixes -> continue search in both sub-trees
                unsigned int keySize = strlen(n->key);
                char nextChar = str[keySize];
                char *newMin = malloc(sizeof(char) * (keySize + 2));
                if (!newMin){
                    printf("Allocation Error : Failed to allocate newMin\n");
                    return false;
                }
                strcpy(newMin, n->key);
                newMin[keySize] = nextChar;
                newMin[keySize + 1] = '\0';
                // newMin is the new minimum prefix for the right subtree
                if (!fillPrefixes(l, str, size, n->right, newMin)){
                    free(newMin);
                    return false;
                }

                char *newMax = duplicate_string(n->key);
                if (!newMax){
                    printf("Error : Failed to duplicate key\n");
                    free(newMax);
                    return false;
                }
                newMax[keySize] = '\0';
                // newMax is the new maximum prefix for the left subtree
                if (!fillPrefixes(l, newMax, size, n->left, wordMin)){
                    free(newMax);
                    return false;
                }

                free(newMax);
                free(newMin);
            }
        }
        
        // node's key is not a prefix of the word (str)
        else {
            if (cmp2 > 0) // node's key is greater than the largest prefix -> continue search in left sub-tree
                return fillPrefixes(l, str, size,  n->left, wordMin);

            else {
                if (cmp1 <= 0) // node's key is less than the smallest prefix -> continue search in right sub-tree
                    return fillPrefixes(l, str, size, n->right, wordMin);

                else if (cmp1 > 0){ // continue search in both sub-tree
                    char *commonPref = getCommonPref(n->key, str);
                    if (!commonPref){
                        printf("Allocation Error : Failed to allocate commonPref\n");
                        return false;
                    }
                    // commonPref is the new minimum prefix for the right sub tree
                    if (!fillPrefixes(l, str, size, n->right, commonPref)){
                        free(commonPref);
                        return false;
                    }
                    // commonPref is the new maximum prefix for the right sub tree 
                    if (!fillPrefixes(l, commonPref, size, n->left, wordMin)){
                        free(commonPref);
                        return false;
                    }
                    free(commonPref);
                }
            }
        }
    }
    return true; 
}


List *setGetAllStringPrefixes(const Set *set, const char *str)
{
   List *prefixList = listNew();
   if (!prefixList){
    printf("Failed to get all prefixes\n");
    return NULL;
   }

    // Minimum prefix possible of a word with a minimum size of 1 (MINSIZE)
    char *wordMin = malloc(sizeof(char) * (MINSIZE + 1));
    if (!wordMin){
        fprintf(stderr, "Memory allocation failed for minimum prefix\n");
        return NULL;
    }
    strncpy(wordMin, str, MINSIZE);
    wordMin[MINSIZE] = '\0';

   BNode *n = set->root;
   size_t strSize = strlen(str);

   // fill prefixes in prefixList
   if (!fillPrefixes(prefixList, str, strSize, n, wordMin)){
    listFree(prefixList, true);
    printf("Error : Failed to fill prefixes in the list\n");
    free(wordMin);
    return NULL;
   }

   free(wordMin);

   return prefixList;
}
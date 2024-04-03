#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "Set.h"

// To be completed. Should provide all functions in Set.h.

typedef struct RNode_t RNode;
typedef struct Edge_t Edge;

struct RNode_t
{
    Edge **edges;
    char *key;
};

struct Set_t
{
    RNode *root;
    size_t size;
};

struct Edge_t
{
    RNode *targetNode;
    char *label;
};

static RNode *newRadixTree(const char *key);
static void freeRadixTreeRec(RNode *n);

/**
 * @brief Create a new tree node with its associated key.
 *
 * @param key
 * @return RNode*
 */
static RNode *newRadixTree(const char *key){
    assert(key != NULL);

    RNode *n = malloc(sizeof(RNode));
    if (!n)
    {
        printf("newRadixTree: allocation error\n");
        return NULL;
    }

    n->edges = malloc(sizeof(Edge *));
    if(!n->edges){
        free(n);
        printf("newRadixTree: allocation error\n");
        return NULL;
    }

    n->key = strdup(key);
    if(!n->key){
        free(n->edges);
        free(n);
        printf("newRadixTree: allocation error\n");
        return NULL;
    }

    return n;
}//end newRadixTree

/**
 * @brief free the tree recursively, including its keys and its edges
 *
 * @param n   the root of the tree to be freed.
 */
static void freeRadixTreeRec(RNode *n){

    if (n == NULL)
        return;

    for (int i = 0; n->edges[i] != NULL; i++)
    {
        freeRadixTreeRec(n->edges[i]->targetNode);
        free(n->edges[i]->label);
        free(n->edges[i]);
    }

    free(n->edges);
    free(n->key);
    free(n);
}//end freeRadixTreeRec

/* header functions */

Set *setCreateEmpty(void){

    Set *radixTree = malloc(sizeof(Set));
    if (radixTree == NULL)
    {
        printf("bestNew: allocation error");
        return NULL;
    }

    radixTree->root = NULL;
    radixTree->size = 0;

    return radixTree;
}//end setCreateEmpty

void setFree(Set *radixTree){
    assert(radixTree != NULL && radixTree->root != NULL);

    freeRadixTreeRec(radixTree->root);
    free(radixTree);
}//end setFree

size_t setNbKeys(const Set *radixTree){
    assert(radixTree != NULL);
    
    if (!radixTree)
        return (size_t)-1;

    return radixTree->size;
}//end setNbKeys

int setInsert(Set *radixTree, const char *key){
    assert(radixTree != NULL && key != NULL);

    if(radixTree->root == NULL)
    {
        radixTree->root = newRadixTree(key);
        if (!radixTree->root)
        {
            return false;
        }

        radixTree->size++;

        return true;
    }

}//end setFree

bool setContains(const Set *set, const char *key){
    assert(set != NULL && key != NULL);



}//end setContains
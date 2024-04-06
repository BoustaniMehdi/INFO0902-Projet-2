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
    size_t edgesNumbers;
    bool isLeaf;
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
static const char *minString(const char *s1, const char *s2);

/**
 * @brief Create a new tree node with its associated key.
 *
 * @param key
 * @return RNode*
 */
static RNode *newRadixTree(const char *key){

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

    if(key != NULL){
        n->key = malloc(strlen(key) + 1);
        if(!n->key){
            free(n->edges);
            free(n);
            printf("newRadixTree: allocation error\n");
            return NULL;
        }

        strcpy(n->key, key);
    }

    return n;
}//end newRadixTree

/**
 * @brief free the tree recursively, including its keys and its edges
 *
 * @param n   the root of the tree to be freed.
 */
static void freeRadixTreeRec(RNode *n){
    assert(n != NULL && n->edges != NULL);

    for (int i = 0; n->edges[i] != NULL; i++)
    {
        freeRadixTreeRec(n->edges[i]->targetNode);

        if(n->edges[i]->label != NULL)
            free(n->edges[i]->label);

        free(n->edges[i]);
    }

    free(n->edges);

    if(n->key != NULL)
        free(n->key);

    free(n);
}//end freeRadixTreeRec

/* header functions */

Set *setCreateEmpty(void){

    Set *radixTree = malloc(sizeof(Set));
    if (radixTree == NULL)
    {
        printf("setCreateEmpty: allocation error");
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

static const char *minString(const char *s1, const char *s2){
    assert(s1 != NULL && s2 != NULL);

    return (strlen(s1) <= strlen(s2)) ? s1: s2;
}

bool setContains(const Set *radixTree, const char *key){
    assert(radixTree != NULL && key != NULL);

    RNode *n = radixTree->root;
    size_t count = 0;
    size_t labelSize;

    while(n != NULL){

        if(n->key != NULL){
            if(strcmp(key, n->key) == 0){
                return true;
            }
        }

        for (size_t i = 0; i < n->edgesNumbers; i++){
            count = 0;
            labelSize = strlen(n->edges[i]->label);

            for(size_t j = 0; j < labelSize && key[j] != '\0'; j++){
                if(key[j] == n->edges[i]->label[j]){
                    count++;
                }
                else{
                    break;
                }
            }
            if(count == labelSize){
                n = n->edges[i]->targetNode;
                break;
            }
        }

        if(count != labelSize){
            break;
        }
            
    }
        
    

    return false;

}//end setContains

int setInsert(Set *radixTree, const char *key){
    assert(radixTree != NULL && key != NULL);

    if (radixTree->root == NULL)
    {
        radixTree->root = newRadixTree(key);
        if (radixTree->root == NULL)
        {
            printf("setInsert: Error while creating the tree\n");
            return false;
        }
        radixTree->size++;
        radixTree->root->edgesNumbers = 0;

        return true;
    }

    RNode *prev = NULL;
    RNode *n = radixTree->root;

    size_t keySize = strlen(key);
    size_t count = 0;

    while(n != NULL){
        prev = n;
        bool found = false;
        //printf("ll");
        for(size_t i = 0; i < n->edgesNumbers; i++){
            printf("edgeees");
            size_t labelSize = strlen(n->edges[i]->label);
            count =  0;

            while(key[count] != '\0' && key[count] == n->edges[i]->label[count] && count < labelSize){
                count++;
            }

            if(count == labelSize && keySize == labelSize){ //la clé à insérer est une arête de l'arbre
                RNode *intermediate = newRadixTree(n->edges[i]->label);
                if(!intermediate){
                    printf("setInsert: Error while creating an intermediate new node\n");
                    return false;
                }

                Edge *newEdge = malloc(sizeof(Edge));
                if(!newEdge){
                    printf("setInsert: allocation error");
                    freeRadixTreeRec(intermediate);
                    return false;
                }

                char *newLabel = malloc(sizeof(strlen(key + count) + 1));
                if(!newLabel){
                    printf("setInsert: allocation error");
                    freeRadixTreeRec(intermediate);
                    free(newEdge);
                    return false;
                }

                strcpy(newLabel, key + count);

                newEdge->label = newLabel;

                newEdge->targetNode = newRadixTree(key + count);
                if(!newEdge->targetNode){
                    printf("setInsert: error while creating a new node");
                    free(newEdge->label);
                    free(newEdge);
                    freeRadixTreeRec(intermediate);
                    return false;
                }

                n->edges[i]->label = realloc(n->edges[i]->label, count + 1);
                if(!n->edges[i]->label){
                    printf("setInsert: error while creating a new node");
                    free(newEdge->label);
                    free(newEdge);
                    freeRadixTreeRec(intermediate);
                    return false;
                }

                n->edges[i]->label[count] = '\0';
                n->edges[i]->targetNode = intermediate;

                intermediate->edges[0] = newEdge;
                intermediate->edgesNumbers = 1;
                intermediate->isLeaf = false;
                radixTree->size++;
                
                return true;
            }

            else if(count > 0 && count < labelSize){ //Le prefixe de la clé est trouvé dans une des arêtes
                RNode *intermediate = newRadixTree(NULL);
                if(!intermediate){
                    printf("setInsert: Error while creating a new node\n");
                    return false;
                }

                Edge *newEdge = malloc(sizeof(Edge));
                if(!newEdge){
                    printf("setInsert: allocation error");
                    freeRadixTreeRec(intermediate);
                    return false;
                }

                char *newLabel = n->edges[i]->label + count;
                size_t newLabelSize = labelSize - count;

                n->edges[i]->label = realloc(n->edges[i]->label, count + 1);
                if(n->edges[i]->label){
                    printf("setInsert: reallocation error");
                    freeRadixTreeRec(intermediate);
                    free(newLabel);
                    return false;
                }

                n->edges[i]->label[count] = '\0';

                newEdge->label = malloc(strlen(newLabel) + 1);
                if(!newEdge->label){
                    free(n->edges);
                    free(n);
                    printf("newRadixTree: allocation error\n");
                    return false;
                }

                strcpy(newEdge->label, newLabel);
                newEdge->targetNode = n->edges[i]->targetNode;
                n->edges[i]->targetNode = intermediate;

                intermediate->edges[0] = newEdge;
                intermediate->edgesNumbers = 1;
                intermediate->isLeaf = false;
                radixTree->size++;

                return true;
            }

        }

        if(n->edgesNumbers == 0){
            Edge *newEdge = malloc(sizeof(Edge));
            if(!newEdge){
                printf("setInsert: allocation error");
                freeRadixTreeRec(n);
                return false;
            }

            n->edges[0] = newEdge;
            strcpy(newEdge->label, key);
            newEdge->targetNode = newRadixTree(key);
            n->edgesNumbers++;
            radixTree->size++;
        }
    }

    return false;

}
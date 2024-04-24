#include "Set.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct RNode_t RNode;
typedef struct Edge_t Edge;
typedef struct EList_t EdgeList;

struct Edge_t
{
    char *label;
    RNode *targetNode;
    struct Edge_t *next;
};

struct EList_t
{
    size_t size;
    Edge *head;
    Edge *last;
};

struct RNode_t
{
    EdgeList *edges;
    char *key;
};

struct Set_t
{
    RNode *root;
    size_t size;
};


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

/**
 * @brief Terminate the program and display a message
 *
 * @param m
 */
static void terminate(char *m)
{
    fprintf(stderr, "%s\n", m);
    exit(EXIT_FAILURE);
}

static void concatenate_string(char **str1, const char *str2){
    size_t len1 = strlen(*str1);
    size_t len2 = strlen(str2);
    
    *str1 = realloc(*str1, (len1 + len2 + 1) * sizeof(char));
    if (!*str1){
        terminate("Allocation Error : Failed to reallocate string\n");
    }

    strcat(*str1, str2);
}

static void copy_string(char **str1, const char *str2){
    size_t len1 = strlen(*str1);
    size_t len2 = strlen(str2);
    
    *str1 = realloc(*str1, (len1 + len2 +1) * sizeof(char));
    if (!*str1){
        terminate("Allocation Error : Failed to reallocate string\n");
    }

    strcpy(*str1, str2);
}

static void freeRec(RNode *n){
   
    Edge *edge = n->edges->head;
    Edge *prev = NULL;
    while (edge != NULL){

        freeRec(edge->targetNode);
        prev = edge;
        edge = edge->next;
        
        free(prev->label);
        free(prev);
    }
    free(n->key);
    free (n->edges);
    free(n);
}

void setFree(Set *set){
    freeRec(set->root);
    free(set);
}

// ----------------------------------------- EDGE ------------------------------------------ //

static EdgeList *edgeListNew(void){
    EdgeList *edgeList = malloc(sizeof(EdgeList));
    if (!edgeList){
        return NULL;
    }
    edgeList->head = NULL;
    edgeList->last = NULL;
    edgeList->size = 0;
    return edgeList;
}

/* ------------------------------------------------------------------------- *
 * Inserts a new element at the beginning of the list.
 *
 * PARAMETERS
 * l            A valid pointer to a List object
 * value        The value to store
 *
 * RETURN
 * res          A boolean equal to true if the new element was successfully
 *              inserted, false otherwise (error)
 * ------------------------------------------------------------------------- */

static bool edgesInsertLast(EdgeList *l, RNode *targetNode, const char *label);

static bool edgesInsertLast(EdgeList *l, RNode *targetNode, const char *label)
{
    Edge *edge = malloc(sizeof(Edge));
    if (!edge){
        printf("Allocation Error :Failed to insert a new edge to edges list \n");
        return false;
    }
    // Initialisation
    edge->next = NULL;
    edge->targetNode = targetNode;
    edge->label = duplicate_string(label);
    // Adding the node to the list
    if (!l->last)
    {
        // First element in the list
        l->last = edge;
        l->head = edge;
    }
    else
    {
        // At least one element in the list
        l->last->next = edge;
        l->last = edge;
    }
    // In both cases, increment size
    l->size++;
    return true;
}

static bool addEdge(RNode *source, RNode *target, const char *label){
    return edgesInsertLast(source->edges, target, label);
}

static void setEdgeLabel(Edge *e, char *newLabel){
    if (e->label){
        free(e->label);
    }
    e->label = duplicate_string(newLabel);
}

// ---------------------------------------- END EDGE ---------------------------------------// 


// ----------------------------------------- RNode -------------------------------------------// 

static RNode *rnNew(const char *key){
    RNode *n = malloc(sizeof(RNode));
    if (!n){
        printf("Error : Failed to allocate a new node\n");
        return NULL;
    }
    n->edges = edgeListNew();
    if (!n->edges){
        free(n);
        return NULL;
    }
    n->key = duplicate_string(key);

    return n;
}

// ----------------------------------------- END RNode ---------------------------------------//


static int isPrefix(const char *word, const char *prefix) {

    // Use strncmp to compare the first strlen(prefix) characters of word with prefix
    return strncmp(word, prefix, strlen(prefix)) == 0;
}


// // ------------------------------------------ SET RADIX ------------------------------------- //

Set *setCreateEmpty(void){
    Set *radix = malloc(sizeof(Set));
    if (!radix){
        printf("radixNew : allocation error\n");
        return NULL;
    }

    radix->root = NULL;
    radix->size = 0;

    return radix;
}

static bool isLeaf(RNode *node){
   return (node->edges->size == 0);
}

bool setContains(const Set *radix, const char *key){
     
    RNode *n = radix->root;
    Edge *e = NULL;

    char *pref = duplicate_string(n->key);
    if (!pref){
        return NULL;
    }
    // pref[0] = '\0';

    char *tmp = duplicate_string(n->key);
    if (!tmp){
        free(pref);
        return NULL;
    }
    // tmp[0] = '\0';
 
    while (n != NULL){
         
        int cmp = strcmp(key, n->key);
        if (cmp == 0){
            free(tmp);
            free(pref);
            return true;
        }
        
        if (isLeaf(n)){
            free(pref);
            free(tmp);
            return false;
        }
        
        e = n->edges->head;
        bool isPref = false;
        while (e != NULL && !isPref){
            concatenate_string(&pref, e->label);
            // strcat(pref, e->label);
            
            if (isPrefix(key, pref)){
                
                copy_string(&tmp, pref);
                // strcpy(tmp, pref);
                n = e->targetNode;
                isPref = true;
            }

            else {
                copy_string(&tmp, pref);
                // strcpy(pref, tmp);
                e = e->next;
            }     
        }
        if (e == NULL){
            free(tmp);
            free(pref);  
            return false;
        }
    }
   
    free(tmp);
    free(pref);
    return false;
}

// // -------------------------------------------------------- INSERT ------------------------------------------ //


static char *getRest(const char *key, const char *pref){
   
    size_t keySize = strlen(key);
    size_t prefSize = strlen(pref);

    if (prefSize > keySize){
        return NULL;
    }

    size_t restSize = keySize - prefSize;

    char *rest = malloc(sizeof(char) * (restSize + 1));

    if (!rest){
        printf("Allocation Error : Failed to allocate rest\n");
        return NULL;
    }

    int j = prefSize;
    for (size_t i = 0; i < restSize; i++){
        rest[i] = key[j];
        j++;
    }

    rest[restSize] = '\0';
    return rest;
}

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

int setInsert(Set *radix, const char *key){
    
    if (!radix)
        return -1;

    // Set is empty
    if (radix->root == NULL){
        radix->root = rnNew("");
        if (!radix->root){
            return -1;
        }
        RNode *newNode = rnNew(key);
        if (!newNode){
            printf("Allocation Error : Failed to create a new Node\n");
            return -1;
        }

        addEdge(radix->root, newNode, key);
        radix->size++;
      
        return 1;
    }

    RNode *n = radix->root;
    Edge *e = NULL;

    char *pref = duplicate_string(n->key);
    if (!pref){
        return -1;
    }

    char *tmp = duplicate_string(n->key);
    if (!tmp){
        free(pref);
        return -1;
    }

    int cmp = 0;
    while (n != NULL){
        
        cmp = strcmp(key, n->key);

        if (cmp == 0){
            free(tmp);
            free(pref);
            return 0;
        }
        
        e = n->edges->head;
        bool prefix = false;
        while (e != NULL && !prefix){

            concatenate_string(&pref, e->label);
            if (isPrefix(key, pref)){
               
                copy_string(&tmp, pref);
                n = e->targetNode;
                 
                prefix = true;   
            }

            else {
                
                copy_string(&pref, tmp); // back to the latest state 
                     
                char *rest = getRest(key, pref);                 
                   
                char *commonPref = getCommonPref(e->label, rest);
                free(rest);
                
                if (!commonPref){

                    free(tmp);
                    free(pref);
                    return -1;
                }
                 
                if (strlen(commonPref) > 0){
                    
                    concatenate_string(&pref, commonPref);
                
                    RNode *newNode = rnNew(key); // create the new node
                        if (!newNode){
                            free(tmp);
                            free(pref);
                            free(commonPref);
                            return -1;
                        }

                    RNode *oldNode = e->targetNode;        
                        
                    if (isPrefix(oldNode->key, key)){ // if the key-to-insert is a prefix of the node's key
                        e->targetNode = newNode;
                            
                        char *newRest = getRest(newNode->key, tmp);
                        setEdgeLabel(e, newRest);

                        char *oldRest = getRest(oldNode->key, pref);
                        addEdge(newNode, oldNode, oldRest);

                        radix->size++;

                        free(tmp);
                        free(pref);
                        free(oldRest);
                        free(commonPref);
                        free(newRest);
                        return 1;

                    }

                    else { // if the key-to-insert is not a prefix of the node's key
                        
                        RNode *emptyNode = rnNew("");
                        if (!emptyNode){
                            free(tmp);
                            free(pref);
                            free(commonPref);
                            return -1;
                        }
                        
                        e->targetNode = emptyNode;
                    
                        char *oldRest;
                        if (strlen(oldNode->key) == 0) // node is empty{}
                            oldRest = getRest(e->label, commonPref);                                

                        else 
                            oldRest = getRest(oldNode->key, pref);  

                        setEdgeLabel(e, commonPref);

                        // add an edge to the old node with a new edge label
                        addEdge(emptyNode, oldNode, oldRest);
                            
                        // add an edge to the new node with a new edge label
                        char *newRest = getRest(newNode->key, pref);  
                        
                        addEdge(emptyNode, newNode, newRest);
                        radix->size++;

                        free(commonPref);
                        free(tmp);
                        free(pref);
                        free(oldRest);
                        free(newRest);
                        return 1;
                    }                       
                }
                else { // there is no common prefix between edge label and key
                    free(commonPref);
                    e = e->next;
                }
            }

        }//end while 2

        if (isLeaf(n) && strcmp(n->key, key) != 0){ // we can't make any progress
            RNode *newNode = rnNew(key);
            if (!newNode){
                free(tmp);
                free(pref);
                return -1;
            }

            char *newRest = getRest(newNode->key, pref);
            addEdge(n, newNode, newRest);
            radix->size++;
            
            free(pref);
            free(tmp);
            free(newRest);
            return 1;
        }

        // There is no edge containing any common prefix with key
        if (e == NULL){
            
            RNode *newNode = rnNew(key);
            if (!newNode){
                free(tmp);
                free(pref);
                return -1;
            }

            char *newRest = getRest(key, pref);
        
            addEdge(n, newNode, newRest);

            radix->size++;

            free(newRest);
            free(tmp);       
            free(pref);    
            return 1;
        }

        else {            
            cmp = strcmp(pref, key);
            if (cmp == 0 && strlen(e->targetNode->key) == 0){
                free(e->targetNode->key); // empty the old node key
                e->targetNode->key = duplicate_string(key);
                radix->size++;
                free(tmp);
                free(pref);
                return 1;
            }       
        }
    }
    free(tmp);
    free(pref);
    return -1;   
}

size_t setNbKeys(const Set *radix){
    if (!radix){
        return (size_t) -1;
    }

    return radix->size;
}

List *setGetAllStringPrefixes(const Set *set, const char *str)
{

   List *prefixList = listNew();
   if (!prefixList){
    printf("Failed to get all prefixes\n");
    return NULL;
   }

    RNode *n = set->root;
    Edge *e = NULL;
    char *pref = duplicate_string(n->key);
    if (!pref){
        return NULL;
    }

    char *tmp = duplicate_string(n->key);
    if (!tmp){
        free(pref);
        return NULL;
    }

    size_t pCount = 0; // prefixes count
    size_t strSize = strlen(str); // maximum prefixes count
    while (n != NULL && pCount < strSize){
        if (strcmp(str, n->key) == 0){ // no need to go further
            listInsertLast(prefixList, duplicate_string(n->key));
            pCount++;
            free(pref);
            free(tmp);
            return prefixList;
        }
       
        if (strlen(n->key) > 0 && isPrefix(str, n->key)){
            listInsertLast(prefixList, duplicate_string(n->key));
            pCount++;
        }
        if (isLeaf(n)){
            free(pref);
            free(tmp);
            return prefixList;
        }
        
        e = n->edges->head;
        bool isPref = false;
        while (e != NULL && !isPref){
            concatenate_string(&pref, e->label);

            if (isPrefix(str, pref)){
                copy_string(&tmp, pref);
                n = e->targetNode;
                isPref = true;
            }

            else {
                copy_string(&pref, tmp);
                e = e->next;
            }
        }

        if (e == NULL){
            free(pref);
            free(tmp);
            return prefixList;
        }
    }
    free(pref);
    free(tmp);
    return prefixList;
}

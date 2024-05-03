#include "Set.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/* STRUCTURES */

typedef struct RNode_t RNode;
typedef struct Edge_t Edge;
typedef struct EList_t EdgeList;

struct Edge_t // Edge 
{
    char *label;
    RNode *targetNode;
    struct Edge_t *next;
};

struct EList_t  // list of edges (inspired by linked list)
{
    size_t size;
    Edge *head;
    Edge *last;
};

struct RNode_t // radix node
{
    EdgeList *edges;
    char *key;
};

struct Set_t // radix set
{
    RNode *root;
    size_t size;
};

/* STATIC FUNCTIONS */
static char *duplicate_string(const char *str);
static void terminate(char *m);
static void concatenate_string(char **str1, const char *str2);
static void copy_string(char **copy, const char *str);
static int isPrefix(const char *word, const char *prefix);
static char *getRest(const char *key, const char *pref);
static char *getCommonPref(const char *str1, const char *str2);

static EdgeList *edgeListNew(void);
static bool edgesInsertLast(EdgeList *l, RNode *targetNode, const char *label);
static bool addEdge(RNode *source, RNode *target, const char *label);
static void setEdgeLabel(Edge *e, char *newLabel);

static RNode *rnNew(const char *key);
static bool isLeaf(RNode *node);

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

/**
 * @brief Concatenates 2 strings 
 * 
 * @param str1 a pointer to a string (will contain result)
 * @param str2 a string
 */
static void concatenate_string(char **str1, const char *str2){
    size_t len1 = strlen(*str1);
    size_t len2 = strlen(str2);
    
    *str1 = realloc(*str1, (len1 + len2 + 1) * sizeof(char));
    if (!*str1){
        terminate("Allocation Error : Failed to reallocate string\n");
    }

    strcat(*str1, str2);
}

/**
 * @brief copies a string
 * 
 * @param copy a pointer to a string (will contain copy)
 * @param str a string
 */
static void copy_string(char **copy, const char *str){
    size_t len1 = strlen(*copy);
    size_t len2 = strlen(str);
    
    *copy = realloc(*copy, (len1 + len2 +1) * sizeof(char));
    if (!*copy){
        terminate("Allocation Error : Failed to reallocate string\n");
    }

    strcpy(*copy, str);
}

/**
 * @brief frees a radix tree recursively a starting node (root node to free whole set)
 * 
 * @param n a pointer to a RNode (radix node)
 */
static void freeRec(RNode *n){
    Edge *edge = NULL;
    if (n->edges->head != NULL)
        edge = n->edges->head;
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

/**
 * @brief gets the remaining substring of a key (key - pref)
 * 
 * @param key a string
 * @param pref a string (normally a substring of key)
 * 
 * @return char* the remaining substring (key - pref)
 *         NULL if there is no substring remain
 */
static char *getRest(const char *key, const char *pref){
   
    size_t keySize = strlen(key);
    size_t prefSize = strlen(pref);

    if (prefSize > keySize){ // cannot get a remaining substring
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

/**
 * @brief Gets the common prefix of 2 strings
 *
 * @param str1 a string
 * @param str2 a string
 * 
 * @return char* the common prefix between str1 and str2               
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
 * @brief Checks if a radix node is a leaf
 * 
 * @param node a pointer to RNode object
 * 
 * @return true if the node is a leaf
 *         false otherwise
 */
static bool isLeaf(RNode *node){
   return (node->edges->size == 0);
}

/**
 * @brief Checks if a given word is a prefix of a string
 *
 * @param str a string
 * @param word a string (word to be checked)
 * 
 * @return bool : true if word is a prefix of str
 *                false otherwise
 */
static int isPrefix(const char *word, const char *prefix) {

    // Use strncmp to compare the first strlen(prefix) characters of word with prefix
    return strncmp(word, prefix, strlen(prefix)) == 0;
}

/**
 * @brief Creates an empty edge list
 * 
 * @return EdgeList*, a pointer to a a valid EdgeList
 *         NULL, in error case
 */
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

/**
 * @brief Inserts a new element (Edge) at the end of an Edgelist.
 * 
 * @param l A valid pointer to a  EdgeList object
 * @param targetNode A pointer to RNode object (target node of the inserted edge)
 * @param label a string, the label of the inserted edge
 * 
 * @return bool, true if the edge was successfully inserted
 *               false other wise
 */
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
    if (!edge->label){
        free(edge);
        return NULL;
    }
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

/**
 * @brief Adds an edge from a source node 
 * 
 * @param source a pointer to RNode, the source node
 * @param target a pointer to RNode, the target node
 * @param label  a string, the label of the new edge between both nodes
 * 
 * @return bool, true if the edge has been successfully inserted
 *               false otherwise
 */
static bool addEdge(RNode *source, RNode *target, const char *label){
    return edgesInsertLast(source->edges, target, label);
}

/**
 * @brief updates the label of an edge
 * 
 * @param e a pointer to Edge
 * @param newLabel a string, the new label 
 */
static void setEdgeLabel(Edge *e, char *newLabel){
    if (e->label){
        free(e->label);
    }
    e->label = duplicate_string(newLabel);
    if (!e->label){
        free(e);
        terminate("Error : Failed to duplicate string");
    }
}

/**
 * @brief Creates a new RNode object (radix node)
 * 
 * @param key a string, the key of the new node
 * 
 * @return RNode*, a pointer to a valid RNode object 
 *         NULL, in case of error
 */
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
    if (!n->key){
        free(n);
        free(n->edges);
        return NULL;
    }

    return n;
}

/* ----------------- RADIX SET OPERATIONS --------------------- */

Set *setCreateEmpty(void){
    Set *radix = malloc(sizeof(Set));
    if (!radix){
        printf("radix : allocation error\n");
        return NULL;
    }

    radix->root = NULL;
    radix->size = 0;
 
    return radix;
}//end setCreateEmpty


bool setContains(const Set *radix, const char *key){
    if (radix->root == NULL) // set is empty
        return false;
    
    RNode *n = radix->root;
    Edge *e = NULL;

    char *pref = duplicate_string(n->key); //initializing an empty pref
    if (!pref){
        terminate("Error : Failed to duplicate pref in setContains");
    }

    char *tmp = duplicate_string(n->key); //initializing an empty string
    if (!tmp){
        free(pref);
        terminate("Error : Failed to duplicate tmp in setContains");
    }
 
    while (n != NULL){
         
        int cmp = strcmp(key, n->key);
        if (cmp == 0){
            free(tmp);
            free(pref);
            return true;
        }
        
        if (isLeaf(n)){ 
            // node is a leaf, there's no further moves
            free(pref);
            free(tmp);
            return false;
        }
        
        e = n->edges->head;
        bool isPref = false;
        while (e != NULL && !isPref){

            // concatenate the current prefix with the current edge label
            concatenate_string(&pref, e->label); 
            
            if (isPrefix(key, pref)){
                
                copy_string(&tmp, pref); // update tmp to the current prefix
                n = e->targetNode;
                isPref = true;
            }

            else {
                copy_string(&pref, tmp); // back the the latest prefix
                e = e->next;
            }     
        }
        if (e == NULL){ 
            // all edges of the current node have been visited and key was not found
            free(tmp);
            free(pref);  
            return false;
        }
    }
   
    free(tmp);
    free(pref);
    return false;
}//end setContains


int setInsert(Set *radix, const char *key){
    
    if (!radix)
        return -1;

    // SET IS EMPTY
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

        if (!addEdge(radix->root, newNode, key)){ // add an edge from root
            free(newNode);
            return -1;
        }
        radix->size++;
      
        return 1;
    }

    // Start at the root
    RNode *n = radix->root;
    Edge *e = NULL;

    char *pref = duplicate_string(n->key); //initializing an empty string
    if (!pref){
        return -1;
    }

    char *tmp = duplicate_string(n->key); //initializing an empty string
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
            
            concatenate_string(&pref, e->label); // concatenate the current label with current prefix

            if (isPrefix(key, pref)){
               
                copy_string(&tmp, pref); // update tmp to the current prefix
                n = e->targetNode;
                 
                prefix = true;   
            }

            else {
                
                copy_string(&pref, tmp); // back to the latest state (latest prefix)
                     
                char *rest = getRest(key, pref); // rest of the current prefix   
                if (!rest){
                    printf("Error : Failed to get rest in setInsert\n");
                    free(tmp);
                    free(pref);
                    return -1;
                }      
                
                char *commonPref = getCommonPref(e->label, rest); // common prefix of the edge label and the rest of current prefix
                free(rest);
                
                if (!commonPref){
                    printf("Error : Failed to get common prefix in setInsert\n");
                    free(tmp);
                    free(pref);
                    return -1;
                }
                 
                if (strlen(commonPref) > 0){
                    
                    concatenate_string(&pref, commonPref); // concatenate the common prefix with the current prefix 
                
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
                            
                        char *newRest = getRest(newNode->key, tmp); // the remaining substring of new node
                        setEdgeLabel(e, newRest); // update the new node's label

                        char *oldRest = getRest(oldNode->key, pref); // the remaining substring of old node
                        if (!addEdge(newNode, oldNode, oldRest)){
                            free(tmp);
                            free(pref);
                            free(oldRest);
                            free(commonPref);
                            free(newRest);
                            return -1;
                        }

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
                        if (!addEdge(emptyNode, oldNode, oldRest)){
                            free(commonPref);
                            free(tmp);
                            free(pref);
                            free(oldRest);
                            return -1;

                        }
                            
                        // add an edge to the new node with a new edge label
                        char *newRest = getRest(newNode->key, pref);  
                        
                        if (!addEdge(emptyNode, newNode, newRest)){
                            free(commonPref);
                            free(tmp);
                            free(pref);
                            free(oldRest);
                            free(newRest);
                            return -1;
                        }

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

        if (isLeaf(n) && strcmp(n->key, key) != 0){ // node is a leaf and we cannot make any progress
            RNode *newNode = rnNew(key);
            if (!newNode){
                free(tmp);
                free(pref);
                return -1;
            }

            char *newRest = getRest(newNode->key, pref);
            if (!addEdge(n, newNode, newRest)){
                free(pref);
                free(tmp);
                free(newRest);
                return -1;
            }

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
        
            if (!addEdge(n, newNode, newRest)){
                free(newRest);
                free(tmp);
                free(pref);
            }

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
                if (!e->targetNode->key){
                    free(tmp);
                    free(pref);
                    return -1;
                }

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
}// end setInsert

size_t setNbKeys(const Set *radix){
    if (!radix){
        return (size_t) -1;
    }

    return radix->size;
}//end setNbKeys

void setFree(Set *set){
    if (!set)
        return;
        
    if (set->root){
        freeRec(set->root);
    }

    free(set);
}// end setFree

List *setGetAllStringPrefixes(const Set *set, const char *str) 
{
   // we proceed as in search
   List *prefixList = listNew();
   if (!prefixList){
    printf("Failed to get all prefixes\n");
    return NULL;
   }

   if (set->root == NULL){
    return prefixList;
   }
   
    RNode *n = set->root;
    Edge *e = NULL;
    char *pref = duplicate_string(n->key); //initializing an empty pref
    if (!pref){
        return NULL;
    }

    char *tmp = duplicate_string(n->key); //initializing an empty string
    if (!tmp){
        free(pref);
        return NULL;
    }

    size_t pCount = 0; // prefixes count
    size_t strSize = strlen(str); // maximum prefixes count
    while (n != NULL && pCount < strSize){
        if (strcmp(str, n->key) == 0){ // no need to go further

            char *copy = duplicate_string(n->key);
            if (!copy){
                free(tmp);
                free(pref);
                return NULL;
            }

            if (!listInsertLast(prefixList, copy)){
                free(copy);
                free(tmp);
                free(pref);
                return NULL;
            }

            pCount++;
            free(pref);
            free(tmp);
            return prefixList;
        }
       
        if (strlen(n->key) > 0 && isPrefix(str, n->key)){ // node is not empty and its key is a  prefix
              
            char *copy = duplicate_string(n->key);
            if (!copy){
                free(tmp);
                free(pref);
                return NULL;
            }

            if (!listInsertLast(prefixList, copy)){
                free(copy);
                free(tmp);
                free(pref);
                return NULL;
            }

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
}//end setGetAllStringPrefixes

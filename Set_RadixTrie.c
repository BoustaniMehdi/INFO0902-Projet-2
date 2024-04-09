#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "Set.h"
#include "List.h"

// To be completed. Should provide all functions in Set.h.


typedef struct Edge_t Edge;

struct RNode_t
{
    List *edges;
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
static int minString(const char *s1, const char *s2);
static Edge *createEdge(RNode *source, RNode *destination, const char* label);

static Edge *createEdge(RNode *source, RNode *destination, const char* label){
    assert(label != NULL && source != NULL && destination != NULL);

    Edge *newEdge = malloc(sizeof(Edge));
    if(!newEdge){
        printf("createEdge: allocation error\n");
        return NULL;
    }

    newEdge->label = malloc(strlen(label) + 1);
    if(!newEdge->label){
        free(newEdge);
        freeRadixTreeRec(source);
        printf("newRadixTree: allocation error\n");
        return NULL;
    }

    strcpy(newEdge->label, label);

    newEdge->targetNode = destination;
    source->edgesNumbers++;

    listInsertLast(source->edges, newEdge);

    return newEdge;
    
}


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

    n->edges = listNew();
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

    n->edgesNumbers = 0;

    return n;
}//end newRadixTree

/**
 * @brief free the tree recursively, including its keys and its edges
 *
 * @param n   the root of the tree to be freed.
 */
static void freeRadixTreeRec(RNode *n){
    assert(n != NULL && n->edges != NULL);

    LNode *current = n->edges->head;

    while(current != NULL){
        Edge *edge = (Edge *)current->value;
        freeRadixTreeRec(edge->targetNode);

        if(edge->label != NULL){
            free(edge->label);
        }

        current = current->next;
    }

    listFree(n->edges, false);

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

static int minString(const char *s1, const char *s2){
    assert(s1 != NULL && s2 != NULL);

    int s1size = strlen(s1);
    int s2size = strlen(s2);

    return (s1size <= s2size) ? s1size: s2size;
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

        LNode *current = n->edges->head;

        while(current != NULL){
            Edge *edge = (Edge *)current->value;
            count = 0;
            labelSize = strlen(edge->label);

            for(size_t j = 0; j < labelSize && key[j] != '\0'; j++){
                if(key[j] == edge->label[j]){
                    count++;
                }
                else{
                    break;
                }
            }

            if(count == labelSize){
                n = edge->targetNode;
                break;
            }

            current = current->next;
        }

        if(count != labelSize){
            break;
        }
            
    }

    return false;

}//end setContains

int setInsert(Set *radixTree, const char *key){
    assert(radixTree != NULL && key != NULL);

    // Arbre vide
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

    RNode *n = radixTree->root;

    size_t insertKeySize = strlen(key);
    size_t count = 0;

    // On stocke l'indice du dernier caractère du prefixe commun
    size_t prefixIndex = 0;

    char *commonLabelPrefix = malloc(insertKeySize * sizeof(char) + 1);
        if(!commonLabelPrefix){
        printf("setInsert: allocation error\n");
        return false;
    }

    RNode *prev = NULL;

    while(n != NULL){

        char *commonKeyPrefix = malloc(insertKeySize * sizeof(char) + 1);
        if(!commonKeyPrefix){
            printf("setInsert: allocation error\n");
            freeRadixTreeRec(n);
            return false;
        }

        LNode *current = n->edges->head;

        while(current != NULL){
            Edge *edge = (Edge *)current->value;
            size_t labelSize = strlen(edge->label);
            printf("%s\n", edge->label);
            count = 0;

            while(key[count] != '\0' && key[prefixIndex] == edge->label[count] && count < labelSize){
                commonLabelPrefix[prefixIndex] = key[count];
                count++;
                prefixIndex++;
            }

            // Le préfixe n'a été trouvé dans aucune arêtes
            if(count == 0){
                //Le noeud n'a plus d'arête à vérifier, on crée un nouveau noeud
                if(current->next == NULL){ 
                    RNode *newNode = newRadixTree(key);
                    if(!newNode){
                        printf("setInsert: error in creation of new node\n");
                        freeRadixTreeRec(n);
                        return false;
                    }

                    Edge *newEdge = createEdge(n, newNode, key);
                    radixTree->size++;

                    return true;
                }

                //On passe à l'arête suivante
                else{
                    current = current->next;
                    continue;
                }
            }
            
            //Le prefixe de la clé est trouvé dans une des arêtes (par exemple on a trouvé le préfixe "te" pour tea)
            if(count > 0 && count < labelSize){
                //Dans ce cas, on avance
                n = edge->targetNode;
                break;
                
            }
        } 

        if(n->key != NULL){
            if(strcmp(key, n->key) == 0){
                printf("L'élement \"%s\" est déjà présent dans l'arbre\n", key);
                return true;
            }

            count = 0;

            while(key[count] != '\0' && key[count] == n->key[count] && count < insertKeySize){
                commonKeyPrefix[count] = key[count];
                count++;
            }

            if(radixTree->root->key != NULL){

                printf("non\n");

                if(count == 0){

                    // Noeud intérmediaire (sans clé)
                    RNode *intermediate = newRadixTree(NULL);
                    if(!intermediate){
                        printf("setInsert: allocation error\n");
                        free(commonLabelPrefix);
                        return false;
                    }
                
                    Edge *newEdge1 = createEdge(intermediate, n, n->key);
                    if(!newEdge1){
                        printf("setInsert: error while creating a new edge\n");
                        freeRadixTreeRec(n);
                        return false;
                    }

                    RNode *newNode = newRadixTree(key);
                    if(!newNode){
                        printf("setInsert: error while creating a new node\n");
                        freeRadixTreeRec(n);
                        return false;
                    }

                    Edge *newEdge2 = createEdge(intermediate, newNode, key);
                    if(!newEdge2){
                        printf("setInsert: error while creating a new edge\n");
                        freeRadixTreeRec(n);
                        return false;
                    }

                    // On ajoute le noeud intermediaire sans clé + un noeud avec la nouvelle clé
                    radixTree->size += 2;

                    radixTree->root = intermediate;

                    return true;
                }

                if(count > 0){
                    if(count >= strlen(n->key)){
                        RNode *newNode = newRadixTree(key);
                        if(!newNode){
                            printf("setInsert: error in creation of new node\n");
                            freeRadixTreeRec(n);
                            return false;
                        }

                        Edge *newEdge = createEdge(n, newNode, key + count);
                        if(!newEdge){
                            return false;
                        }

                        radixTree->size++;
                        n = newEdge->targetNode;

                    }

                    else{
                        // TODO
                    }
                }
            }
        }

        break;
    }

    return true;
}

//-------------------------- FONCTIONS DE TESTS ---------------------------//

void getRoot(Set *radixTree){
    if(radixTree->root->key == NULL){
        printf("OKK");
    }
    else{
        printf("%s", radixTree->root->key);
    }   

}

// Fonction utilitaire pour générer le fichier DOT récursivement
void writeRadixTreeDOT(FILE *file, RNode *node) {
    if (node == NULL)
        return;

    // Écrire le nœud
    fprintf(file, "\t\"%p\" [label=\"%s\"];\n", (void *)node, node->key ? node->key : "NULL");

    // Parcourir toutes les arêtes sortantes du nœud
    LNode *current = node->edges->head;
    while (current != NULL) {
        Edge *edge = (Edge *)current->value;

        // Écrire l'arête
        fprintf(file, "\t\"%p\" -> \"%p\" [label=\"%s\"];\n", (void *)node, (void *)(edge->targetNode), edge->label);

        // Appeler récursivement la fonction pour écrire le sous-arbre associé à cette arête
        writeRadixTreeDOT(file, edge->targetNode);

        current = current->next;
    }
}

// Fonction utilitaire pour générer le fichier DOT de l'arbre entier
void generateRadixTreeDOT(Set *radixTree, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s.\n", filename);
        return;
    }

    // Écrire l'en-tête du fichier DOT
    fprintf(file, "digraph RadixTree {\n");

    // Écrire le contenu de l'arbre
    writeRadixTreeDOT(file, radixTree->root);

    // Écrire la fin du fichier DOT
    fprintf(file, "}\n");

    fclose(file);
}


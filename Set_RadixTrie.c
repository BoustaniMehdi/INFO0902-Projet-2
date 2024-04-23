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
static Edge *createEdge(RNode *source, RNode *destination, const char* label);

static char *duplicate_string(const char *str);

static Edge *createEdge(RNode *source, RNode *destination, const char* label){
    assert(label != NULL && source != NULL && destination != NULL);

    Edge *newEdge = malloc(sizeof(Edge));
    if(!newEdge){
        printf("createEdge: allocation error\n");
        return NULL;
    }

    newEdge->label = duplicate_string(label);

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
        n->key = duplicate_string(key);
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
        if(edge->targetNode)
            freeRadixTreeRec(edge->targetNode);

        if(edge->label != NULL){
            free(edge->label);
        }

        current = current->next;
    }

    listFree(n->edges, true);

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

bool setContains(const Set *radixTree, const char *key){
    assert(radixTree != NULL && key != NULL);

    RNode *n = radixTree->root;

    size_t count = 0;

    // Chaîne de caractères utilisée pour stocker la clé à chercher et lui apporter des modifications (la couper par exemple)
    char *tempKey = duplicate_string(key);

    while(n != NULL){

        if(n->key && strcmp(key, n->key) == 0)
            return true;

        // Arête courante
        LNode *current = n->edges->head;

        while(current != NULL){
            Edge *edge = (Edge *)current->value;
            size_t labelSize = strlen(edge->label);

            count = 0;
            
            // On compare la clé à insérer aux arêtes de l'arbre (search) 
            while(tempKey[0] == edge->label[0] && tempKey[count] != '\0' && tempKey[count] == edge->label[count]){
                count++;
            }

            // Le préfixe n'a été trouvé dans aucune arêtes
            if(count == 0){
                // Le noeud n'a plus d'arête à vérifier, la clé n'est pas présente
                if(current->next == NULL)
                    return false;

                // On passe à l'arête suivante s'il y en a
                else{
                    current = current->next;
                    continue;
                }
            }
            
            // Le prefixe de la clé est trouvé dans une des arêtes (par exemple on a trouvé le préfixe "te" pour tea)
            else if(count > 0 && count <= labelSize){
                //On passe au noeud suivant
                n = edge->targetNode;

                if(n->key){
                    if(strcmp(n->key, key))
                        return false;
                }

                // Nous gardons que la clé à insérer sans son préfixe déjà trouvé
                tempKey += count;

                // On break pour revenir à la boucle de départ pour ensuite répéter le processus (récursif)
                break;
            }

            else{
                return true;
            }
        }

        if(n->key && strcmp(key, n->key) == 0)
            return true;

        else{
            if(current){
                Edge *e = (Edge *)current->value;
                n = e->targetNode;
            }

            continue;
        }

        free(tempKey);

        return false;
            
    }

    return false;

}//end setContains

int setInsert(Set *radixTree, const char *key){
    assert(radixTree != NULL && key != NULL);

    // La clé insérée est vide
    if(!strcmp(key, ""))
        return false;

    // Si l'arbre est vide, on crée un noeud (racine) seul
    if(!radixTree->root){
        
        radixTree->root = newRadixTree(key);

        if(!radixTree->root){
            printf("setInsert: error while creating the tree\n");
            return false;
        }

        radixTree->size++;
        radixTree->root->edgesNumbers = 0;

        return true;
    }

    // On commence à la racine de l'arbre
    RNode *n = radixTree->root;

    size_t keySize = strlen(key);

    // Compteur pour rechercher la clé dans l'arbre et le modifier en conséquence
    size_t count1 = 0;
    size_t count2 = 0;

    // Chaîne de caractères utilisée pour stocker la clé à insérer et lui apporter des modifications
    char *tempKey = duplicate_string(key);
    char *tempString = duplicate_string(key);

    // Noeud rencontré précedemment
    RNode *prev = NULL;

    char *accumulatorString = malloc((keySize + 1)* sizeof(char));
    if(!accumulatorString){
        printf("setInsert: allocation error\n");
        freeRadixTreeRec(n);
        return false;
    }

    while(n != NULL){

        prev = n;

        // Chaîne de caractères permettant de stocker le préfixe commun (label-clé ou clé-clé)
        char *commonKeyPrefix = malloc((keySize + 1)* sizeof(char));
        if(!commonKeyPrefix){
            printf("setInsert: allocation error\n");
            free(accumulatorString);
            freeRadixTreeRec(n);
            return false;
        }

        // Arête courante
        LNode *current = n->edges->head;

        // Arête rencontrée précedemment
        LNode *prevEdge = NULL;

        while(current != NULL){
            Edge *edge = (Edge *)current->value;
            size_t labelSize = strlen(edge->label);

            count1 = 0;
            prevEdge = current;
            
            // On compare la clé à insérer aux arêtes de l'arbre (search) 
            while(tempKey[0] == edge->label[0] && tempKey[count1] != '\0' && tempKey[count1] == edge->label[count1]){
                commonKeyPrefix[count1] = key[count1];
                count1++;
            }

            commonKeyPrefix[count1] = '\0';

            // Le préfixe n'a été trouvé dans aucune arêtes
            if(count1 == 0){
                // Le noeud n'a plus d'arête à vérifier, on crée un nouveau noeud
                if(current->next == NULL){ 
                    RNode *newNode = newRadixTree(key);
                    if(!newNode){
                        printf("setInsert: error in creation of new node\n");
                        free(commonKeyPrefix);
                        return false;
                    }
                    radixTree->size++;

                    if(strlen(tempKey)){
                        Edge *newEdge = createEdge(n, newNode, tempKey);
                        if(!newEdge){
                            printf("setInsert: error while creating a new edge\n");
                            free(commonKeyPrefix);
                            return false;
                        }
                    }
                    else{
                        Edge *newEdge = createEdge(n, newNode, key);
                        if(!newEdge){
                            printf("setInsert: error while creating a new edge\n");
                            free(commonKeyPrefix);
                            return false;
                        }
                    }

                    return true;
                }

                // On passe à l'arête suivante s'il y en a
                else{
                    current = current->next;
                    continue;
                }
            }
            
            // Le prefixe de la clé est trouvé dans une des arêtes (par exemple on a trouvé le préfixe "te" pour tea)
            else if(count1 > 0 && count1 <= labelSize){
                // Ici on gère le cas où on insert un noeud ayant la même clé qu'une arête et où le prochain noeud ne possède pas de clé. 
                // Dans ce cas, on place la clé à insérer dans le noeud vide.
                if(!strcmp(tempKey, edge->label) && edge->targetNode->key == NULL)
                {
                    edge->targetNode->key = duplicate_string(key);
                    radixTree->size++;

                    return true;
                }
                
                //On passe au noeud suivant
                n = edge->targetNode;

                // Nous gardons que la clé à insérer sans son préfixe déjà trouvé
                tempKey += count1;

                // On break pour revenir à la boucle de départ pour ensuite répéter le processus (récursif)
                break;
            }

            else{
                // printf("L'élement \"%s\" est déjà présent dans l'arbre\n", key);
                return false;
            }
        } 

        // On vérifie avant tout si le noeud courant n'est pas un noeud vide (intermediaire)
        if(n->key){
            if(strcmp(key, n->key) == 0){
                // printf("L'élement \"%s\" est déjà présent dans l'arbre\n", key);
                return false;
            }

            // On met le compteur de la taille du préfixe commun à 0
            count2 = 0;

            // On vérifie si la clé à insérer et la clé du noeud actuel ont un préfixe en commun ou non
            while(key[count2] != '\0' && key[count2] == n->key[count2] && count2 < keySize){
                commonKeyPrefix[count2] = key[count2];
                count2++;
            }

            commonKeyPrefix[count2] = '\0';

            if(count2 == 0){
                // Création du noeud vide (sans clé)
                RNode *intermediate = newRadixTree(NULL);
                if(!intermediate){
                    printf("setInsert: allocation error\n");
                    free(commonKeyPrefix);
                    return false;
                }

                // On relie le noeud vide au noeud courant
                Edge *newEdge1 = createEdge(intermediate, n, n->key);
                if(!newEdge1){
                    printf("setInsert: error while creating a new edge\n");
                    free(commonKeyPrefix);
                    return false;
                }

                // Création du nouveau noeud pour la clé à insérer
                RNode *newNode = newRadixTree(key);
                if(!newNode){
                    printf("setInsert: error while creating a new node\n");
                    free(commonKeyPrefix);
                    return false;
                }
                radixTree->size++;

                // On relie le noeud vide au nouveau noeud
                Edge *newEdge2 = createEdge(intermediate, newNode, key);
                if(!newEdge2){
                    printf("setInsert: error while creating a new edge\n");
                    free(commonKeyPrefix);
                    return false;
                }

                radixTree->root = intermediate;

                return true;
            }

            else{      
                // Chaîne de caractères temporaire pour "couper" la clé à insérer
                char *temp = duplicate_string(n->key);

                // Si la clé à insérer est le préfixe d'une clé de l'arbre, on n'a pas besoin de noeud supplémentaire (exemple: tea -> team)
                if(strcmp(commonKeyPrefix, key) == 0){ 
                    // On récupère l'arête courante
                    Edge *e = (Edge *)current->value;

                    // On crée le noeud contenant la clé à insérer
                    RNode *newNode1 = newRadixTree(key);
                    if(!newNode1){
                        printf("setInsert: error while creating a new node\n");
                        free(commonKeyPrefix);
                        return false;
                    }

                    // Le noeud courant va maintenant pointer vers le nouveau noeud
                    e->targetNode = newNode1;

                    // Le label de l'arête courante correspond au préfixe commun
                    strcpy(e->label, key + strlen(accumulatorString));
                    
                    // On relie le nouveau noeud au noeud courant 
                    Edge *newEdge2 = createEdge(newNode1, n, temp + count2);
                    if(!newEdge2){
                        printf("setInsert: error while creating a new edge\n");
                        free(commonKeyPrefix);
                        return false;
                    }

                    // Si le noeud actuel a un prédecesseur, il faut modifier le label de l'arête les reliant avec
                    if(prev->key){
                        strcpy(e->label, tempString);
                    }
                }

                // La clé courante est un préfixe de la clé à insérer
                else if(count2 >= strlen(n->key)){
                    tempString = duplicate_string(tempKey);

                    // Si le noeud courant possède des arêtes, il faut d'abord vérifier si une de ses arêtes n'est pas
                    // de nouveau un prefixe de la clé à insérer sans ses premiers caractères (tempKey) 
                    if(current){
                        Edge *e = (Edge *)current->value;

                        n = e->targetNode;

                        strcpy(tempKey, key + count2);

                        continue;
                    }
                    
                    // Sinon, on crée simplement un nouveau noeud relié au noeud courant
                    else{
                        RNode *newNode1 = newRadixTree(key);
                        if(!newNode1){
                            printf("setInsert: error while creating a new node\n");
                            free(commonKeyPrefix);
                            return false;
                        }

                        Edge *newEdge1 = createEdge(n, newNode1, key + count2);
                        if(!newEdge1){
                            printf("setInsert: error while creating a new edge\n");
                            free(commonKeyPrefix);
                            return false;
                        }
                    }
                }

                else{
                    // Si ce noeud a un prédécesseur, il faut adapter le label de l'arête précedente en fonction du préfixe commun trouvé
                    if(prevEdge){

                        Edge *e = (Edge *)prevEdge->value;

                        char *fusion = malloc((strlen(n->key) + 1)* sizeof(char));
                        if(!fusion){
                            printf("setInsert: allocation error\n");
                            free(commonKeyPrefix);
                            return false;
                        }

                        strncpy(fusion, e->label, count1);

                        strcpy(e->label, fusion);
                        
                        RNode *intermediate = newRadixTree(NULL);
                        if(!intermediate){
                            printf("setInsert: error while creating a new node\n");
                            free(commonKeyPrefix);
                            return false;
                        }

                        e->targetNode = intermediate;
                        

                        Edge *newEdge1 = createEdge(intermediate, n, temp + count2);
                        if(!newEdge1){
                            printf("setInsert: error while creating a new edge\n");
                            free(commonKeyPrefix);
                            return false;
                        }

                        RNode *newNode2 = newRadixTree(key);
                        if(!newNode2){
                            printf("setInsert: error while creating a new node\n");
                            free(commonKeyPrefix);
                            return false;
                        }

                        Edge *newEdge2 = createEdge(intermediate, newNode2, key + count2);
                        if(!newEdge2){
                            printf("setInsert: error while creating a new edge\n");
                            free(commonKeyPrefix);
                            return false;
                        }
                    }
                    
                    else{
                        // Le noeud courant devient un noeud vide (sans clé)
                        n->key = NULL;

                        RNode *newNode1 = newRadixTree(temp);
                        if(!newNode1){
                            printf("setInsert: error while creating a new node\n");
                            free(commonKeyPrefix);
                            return false;
                        }

                        Edge *newEdge1 = createEdge(n, newNode1, temp + count2);
                        if(!newEdge1){
                            printf("setInsert: error while creating a new edge\n");
                            free(commonKeyPrefix);
                            return false;
                        }

                        RNode *newNode2 = newRadixTree(key);
                        if(!newNode2){
                            printf("setInsert: error while creating a new node\n");
                            free(commonKeyPrefix);
                            return false;
                        }

                        Edge *newEdge2 = createEdge(n, newNode2, key + count2);
                        if(!newEdge2){
                            printf("setInsert: error while creating a new edge\n");
                            free(commonKeyPrefix);
                            return false;
                        }
                    }
                }
                

                //S'il n'y a qu'un noeud dans l'arbre et qu'il a un préfixe commun avec la clé à insérer, on crée un noeud intermediaire relié au 2
                if(radixTree->size == 1){
                    // Noeud intérmediaire (sans clé)
                    RNode *intermediate = newRadixTree(NULL);
                    if(!intermediate){
                        printf("setInsert: allocation error\n");
                        free(commonKeyPrefix);
                        return false;
                    }

                    Edge *newEdge2 = createEdge(intermediate, radixTree->root, commonKeyPrefix);
                    if(!newEdge2){
                        printf("setInsert: error while creating a new edge\n");
                        free(commonKeyPrefix);
                        return false;
                    }

                    radixTree->root = intermediate;
                }
                

                // On ajoute le noeud intermediaire sans clé + un noeud avec la nouvelle clé
                radixTree->size++;

                free(temp);
                free(commonKeyPrefix);
                
                return true;
            
            }
        }

        // Si la clé vaut NULL (i.e le neoud est vide), on regarde s'il a bien des arêtes et si oui on passe à son successeur
        else{

            //On vérifie si la clé à insérer a un prefixe commun avec le prefixe des noeuds successeurs du noeud vide courant
            if(prevEdge){
                Edge *edge = (Edge *)prevEdge->value;

                if(count1 > 0 && count1 < strlen(edge->label)){

                    RNode *intermediate = newRadixTree(NULL);
                    if(!intermediate){
                        printf("setInsert: error while creating a new node\n");
                        free(commonKeyPrefix);
                        return false;
                    }

                    Edge *newEdge1 = createEdge(intermediate, edge->targetNode, edge->label + count1);
                    if(!newEdge1){
                        printf("setInsert: error while creating a new edge\n");
                        free(commonKeyPrefix);
                        return false;
                    }

                    char *newTemp = duplicate_string(accumulatorString);
                    strcat(newTemp, edge->label);

                    size_t count3 = 0;

                    char *commonKeyPrefix2 = malloc((keySize + 1)* sizeof(char));
                    if(!commonKeyPrefix2){
                        printf("setInsert: allocation error\n");
                        freeRadixTreeRec(n);
                        return false;
                    }

                    // On fait une nouvelle comparaisons entre l'ancien préfixe commun et la nouvelle clé
                    while(newTemp[count3] != '\0' && newTemp[count3] == key[count3]){
                        commonKeyPrefix2[count3] = newTemp[count3];
                        count3++;
                    }

                    edge->targetNode = intermediate;
                    strcpy(edge->label, commonKeyPrefix2 + strlen(accumulatorString));

                    RNode *newNode2 = newRadixTree(key);
                    if(!newNode2){
                        printf("setInsert: error while creating a new node\n");
                        free(commonKeyPrefix);
                        return false;
                    }
                    radixTree->size++;

                    Edge *newEdge2 = createEdge(intermediate, newNode2, tempKey);
                    if(!newEdge2){
                        printf("setInsert: error while creating a new edge\n");
                        free(commonKeyPrefix);
                        return false;
                    }

                    return true;
                }   
            }
            if(current){
                Edge *e = (Edge *)current->value;

                n = e->targetNode;
                strcat(accumulatorString, e->label);
            }
            continue;
        }   
    }

    return true;
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


List *setGetAllStringPrefixes(const Set *set, const char *str)
{
    List *prefixList = listNew();
    if (!prefixList){
        printf("Failed to get all prefixes\n");
        return NULL;
    }
    size_t wordLength = strlen(str);

    char *prefix = malloc(sizeof(char) * wordLength + 1 );
    if (!prefix){
        printf("Failed to allocate prefix \n");
        listFree(prefixList, true);
        return NULL;
    }

    for(size_t i = 0; i < wordLength; i++){
        prefix[i] = str[i];
        prefix[i+1] = '\0';

        if(setContains(set, prefix)){
            listInsertLast(prefixList, duplicate_string(prefix));
        }
    }

   return prefixList;
}


//-------------------------- FONCTIONS DE TESTS ---------------------------//

// Fonction utilitaire pour générer le fichier DOT récursivement
void writeRadixTreeDOT(FILE *file, RNode *node) {
    if (node == NULL)
        return;

    // Écrire le nœud
    fprintf(file, "\t\"%p\" [label=\"%s\"];\n", (void *)node, node->key ? node->key : "");


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


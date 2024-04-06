#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "Set.h"


int main() {
    // Créer un ensemble vide
    Set *mySet = setCreateEmpty();
    if (mySet == NULL) {
        printf("Erreur lors de la création de l'ensemble.\n");
        return 1;
    }

    setInsert(mySet, "apple");
    //printf("Ajout de l'élément \"%s\" à l'ensemble.\n", keys[i]);

    setInsert(mySet, "app");

    // // Ajouter des éléments à l'ensemble
    // const char *keys[] = {"apple", "banana", "orange", "grape", "pineapple"};
    // for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
    //     // Vérifier si l'élément est déjà présent dans l'ensemble
    //     if (!setContains(mySet, keys[i])) {
    //         // Si l'élément n'est pas présent, l'ajouter
    //         // Notez que vous devez implémenter cette fonction
    //         setInsert(mySet, keys[i]);
    //         printf("Ajout de l'élément \"%s\" à l'ensemble.\n", keys[i]);
    //     } else {
    //         printf("L'élément \"%s\" est déjà présent dans l'ensemble.\n", keys[i]);
    //     }
    // }

    // Afficher le nombre d'éléments dans l'ensemble
    printf("Nombre d'éléments dans l'ensemble : %zu\n", setNbKeys(mySet));

    // Libérer la mémoire utilisée par l'ensemble
    setFree(mySet);

    return 0;
}

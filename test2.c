#include <stdio.h>
#include "Set.h"
#include <stdlib.h>


int main() {
    Set *bst = setCreateEmpty();
    // Ajoutez vos opérations d'insertion d'éléments ici

    const char *keys[] = {"boat", "team", "tea", "bo"};
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
        // Vérifier si l'élément est déjà présent dans l'ensemble
        if (!setContains(bst, keys[i])) {
            // Si l'élément n'est pas présent, l'ajouter
            setInsert(bst, keys[i]);
            printf("Ajout de l'élément \"%s\" à l'ensemble.\n", keys[i]);
        } else {
            printf("L'élément \"%s\" est déjà présent dans l'ensemble.\n", keys[i]);
        }
    }

    // Générez le fichier DOT
    generateDot(bst);

    // Utilisez Graphviz pour générer l'image PNG
    system("dot -Tpng bst.dot -o bst.png");

    setFree(bst);
    return 0;
}

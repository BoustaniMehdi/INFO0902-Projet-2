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

    // Ouvrir le fichier english.txt
    FILE *file = fopen("english.txt", "r");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier english.txt.\n");
        return 1;
    }

    char word[100]; // Assumer que la longueur maximale d'un mot est 100
    int line_number = 1;
    int i = 0;

    // Lire chaque ligne du fichier
    while (fgets(word, sizeof(word), file) != NULL) {
        // Si on est entre la 15e et la 30e ligne, insérer le mot dans l'ensemble
        if (line_number <= 15 && line_number <= 30) {
            // Supprimer le saut de ligne à la fin
            word[strcspn(word, "\n")] = '\0';

            // Insérer le mot dans l'ensemble
            if (setInsert(mySet, word))
                printf("Ajout de l'élément \"%s\" à l'ensemble.\n", word);

            // Incrémenter le compteur de mots insérés
            i++;
        }

        // Arrêter la lecture du fichier après la 30e ligne
        if (line_number >= 30)
            break;

        // Incrémenter le numéro de ligne
        line_number++;
    }

    printf("%d\n", setContains(mySet, "pyrosomes"));

    // Fermer le fichier
    fclose(file);

    // Afficher le nombre d'éléments dans l'ensemble
    printf("Nombre d'éléments dans l'ensemble : %zu\n", setNbKeys(mySet));

    generateRadixTreeDOT(mySet, "tree.dot");

    system("dot -Tpng tree.dot -o tree.png");

    // Libérer la mémoire utilisée par l'ensemble
    setFree(mySet);

    return 0;
}

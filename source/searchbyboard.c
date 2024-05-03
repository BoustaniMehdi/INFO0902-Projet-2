#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "Board.h"
#include "List.h"
#include "Set.h"

#define BUFFER_SIZE 500

static List *readLines(const char *filename);

static List *readLines(const char *filename)
{

    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        fprintf(stderr, "readLines: Error while opening '%s'.\n", filename);
        exit(1);
    }

    List *lines = listNew();
    if (!lines)
    {
        fprintf(stderr, "readLines: Error in 'listNew'.\n");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, fp))
    {
        size_t length = strlen(buffer);

        if (buffer[length - 1] == '\n')
            buffer[--length] = '\0';

        char *copy = malloc(length + 1);
        if (!copy)
        {
            fprintf(stderr, "readLines: Error in 'malloc'.\n");
            exit(1);
        }
        memcpy(copy, buffer, length + 1);
        listInsertLast(lines, copy);
    }
    fclose(fp);

    return lines;
}


int main(int argc, char **argv)
{
    // Check arguments
    if (argc != 3)
    {
        printf("Usage: %s <File> <board_size>\n", argv[0]);
        return -1;
    }

    // grid size
    size_t size = atoi(argv[2]);

    // Load the lexicon
    List *words = readLines(argv[1]);

    printf("%zu words have been read.\n", listSize(words));

    // ---------------------------
    // Search driven by the board
    // ---------------------------

    // creation of the Set
    // ---------------------------

    printf("Creation of the set...");
    clock_t begin = clock();

    Set *set = setCreateEmpty();
    for (LNode *p = words->head; p != NULL; p = p->next)
    {
        if (setInsert(set, p->value) == 0)
            printf("\nkey '%s' is duplicated in %s\n", (char *)p->value, argv[1]);
    }

    clock_t end = clock();
    printf("Finished in %ld ms\n", (end - begin) * 1000 / CLOCKS_PER_SEC);

    // create a random board
    // ---------------------
    srand(42); // change into srand(time(NULL)) if you want random boards
    Board *board = boardCreate(size, NULL);

    boardDisplay(board);

    // search driven by the board
    // --------------------------
    printf("Search driven by the board...");
    begin = clock();
    List *result = boardGetAllWordsFromSet(board, set);
    end = clock();

    printf("%zu words found on the grid\n", listSize(result));
    printf("Finished in %ld ms\n", (end - begin) * 1000 / CLOCKS_PER_SEC);

    // display longest word found
    // --------------------------
    char *longestWord;
    size_t maxLength = 0;
    for (LNode *p = result->head; p != NULL; p = p->next)
    {
        size_t lengthWord = strlen(p->value);
        if (lengthWord > maxLength)
        {
            maxLength = lengthWord;
            longestWord = p->value;
        }
    }

    if (maxLength == 0)
    {
        printf("No word was found.\n");
    }
    else if (boardContainsWord(board, longestWord))
    {
        printf("Longest word found: %s\n", longestWord);
        boardDisplay(board);
    }
    else
    {
        printf("Something went wrong.\n");
    }

    // /* Uncomment to print all words found
    // for (LNode *p = result->head; p != NULL; p = p->next)
    //     printf("[%s]", (char *)p->value);
    // printf("\n");


    listFree(result, true);
    listFree(words, true);
    boardFree(board);
    setFree(set);

    return 0;
}

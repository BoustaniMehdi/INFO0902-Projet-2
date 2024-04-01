#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "Board.h"
#include "List.h"

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
        printf("Usage: %s <File> <size>\n", argv[0]);
        return -1;
    }

    // grid size
    size_t size = atoi(argv[2]);

    // Load the lexicon
    List *words = readLines(argv[1]);

    printf("%zu words have been read.\n", listSize(words));

    if (!words)
        return -1;

    // ----------------------------------
    // Search driven by the lexicon:
    // ----------------------------------

    // create a random board
    // ---------------------

    // create a random board
    srand(42); // change into srand(time(NULL)) to get a random board
    Board *board = boardCreate(size, NULL);

    boardDisplay(board);

    // search driven by the lexicon
    // ----------------------------

    printf("Search driven by the lexicon...");
    clock_t begin = clock();
    List *result = listNew();

    for (LNode *p = words->head; p != NULL; p = p->next)
    {
        if (boardContainsWord(board, p->value))
        {
            char *copy = malloc(strlen(p->value) + 1);
            if (!copy)
            {
                fprintf(stderr, "Error in 'malloc'.\n");
                exit(1);
            }
            memcpy(copy, p->value, strlen(p->value) + 1);
            listInsertLast(result, copy);
        }
    }

    clock_t end = clock();
    unsigned long millis = (end - begin) * 1000 / CLOCKS_PER_SEC;
    printf("\n%zu words found on the board\n", listSize(result));
    printf("Finished in %ld ms\n", millis);

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

    /* Uncomment to print all words found
    for (LNode *p = result->head; p!=NULL; p = p->next)
        printf("[%s]", (char *)p->value);
    printf("\n");
    */

    listFree(result, true);
    listFree(words, true);
    boardFree(board);

    return 0;
}

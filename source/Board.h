
#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdbool.h>
#include "List.h"
#include "Set.h"

/* Board (opaque) structure */
typedef struct Board_t Board;

/**
 * @brief Create a square board. If 'letters' is NULL, the board is filled in with random letters.
 *        Otherwise, it is filled in with the content of 'letters' (row-major order).
 *
 * @param size             The size of the board (number of rows/columns)
 * @param letters          NULL or an array of size size*size.
 * @return Board*          The created board
 */
Board *boardCreate(size_t size, const char *letters);

/**
 * @brief Free the board structure
 *
 * @param board            A pointer to a board.
 */
void boardFree(Board *board);

/**
 * @brief Return true if the word appears in the board, false otherwise.
 *
 * @param board            A pointer to a board
 * @param word             A valid string
 * @return true            if the word is contained in the board
 * @return false           if the word does not appear in the board
 */
bool boardContainsWord(Board *board, const char *word);

/**
 * @brief Display the board. If boardContainsWord was called before and the word was found,
 *        the letters corresponding to that word are highlighted on the board.
 *
 * @param board            A pointer to a board
 */
void boardDisplay(Board *board);

/**
 * @brief Return a list of all words from the set that appears on the board using
 *        the search driven by the board. The returned list and its content needs to be
 *        freed by the user. The order of the words in the list is arbitrary. The list
 *        should not contain duplicates.
 *
 * @param board            A pointer to a board
 * @param set              A set containing words
 * @return List*           A list of strings corresponding to the word on the board
 */
List *boardGetAllWordsFromSet(Board *board, Set *set);

#endif // !_BOARD_H_

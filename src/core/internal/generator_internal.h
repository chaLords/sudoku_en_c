
/**
 * @file generator_internal.h
 * @brief Internal generator functions not exposed in the public API
 * @author Gonzalo Ramírez
 * @date 2025-10-19
 * 
 * This header contains private functions used by the Sudoku generator
 * during the board generation process. These functions are implementation
 * details and should not be used by external code.
 */

#ifndef SUDOKU_GENERATOR_INTERNAL_H
#define SUDOKU_GENERATOR_INTERNAL_H

#include "sudoku/core/types.h"
#include <stdbool.h>

/**
 * @brief Completes a partially filled board using recursive backtracking
 * 
 * Used internally by the hybrid generator. Assumes the board has at least
 * the main diagonal subgrids filled (typically by Fisher-Yates).
 * 
 * This function uses systematic exploration with backtracking to fill all
 * remaining empty cells while respecting Sudoku rules. It tries numbers
 * in randomized order to ensure variety in generated boards.
 * 
 * @param board Pointer to the board to complete (WILL BE MODIFIED)
 * @return true if successfully completed, false if no solution exists
 * 
 * @note This function modifies the board in-place
 * @note This is an internal function - not part of the public API
 * @warning Recursive function - may consume significant stack space
 * 
 * @pre board != NULL
 * @post If returns true, board contains a valid complete Sudoku
 */
bool sudoku_complete_backtracking(SudokuBoard *board);

/**
 * @brief Counts the number of solutions to a puzzle
 * 
 * Uses exhaustive backtracking to count solutions, stopping when the
 * specified limit is reached. This is used during puzzle generation
 * to verify that puzzles have unique solutions.
 * 
 * @param board Pointer to the board (will be temporarily modified but restored)
 * @param limit Maximum number of solutions to count before stopping
 * @return Number of solutions found (up to limit)
 * 
 * @note This function modifies the board during search but restores it
 *       to its original state before returning
 * @note Useful for uniqueness checking: call with limit=2, if result is 1
 *       the puzzle has a unique solution
 * @warning For puzzles with many solutions, this can be slow
 *          Use limit=2 for uniqueness verification (1=unique, 2+=multiple)
 */
int sudoku_count_solutions(SudokuBoard *board, int limit);

#endif // SUDOKU_GENERATOR_INTERNAL_H

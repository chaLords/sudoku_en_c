/**
 * @file board_internal.h
 * @brief Internal board operations not exposed in the public API
 * @author Gonzalo Ramírez
 * @date 2025-01-19
 * 
 * This header contains private functions for board manipulation that are
 * used internally by the generator and solver algorithms. These functions
 * are not part of the public API and should not be used by external code.
 */

#ifndef SUDOKU_BOARD_INTERNAL_H
#define SUDOKU_BOARD_INTERNAL_H

#include "sudoku/core/types.h"
#include <stdbool.h>

/**
 * @brief Finds the first empty cell in the board using row-major traversal
 * 
 * This function performs a linear search through the board from left to right,
 * top to bottom, looking for the first cell that contains a zero (empty cell).
 * This traversal strategy is optimal for backtracking algorithms because it
 * maintains a consistent and predictable order of cell exploration.
 * 
 * The function is used internally by the backtracking algorithm during board
 * completion. It represents a design decision: we could search for empty cells
 * in different orders (column-major, spiral, most-constrained-first), but
 * row-major order provides a good balance between simplicity and performance.
 *
 * @param board Pointer to the Sudoku board to search (not modified)
 * @param pos Pointer to a Position structure where the coordinates will be stored
 * @return true if an empty cell was found (pos contains its coordinates)
 * @return false if the board is complete (no empty cells exist)
 * 
 * @pre board != NULL && pos != NULL
 * @post If true is returned, pos->row and pos->col contain valid coordinates
 * @post The board structure is never modified by this function
 * 
 * @note This is an internal function - not part of the public API
 * @note Time complexity: O(n²) where n = SUDOKU_SIZE, but typically terminates early
 * @note Space complexity: O(1) - no additional memory allocation
 */
bool sudoku_find_empty_cell(const SudokuBoard *board, SudokuPosition *pos);
// Funciones de llenado de tablero (internas)
void fillSubGrid(SudokuBoard *board, const SudokuSubGrid *sg);
void fillDiagonal(SudokuBoard *board);

// Actualización de contadores (wrapper)
void sudoku_board_update_stats(SudokuBoard *board);

/**
 * @brief Complete internal definition of SudokuBoard
 * 
 * This structure contains the actual implementation of the board.
 * Client code cannot see this definition - they only work with
 * SudokuBoard* pointers through the public API.
 * 
 * This separation allows us to change the internal representation
 * in the future without breaking client code.
 * 
 * Current layout (v2.2.x compatible):
 * - cells: 9x9 array of integers (0 = empty, 1-9 = filled)
 * - clues: cached count of filled cells
 * - empty: cached count of empty cells
 * 
 * Future considerations (v3.0):
 * - Variable size boards: int size; int *cells;
 * - Validity caching: bool is_valid; bool validity_cached;
 * - Solution storage: int *solution;
 */
struct SudokuBoard {
    int cells[SUDOKU_SIZE][SUDOKU_SIZE];    /**< The 9x9 grid of cell values */
    int clues;                               /**< Number of filled cells (non-zero) */
    int empty;                               /**< Number of empty cells (zero) */
};
#endif // SUDOKU_BOARD_INTERNAL_H

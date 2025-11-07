/**
 * @file board.h
 * @brief Board manipulation and utility functions
 * @author Gonzalo Ramírez
 * @date 2025-10-17
 * 
 * This file provides the public API for basic Sudoku board operations,
 * including initialization, statistics updates, and subgrid utilities.
 */

#ifndef SUDOKU_CORE_BOARD_H
#define SUDOKU_CORE_BOARD_H

#include <sudoku/core/types.h>

/**
 * @brief Initialize an empty Sudoku board
 * 
 * Sets all cells to 0 (empty) and initializes statistics.
 * 
 * @param board Pointer to the board to initialize
 * 
 * @note After calling this function:
 *       - All cells will be 0
 *       - clues will be 0
 *       - empty will be 81
 */
void sudoku_board_init(SudokuBoard *board);

/**
 * @brief Update board statistics (clues and empty cells)
 * 
 * Counts the number of filled and empty cells in the board
 * and updates the board's statistics accordingly.
 * 
 * @param board Pointer to the board to update
 * 
 * @note Call this function after manually modifying board cells
 *       to keep statistics accurate.
 */
void sudoku_board_update_stats(SudokuBoard *board);

/**
 * @brief Create a subgrid descriptor from an index
 * 
 * Converts a subgrid index (0-8) into a SubGrid structure
 * with the calculated base position.
 * 
 * Subgrid indexing:
 * @code
 *     0 1 2
 *     3 4 5
 *     6 7 8
 * @endcode
 * 
 * @param index Subgrid index (0-8)
 * @return SudokuSubGrid structure with index and base position
 * 
 * @note No validation is performed on the index.
 *       Caller must ensure 0 <= index <= 8.
 */
SudokuSubGrid sudoku_subgrid_create(int index);

/**
 * @brief Get a cell position within a subgrid
 * 
 * Converts a cell index within a subgrid (0-8) to its
 * absolute position on the board.
 * 
 * Cell indexing within a 3x3 subgrid:
 * @code
 *     0 1 2
 *     3 4 5
 *     6 7 8
 * @endcode
 * 
 * @param sg Pointer to the subgrid descriptor
 * @param cell_index Cell index within the subgrid (0-8)
 * @return SudokuPosition with absolute row and column
 * 
 * @note No validation is performed on cell_index.
 *       Caller must ensure 0 <= cell_index <= 8.
 */
SudokuPosition sudoku_subgrid_get_position(const SudokuSubGrid *sg, int cell_index);

#endif // SUDOKU_CORE_BOARD_H

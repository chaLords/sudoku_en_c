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
#include <stdbool.h>
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

/**
 * @brief Create a new Sudoku board
 * 
 * Allocates memory for a new board and initializes it to empty state.
 * The board must be freed with sudoku_board_destroy() when no longer needed.
 * 
 * @return Pointer to newly created board, or NULL if allocation failed
 * 
 * @post If successful, returned board is initialized (all cells = 0)
 * 
 * @see sudoku_board_destroy() to free the allocated board
 * @see sudoku_board_init() for re-initialization of existing boards
 */
SudokuBoard* sudoku_board_create(void);

/**
 * @brief Destroy a Sudoku board and free its memory
 * 
 * Frees all resources associated with the board. After calling this,
 * the board pointer becomes invalid and must not be used.
 * 
 * @param board Pointer to board to destroy (can be NULL, in which case
 *              this function does nothing)
 * 
 * @note It is safe to pass NULL to this function
 * @note After calling this, the board pointer is no longer valid
 * 
 * @see sudoku_board_create() to allocate new boards
 */
void sudoku_board_destroy(SudokuBoard *board);
/**
 * @brief Get the value of a cell
 * 
 * Retrieves the current value at the specified position.
 * 
 * @param board Pointer to the board
 * @param row Row index (0-8)
 * @param col Column index (0-8)
 * @return Cell value (0-9), where 0 means empty
 * 
 * @pre board != NULL
 * @pre 0 <= row < SUDOKU_SIZE
 * @pre 0 <= col < SUDOKU_SIZE
 * 
 * @note Violating preconditions results in undefined behavior
 */
int sudoku_board_get_cell(const SudokuBoard *board, int row, int col);

/**
 * @brief Set the value of a cell
 * 
 * Updates the cell at the specified position with a new value.
 * Does not perform Sudoku rule validation - use sudoku_is_safe_position()
 * first if you need to verify the move is legal.
 * 
 * @param board Pointer to the board
 * @param row Row index (0-8)
 * @param col Column index (0-8)
 * @param value New cell value (0-9), where 0 means empty
 * @return true if successful, false if parameters are invalid
 * 
 * @pre board != NULL
 * @post Board statistics (clues/empty) are NOT automatically updated.
 *       Call sudoku_board_update_stats() if you need accurate counts.
 * 
 * @note This function does NOT validate Sudoku rules
 * @see sudoku_is_safe_position() to check if a placement is legal
 * @see sudoku_board_update_stats() to recalculate statistics
 */
bool sudoku_board_set_cell(SudokuBoard *board, int row, int col, int value);

/**
 * @brief Get the number of filled cells (clues)
 * 
 * @param board Pointer to the board
 * @return Number of non-zero cells
 * 
 * @pre board != NULL
 * 
 * @note This value is cached. After manual cell modifications, call
 *       sudoku_board_update_stats() to ensure accuracy.
 */
int sudoku_board_get_clues(const SudokuBoard *board);

/**
 * @brief Get the number of empty cells
 * 
 * @param board Pointer to the board
 * @return Number of zero-valued cells
 * 
 * @pre board != NULL
 * 
 * @note This value is cached. After manual cell modifications, call
 *       sudoku_board_update_stats() to ensure accuracy.
 */
int sudoku_board_get_empty(const SudokuBoard *board);

#endif // SUDOKU_CORE_BOARD_H

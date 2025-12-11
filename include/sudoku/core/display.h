// include/sudoku/core/display.h

/**
 * @file display.h
 * @brief Public API for Sudoku board visualization
 * @author Gonzalo Ramírez
 * @date 2025-11-04
 */

#ifndef SUDOKU_DISPLAY_H
#define SUDOKU_DISPLAY_H

#include "sudoku/core/types.h"
/**
 * @brief Print Sudoku board with automatic size adaptation
 * 
 * **NEW IN v3.0.2**: Universal display supporting ALL board sizes.
 * 
 * Automatically adapts to board dimensions:
 * - 4×4 to 9×9: 1-digit cells
 * - 16×16 to 99×99: 2-digit cells  
 * - 100×100: 3-digit cells
 * 
 * @param[in] board Pointer to board to display (any size)
 * 
 * @note Supports 4×4, 9×9, 16×16, 25×25, up to 100×100 boards
 * @note Requires UTF-8 terminal for box-drawing characters
 */
void sudoku_display_print_board(const SudokuBoard *board);

/**
 * @brief Print board in compact format (no borders)
 * 
 * Useful for logging and debugging. Prints one row per line
 * with minimal formatting.
 * 
 * @param[in] board Pointer to board to display
 */
void sudoku_display_print_compact(const SudokuBoard *board);
#endif // SUDOKU_DISPLAY_H

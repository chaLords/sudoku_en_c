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
 * @brief Prints the Sudoku board with formatted box-drawing characters
 * 
 * Displays the board in a visually appealing format using Unicode
 * box-drawing characters. Empty cells appear as dots, filled cells
 * show their numbers. The 3x3 subgrid structure is clearly visible.
 * 
 * @param board Pointer to the board to display
 */
void sudoku_display_print_board(const SudokuBoard *board);

#endif // SUDOKU_DISPLAY_H

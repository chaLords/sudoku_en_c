/**
 * @file validation.c
 * @brief Implementation of Sudoku validation rules
 * @author Gonzalo Ramírez
 * @date 2025-01-19
 * 
 * This module provides the core validation logic for Sudoku puzzles.
 * It implements the three fundamental rules: no duplicates in rows,
 * columns, or 3x3 subgrids.
 */

#include <sudoku/core/validation.h>

bool sudoku_is_safe_position(const SudokuBoard *board, const SudokuPosition *pos, int num) {
    // Check row - no cell in this row should have the same number
    for (int x = 0; x < SUDOKU_SIZE; x++) {
        if (board->cells[pos->row][x] == num) {
            return false;
        }
    }
    
    // Check column - no cell in this column should have the same number
    for (int x = 0; x < SUDOKU_SIZE; x++) {
        if (board->cells[x][pos->col] == num) {
            return false;
        }
    }
    
    // Check 3x3 subgrid
    // Calculate the top-left corner of the subgrid containing this position
    int start_row = pos->row - (pos->row % SUBGRID_SIZE);
    int start_col = pos->col - (pos->col % SUBGRID_SIZE);
    
    // Check all 9 cells in the subgrid
    for (int i = 0; i < SUBGRID_SIZE; i++) {
        for (int j = 0; j < SUBGRID_SIZE; j++) {
            if (board->cells[i + start_row][j + start_col] == num) {
                return false;
            }
        }
    }
    
    // If all checks passed, the position is safe
    return true;
}

bool sudoku_find_empty_cell(const SudokuBoard *board, SudokuPosition *pos) {
    // Traverse the board row by row, column by column
    for (pos->row = 0; pos->row < SUDOKU_SIZE; pos->row++) {
        for (pos->col = 0; pos->col < SUDOKU_SIZE; pos->col++) {
            // An empty cell is represented by 0
            if (board->cells[pos->row][pos->col] == 0) {
                return true;  // Found an empty cell
            }
        }
    }
    
    // No empty cells - the board is complete
    return false;
}

bool sudoku_validate_board(const SudokuBoard *board) {
    // Verify each cell in the board
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++) {
            // Skip empty cells (though a verified board should be complete)
            if (board->cells[i][j] == 0) {
                continue;
            }
            
            int num = board->cells[i][j];
            
            // Create a temporary copy of the board with this cell empty
            SudokuBoard temp = *board;
            temp.cells[i][j] = 0;
            
            // If the number cannot be legally placed in this position,
            // then the board has a conflict
            SudokuPosition pos = {i, j};
            if (!sudoku_is_safe_position(&temp, &pos, num)) {
                return false;
            }
        }
    }
    
    // All cells passed verification
    return true;
}

/**
* @file board.c 
* @brief Implementation of the Sudoku Board
* @author Gonzalo Ramírez
* @date 2025-10-20
*
*/

#include <sudoku/core/board.h>

// ═══════════════════════════════════════════════════════════════════
//                    INITIALIZATION FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * Initializes an empty board
 * Pointer usage: Modifies the board directly without copying
 */
void sudoku_board_init(SudokuBoard *board) {
    for(int i = 0; i < SUDOKU_SIZE; i++) {
        for(int j = 0; j < SUDOKU_SIZE; j++) {
            board->cells[i][j] = 0;
        }
    }
    board->clues = 0;
    board->empty = TOTAL_CELLS;
}

/**
 * Updates the clue and empty cell counters
 */
void sudoku_board_update_stats(SudokuBoard *board) {
    int count = 0;
    for(int i = 0; i < SUDOKU_SIZE; i++) {
        for(int j = 0; j < SUDOKU_SIZE; j++) {
            if(board->cells[i][j] != 0) count++;
        }
    }
    board->clues = count;
    board->empty = TOTAL_CELLS - count;
}

/**
 * Creates a SubGrid from an index (0-8)
 * Automatically calculates the base position
 */
SudokuSubGrid sudoku_subgrid_create(int index) {
    SudokuSubGrid sg;
    sg.index = index;
    sg.base.row = (index / 3) * 3;
    sg.base.col = (index % 3) * 3;
    return sg;
}

/**
 * Gets a Position within a SubGrid
 * @param sg: The subgrid
 * @param cell_index: index 0-8 within the subgrid
 */
SudokuPosition sudoku_subgrid_get_position(const SudokuSubGrid *sg, int cell_index) {
    SudokuPosition pos;
    pos.row = sg->base.row + (cell_index / 3);
    pos.col = sg->base.col + (cell_index % 3);
    return pos;
}



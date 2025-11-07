/**
 * @file board.c 
 * @brief Implementation of Sudoku board operations and utilities
 * @author Gonzalo Ramírez
 * @date 2025-11-06
 * 
 * This module provides the foundational layer for all Sudoku board operations.
 * It implements basic board management (initialization, statistics tracking)
 * and geometric utilities (subgrid addressing, position calculations).
 * 
 * The module is divided into two conceptual sections:
 * 
 * 1. PUBLIC API: Functions for basic board manipulation, exposed to all users
 *    of the library. These include initialization, statistics updates, and
 *    subgrid geometric calculations.
 * 
 * 2. INTERNAL HELPERS: Functions used exclusively during puzzle generation,
 *    not exposed in the public API. These handle diagonal filling using
 *    Fisher-Yates shuffling, which is the first step of the generation algorithm.
 * 
 * Design decision: The internal generation helpers live here rather than in
 * a separate module to keep closely related board manipulation code together
 * and avoid excessive file fragmentation. However, they are clearly marked
 * as internal and are not part of the public API surface.
 */

#include <stdio.h>                          // For printf() in verbose generation output
#include "sudoku/core/board.h"              // Our own public header
#include "sudoku/core/types.h"              // For SUDOKU_SIZE, TOTAL_CELLS, structure definitions
#include "internal/board_internal.h"        // For internal function declarations
#include "internal/algorithms_internal.h"   // For sudoku_generate_permutation()
#include "internal/config_internal.h"                // For VERBOSITY_LEVEL global

// ═══════════════════════════════════════════════════════════════════
//                    PUBLIC API: BOARD INITIALIZATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Initialize a Sudoku board to completely empty state
 * 
 * Sets all cells in the 9x9 grid to zero (representing empty cells)
 * and resets metadata counters. This is the required first step before
 * using a board structure for generation or solving.
 * 
 * The function ensures the board is in a known, consistent state. After
 * initialization, the board has zero clues and 81 empty cells.
 * 
 * @param[out] board Pointer to the board structure to initialize
 * 
 * @pre board != NULL
 * @post All cells contain 0 (empty)
 * @post board->clues == 0
 * @post board->empty == TOTAL_CELLS (81)
 * 
 * @note This function has no return value. Initialization cannot fail
 *       as it's a simple memory write operation.
 * @note The board should be properly allocated before calling this function
 * 
 * Example usage:
 * @code
 * SudokuBoard board;
 * sudoku_board_init(&board);
 * // Board is now ready for use in generation or solving
 * @endcode
 */
void sudoku_board_init(SudokuBoard *board) {
    // Zero out all cells in the 9x9 grid
    // Nested loops iterate row-major: row 0 cols 0-8, row 1 cols 0-8, etc.
    for(int i = 0; i < SUDOKU_SIZE; i++) {
        for(int j = 0; j < SUDOKU_SIZE; j++) {
            board->cells[i][j] = 0;  // 0 represents empty cell
        }
    }
    
    // Initialize metadata to reflect empty board state
    board->clues = 0;           // No filled cells yet
    board->empty = TOTAL_CELLS; // All 81 cells are empty
}

/**
 * @brief Recalculate and update board statistics
 * 
 * Scans the entire board to count filled and empty cells, updating
 * the board's metadata fields. This should be called after any series
 * of cell modifications to ensure statistics remain accurate.
 * 
 * The function counts cells containing non-zero values (clues) and
 * derives the empty cell count from the total. This is more efficient
 * than tracking modifications incrementally in many scenarios.
 * 
 * @param[in,out] board Pointer to the board to update
 * 
 * @pre board != NULL
 * @post board->clues == number of non-zero cells
 * @post board->empty == TOTAL_CELLS - board->clues
 * 
 * @note Time complexity: O(n²) where n=9, always scans all 81 cells
 * @note This is safe to call multiple times or unnecessarily - it's idempotent
 * 
 * Common usage pattern:
 * @code
 * // After modifying several cells during generation/elimination
 * board.cells[3][4] = 7;
 * board.cells[5][2] = 0;  // Removing a cell
 * // ... more modifications ...
 * sudoku_board_update_stats(&board);  // Refresh statistics
 * @endcode
 */
void sudoku_board_update_stats(SudokuBoard *board) {
    int count = 0;
    
    // Scan entire board counting filled cells
    for(int i = 0; i < SUDOKU_SIZE; i++) {
        for(int j = 0; j < SUDOKU_SIZE; j++) {
            if(board->cells[i][j] != 0) {
                count++;  // Found a clue (non-empty cell)
            }
        }
    }
    
    // Update metadata based on count
    board->clues = count;
    board->empty = TOTAL_CELLS - count;
}

// ═══════════════════════════════════════════════════════════════════
//                    PUBLIC API: SUBGRID GEOMETRY
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Create a subgrid structure from an index
 * 
 * Constructs a SudokuSubGrid structure representing one of the nine
 * 3x3 subgrids that compose the 9x9 Sudoku board. Subgrids are indexed
 * 0-8 in row-major order (left to right, top to bottom).
 * 
 * Subgrid layout:
 * @code
 *  0 | 1 | 2
 * ---+---+---
 *  3 | 4 | 5
 * ---+---+---
 *  6 | 7 | 8
 * @endcode
 * 
 * The function calculates the top-left corner (base position) of the
 * subgrid using integer arithmetic: index 4 (center) has base (3,3),
 * index 8 (bottom-right) has base (6,6), etc.
 * 
 * @param[in] index Subgrid index (0-8, where 0 is top-left, 8 is bottom-right)
 * @return Initialized SudokuSubGrid structure with index and base position
 * 
 * @pre 0 <= index <= 8
 * @post return.index == index
 * @post return.base contains valid top-left corner coordinates
 * 
 * @note The calculation uses integer division to map index to grid position:
 *       - row = (index / 3) * 3 gives rows {0,0,0, 3,3,3, 6,6,6}
 *       - col = (index % 3) * 3 gives cols {0,3,6, 0,3,6, 0,3,6}
 * 
 * Example: Creating center subgrid (index 4)
 * @code
 * SudokuSubGrid center = sudoku_subgrid_create(4);
 * // center.index == 4
 * // center.base.row == 3, center.base.col == 3
 * @endcode
 */
SudokuSubGrid sudoku_subgrid_create(int index) {
    SudokuSubGrid sg;
    sg.index = index;
    
    // Calculate top-left corner of this subgrid
    // Example: index 4 -> (4/3)*3 = 1*3 = 3 (row), (4%3)*3 = 1*3 = 3 (col)
    // Example: index 7 -> (7/3)*3 = 2*3 = 6 (row), (7%3)*3 = 1*3 = 3 (col)
    sg.base.row = (index / 3) * 3;
    sg.base.col = (index % 3) * 3;
    
    return sg;
}

/**
 * @brief Get absolute board position from subgrid-relative cell index
 * 
 * Converts a cell index within a 3x3 subgrid (0-8) to absolute board
 * coordinates (row, column). This allows treating each subgrid as its
 * own mini-grid with positions 0-8, which simplifies algorithms.
 * 
 * Cell indices within a subgrid:
 * @code
 *  0 1 2
 *  3 4 5
 *  6 7 8
 * @endcode
 * 
 * The function maps these indices to absolute positions by adding the
 * subgrid's base offset to the relative position within the subgrid.
 * 
 * @param[in] sg Pointer to the subgrid structure defining base position
 * @param[in] cell_index Cell index within subgrid (0-8, row-major)
 * @return Absolute position on the full 9x9 board
 * 
 * @pre sg != NULL
 * @pre 0 <= cell_index <= 8
 * @post return contains valid board coordinates (0-8, 0-8)
 * 
 * @note Calculation breaks cell_index into row/col offsets within subgrid:
 *       - row offset = cell_index / 3 (which row of subgrid: 0,1,2)
 *       - col offset = cell_index % 3 (which col of subgrid: 0,1,2)
 *       Then adds subgrid's base to get absolute position
 * 
 * Example: Center subgrid (base 3,3), cell 4 (center of subgrid)
 * @code
 * SudokuSubGrid center = sudoku_subgrid_create(4);
 * SudokuPosition pos = sudoku_subgrid_get_position(&center, 4);
 * // pos.row == 3 + (4/3) = 3 + 1 = 4
 * // pos.col == 3 + (4%3) = 3 + 1 = 4
 * // Result: position (4,4) which is the center of the entire board
 * @endcode
 */
SudokuPosition sudoku_subgrid_get_position(const SudokuSubGrid *sg, int cell_index) {
    SudokuPosition pos;
    
    // Calculate position relative to subgrid's top-left corner
    // cell_index 5 in a subgrid means row 1 (5/3=1), col 2 (5%3=2)
    pos.row = sg->base.row + (cell_index / 3);
    pos.col = sg->base.col + (cell_index % 3);
    
    return pos;
}

// ═══════════════════════════════════════════════════════════════════
//                    INTERNAL: GENERATION HELPERS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Indices of the three diagonal subgrids (top-left, center, bottom-right)
 * 
 * These three subgrids are independent of each other - placing numbers in
 * subgrid 0 cannot conflict with subgrids 4 or 8 since they share no rows,
 * columns, or regions. This property allows us to fill them with random
 * permutations in the first phase of generation without validation.
 */
static const int DIAGONAL_INDICES[3] = {0, 4, 8};

/**
 * @brief Fill a specific 3x3 subgrid with random numbers using Fisher-Yates
 * 
 * Internal helper function used during diagonal initialization phase of
 * puzzle generation. Fills all 9 cells of the specified subgrid with a
 * random permutation of numbers 1-9.
 * 
 * This function is safe to call on diagonal subgrids (0, 4, 8) without
 * validation since they don't constrain each other. For non-diagonal subgrids,
 * this would likely create conflicts and should not be used.
 * 
 * @param[in,out] board Pointer to the board being generated
 * @param[in] sg Pointer to the subgrid structure to fill
 * 
 * @pre board != NULL && sg != NULL
 * @pre sg represents one of the three diagonal subgrids for conflict-free filling
 * @post All 9 cells in the subgrid contain unique numbers 1-9
 * 
 * @note This function produces verbose output when VERBOSITY_LEVEL == 2
 * @note Uses sudoku_generate_permutation() which implements Fisher-Yates shuffle
 * 
 * @internal This is an internal helper not exposed in public API
 */
void fillSubGrid(SudokuBoard *board, const SudokuSubGrid *sg) {
    // Generate random permutation of 1-9
    int numbers[SUDOKU_SIZE];
    sudoku_generate_permutation(numbers, SUDOKU_SIZE, 1);
    
    // Verbose mode: show which subgrid we're filling and its location
    if(VERBOSITY_LEVEL == 2) {
        printf("   SubGrid %d (base: %d,%d): ", 
               sg->index, sg->base.row, sg->base.col);
    }
    
    // Place each number in the corresponding cell of the subgrid
    for(int i = 0; i < SUDOKU_SIZE; i++) {
        SudokuPosition pos = sudoku_subgrid_get_position(sg, i);
        board->cells[pos.row][pos.col] = numbers[i];
        
        // Verbose mode: show the numbers being placed
        if(VERBOSITY_LEVEL == 2) {
            printf("%d ", numbers[i]);
        }
    }
    
    if(VERBOSITY_LEVEL == 2) {
        printf("\n");
    }
}

/**
 * @brief Fill the three diagonal subgrids with random permutations
 * 
 * First phase of the puzzle generation algorithm. Fills subgrids 0, 4, and 8
 * (top-left, center, bottom-right) with random permutations of 1-9.
 * 
 * These three subgrids are independent because they share no rows, columns,
 * or regions with each other. This allows us to fill them with truly random
 * permutations without any validation, which is both efficient and ensures
 * uniform distribution across all possible starting configurations.
 * 
 * After this function completes, the board has 27 cells filled (3 subgrids
 * × 9 cells each), providing a strong foundation for the backtracking
 * algorithm to complete the remaining 54 cells.
 * 
 * @param[in,out] board Pointer to the board to initialize
 * 
 * @pre board != NULL
 * @pre board is initialized (typically via sudoku_board_init)
 * @post Subgrids 0, 4, 8 contain random valid permutations of 1-9
 * @post 27 cells are filled, 54 remain empty
 * 
 * @note Output verbosity controlled by global VERBOSITY_LEVEL:
 *       - Level 1: Single line progress indicator
 *       - Level 2: Detailed per-subgrid output
 *       - Other: No output
 * 
 * @internal This is an internal helper not exposed in public API
 */
void fillDiagonal(SudokuBoard *board) {
    // Compact mode: show combined phase indicator
    if(VERBOSITY_LEVEL == 2) {
        printf("🎲 Filling diagonal with Fisher-Yates...\n");
    } else if(VERBOSITY_LEVEL == 1) {
        printf("🎲 Diagonal + Backtracking...");
        fflush(stdout);  // Ensure output appears before potentially long backtracking
    }
    
    // Fill each of the three independent diagonal subgrids
    for(int i = 0; i < 3; i++) {
        SudokuSubGrid sg = sudoku_subgrid_create(DIAGONAL_INDICES[i]);
        fillSubGrid(board, &sg);
    }
    
    // Detailed mode: show completion confirmation
    if(VERBOSITY_LEVEL == 2) {
        printf("✅ Diagonal successfully filled!\n\n");
    }
}

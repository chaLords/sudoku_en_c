/**
 * @file elimination.c
 * @brief Three-phase cell elimination strategy for puzzle generation
 * @author Gonzalo Ramírez
 * @date 2025-11-06
 * 
 * This module implements the sophisticated three-phase elimination algorithm
 * that transforms a complete valid Sudoku board into a playable puzzle with
 * unique solution. The strategy balances computational efficiency with puzzle
 * quality through increasingly selective elimination phases.
 * 
 * Algorithm Overview:
 * 
 * PHASE 1: Balanced Initial Distribution
 * Removes exactly one randomly-selected number from each of the 9 subgrids,
 * ensuring even distribution of empty cells across the board. This phase is
 * fast (O(n)) and prevents creation of visually unbalanced puzzles.
 * 
 * PHASE 2: Heuristic Safe Elimination
 * Iteratively removes numbers that have no alternative valid positions in
 * their constraint regions (row, column, or subgrid). These removals are
 * "safe" because they cannot introduce solution ambiguity - the removed
 * number could only go in that specific position. This phase is moderately
 * fast (O(n²)) and typically removes 10-20 additional cells.
 * 
 * PHASE 3: Exhaustive Verified Elimination
 * Attempts free elimination of remaining cells in random order, using
 * exhaustive backtracking to verify that each removal maintains exactly
 * one solution. This is computationally expensive (potentially O(9^m) per
 * verification where m = empty cells) but guarantees puzzle uniqueness,
 * which is a fundamental requirement for quality Sudoku puzzles.
 * 
 * The three-phase approach is significantly more efficient than naive
 * exhaustive verification from the start, as Phases 1-2 quickly remove
 * ~15-20 cells before entering the expensive Phase 3 verification loop.
 * 
 * Design Decisions:
 * - Phase 1 uses Fisher-Yates shuffled numbers to ensure uniform randomness
 * - Phase 2 checks alternatives in all three constraint dimensions
 * - Phase 3 uses dynamic memory allocation to avoid stack overflow with
 *   large position arrays
 * - All phases support configurable verbosity for debugging and education
 */

#include "elimination_internal.h"  // For our own function declarations
#include "board_internal.h"        // For internal function declarations
#include "sudoku/core/validation.h"         // For sudoku_is_safe_position(), countSolutionsExact()
#include "events_internal.h"
#include <stdlib.h>                         // For malloc(), free(), rand()
#include <stdio.h>                          // For printf(), fprintf()
#include <stdbool.h>                        // For bool type

// ═══════════════════════════════════════════════════════════════════
//                    PHASE 3: EXHAUSTIVE VERIFIED ELIMINATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Phase 3: Free elimination with exhaustive uniqueness verification
 * 
 * Final and most computationally intensive elimination phase. Attempts to
 * remove additional cells in random order, using exhaustive backtracking
 * solution counting to verify that each removal maintains exactly one
 * unique solution.
 * 
 * This phase is critical for achieving lower clue counts and approaching
 * minimal puzzles. Unlike Phases 1-2 which use heuristics, this phase
 * provides mathematical guarantees of solution uniqueness through
 * exhaustive verification.
 * 
 * Algorithm:
 * 1. Collect all filled cell positions into dynamic array
 * 2. Shuffle array using Fisher-Yates for random removal order
 * 3. For each position (up to target count):
 *    a. Temporarily remove the cell
 *    b. Count solutions using exhaustive backtracking (limit=2)
 *    c. If exactly 1 solution: keep removal
 *    d. If != 1 solution: restore cell (would create ambiguity or unsolvability)
 * 4. Result: puzzle with maximum possible removals while maintaining uniqueness
 * 
 * Design decisions:
 * - Uses dynamic allocation to avoid stack overflow with large position array
 * - Shuffles positions to ensure different elimination orders across runs
 * - Limits solution counting to 2 (only need to distinguish 1 vs multiple)
 * - Stops after 'target' successful removals even if more are possible
 * 
 * @param[in,out] board Board with Phases 1-2 eliminations applied
 * @param[in] target Maximum number of additional cells to attempt removing
 * @return Number of cells successfully removed (0 to target)
 * 
 * @pre board != NULL
 * @pre board contains valid partially-filled Sudoku with unique solution
 * @pre target >= 0
 * @post Additional cells removed while maintaining exactly one solution
 * @post If return value < target, no more safe removals were possible
 * 
 * @note Time complexity: Potentially O(target × 9^m) where m = empty cells
 *       at time of each verification. This can be very slow on boards with
 *       many empty cells.
 * @note Memory: Allocates O(filled_cells) space for position array
 * @note Returns 0 if memory allocation fails (prints error to stderr)
 * 
 * @warning Computationally expensive! Each countSolutionsExact() call
 *          may explore thousands or millions of board states. On boards
 *          with 40+ empty cells, individual verifications can take seconds.
 */
int phase3Elimination(SudokuBoard *board, int target) {
    // ✅ EVENTO: Inicio de Phase 3
    emit_event(SUDOKU_EVENT_PHASE3_START, board, 3, 0);
    
    // Allocate dynamic memory for position array
    SudokuPosition *positions = (SudokuPosition *)malloc(
        TOTAL_CELLS * sizeof(SudokuPosition)
    );
    
    if (positions == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in phase3Elimination\n");
        return 0;
    }
    
    int filled_count = 0;
    
    // Collect all currently filled cell positions
    for (int row = 0; row < SUDOKU_SIZE; row++) {
        for (int col = 0; col < SUDOKU_SIZE; col++) {
            if (board->cells[row][col] != 0) {
                positions[filled_count].row = row;
                positions[filled_count].col = col;
                filled_count++;
            }
        }
    }
    
    // Shuffle positions using Fisher-Yates
    for (int i = filled_count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        SudokuPosition temp = positions[i];
        positions[i] = positions[j];
        positions[j] = temp;
    }
    
    int removed = 0;
    
    // Attempt to remove cells in shuffled order until target reached
    for (int i = 0; i < filled_count && removed < target; i++) {
        SudokuPosition pos = positions[i];
        int original_value = board->cells[pos.row][pos.col];
        
        // Temporarily remove the cell
        board->cells[pos.row][pos.col] = 0;
        
        // Count solutions with limit=2
        if (countSolutionsExact(board, 2) == 1) {
            // Exactly one solution: keep the removal
            removed++;
            
            // ✅ EVENTO: Celda removida exitosamente en Phase 3
            emit_event_cell(SUDOKU_EVENT_PHASE3_CELL_REMOVED,
                            board,
                            3,               // phase_number
                            removed,         // cells_removed_total
                            pos.row,         // row
                            pos.col,         // col
                            original_value); // value
        } else {
            // Multiple solutions or no solution: restore the cell
            board->cells[pos.row][pos.col] = original_value;
        }
    }
    
    // Free dynamically allocated memory
    free(positions);
    
    // ✅ EVENTO: Phase 3 completada
    emit_event(SUDOKU_EVENT_PHASE3_COMPLETE, board, 3, removed);
    
    return removed;
}

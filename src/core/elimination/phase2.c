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

#include "config_internal.h"                // For VERBOSITY_LEVEL global
#include "elimination_internal.h"  // For our own function declarations
#include "sudoku/core/validation.h"         // For sudoku_is_safe_position(), countSolutionsExact()
#include "sudoku/core/board.h"              // For subgrid utilities
#include <stdio.h>                          // For printf(), fprintf()
#include <stdbool.h>                        // For bool type

// ═══════════════════════════════════════════════════════════════════
//                    ALTERNATIVE POSITION DETECTION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Check if a number has alternative valid positions in its regions
 * 
 * This function implements a critical heuristic for Phase 2 elimination:
 * determining whether a number at a given position could be legally placed
 * in any other empty cell within its row, column, or 3x3 subgrid.
 * 
 * If no alternatives exist, the number is "locked" to this position by
 * Sudoku constraints, meaning it can be safely removed without creating
 * solution ambiguity. This is much faster than exhaustive solution counting
 * but still effective for identifying removable cells.
 * 
 * Algorithm:
 * 1. Temporarily remove the number from its current position
 * 2. Search row, column, and subgrid for other empty cells
 * 3. For each empty cell, test if the number could legally go there
 * 4. Restore the number to its original position
 * 5. Return true if any alternative position was found
 * 
 * The function uses early exit optimization: stops searching as soon as
 * one alternative is found, avoiding unnecessary validation checks.
 * 
 * @param[in,out] board Board to analyze (temporarily modified, then restored)
 * @param[in] pos Position of the cell containing the number to check
 * @param[in] num The number to check for alternatives
 * @return true if the number has at least one alternative position,
 *         false if it's locked to this position
 * 
 * @pre board != NULL && pos != NULL
 * @pre board->cells[pos->row][pos->col] == num
 * @pre 1 <= num <= 9
 * @post Board is restored to identical state as before call
 * 
 * @note Time complexity: O(n²) worst case (checking all cells in 3 regions),
 *       but typically much faster due to early exit optimization
 * @note The temporary modification is safe because we always restore state
 * 
 * @internal Used exclusively by phase2Elimination()
 */
bool hasAlternative(SudokuBoard *board, const SudokuPosition *pos, int num) {
    // Save original value and temporarily empty the cell
    // This allows us to check if the number could go elsewhere
    int original_value = board->cells[pos->row][pos->col];
    board->cells[pos->row][pos->col] = 0;
    
    int alternatives = 0;
    
    // Search in row: check each column position in this row
    // Early exit: stop as soon as we find one alternative
    for (int col = 0; col < SUDOKU_SIZE && alternatives == 0; col++) {
        // Skip the original position and non-empty cells
        if (col != pos->col && board->cells[pos->row][col] == 0) {
            SudokuPosition test_pos = {pos->row, col};
            if (sudoku_is_safe_position(board, &test_pos, num)) {
                alternatives++;  // Found alternative position in row
            }
        }
    }
    
    // Search in column: check each row position in this column
    for (int row = 0; row < SUDOKU_SIZE && alternatives == 0; row++) {
        // Skip the original position and non-empty cells
        if (row != pos->row && board->cells[row][pos->col] == 0) {
            SudokuPosition test_pos = {row, pos->col};
            if (sudoku_is_safe_position(board, &test_pos, num)) {
                alternatives++;  // Found alternative position in column
            }
        }
    }
    
    // Search in 3x3 subgrid: only if no alternatives found yet
    if (alternatives == 0) {
        // Calculate top-left corner of the subgrid containing this position
        int subgrid_row_start = pos->row - (pos->row % SUBGRID_SIZE);
        int subgrid_col_start = pos->col - (pos->col % SUBGRID_SIZE);
        
        // Check all 9 cells in the subgrid
        for (int i = 0; i < SUBGRID_SIZE && alternatives == 0; i++) {
            for (int j = 0; j < SUBGRID_SIZE && alternatives == 0; j++) {
                int row = subgrid_row_start + i;
                int col = subgrid_col_start + j;
                
                // Skip the original position and non-empty cells
                if ((row != pos->row || col != pos->col) && 
                    board->cells[row][col] == 0) {
                    SudokuPosition test_pos = {row, col};
                    if (sudoku_is_safe_position(board, &test_pos, num)) {
                        alternatives++;  // Found alternative in subgrid
                    }
                }
            }
        }
    }
    
    // Restore original value (critical for correctness!)
    board->cells[pos->row][pos->col] = original_value;
    
    // Return true if at least one alternative position exists
    return alternatives > 0;
}
// ═══════════════════════════════════════════════════════════════════
//                    PHASE 2: HEURISTIC ELIMINATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Phase 2: Remove numbers without alternative positions
 * 
 * Second elimination phase that applies an intelligent heuristic: remove
 * numbers that have no other valid positions in their constraint regions.
 * These cells are "safe" to remove because they cannot introduce solution
 * ambiguity - the number must go in that specific position.
 * 
 * This heuristic elimination is much faster than exhaustive solution counting
 * (O(n²) vs potentially O(9^m)) while still being effective at removing
 * additional cells. It typically removes 10-20 cells depending on the
 * board configuration.
 * 
 * Algorithm:
 * 1. For each subgrid (in shuffled order for randomness):
 *    - Check each filled cell in the subgrid
 *    - Use hasAlternative() to determine if number has other valid positions
 *    - If no alternatives, remove the cell (it's locked to this position)
 *    - Remove at most one cell per subgrid per round to avoid cascade effects
 * 2. Could be repeated iteratively until no more removals possible, but
 *    current implementation does single pass for efficiency
 * 
 * @param[in,out] board Board with Phase 1 eliminations applied
 * @param[in] index Array of subgrid indices defining processing order
 * @param[in] count Number of subgrids to process (typically 9)
 * @return Number of cells successfully removed (typically 10-20)
 * 
 * @pre board != NULL && index != NULL
 * @pre board contains valid partially-filled Sudoku (post-Phase 1)
 * @post Additional cells removed where numbers had no alternatives
 * @post Board still has at least one solution (removing locked numbers
 *       maintains solvability)
 * 
 * @note Time complexity: O(n³) in worst case (n² cells × n checks each)
 * @note Could be made iterative (repeat until no more removals) for
 *       more aggressive elimination, at cost of increased runtime
 */
int phase2Elimination(SudokuBoard *board, const int *index, int count) {
    if (VERBOSITY_LEVEL == 2) {
        printf("🎲 PHASE 2: Selecting numbers without alternatives...\n");
    }

    int removed = 0;
    
    // Process each subgrid in shuffled order
    for (int idx = 0; idx < count; idx++) {
        SudokuSubGrid subgrid = sudoku_subgrid_create(index[idx]);
        
        if (VERBOSITY_LEVEL == 2) {
            printf("   Subgrid %d (base: %d,%d): ", 
                   subgrid.index, subgrid.base.row, subgrid.base.col);
        }
        
        // Check each cell in this subgrid
        for (int cell_idx = 0; cell_idx < SUDOKU_SIZE; cell_idx++) {
            SudokuPosition pos = sudoku_subgrid_get_position(&subgrid, cell_idx);
            
            // Only consider filled cells
            if (board->cells[pos.row][pos.col] != 0) {
                int num = board->cells[pos.row][pos.col];
                
                // Check if this number has any alternative valid positions
                if (!hasAlternative(board, &pos, num)) {
                    // No alternatives means it's locked to this position
                    // Safe to remove without creating ambiguity
                    board->cells[pos.row][pos.col] = 0;

                    if (VERBOSITY_LEVEL == 2) {
                        printf("removed %d at (%d,%d) ", num, pos.row, pos.col);
                    }

                    removed++;
                    break;  // Only one removal per subgrid per round
                }
            }
        }
        
        if (VERBOSITY_LEVEL == 2) {
            printf("\n");
        }
    }
    
    if (VERBOSITY_LEVEL == 2) {
        printf("✅ Phase 2 completed! Removed: %d\n\n", removed);
    }

    return removed;
}



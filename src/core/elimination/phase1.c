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

#include <stdbool.h>
#include "algorithms_internal.h"
#include "elimination_internal.h"
#include "board_internal.h"        // ← ESTA LÍNEA ES CRÍTICA
#include "events_internal.h"
#include "sudoku/core/board.h"

// ═══════════════════════════════════════════════════════════════════
//                    PHASE 1: BALANCED DISTRIBUTION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Phase 1: Remove one random number from each subgrid
 * 
 * First elimination phase that ensures balanced distribution of empty cells
 * across all nine 3x3 subgrids. Uses Fisher-Yates shuffled numbers to select
 * which value to remove from each subgrid, ensuring uniform randomness.
 * 
 * This phase is critical for creating visually balanced puzzles. Without it,
 * random elimination could create puzzles where some subgrids are nearly full
 * while others are nearly empty, resulting in uneven difficulty and poor
 * aesthetic appearance.
 * 
 * Algorithm:
 * 1. Generate random permutation of numbers 1-9 using Fisher-Yates
 * 2. For each subgrid (in order determined by index array):
 *    - Select the corresponding number from the permutation
 *    - Find and remove first occurrence of that number in the subgrid
 * 3. Result: exactly 9 cells removed, one per subgrid, evenly distributed
 * 
 * @param[in,out] board Board with complete solution to start elimination
 * @param[in] index Array of subgrid indices defining processing order
 *                  (typically shuffled for randomness)
 * @param[in] count Number of subgrids to process (typically 9 for full board)
 * @return Number of cells successfully removed (typically 9)
 * 
 * @pre board != NULL && index != NULL
 * @pre board contains complete valid Sudoku solution
 * @pre count <= 9 (maximum number of subgrids)
 * @post Exactly one cell per subgrid is empty (or fewer if count < 9)
 * @post Board still has at least one valid solution (removing one per subgrid
 *       cannot eliminate all solutions)
 * 
 * @note Time complexity: O(n²) where n=9, dominated by subgrid scanning
 * @note Uses event system instead of printf - presentation is handled by callbacks
 */
int phase1Elimination(SudokuBoard *board, const int *index, int count) {
    // Emit phase start event (replaces all printf for start)
    emit_event(SUDOKU_EVENT_PHASE1_START, board, 1, 0);
    
    // Generate random permutation of 1-9 to determine which number to remove
    // from each subgrid. Using Fisher-Yates ensures uniform distribution.
    int numbers[SUDOKU_SIZE];
    sudoku_generate_permutation(numbers, SUDOKU_SIZE, 1);
    
    int removed = 0;
    
    // Process each subgrid in the order specified by index array
    for (int idx = 0; idx < count; idx++) {
        SudokuSubGrid subgrid = sudoku_subgrid_create(index[idx]);

        // Select which number to remove from this subgrid
        int target_value = numbers[idx];
        
        // Find and remove the first occurrence of target_value in this subgrid
        for (int cell_idx = 0; cell_idx < SUDOKU_SIZE; cell_idx++) {
            SudokuPosition pos = sudoku_subgrid_get_position(&subgrid, cell_idx);
            
            if (board->cells[pos.row][pos.col] == target_value) {
                // IMPORTANT: Save the value BEFORE removing it
                int removed_value = board->cells[pos.row][pos.col];
                
                // Remove the cell
                board->cells[pos.row][pos.col] = 0;
                removed++;
                
                // Emit cell selected event (replaces printf)
                emit_event_cell(SUDOKU_EVENT_PHASE1_CELL_SELECTED, board, 1,
                               removed, pos.row, pos.col, removed_value);
                
                break;  // Only remove one occurrence per subgrid
            }
        }
    }
    
    // Emit phase complete event (replaces all completion printf)
    emit_event(SUDOKU_EVENT_PHASE1_COMPLETE, board, 1, removed);
    
    return removed;
}


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

#include "algorithms_internal.h"   // For sudoku_generate_permutation()
#include "config_internal.h"                // For VERBOSITY_LEVEL global
#include "elimination_internal.h"  // For our own function declarations
#include "sudoku/core/board.h"              // For subgrid utilities
#include <stdio.h>                          // For printf(), fprintf()
#include <stdbool.h>                        // For bool type

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
 * @note Output controlled by VERBOSITY_LEVEL:
 *       Level 0: No output
 *       Level 1: Start and completion messages
 *       Level 2: Detailed per-subgrid reporting
 */
int phase1Elimination(SudokuBoard *board, const int *index, int count) {
    // Verbose mode: show phase start with detailed banner
    if (VERBOSITY_LEVEL == 2) {
        printf("🎲 PHASE 1: Selecting numbers per subgrid with Fisher-Yates...\n");
    } else if (VERBOSITY_LEVEL == 1) {
        printf("🎲 Phase 1: Fisher-Yates selection...");
        fflush(stdout);  // Ensure output appears before potentially long operation
    }
    
    // Generate random permutation of 1-9 to determine which number to remove
    // from each subgrid. Using Fisher-Yates ensures uniform distribution.
    int numbers[SUDOKU_SIZE];
    sudoku_generate_permutation(numbers, SUDOKU_SIZE, 1);
    
    int removed = 0;
    
    // Process each subgrid in the order specified by index array
    for (int idx = 0; idx < count; idx++) {
        SudokuSubGrid subgrid = sudoku_subgrid_create(index[idx]);
        
        if (VERBOSITY_LEVEL == 2) {
            printf("   Subgrid %d (base: %d,%d): ", 
                   subgrid.index, subgrid.base.row, subgrid.base.col);
        }

        // Select which number to remove from this subgrid
        int target_value = numbers[idx];
        
        // Find and remove the first occurrence of target_value in this subgrid
        for (int cell_idx = 0; cell_idx < SUDOKU_SIZE; cell_idx++) {
            SudokuPosition pos = sudoku_subgrid_get_position(&subgrid, cell_idx);
            
            if (board->cells[pos.row][pos.col] == target_value) {
                board->cells[pos.row][pos.col] = 0;  // Remove by setting to 0
                removed++;
                
                if (VERBOSITY_LEVEL == 2) {
                    printf("removed %d at (%d,%d)", 
                           target_value, pos.row, pos.col);
                }
                break;  // Only remove one occurrence per subgrid
            }
        }
        
        if (VERBOSITY_LEVEL == 2) {
            printf("\n");
        }
    }
    
    // Completion messages
    if (VERBOSITY_LEVEL >= 1) {
        printf("✅ Phase 1 completed!\n");
    }
    
    if (VERBOSITY_LEVEL >= 1) {
        printf("📊 PHASE 1 TOTAL: Removed %d cells\n\n", removed);
    }
    
    return removed;
}



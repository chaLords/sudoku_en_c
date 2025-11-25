/**
 * @file elimination_internal.h
 * @brief Internal interface for the three-phase elimination system
 * @author Gonzalo Ramírez
 * @date 2025-11-21
 * 
 * This header defines the internal API for the elimination phases that
 * transform a complete valid Sudoku into a playable puzzle with unique
 * solution. These functions are not part of the public API.
 * 
 * ARCHITECTURAL NOTES:
 * 
 * The elimination system consists of three phases:
 * 
 * Phase 1: Balanced distribution - removes one random cell per subgrid
 * Phase 2: Heuristic elimination - removes cells without alternatives
 * Phase 3: Verified elimination - removes cells while maintaining uniqueness
 * 
 * CONFIGURABLE BOARD SIZE SUPPORT (v3.0+):
 * 
 * All three phases now support dynamic board sizes. The key changes are:
 * - No more hardcoded SUDOKU_SIZE (9) or SUBGRID_SIZE (3)
 * - Dynamic memory allocation in Phase 1 and Phase 3
 * - calculate_phase3_target() replaces the obsolete PHASE3_TARGET constant
 * - New phase3EliminationAuto() wrapper for simplified usage
 * 
 * USAGE RECOMMENDATION:
 * 
 * For most use cases, call the phases in order:
 *     phase1Elimination(board, indices, count);
 *     while (phase2Elimination(board, indices, count) > 0);
 *     phase3EliminationAuto(board);  // Use Auto version!
 * 
 * Only use phase3Elimination(board, target) directly if you need
 * explicit control over the removal target.
 */

#ifndef SUDOKU_ELIMINATION_INTERNAL_H
#define SUDOKU_ELIMINATION_INTERNAL_H

#include <stdbool.h>
#include "sudoku/core/types.h"

// ═══════════════════════════════════════════════════════════════
//                    PHASE 1: BALANCED DISTRIBUTION
// ═══════════════════════════════════════════════════════════════

/**
 * @brief Phase 1: Remove one random number from each subgrid
 * 
 * Removes exactly one randomly-selected cell from each subgrid, ensuring
 * balanced distribution of empty cells across the puzzle. Uses Fisher-Yates
 * shuffle to select which number to remove from each subgrid.
 * 
 * CONFIGURABLE SIZE SUPPORT:
 * - Allocates permutation array dynamically based on board_size
 * - Iterates using board_size instead of hardcoded 9
 * 
 * @param board Board with complete solution
 * @param index Array of subgrid indices to process (typically ALL_INDICES)
 * @param count Number of subgrids to process (typically 9 for 9×9 board)
 * @return Number of cells removed (typically equals count)
 * 
 * @pre board != NULL && index != NULL
 * @pre Board contains a complete valid Sudoku solution
 * @post One cell per subgrid is emptied
 */
int phase1Elimination(SudokuBoard *board, const int *index, int count);

// ═══════════════════════════════════════════════════════════════
//                    PHASE 2: HEURISTIC ELIMINATION
// ═══════════════════════════════════════════════════════════════

/**
 * @brief Phase 2: Remove numbers that have no alternative positions
 * 
 * Removes cells whose numbers cannot validly go in any other position
 * within their row, column, or subgrid. These removals are "safe" because
 * a solver would be forced to place that exact number there.
 * 
 * ITERATIVE USAGE:
 * This function should be called in a loop until it returns 0:
 * 
 *     int removed;
 *     do {
 *         removed = phase2Elimination(board, indices, count);
 *     } while (removed > 0);
 * 
 * Removing one cell can create new "no alternative" situations for other
 * cells, hence the iterative approach.
 * 
 * CONFIGURABLE SIZE SUPPORT:
 * - hasAlternative() checks use dynamic row/column/subgrid bounds
 * - Subgrid iteration uses board->board_size
 * 
 * @param board Board with partially filled solution
 * @param index Array of subgrid indices to process
 * @param count Number of subgrids to process
 * @return Number of cells removed in this round (0 when converged)
 */
int phase2Elimination(SudokuBoard *board, const int *index, int count);

// ═══════════════════════════════════════════════════════════════
//                    PHASE 3: VERIFIED ELIMINATION
// ═══════════════════════════════════════════════════════════════

/**
 * @brief Phase 3: Free elimination with unique solution verification
 * 
 * Attempts to remove additional cells in random order, verifying after each
 * removal that the puzzle still has exactly one solution. This is the most
 * computationally expensive phase due to exhaustive solution counting.
 * 
 * CONFIGURABLE SIZE SUPPORT:
 * - Position array allocated as board_size² × sizeof(SudokuPosition)
 * - Target should be calculated proportionally (see phase3EliminationAuto)
 * 
 * @param board Board to perform Phase 3 elimination on
 * @param target Maximum number of cells to attempt removing
 * @return Number of cells successfully removed (≤ target)
 * 
 * @note For automatic target calculation, use phase3EliminationAuto() instead
 * @see phase3EliminationAuto
 */
int phase3Elimination(SudokuBoard *board, int target);

/**
 * @brief Phase 3 elimination with automatic target calculation
 * 
 * ✅ RECOMMENDED API for Phase 3 invocation
 * 
 * This wrapper function automatically calculates an appropriate removal
 * target based on the board's dimensions, eliminating the need for the
 * caller to manage size-dependent constants or calculations.
 * 
 * The target is calculated as a percentage of total cells, with the
 * percentage adjusted based on board size to maintain consistent difficulty:
 * 
 * - Small boards (≤9×9): 31% of cells (~25 for 9×9)
 * - Medium boards (≤16×16): 27% of cells (~69 for 16×16)
 * - Large boards (>16×16): 23% of cells (~144 for 25×25)
 * 
 * MIGRATION FROM PHASE3_TARGET CONSTANT:
 * 
 * Replace this:
 *     stats->phase3_removed = phase3Elimination(board, PHASE3_TARGET);
 * 
 * With this:
 *     stats->phase3_removed = phase3EliminationAuto(board);
 * 
 * @param board Board to perform Phase 3 elimination on
 * @return Number of cells successfully removed
 * 
 * @note Internally calls calculate_phase3_target() and phase3Elimination()
 * @see phase3Elimination for manual target control
 */
int phase3EliminationAuto(SudokuBoard *board);

// ═══════════════════════════════════════════════════════════════
//                    AUXILIARY INTERNAL FUNCTIONS
// ═══════════════════════════════════════════════════════════════

/**
 * @brief Check if a number has alternative valid positions
 * 
 * Determines whether the number in a given cell could validly be placed
 * in any other empty cell within its row, column, or subgrid. Used by
 * Phase 2 to identify "safe" removals.
 * 
 * CONFIGURABLE SIZE SUPPORT:
 * - Row/column iteration uses board_size
 * - Subgrid bounds calculated dynamically using subgrid_size
 * 
 * @param board The Sudoku board
 * @param pos Position containing the number to check
 * @param num The number to check for alternatives
 * @return true if at least one alternative position exists
 * 
 * @internal Used by phase2Elimination()
 */
bool hasAlternative(SudokuBoard *board, const SudokuPosition *pos, int num);

/**
 * @brief Count solutions with early exit optimization
 * 
 * Uses exhaustive backtracking to count the number of valid solutions
 * for the current board state, but exits early once the limit is reached.
 * For Phase 3 verification, limit=2 is sufficient (we only need to know
 * if there are 0, 1, or 2+ solutions).
 * 
 * CRITICAL OPTIMIZATION:
 * The early exit transforms worst-case O(N^m) into practical runtime
 * by stopping as soon as we know "more than one solution exists".
 * 
 * CONFIGURABLE SIZE SUPPORT:
 * - Number range is 1..board_size instead of 1..9
 * 
 * @param board Board to count solutions for
 * @param limit Stop counting after finding this many solutions
 * @return Number of solutions found (capped at limit)
 * 
 * @internal Used by phase3Elimination()
 */
int countSolutionsExact(SudokuBoard *board, int limit);

#endif  /* SUDOKU_ELIMINATION_INTERNAL_H */

/**
 * EDUCATIONAL NOTES FOR STUDENTS:
 * 
 * 1. INTERNAL vs PUBLIC HEADERS:
 *    
 *    This header is in src/core/internal/, not include/sudoku/core/.
 *    The difference:
 *    - include/: Public API, installed for external users
 *    - src/internal/: Private API, used only within the library
 *    
 *    External code should never #include this file directly.
 *    They should use the public generator.h interface.
 * 
 * 2. WHY TWO PHASE 3 FUNCTIONS?
 *    
 *    phase3Elimination(board, target) - Original signature, explicit control
 *    phase3EliminationAuto(board) - New wrapper, automatic calculation
 *    
 *    This pattern is called "API evolution". We keep the old function
 *    for backwards compatibility and advanced use cases, while providing
 *    a simpler alternative for the common case.
 * 
 * 3. THE DEPRECATION OF PHASE3_TARGET:
 *    
 *    The old constant #define PHASE3_TARGET 25 was problematic because:
 *    - It doesn't scale to different board sizes
 *    - It required recompilation to change
 *    - It was a "magic number" without context
 *    
 *    The new approach (calculate_phase3_target) is superior because:
 *    - It adapts automatically to any board size
 *    - It's calculated at runtime, not compile time
 *    - The calculation logic is documented and understandable
 */

/**
 * @file validation.h
 * @brief Validation and checking functions for Sudoku puzzles
 * @author Gonzalo Ramírez
 * @date 2025-11-06
 * 
 * This file provides the public API for validating Sudoku boards
 * and checking if placements are valid according to Sudoku rules.
 * It includes functions for position validation, board verification,
 * and solution counting for uniqueness checks.
 */

#ifndef SUDOKU_CORE_VALIDATION_H
#define SUDOKU_CORE_VALIDATION_H

#include <sudoku/core/types.h>

// ═══════════════════════════════════════════════════════════════════
//                    POSITION VALIDATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Check if a number can be legally placed at a position
 * 
 * This function validates the three fundamental rules of Sudoku:
 * - The number does not exist in the same row
 * - The number does not exist in the same column
 * - The number does not exist in the corresponding 3x3 subgrid
 * 
 * This is the core validation function used by both generation
 * and solving algorithms. It performs read-only examination of
 * the board without any modifications.
 *
 * @param[in] board Pointer to the Sudoku board to check
 * @param[in] pos Position where the number would be placed
 * @param[in] num Number to validate (must be 1-9)
 * @return true if the placement is legal, false if it violates rules
 * 
 * @pre board != NULL
 * @pre pos != NULL
 * @pre 1 <= num <= 9
 * @pre 0 <= pos->row < SUDOKU_SIZE
 * @pre 0 <= pos->col < SUDOKU_SIZE
 * 
 * @note The board is not modified by this function (const parameter)
 * @note Empty cells (value 0) in the board are ignored during checking
 * 
 * @warning Behavior is undefined if num is outside range 1-9
 * @warning Behavior is undefined if pos contains invalid coordinates
 * 
 * @see sudoku_validate_board() for complete board validation
 */
bool sudoku_is_safe_position(const SudokuBoard *board, 
                             const SudokuPosition *pos, 
                             int num);

// ═══════════════════════════════════════════════════════════════════
//                    BOARD VALIDATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Validate the entire board for rule compliance
 * 
 * Performs comprehensive validation by checking that every filled cell
 * on the board follows all Sudoku rules. This means no number appears
 * twice in any row, column, or 3x3 subgrid.
 * 
 * This function is used to verify that a generated or user-modified
 * puzzle is valid before attempting to solve it or present it to users.
 * 
 * @param[in] board Pointer to the board to validate
 * @return true if board is valid with no conflicts, false if any rule violations exist
 * 
 * @pre board != NULL
 * 
 * @note Empty cells (value 0) are skipped during validation
 * @note This function can validate both complete and incomplete boards
 * @note A completely empty board (all zeros) is considered valid
 * 
 * Example usage:
 * @code
 * SudokuBoard board;
 * sudoku_generate(&board, NULL);
 * if(sudoku_validate_board(&board)) {
 *     printf("Generated puzzle is valid!\n");
 * }
 * @endcode
 * 
 * @see sudoku_is_safe_position() for single position checking
 */

bool sudoku_find_empty_cell(const SudokuBoard *board, SudokuPosition *pos);

bool sudoku_validate_board(const SudokuBoard *board);

// ═══════════════════════════════════════════════════════════════════
//                    SOLUTION COUNTING
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Count the number of solutions for a partially-filled board
 * 
 * Uses exhaustive backtracking to count how many complete valid
 * solutions exist for the current board state. Stops searching
 * after finding 'limit' solutions for efficiency.
 * 
 * This function is critical for Phase 3 elimination during puzzle
 * generation. It verifies that after removing a cell, the puzzle
 * still has exactly one unique solution.
 * 
 * @param[in,out] board Board to analyze (temporarily modified during search,
 *                      restored to original state before return)
 * @param[in] limit Maximum number of solutions to find before stopping
 *                  (typically 2, since we only need to distinguish
 *                  "unique" vs "multiple")
 * @return Number of solutions found (capped at 'limit')
 * 
 * @pre board != NULL
 * @pre limit >= 1
 * @post Board is restored to its original state
 * 
 * @note Time complexity is O(9^m) where m = number of empty cells
 * @note For a completely empty board, this would explore all possible
 *       Sudoku boards, which is computationally infeasible. Typically
 *       used only on nearly-complete boards
 * @note Setting limit=2 is optimal for uniqueness checking: we only
 *       need to know if there's exactly 1 solution or more than 1
 * 
 * @warning This is a computationally expensive operation. On boards
 *          with many empty cells, it may take significant time
 * 
 * Example usage:
 * @code
 * SudokuBoard board;
 * // ... partially fill board ...
 * int solutions = countSolutionsExact(&board, 2);
 * if(solutions == 1) {
 *     printf("Puzzle has unique solution\n");
 * } else if(solutions > 1) {
 *     printf("Puzzle has multiple solutions\n");
 * }
 * @endcode
 * 
 * @see sudoku_validate_board() for checking validity without counting solutions
 */
int countSolutionsExact(SudokuBoard *board, int limit);

#endif // SUDOKU_CORE_VALIDATION_H

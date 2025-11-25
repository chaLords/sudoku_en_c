/**
 * @file validation.c
 * @brief Implementation of Sudoku validation rules (Adapted for configurable sizes)
 * @author Gonzalo Ramírez
 * @date 2025-11-20
 * @version 2.2.2-phase2b
 * 
 * PHASE 2B ADAPTATION: This module has been refactored to support configurable
 * board sizes (4×4, 9×9, 16×16, 25×25). All hardcoded SUDOKU_SIZE and SUBGRID_SIZE
 * constants have been replaced with dynamic access to board->board_size and
 * board->subgrid_size fields.
 * 
 * This module provides the core validation logic for Sudoku puzzles.
 * It implements the three fundamental rules: no duplicates in rows,
 * columns, or subgrids (size determined by board configuration).
 * 
 * The validation functions are used throughout the library:
 * - During generation to ensure legal number placements
 * - After completion to verify puzzle integrity
 * - During elimination to check solution uniqueness
 * 
 * All functions are designed to be pure (no side effects) except for
 * countSolutionsExact which temporarily modifies the board but always
 * restores it to its original state.
 */

#include "sudoku/core/validation.h"
#include "sudoku/core/types.h"
#include "internal/board_internal.h"

// ═══════════════════════════════════════════════════════════════════
//                    POSITION VALIDATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Check if a number can be legally placed at a specific position
 * 
 * PHASE 2B CHANGES: Adapted to use board->board_size and board->subgrid_size
 * instead of hardcoded SUDOKU_SIZE and SUBGRID_SIZE constants. This allows
 * the function to work correctly with boards of any supported size.
 * 
 * This is the fundamental validation function used throughout the library.
 * It verifies all three Sudoku rules: row uniqueness, column uniqueness,
 * and subgrid uniqueness.
 * 
 * The function assumes the target cell is empty (or we're checking if the
 * current value is valid). It does NOT check if the cell itself already
 * contains the number - that's the caller's responsibility.
 * 
 * @param board Pointer to the board to check (read-only)
 * @param pos Position where we want to place the number
 * @param num Number to validate (should be 1 to board_size)
 * @return true if placement is legal, false if it violates any rule
 */
bool sudoku_is_safe_position(const SudokuBoard *board, 
                              const SudokuPosition *pos, 
                              int num) {
    // Extract board geometry dynamically from the board instance
    // This replaces hardcoded SUDOKU_SIZE and SUBGRID_SIZE constants
    int board_size = board->board_size;
    int subgrid_size = board->subgrid_size;
    
    // Rule 1: Check row constraint
    // No other cell in this row should contain the same number
    // Iterate to board_size instead of SUDOKU_SIZE (9)
    for (int x = 0; x < board_size; x++) {
        if (board->cells[pos->row][x] == num) {
            return false;  // Found duplicate in row
        }
    }
    
    // Rule 2: Check column constraint
    // No other cell in this column should contain the same number
    // Iterate to board_size instead of SUDOKU_SIZE (9)
    for (int x = 0; x < board_size; x++) {
        if (board->cells[x][pos->col] == num) {
            return false;  // Found duplicate in column
        }
    }
    
    // Rule 3: Check subgrid constraint
    // Calculate the top-left corner of the subgrid containing this position
    // Formula: (pos / subgrid_size) * subgrid_size
    // This is more efficient than: pos - (pos % subgrid_size)
    // Examples:
    //   4×4 board (subgrid_size=2): position 3 → (3/2)*2 = 2 (starts at 2)
    //   9×9 board (subgrid_size=3): position 5 → (5/3)*3 = 3 (starts at 3)
    //   16×16 board (subgrid_size=4): position 7 → (7/4)*4 = 4 (starts at 4)
    int start_row = (pos->row / subgrid_size) * subgrid_size;
    int start_col = (pos->col / subgrid_size) * subgrid_size;
    
    // Check all cells in the subgrid (subgrid_size × subgrid_size)
    // Iterate to subgrid_size instead of SUBGRID_SIZE (3)
    for (int i = 0; i < subgrid_size; i++) {
        for (int j = 0; j < subgrid_size; j++) {
            if (board->cells[start_row + i][start_col + j] == num) {
                return false;  // Found duplicate in subgrid
            }
        }
    }
    
    // All three rules verified - the placement is safe
    return true;
}

// ═══════════════════════════════════════════════════════════════════
//                    BOARD TRAVERSAL UTILITIES
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Find the first empty cell in the board using row-major traversal
 * 
 * PHASE 2B CHANGES: Adapted to iterate to board->board_size instead of
 * hardcoded SUDOKU_SIZE, allowing it to work with boards of any size.
 * 
 * This function performs a left-to-right, top-to-bottom scan of the board
 * looking for the first cell containing zero (empty cell). This traversal
 * order is optimal for backtracking algorithms as it maintains a consistent
 * and predictable exploration pattern.
 * 
 * @param board Pointer to the board to search (read-only)
 * @param pos Pointer to position structure where result will be stored
 * @return true if empty cell found (pos contains its coordinates),
 *         false if board is complete (no empty cells)
 * 
 * @note This function is declared in board_internal.h but implemented here
 *       due to historical reasons. Ideally it would be in board.c as it's
 *       more of a traversal operation than a validation operation.
 *       Future refactoring could relocate it for better architecture.
 */
bool sudoku_find_empty_cell(const SudokuBoard *board, SudokuPosition *pos) {
    // Extract board size dynamically
    int board_size = board->board_size;
    
    // Traverse the board row by row, column by column (row-major order)
    // Iterate to board_size instead of SUDOKU_SIZE (9)
    for (pos->row = 0; pos->row < board_size; pos->row++) {
        for (pos->col = 0; pos->col < board_size; pos->col++) {
            // Empty cells are represented by 0
            if (board->cells[pos->row][pos->col] == 0) {
                return true;  // Found an empty cell, coordinates stored in pos
            }
        }
    }
    
    // Scanned entire board without finding empty cell - it's complete
    return false;
}

// ═══════════════════════════════════════════════════════════════════
//                    COMPLETE BOARD VALIDATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Validate that the entire board is free of rule violations
 * 
 * PHASE 2B CHANGES: Adapted to iterate to board->board_size. Also modified
 * the temporary board modification approach to work with dynamic memory
 * (cannot use simple struct copy with int **cells).
 * 
 * Performs comprehensive validation by checking every filled cell against
 * all three Sudoku rules. This is used to verify integrity of generated
 * puzzles or validate user-proposed solutions.
 * 
 * Algorithm: For each occupied cell, temporarily remove its value, then
 * verify that value would be legal in that position using standard safety
 * checks. If any cell fails this test, the board contains conflicts.
 * 
 * This approach is elegant because it reuses sudoku_is_safe_position(),
 * avoiding code duplication of the validation logic.
 * 
 * @param board Pointer to the board to validate (read-only)
 * @return true if board is valid (no conflicts), false if violations exist
 * 
 * @note Empty cells (value 0) are skipped and don't affect validity
 * @note Time complexity: O(n³) where n=board_size, as we check all constraints
 *       for each of the n² cells
 * @note A completely empty board is considered valid
 * @note The const qualifier on board parameter is temporarily "cast away"
 *       during validation, but the board is immediately restored to its
 *       original state. This is safe but technically violates const-correctness.
 */
bool sudoku_validate_board(const SudokuBoard *board) {
    // Extract board size dynamically
    int board_size = board->board_size;
    
    // Verify each cell in the board_size × board_size grid
    // Iterate to board_size instead of SUDOKU_SIZE (9)
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            // Skip empty cells - they don't violate any rules
            if (board->cells[i][j] == 0) {
                continue;
            }
            
            // Save the current cell's value
            int num = board->cells[i][j];
            
            // PHASE 2B NOTE: We cannot use "SudokuBoard temp = *board;" anymore
            // because board->cells is now int** (pointer), not int[][] (array).
            // Copying the struct would copy the pointer, not the actual data.
            // Instead, we temporarily modify the board and immediately restore it.
            
            // Temporarily mark cell as empty to test if its value is legal
            // We need to cast away const - this is safe because we restore immediately
            int original = board->cells[i][j];
            ((SudokuBoard*)board)->cells[i][j] = 0;
            
            // Check if this number could legally be placed at this position
            // If not, then having it there violates Sudoku rules
            SudokuPosition pos = {i, j};
            bool is_safe = sudoku_is_safe_position(board, &pos, num);
            
            // Immediately restore the original value
            ((SudokuBoard*)board)->cells[i][j] = original;
            
            if (!is_safe) {
                return false;  // Conflict detected
            }
        }
    }
    
    // All cells passed verification - board is valid
    return true;
}

// ═══════════════════════════════════════════════════════════════════
//                    SOLUTION COUNTING
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Count number of solutions using exhaustive backtracking
 * 
 * PHASE 2B CHANGES: Adapted to try numbers from 1 to board->board_size
 * instead of hardcoded 1 to 9. This allows the function to work correctly
 * with boards of any size (4×4 tries 1-4, 16×16 tries 1-16, etc.).
 * 
 * Performs exhaustive search to count how many complete valid solutions
 * exist for the current board state. Stops searching after finding 'limit'
 * solutions for efficiency.
 * 
 * This function is critical for Phase 3 elimination during puzzle generation.
 * After removing a cell, we call this with limit=2 to verify the puzzle still
 * has exactly one solution. We don't need to count all solutions (which could
 * be millions), just distinguish between "exactly 1" and "more than 1".
 * 
 * Algorithm: Classic recursive backtracking
 * 1. Find first empty cell (if none, we have a complete solution, return 1)
 * 2. Try each number 1 to board_size in that cell
 * 3. If legal, place it and recursively count solutions for remaining cells
 * 4. Accumulate total solution count across all valid placements
 * 5. Important: Backtrack (undo placement) before trying next number
 * 
 * @param board Board to analyze (will be temporarily modified but restored)
 * @param limit Maximum number of solutions to find before stopping
 *              (typically 2: we only need to know if unique or not)
 * @return Number of solutions found (capped at 'limit')
 * 
 * @pre board != NULL
 * @pre limit >= 1
 * @post Board is guaranteed to be in identical state as before call
 *       (all temporary modifications are undone via backtracking)
 * 
 * @note Time complexity: O(board_size^m) where m = number of empty cells
 *       This is exponential and can be very slow on boards with many empty cells
 *       For 16×16 boards, this is significantly slower than 9×9 boards!
 * @note Space complexity: O(m) due to recursion stack depth
 * @note Setting limit=2 is optimal for uniqueness checking: we only need to
 *       distinguish "exactly 1 solution" from "multiple solutions"
 * 
 * @warning Computationally expensive! On a completely empty board, this would
 *          attempt to enumerate all valid Sudoku boards, which is infeasible.
 *          Only use on boards with sufficient filled cells (40+ for 9×9,
 *          proportionally more for larger boards).
 * 
 * Example usage:
 * @code
 * // After removing a cell during generation
 * int solutions = countSolutionsExact(board, 2);
 * if (solutions == 1) {
 *     // Good! Puzzle still has unique solution, keep the removal
 * } else {
 *     // Bad! Multiple solutions or no solution, undo the removal
 * }
 * @endcode
 */
int countSolutionsExact(SudokuBoard *board, int limit) {
    SudokuPosition pos;
    
    // Base case: if no empty cells remain, we have a complete solution
    if (!sudoku_find_empty_cell(board, &pos)) {
        return 1;
    }
    
    // Extract board size to determine range of valid numbers
    int board_size = board->board_size;
    
    int totalSolutions = 0;
    
    // Recursive case: try each number 1 to board_size in the first empty cell
    // For 4×4: tries 1,2,3,4
    // For 9×9: tries 1,2,3,4,5,6,7,8,9
    // For 16×16: tries 1,2,3,...,16
    // Iterate to board_size instead of SUDOKU_SIZE (9)
    for (int num = 1; num <= board_size; num++) {
        // Only try numbers that satisfy Sudoku rules at this position
        if (sudoku_is_safe_position(board, &pos, num)) {
            // Place number temporarily
            board->cells[pos.row][pos.col] = num;
            
            // Recursively count solutions for the resulting board state
            totalSolutions += countSolutionsExact(board, limit);
            
            // Early exit optimization: if we've already found enough solutions
            // to exceed the limit, no point continuing the search
            if (totalSolutions >= limit) {
                // Backtrack before returning (critical for correctness!)
                board->cells[pos.row][pos.col] = 0;
                return totalSolutions;
            }
            
            // Backtrack: remove the number to try next possibility
            // This ensures board is restored to state before this iteration
            board->cells[pos.row][pos.col] = 0;
        }
    }
    
    // Return total solutions found across all valid placements
    // Note: board is now in identical state as when function was called
    return totalSolutions;
}

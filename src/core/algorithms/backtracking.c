/**
 * @file backtracking.c
 * @brief Recursive backtracking algorithm for Sudoku board completion
 * @author Gonzalo Ramírez
 * @date 2025-10-19
 * 
 * This module implements the backtracking algorithm used to complete a
 * partially filled Sudoku board. The algorithm uses recursive exploration
 * with intelligent pruning to efficiently find a valid complete board.
 */

#include "../internal/generator_internal.h"
#include "../internal/board_internal.h"
#include "sudoku/core/validation.h"
#include <stdlib.h>
#include <assert.h>

/**
 * @brief Shuffles an array using the Fisher-Yates algorithm
 * 
 * This helper function randomizes the order of elements in an array using
 * the Fisher-Yates shuffle algorithm, which guarantees a uniformly random
 * permutation. Each possible ordering has equal probability of occurring.
 * 
 * The algorithm iterates backwards through the array, and for each position
 * selects a random element from the remaining unshuffled portion (including
 * the current position) and swaps it into place.
 * 
 * We use the traditional three-assignment swap with a temporary variable
 * because it is clear, efficient on modern CPUs, and universally understood
 * by all programmers. While other swap techniques exist (like XOR swap),
 * they offer no practical advantage and sacrifice readability.
 *
 * @param array Pointer to the array to shuffle (modified in place)
 * @param size Number of elements in the array
 * 
 * @note This function has internal linkage (static) - only visible in this file
 * @note Requires that srand() has been called to seed the random generator
 * @note Time complexity: O(n) where n is the size of the array
 * @note Space complexity: O(1) - only uses a temporary variable for swapping
 */
static void shuffle_numbers(int *array, int size) {
    // Iterate backwards through the array
    // For each position i, we'll swap it with a random position from 0 to i
    for(int i = size - 1; i > 0; i--) {
        // Generate a random index j where 0 <= j <= i
        // This ensures we only swap with elements in the unshuffled portion
        int j = rand() % (i + 1);
        
        // Perform the swap using the traditional three-assignment pattern
        // This is the clearest and most efficient way to swap on modern CPUs
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

/**
 * @brief Completes a partially filled board using recursive backtracking
 * 
 * This is the core algorithm of the hybrid Sudoku generator. It assumes
 * the board has at least some cells filled (typically the main diagonal
 * subgrids filled by Fisher-Yates) and completes the remaining cells.
 * 
 * The algorithm uses systematic exploration with backtracking:
 * - Find an empty cell (if none exist, we're done - success!)
 * - Try placing each number 1-9 in random order
 * - For each valid number, recursively try to complete the rest
 * - If recursion succeeds, propagate success up the call stack
 * - If recursion fails, backtrack by removing the number and trying the next
 * - If all numbers fail, return false to trigger backtracking in the caller
 * 
 * The randomization of number order ensures variety in generated boards.
 * Without shuffling, the algorithm would be deterministic and always produce
 * identical boards for the same starting configuration.
 *
 * @param board Pointer to the board to complete (WILL BE MODIFIED IN PLACE)
 * @return true if the board was successfully completed
 * @return false if no valid completion exists
 * 
 * @pre board != NULL
 * @post If returns true, board contains a valid complete Sudoku
 * @post If returns false, board is in an indeterminate partial state
 * 
 * @warning Recursive function - uses stack space proportional to empty cells
 * @note Maximum recursion depth is bounded by number of empty cells (< 81)
 * @note Excellent average performance due to early pruning via validation
 */
bool sudoku_complete_backtracking(SudokuBoard *board) {
    // Precondition validation: ensure we received a valid board pointer
    assert(board != NULL);
    
    // Declare a position structure to store coordinates of empty cell
    SudokuPosition pos;
    
    // BASE CASE: Try to find an empty cell in the board
    // If no empty cells remain, the board is complete - we succeeded!
    if(!sudoku_find_empty_cell(board, &pos)) {
        return true;  // Success! Propagate true up the recursion
    }
    
    // RECURSIVE CASE: We found an empty cell that needs to be filled
    
    // Create an array with numbers 1 through 9
    int numbers[SUDOKU_SIZE];
    for(int i = 0; i < SUDOKU_SIZE; i++) {
        numbers[i] = i + 1;
    }
    
    // Shuffle the numbers to create variety in generated boards
    // Without this, we'd always try numbers in the same order (1,2,3...)
    // which would produce identical boards every time
    shuffle_numbers(numbers, SUDOKU_SIZE);
    
    // Try each number in our randomized order
    for(int i = 0; i < SUDOKU_SIZE; i++) {
        int num = numbers[i];
        
        // PRUNING: Check if this number violates any Sudoku rules
        // This validation dramatically reduces the search space by
        // immediately eliminating invalid choices
        if(sudoku_is_safe_position(board, &pos, num)) {
            // The number is valid! Place it tentatively in the cell
            board->cells[pos.row][pos.col] = num;
            
            // RECURSION: Try to complete the rest of the board with this
            // number in place. If the recursion returns true, we found
            // a complete valid solution!
            if(sudoku_complete_backtracking(board)) {
                return true;  // Success! Let the recursion unwind
            }
            
            // BACKTRACKING: If we reach here, recursion returned false,
            // which means this number eventually led to a dead end.
            // We need to undo this choice and try a different number.
            // This undoing is what gives "backtracking" its name.
            board->cells[pos.row][pos.col] = 0;
            
            // The loop continues to try the next number
        }
        // If the number wasn't safe, we simply skip to the next iteration
        // This is implicit pruning - we never attempt invalid placements
    }
    
    // FAILURE CASE: We tried all nine numbers and none led to success
    // This means the current board state has no valid completion
    // Return false to trigger backtracking in our caller
    // Note: In our hybrid generator this is extremely rare because we
    // start with valid diagonal subgrids, but we handle it correctly
    return false;
}

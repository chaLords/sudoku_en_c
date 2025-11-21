/**
 * @file backtracking.c
 * @brief Recursive backtracking algorithm for Sudoku board completion
 * @author Gonzalo Ramírez
 * @date 2025-10-19
 * 
 * This module implements the backtracking algorithm used to complete a
 * partially filled Sudoku board. The algorithm uses recursive exploration
 * with intelligent pruning to efficiently find a valid complete board.
 * 
 * Version 2.3.0: Adapted for configurable board sizes (4×4, 9×9, 16×16, 25×25)
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
 * - Try placing each number from 1 to board_size in random order
 * - For each valid number, recursively try to complete the rest
 * - If recursion succeeds, propagate success up the call stack
 * - If recursion fails, backtrack by removing the number and trying the next
 * - If all numbers fail, return false to trigger backtracking in the caller
 * 
 * The randomization of number order ensures variety in generated boards.
 * Without shuffling, the algorithm would be deterministic and always produce
 * identical boards for the same starting configuration.
 * 
 * CONFIGURABLE SIZE ADAPTATION:
 * This function now works with boards of any valid size (4×4, 9×9, 16×16, etc.)
 * by allocating the numbers array dynamically based on board->board_size.
 * This requires careful memory management to avoid leaks in the recursive calls.
 *
 * @param board Pointer to the board to complete (WILL BE MODIFIED IN PLACE)
 * @return true if the board was successfully completed
 * @return false if no valid completion exists or memory allocation fails
 * 
 * @pre board != NULL
 * @pre board->board_size > 0
 * @post If returns true, board contains a valid complete Sudoku
 * @post If returns false, board is in an indeterminate partial state
 * 
 * @warning Recursive function - uses stack space proportional to empty cells
 * @warning Each recursive call allocates board_size integers on the heap
 * @note Maximum recursion depth bounded by number of empty cells (< board_size²)
 * @note Excellent average performance due to early pruning via validation
 * @note Memory usage: O(depth × board_size) where depth is recursion level
 */
bool sudoku_complete_backtracking(SudokuBoard *board) {
    // Precondition validation: ensure we received a valid board pointer
    assert(board != NULL);
    assert(board->board_size > 0);
    
    // Declare a position structure to store coordinates of empty cell
    SudokuPosition pos;
    
    // BASE CASE: Try to find an empty cell in the board
    // If no empty cells remain, the board is complete - we succeeded!
    if(!sudoku_find_empty_cell(board, &pos)) {
        return true;  // Success! Propagate true up the recursion
    }
    
    // RECURSIVE CASE: We found an empty cell that needs to be filled
    
    // ✅ CHANGE 1: Dynamic allocation based on actual board size
    // Instead of a fixed-size array, we allocate exactly what we need
    // For a 4×4 board: allocate 4 ints (16 bytes)
    // For a 9×9 board: allocate 9 ints (36 bytes)
    // For a 16×16 board: allocate 16 ints (64 bytes)
    int *numbers = malloc(board->board_size * sizeof(int));
    
    // Critical error handling: if malloc fails, we cannot continue
    // This is rare but must be handled to prevent undefined behavior
    if(numbers == NULL) {
        // Memory allocation failed - return false to indicate failure
        // The board state is unchanged, so this is safe
        return false;
    }
    
    // ✅ CHANGE 2: Fill array with numbers 1 to board_size (not hardcoded to 9)
    // For 4×4: [1, 2, 3, 4]
    // For 9×9: [1, 2, 3, 4, 5, 6, 7, 8, 9]
    // For 16×16: [1, 2, 3, ..., 16]
    for(int i = 0; i < board->board_size; i++) {
        numbers[i] = i + 1;
    }
    
    // Shuffle the numbers to create variety in generated boards
    // Without this, we'd always try numbers in the same order (1,2,3...)
    // which would produce identical boards every time
    shuffle_numbers(numbers, board->board_size);
    
    // ✅ CHANGE 3: Iterate up to board_size (not hardcoded SUDOKU_SIZE)
    // This ensures we try all possible numbers for this board size
    for(int i = 0; i < board->board_size; i++) {
        int num = numbers[i];
        
        // PRUNING: Check if this number violates any Sudoku rules
        // The validation functions have already been adapted to work with
        // any board size, so this just works transparently
        if(sudoku_is_safe_position(board, &pos, num)) {
            // The number is valid! Place it tentatively in the cell
            board->cells[pos.row][pos.col] = num;
            
            // RECURSION: Try to complete the rest of the board with this
            // number in place. If the recursion returns true, we found
            // a complete valid solution!
            if(sudoku_complete_backtracking(board)) {
                // ✅ CRITICAL: Free memory before returning success
                // This is essential to prevent memory leaks
                // Every malloc must have a corresponding free
                free(numbers);
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
    
    // FAILURE CASE: We tried all numbers and none led to success
    // This means the current board state has no valid completion
    
    // ✅ CRITICAL: Free memory before returning failure
    // Even in failure cases, we must free allocated memory
    // This ensures no leaks occur during backtracking
    free(numbers);
    
    // Return false to trigger backtracking in our caller
    // Note: In our hybrid generator this is extremely rare because we
    // start with valid diagonal subgrids, but we handle it correctly
    return false;
}

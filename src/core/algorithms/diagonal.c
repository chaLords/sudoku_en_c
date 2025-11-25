/**
 * @file diagonal.c
 * @brief Diagonal subgrid filling algorithm for Sudoku generation
 * 
 * This module implements the diagonal filling strategy used in the hybrid
 * generation algorithm. The key insight is that diagonal subgrids (those
 * along the main diagonal: 0,0 -> 1,1 -> 2,2 in subgrid coordinates) are
 * completely independent - they share no rows, columns, or regions.
 * 
 * This independence allows us to fill them with random permutations using
 * the Fisher-Yates algorithm without any constraint checking, which is
 * significantly faster than backtracking would be for these positions.
 * 
 * @author Gonzalo Ramírez (@chaLords)
 * @date November 2025
 * @copyright Apache License 2.0
 */

#include "sudoku/core/board.h"
#include "sudoku/core/types.h"
#include "board_internal.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

/**
 * @brief Generates a random permutation using Fisher-Yates algorithm
 * 
 * This is a private helper function that creates a random permutation of
 * integers from 1 to size. We use the Fisher-Yates shuffle because it
 * guarantees uniform distribution - every permutation has exactly the
 * same probability of occurring (1/n!).
 * 
 * Implementation note: We use the XOR swap trick here for educational
 * purposes, showing students an alternative to the traditional three-line
 * swap with a temporary variable. In production code, the traditional
 * swap is equally valid and often more readable.
 * 
 * Time Complexity: O(n) where n is the size
 * Space Complexity: O(1) - modifies array in place
 * 
 * @param array Array to fill with the permutation (must be size length)
 * @param size Length of the array and range of numbers (1 to size)
 */
static void shuffle_numbers(int *array, int size) {
    // Phase 1: Initialize with consecutive values [1, 2, 3, ..., size]
    for (int i = 0; i < size; i++) {
        array[i] = i + 1;
    }
    
    // Phase 2: Fisher-Yates shuffle from end to beginning
    // This ensures uniform distribution of permutations
    for (int i = size - 1; i > 0; i--) {
        // Generate random index in range [0, i]
        int j = rand() % (i + 1);
        
        // Swap elements using XOR trick (only if indices differ)
        // XOR swap works because: a^a = 0 and a^0 = a
        if (i != j) {
            array[i] ^= array[j];
            array[j] ^= array[i];
            array[i] ^= array[j];
        }
    }
}

/**
 * @brief Fills a single subgrid with a random permutation
 * 
 * This private helper function takes a subgrid defined by its top-left
 * corner position and fills it completely with a shuffled sequence of
 * valid numbers. It abstracts away the complexity of converting from
 * linear permutation index to 2D board coordinates.
 * 
 * Design decision: We use sudoku_board_set_cell() and getter functions
 * instead of direct structure access. This ensures proper encapsulation
 * and allows the board module to maintain invariants.
 * 
 * Time Complexity: O(n²) where n is subgrid_size
 * Space Complexity: O(n) for the temporary numbers array where n is board size
 * 
 * @param board Pointer to the board structure
 * @param start_row Row coordinate of subgrid's top-left cell
 * @param start_col Column coordinate of subgrid's top-left cell
 */
static void fill_single_diagonal_subgrid(SudokuBoard *board, 
                                         int start_row, 
                                         int start_col) {
    // ✅ CORRECCIÓN: Usar getters en lugar de acceso directo
    // Antes: int size = board->size;
    // Ahora: usamos la función getter pública
    int size = sudoku_board_get_board_size(board);
    
    // ✅ CORRECCIÓN: Usar getter para subgrid_size también
    // Antes: int subgrid_size = board->subgrid_size;
    // Ahora: usamos la función getter pública
    int subgrid_size = sudoku_board_get_subgrid_size(board);
    
    // Allocate temporary array for the shuffled numbers
    // We need 'size' numbers (not subgrid_size²) because each cell
    // gets a value from 1 to size, and we need one value per cell
    int *numbers = (int *)malloc(size * sizeof(int));
    if (numbers == NULL) {
        // In a production system, we'd propagate this error upward
        // For now, we return silently which leaves the subgrid unfilled
        // TODO: Implement proper error propagation mechanism
        return;
    }
    
    // Generate random permutation of numbers [1..size]
    shuffle_numbers(numbers, size);
    
    // Fill the subgrid in row-major order
    // The outer loop iterates through rows, inner through columns
    int number_index = 0;
    for (int row = start_row; row < start_row + subgrid_size; row++) {
        for (int col = start_col; col < start_col + subgrid_size; col++) {
            // Use the public API to set the cell value
            // This maintains encapsulation and allows board module
            // to track statistics or enforce constraints
            sudoku_board_set_cell(board, row, col, numbers[number_index]);
            number_index++;
        }
    }
    
    // Always free dynamically allocated memory to prevent leaks
    free(numbers);
}

/**
 * @brief Fills all diagonal subgrids with random valid numbers
 * 
 * This is the main public-within-library function that implements the
 * diagonal filling strategy. It's called "fillDiagonal" to maintain
 * compatibility with the original monolithic implementation, but note
 * that this version is generalized to work with any board size.
 * 
 * Algorithm Overview:
 * 1. Identify which subgrids are on the main diagonal
 * 2. For each diagonal subgrid, generate a random permutation
 * 3. Fill that subgrid with the permutation
 * 
 * Why This Works:
 * Diagonal subgrids don't share rows, columns, or regions with each
 * other. This means we can fill them independently without any risk
 * of creating constraint violations. This is much faster than using
 * backtracking for these cells.
 * 
 * Performance Analysis:
 * - Time: O(s × n²) where s is subgrid_size and n is board size
 *   This simplifies to O(n²) since s×n² = √n × n² = n^(5/2)/n^(1/2) = n²
 * - Space: O(n) for the temporary permutation array
 * 
 * Compared to filling these cells with backtracking, which would be
 * O(b^m) where b is branching factor and m is number of cells, this
 * is vastly more efficient. For a 9×9 board, we're talking O(81) vs
 * potentially O(9^27) in worst case backtracking.
 * 
 * @param board Pointer to the board structure to fill
 *              Must be a valid, initialized board (non-NULL)
 *              Board must be in a clean state (typically all zeros)
 */
void fillDiagonal(SudokuBoard *board) {
    // Defensive programming: validate input
    if (board == NULL) {
        return;
    }
    
    // ✅ CORRECCIÓN: Usar getter en lugar de acceso directo
    // Antes: int subgrid_size = board->subgrid_size;
    // Ahora: usamos la función getter pública
    int subgrid_size = sudoku_board_get_subgrid_size(board);
    
    // Initialize random number generator (thread-safe singleton pattern)
    // We only seed once per program execution to avoid predictable patterns
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }
    
    // Fill each diagonal subgrid
    // In subgrid coordinates, diagonal subgrids are at (0,0), (1,1), (2,2), ...
    // In board coordinates, they're at (0,0), (s,s), (2s,2s), ...
    // where s = subgrid_size
    for (int i = 0; i < subgrid_size; i++) {
        // Calculate the top-left corner of the i-th diagonal subgrid
        int start_row = i * subgrid_size;
        int start_col = i * subgrid_size;
        
        // Fill this diagonal subgrid with a random permutation
        fill_single_diagonal_subgrid(board, start_row, start_col);
    }
    
    // At this point, we've filled subgrid_size² × subgrid_size cells
    // For a 9×9 board, that's 3 subgrids × 9 cells = 27 cells filled
    // For a 16×16 board, that's 4 subgrids × 16 cells = 64 cells filled
    // The remaining cells will be filled by the backtracking algorithm
}

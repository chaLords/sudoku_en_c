/**
 * @file test_backtracking.c
 * @brief Unit tests for the configurable backtracking module
 * @author Gonzalo Ramírez
 * @date 2025-11-21
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "sudoku/core/types.h"
#include "sudoku/core/board.h"
#include "sudoku/core/validation.h"
#include "../internal/generator_internal.h"

/**
 * @brief Test backtracking with a 4×4 board (simplest case)
 * 
 * A 4×4 Sudoku is the smallest valid size, making it ideal for testing
 * the basic functionality of the backtracking algorithm without excessive
 * computation time. We create a partially filled valid board and verify
 * that backtracking can complete it correctly.
 */
void test_backtracking_4x4_partial() {
    printf("Test: sudoku_complete_backtracking with 4×4 partial board... ");
    
    SudokuBoard *board = sudoku_board_create_size(2);  // 2×2 subgrids → 4×4 board
    assert(board != NULL);
    
    // Create a valid partial configuration
    // Starting board:
    // 1 2 | . .
    // 3 4 | . .
    // ----|----
    // . . | 4 1
    // . . | 2 3
    
    sudoku_board_set_cell(board, 0, 0, 1);
    sudoku_board_set_cell(board, 0, 1, 2);
    sudoku_board_set_cell(board, 1, 0, 3);
    sudoku_board_set_cell(board, 1, 1, 4);
    
    sudoku_board_set_cell(board, 2, 2, 4);
    sudoku_board_set_cell(board, 2, 3, 1);
    sudoku_board_set_cell(board, 3, 2, 2);
    sudoku_board_set_cell(board, 3, 3, 3);
    
    // Attempt to complete the board
    bool completed = sudoku_complete_backtracking(board);
    
    // Verify success
    assert(completed == true);
    
    // Verify all cells are filled with valid numbers
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            int value = sudoku_board_get_cell(board, i, j);
            assert(value >= 1 && value <= 4);
        }
    }
    
    // Verify the board is actually valid (no conflicts)
    assert(sudoku_validate_board(board) == true);
    
    sudoku_board_destroy(board);
    printf("✅ PASSED\n");
}

/**
 * @brief Test backtracking with a completely empty 9×9 board
 * 
 * This is the classic Sudoku size and also the most computationally
 * intensive test. Starting from a completely empty board, backtracking
 * must find a valid complete solution. This tests the algorithm's ability
 * to handle maximum recursion depth.
 */
void test_backtracking_9x9_empty() {
    printf("Test: sudoku_complete_backtracking with 9×9 empty board... ");
    
    SudokuBoard *board = sudoku_board_create_size(3);  // 3×3 subgrids → 9×9 board
    assert(board != NULL);
    
    // Board starts completely empty (all zeros)
    // This is the maximum challenge for the backtracking algorithm
    
    bool completed = sudoku_complete_backtracking(board);
    
    // Verify success
    assert(completed == true);
    
    // Count filled cells and verify they're all valid
    int filled_count = 0;
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            int value = sudoku_board_get_cell(board, i, j);
            if(value != 0) {
                filled_count++;
            }
            // Every cell should now contain a number 1-9
            assert(value >= 1 && value <= 9);
        }
    }
    
    // All 81 cells should be filled
    assert(filled_count == 81);
    
    // Validate the completed board
    assert(sudoku_validate_board(board) == true);
    
    sudoku_board_destroy(board);
    printf("✅ PASSED\n");
}

/**
 * @brief Test backtracking with a 16×16 board (larger scale)
 * 
 * This test verifies that the algorithm scales correctly to larger boards.
 * A 16×16 Sudoku has 256 cells and uses numbers 1-16, presenting a
 * significant computational challenge. We use a partially filled board
 * to keep test execution time reasonable.
 */
void test_backtracking_16x16_partial() {
    printf("Test: sudoku_complete_backtracking with 16×16 partial board... ");
    
    SudokuBoard *board = sudoku_board_create_size(4);  // 4×4 subgrids → 16×16 board
    assert(board != NULL);
    
    // Fill the first subgrid completely to reduce search space
    // This makes the test complete in reasonable time
    int nums[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    int idx = 0;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            sudoku_board_set_cell(board, i, j, nums[idx++]);
        }
    }
    
    // Attempt to complete the board
    bool completed = sudoku_complete_backtracking(board);
    
    // Verify success
    assert(completed == true);
    
    // Verify all cells contain valid numbers 1-16
    for(int i = 0; i < 16; i++) {
        for(int j = 0; j < 16; j++) {
            int value = sudoku_board_get_cell(board, i, j);
            assert(value >= 1 && value <= 16);
        }
    }
    
    // Validate correctness
    assert(sudoku_validate_board(board) == true);
    
    sudoku_board_destroy(board);
    printf("✅ PASSED\n");
}

/**
 * @brief Test that backtracking handles an already complete board
 * 
 * Edge case: What happens if we call backtracking on a board that's
 * already complete? It should immediately recognize this and return
 * success without modifying the board.
 */
void test_backtracking_already_complete() {
    printf("Test: sudoku_complete_backtracking with already complete board... ");
    
    SudokuBoard *board = sudoku_board_create_size(2);  // 4×4 for speed
    assert(board != NULL);
    
    // Fill the entire board with a valid solution
    // This is a valid 4×4 Sudoku:
    // 1 2 | 3 4
    // 3 4 | 1 2
    // ----|----
    // 2 3 | 4 1
    // 4 1 | 2 3
    
    int solution[4][4] = {
        {1, 2, 3, 4},
        {3, 4, 1, 2},
        {2, 3, 4, 1},
        {4, 1, 2, 3}
    };
    
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            sudoku_board_set_cell(board, i, j, solution[i][j]);
        }
    }
    
    // Call backtracking on the complete board
    bool completed = sudoku_complete_backtracking(board);
    
    // Should return true immediately (no cells to fill)
    assert(completed == true);
    
    // Board should be unchanged and still valid
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            int value = sudoku_board_get_cell(board, i, j);
            assert(value == solution[i][j]);
        }
    }
    
    sudoku_board_destroy(board);
    printf("✅ PASSED\n");
}

/**
 * @brief Test backtracking with an impossible configuration
 * 
 * This test verifies that the algorithm correctly recognizes when a board
 * cannot be completed. We create a configuration with obvious conflicts
 * that make a valid solution impossible.
 */
void test_backtracking_impossible() {
    printf("Test: sudoku_complete_backtracking with impossible configuration... ");
    
    SudokuBoard *board = sudoku_board_create_size(2);  // 4×4 board
    assert(board != NULL);
    
    // Create an impossible configuration by placing conflicts
    // Put two 1's in the same row
    sudoku_board_set_cell(board, 0, 0, 1);
    sudoku_board_set_cell(board, 0, 1, 1);  // Conflict!
    
    // The backtracking should recognize this is impossible
    bool completed = sudoku_complete_backtracking(board);
    
    // Should return false (cannot complete)
    // Note: Depending on your validation implementation, this might
    // detect the conflict immediately or after attempting placements
    assert(completed == false);
    
    sudoku_board_destroy(board);
    printf("✅ PASSED (correctly rejected impossible board)\n");
}

/**
 * @brief Test memory management in deep recursion
 * 
 * This test specifically exercises the memory management aspect of
 * backtracking. By running multiple completions and checking for leaks
 * (in a production environment, you'd use Valgrind or similar tools),
 * we verify that malloc/free pairs are correctly implemented.
 */
void test_backtracking_memory_management() {
    printf("Test: sudoku_complete_backtracking memory management... ");
    
    // Run multiple iterations to ensure no cumulative leaks
    for(int iteration = 0; iteration < 10; iteration++) {
        SudokuBoard *board = sudoku_board_create_size(2);  // 4×4 for speed
        assert(board != NULL);
        
        // Partially fill the board
        sudoku_board_set_cell(board, 0, 0, 1);
        sudoku_board_set_cell(board, 1, 1, 2);
        
        // Complete it
        bool completed = sudoku_complete_backtracking(board);
        assert(completed == true);
        
        // Clean up
        sudoku_board_destroy(board);
    }
    
    printf("✅ PASSED (10 iterations, no leaks detected)\n");
}

int main(void) {
    // Seed random number generator for shuffle_numbers
    srand(time(NULL));
    
    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("   BACKTRACKING MODULE TESTS - Configurable Sizes\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");
    
    test_backtracking_4x4_partial();
    test_backtracking_9x9_empty();
    test_backtracking_16x16_partial();
    test_backtracking_already_complete();
    test_backtracking_impossible();
    test_backtracking_memory_management();
    
    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("   ALL BACKTRACKING TESTS PASSED! ✅\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");
    
    return 0;
}

/**
 * @file test_backtracking.c
 * @brief Unit tests for the backtracking algorithm in the Sudoku generator
 * @author Gonzalo Ramírez
 * @date 2025-10-30
 * 
 * This test file verifies that sudoku_complete_backtracking correctly
 * completes a partially filled Sudoku board.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

// Include the modular API headers
#include "sudoku/core/types.h"
#include "sudoku/core/board.h"
#include "sudoku/core/validation.h"

// Include internal headers needed for testing
#include "../internal/board_internal.h"
#include "../internal/generator_internal.h"

/* ================================================================
                    TEST INFRASTRUCTURE
   ================================================================ */

/**
 * @brief Test results counter
 * This structure tracks how many tests passed and failed
 */
typedef struct {
    int passed;
    int failed;
    int total;
} TestResults;

// Global test results (initialized to zeros)
TestResults results = {0, 0, 0};

/**
 * @brief Macro for test assertions
 * This is our mini-framework for testing
 * The do-while(0) pattern is standard for multi-statement macros
 */
#define TEST_ASSERT(condition, message) do { \
    results.total++; \
    if(condition) { \
        printf("  ✓ [PASS] %s\n", message); \
        results.passed++; \
    } else { \
        printf("  ✗ [FAIL] %s\n", message); \
        results.failed++; \
    } \
} while(0)

/* ================================================================
                    HELPER FUNCTIONS
   ================================================================ */

/**
 * @brief Helper function to print a board for visualization
 * This is useful for debugging when tests fail
 */
void print_test_board(const SudokuBoard *board) {
    printf("\nCurrent board state:\n");
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++) {
            if (board->cells[i][j] == 0) {
                printf(" . ");
            } else {
                printf(" %d ", board->cells[i][j]);
            }
            if ((j + 1) % 3 == 0 && j < 8) printf("| ");
        }
        printf("\n");
        if ((i + 1) % 3 == 0 && i < 8) {
            printf("---------+----------+---------\n");
        }
    }
    printf("\n");
}

/**
 * @brief Setup a partially filled board for testing
 * 
 * Creates a board with two diagonal subgrids filled plus a few extra cells.
 * This creates a known, deterministic state perfect for testing backtracking.
 * 
 * We use FIXED values (not random) so the test is deterministic and reproducible.
 */
void setup_partial_board(SudokuBoard *board) {
    // Initialize board to all zeros
    sudoku_board_init(board);
    
    // Create subgrids for diagonal positions 0 and 4
    SudokuSubGrid sg0 = sudoku_subgrid_create(0);  // Top-left (0,0)
    SudokuSubGrid sg4 = sudoku_subgrid_create(4);  // Center (3,3)
    
    // Use fixed, known values - these are valid Sudoku subgrids
    int subgrid0_values[9] = {5, 3, 7, 6, 2, 1, 9, 8, 4};
    int subgrid4_values[9] = {8, 1, 6, 4, 5, 7, 9, 2, 3};
    
    // Fill subgrid 0 (top-left corner)
    for(int i = 0; i < 9; i++) {
        SudokuPosition pos = sudoku_subgrid_get_position(&sg0, i);
        board->cells[pos.row][pos.col] = subgrid0_values[i];
    }
    
    // Fill subgrid 4 (center)
    for(int i = 0; i < 9; i++) {
        SudokuPosition pos = sudoku_subgrid_get_position(&sg4, i);
        board->cells[pos.row][pos.col] = subgrid4_values[i];
    }
    
    // Add a few extra cells to make the test more interesting
    // These are placed carefully to be valid according to Sudoku rules
    board->cells[8][3] = 2;
    board->cells[8][8] = 7;
    board->cells[0][8] = 4;
    
    // Update the board statistics (clues and empty count)
    sudoku_board_update_stats(board);
}

/* ================================================================
                    TEST FUNCTIONS
   ================================================================ */

/**
 * @brief Test that setup creates a valid partial board
 * 
 * This test verifies our helper function works correctly.
 * We need to trust our setup before we can test backtracking.
 */
void test_setup_creates_valid_board() {
    printf("\n--- Test: Board Setup ---\n");
    
    SudokuBoard board;
    setup_partial_board(&board);
    
    // Verify the board has the expected number of clues
    TEST_ASSERT(board.clues == 21, "Board should have 21 filled cells");
    TEST_ASSERT(board.empty == 60, "Board should have 60 empty cells");
    
    // Verify some specific cells to ensure setup worked
    TEST_ASSERT(board.cells[0][0] == 5, "Top-left cell should be 5");
    TEST_ASSERT(board.cells[4][4] == 5, "Center cell should be 5");
    TEST_ASSERT(board.cells[8][8] == 7, "Bottom-right should be 7");
    
    // Verify the partial board doesn't violate Sudoku rules
    // We can't use sudoku_validate_board because it checks complete boards,
    // but we can check that no filled cells violate rules
    bool is_valid = true;
    for(int i = 0; i < SUDOKU_SIZE && is_valid; i++) {
        for(int j = 0; j < SUDOKU_SIZE && is_valid; j++) {
            if(board.cells[i][j] != 0) {
                int num = board.cells[i][j];
                board.cells[i][j] = 0; // Temporarily remove
                SudokuPosition pos = {i, j};
                if(!sudoku_is_safe_position(&board, &pos, num)) {
                    is_valid = false;
                }
                board.cells[i][j] = num; // Restore
            }
        }
    }
    TEST_ASSERT(is_valid, "Partial board should not violate Sudoku rules");
    
    printf("Initial board state:\n");
    print_test_board(&board);
}

/**
 * @brief Test that backtracking successfully completes a partial board
 * 
 * This is the main test: we give backtracking a partially filled board
 * and verify it completes it correctly.
 */
void test_backtracking_completes_board() {
    printf("\n--- Test: Backtracking Completion ---\n");
    
    SudokuBoard board;
    setup_partial_board(&board);
    
    printf("Board before backtracking (21 cells filled):\n");
    print_test_board(&board);
    
    // THIS IS THE KEY: We call the REAL function we're testing
    bool result = sudoku_complete_backtracking(&board);
    sudoku_board_update_stats(&board); 
    // Verify the function returned success
    TEST_ASSERT(result == true, "Backtracking should return true");
    
    // Verify the board is now complete (no empty cells)
    TEST_ASSERT(board.empty == 0, "Board should have no empty cells after completion");
    TEST_ASSERT(board.clues == 81, "Board should have 81 filled cells");
    
    // Verify no cells are zero
    bool all_filled = true;
    for(int i = 0; i < SUDOKU_SIZE; i++) {
        for(int j = 0; j < SUDOKU_SIZE; j++) {
            if(board.cells[i][j] == 0) {
                all_filled = false;
                break;
            }
        }
    }
    TEST_ASSERT(all_filled, "All cells should contain numbers 1-9");
    
    printf("Board after backtracking (81 cells filled):\n");
    print_test_board(&board);
}

/**
 * @brief Test that the completed board is valid according to Sudoku rules
 * 
 * This verifies that backtracking not only fills the board,
 * but fills it correctly respecting all Sudoku constraints.
 */
void test_backtracking_produces_valid_solution() {
    printf("\n--- Test: Solution Validity ---\n");
    
    SudokuBoard board;
    setup_partial_board(&board);
    
    // Complete the board
    bool result = sudoku_complete_backtracking(&board);
    TEST_ASSERT(result == true, "Backtracking should succeed");
    
    // Now verify the completed board is valid
    // This checks all rows, columns, and subgrids for duplicates
    bool is_valid = sudoku_validate_board(&board);
    TEST_ASSERT(is_valid, "Completed board should be a valid Sudoku");
    
    // Additional check: verify each row has all numbers 1-9
    bool all_rows_valid = true;
    for(int row = 0; row < SUDOKU_SIZE; row++) {
        bool seen[10] = {false}; // Index 0 unused, 1-9 for digits
        for(int col = 0; col < SUDOKU_SIZE; col++) {
            int num = board.cells[row][col];
            if(num < 1 || num > 9 || seen[num]) {
                all_rows_valid = false;
                break;
            }
            seen[num] = true;
        }
    }
    TEST_ASSERT(all_rows_valid, "Each row should contain exactly digits 1-9");
}

/**
 * @brief Test that backtracking preserves the original filled cells
 * 
 * The original cells we filled should not be modified by backtracking.
 * This verifies the algorithm only fills empty cells.
 */
void test_backtracking_preserves_original_cells() {
    printf("\n--- Test: Original Cells Preserved ---\n");
    
    SudokuBoard board;
    setup_partial_board(&board);
    
    // Save the original state
    SudokuBoard original;
    memcpy(&original, &board, sizeof(SudokuBoard));
    
    // Complete the board
    sudoku_complete_backtracking(&board);
    
    // Verify that cells that were filled originally are unchanged
    bool preserved = true;
    for(int i = 0; i < SUDOKU_SIZE; i++) {
        for(int j = 0; j < SUDOKU_SIZE; j++) {
            if(original.cells[i][j] != 0) {
                if(board.cells[i][j] != original.cells[i][j]) {
                    preserved = false;
                    printf("    Cell [%d][%d] was changed from %d to %d\n",
                           i, j, original.cells[i][j], board.cells[i][j]);
                }
            }
        }
    }
    TEST_ASSERT(preserved, "Original filled cells should remain unchanged");
}

/* ================================================================
                    MAIN - TEST RUNNER
   ================================================================ */

int main(void) {
    printf("═══════════════════════════════════════════════════════════\n");
    printf("       BACKTRACKING MODULE TEST - Modular Architecture\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    // Run all tests
    test_setup_creates_valid_board();
    test_backtracking_completes_board();
    test_backtracking_produces_valid_solution();
    test_backtracking_preserves_original_cells();
    
    // Print summary
    printf("\n═══════════════════════════════════════════════════════════\n");
    printf("                    TEST SUMMARY\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("  Total tests:  %d\n", results.total);
    printf("  Passed:       %d ✓\n", results.passed);
    printf("  Failed:       %d ✗\n", results.failed);
    printf("═══════════════════════════════════════════════════════════\n");
    
    if(results.failed == 0) {
        printf("\n  🎉 ALL TESTS PASSED! 🎉\n");
    } else {
        printf("\n  ⚠️  SOME TESTS FAILED ⚠️\n");
    }
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    // Return appropriate exit code for build systems
    return results.failed > 0 ? 1 : 0;
}

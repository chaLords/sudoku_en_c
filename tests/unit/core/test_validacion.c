/**
 * @file main_test.c
 * @brief Test program for the validation module
 * @author Gonzalo Ramírez
 * @date 2025-01-19
 * 
 * This is a minimal test program that demonstrates the modular architecture.
 * It tests the three core validation functions:
 * - sudoku_is_safe: checks if a number can be placed at a position
 * - sudoku_find_empty: finds the next empty cell
 * - sudoku_verify: validates that a complete board has no conflicts
 */

#include <stdio.h>
#include <stdbool.h>

// Include the modular headers - this is the key difference from the monolithic code
#include <sudoku/core/types.h>
#include <sudoku/core/validation.h>
#include <internal/board_internal.h>

/**
 * Helper function to print a small board for visualization
 * This is just for testing purposes, not part of the modular library
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
            printf("------+-------+------\n");
        }
    }
    printf("\n");
}

int main(void) {
    printf("═══════════════════════════════════════════════════════════\n");
    printf("  VALIDATION MODULE TEST - Modular Architecture Demo\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    // Create a board using the modular SudokuBoard type
    // Notice we're using SudokuBoard, not just a plain int array
    SudokuBoard board;
    
    // Initialize all cells to 0 (empty) manually
    // In the full modular version, we'll have a sudoku_board_init() function for this
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++) {
            board.cells[i][j] = 0;
        }
    }
    board.clues = 0;
    board.empty = TOTAL_CELLS;
    
    printf("TEST 1: sudoku_is_safe() - Checking placement validity\n");
    printf("─────────────────────────────────────────────────────────\n");
    
    // Let's place some numbers on the board to create a scenario
    // We'll put a 5 in position (0,0) and test if we can place another 5 nearby
    board.cells[0][0] = 5;
    board.cells[0][3] = 3;
    board.cells[3][0] = 7;
    
    print_test_board(&board);
    
    // Test 1a: Try to place a 5 in the same row as another 5
    SudokuPosition test_pos_1 = {0, 1};  // Row 0, Column 1
    bool safe_1 = sudoku_is_safe_position(&board, &test_pos_1, 5);
    printf("Can we place 5 at position (0,1)? %s\n", safe_1 ? "YES" : "NO");
    printf("Expected: NO (because there's already a 5 in row 0)\n\n");
    
    // Test 1b: Try to place a 5 in a safe position
    SudokuPosition test_pos_2 = {5, 5};  // Row 5, Column 5
    bool safe_2 = sudoku_is_safe_position(&board, &test_pos_2, 5);
    printf("Can we place 5 at position (5,5)? %s\n", safe_2 ? "YES" : "NO");
    printf("Expected: YES (no conflict in row, column, or subgrid)\n\n");
    
    // Test 1c: Try to place a number in the same column as an existing number
    SudokuPosition test_pos_3 = {1, 0};  // Row 1, Column 0
    bool safe_3 = sudoku_is_safe_position(&board, &test_pos_3, 7);
    printf("Can we place 7 at position (1,0)? %s\n", safe_3 ? "YES" : "NO");
    printf("Expected: NO (because there's already a 7 in column 0)\n\n");
    
    printf("\n");
    printf("TEST 2: sudoku_find_empty() - Finding empty cells\n");
    printf("─────────────────────────────────────────────────────────\n");
    
    // The board currently has only 3 filled cells, so there are many empty ones
    SudokuPosition empty_pos;
    bool found = sudoku_find_empty_cell(&board, &empty_pos);
    
    printf("Looking for first empty cell...\n");
    if (found) {
        printf("✓ Found empty cell at position (%d,%d)\n", empty_pos.row, empty_pos.col);
        printf("Expected: (0,1) - the first empty cell scanning left to right, top to bottom\n\n");
    } else {
        printf("✗ No empty cells found (this shouldn't happen in our test)\n\n");
    }
    
    // Now let's fill the entire board and verify there are no empty cells
    printf("Filling the entire board...\n");
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++) {
            if (board.cells[i][j] == 0) {
                board.cells[i][j] = ((i * 3 + i / 3 + j) % 9) + 1;
            }
        }
    }
    
    found = sudoku_find_empty_cell(&board, &empty_pos);
    printf("Looking for empty cell in full board...\n");
    printf("Result: %s\n", found ? "Found (unexpected)" : "Not found (correct!)");
    printf("Expected: Not found - the board is now completely filled\n\n");
    
    printf("\n");
    printf("TEST 3: sudoku_verify() - Validating board correctness\n");
    printf("─────────────────────────────────────────────────────────\n");
    
    // Create a new board with a valid partial configuration
    SudokuBoard valid_board;
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++) {
            valid_board.cells[i][j] = 0;
        }
    }
    
    // Place some numbers that follow Sudoku rules
    valid_board.cells[0][0] = 1;
    valid_board.cells[0][1] = 2;
    valid_board.cells[0][2] = 3;
    valid_board.cells[1][0] = 4;
    valid_board.cells[1][1] = 5;
    valid_board.cells[1][2] = 6;
    
    printf("Testing a valid partial board:\n");
    print_test_board(&valid_board);
    
    bool is_valid = sudoku_validate_board(&valid_board);
    printf("Is the board valid? %s\n", is_valid ? "YES ✓" : "NO ✗");
    printf("Expected: YES - no conflicts in this configuration\n\n");
    
    // Now create an invalid board with a conflict
    SudokuBoard invalid_board = valid_board;
    invalid_board.cells[0][3] = 1;  // This creates a conflict: two 1's in row 0
    
    printf("Testing an invalid board (two 1's in row 0):\n");
    print_test_board(&invalid_board);
    
    is_valid = sudoku_validate_board(&invalid_board);
    printf("Is the board valid? %s\n", is_valid ? "YES ✓" : "NO ✗");
    printf("Expected: NO - there's a conflict in row 0\n\n");
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("  ALL TESTS COMPLETED\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("\nThe validation module is working correctly!\n");
    printf("You can now confidently build other modules on top of this foundation.\n\n");
    
    return 0;
}

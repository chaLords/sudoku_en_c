/**
 * @file test_phase2a.c
 * @brief Test suite for Phase 2A: Dynamic Memory Architecture
 * @author Gonzalo Ramírez
 * @date 2025-11-20
 * 
 * This test suite verifies that the transition from static to dynamic
 * memory allocation works correctly across all supported board sizes.
 * 
 * WHAT WE'RE TESTING:
 * - Memory allocation succeeds for valid sizes
 * - Memory allocation fails gracefully for invalid sizes
 * - Dimension tracking is accurate
 * - Cell access works with dynamic arrays
 * - Statistics update correctly
 * - Memory is properly freed (no leaks)
 * 
 * COMPILATION:
 *   gcc test_phase2a.c ../src/core/board.c -I../include -o test_phase2a -std=c11 -Wall -Wextra
 * 
 * RUN:
 *   ./test_phase2a
 * 
 * MEMORY LEAK CHECK (with Valgrind):
 *   valgrind --leak-check=full --show-leak-kinds=all ./test_phase2a
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sudoku/core/board.h"

// ═══════════════════════════════════════════════════════════════════
//                    TEST FRAMEWORK UTILITIES
// ═══════════════════════════════════════════════════════════════════

typedef struct {
    int passed;
    int failed;
    int total;
} TestResults;

#define TEST_START() TestResults results = {0, 0, 0}
#define TEST_END() return results

#define TEST_CASE(name) \
    printf("\n═══════════════════════════════════════════════════════════\n"); \
    printf("TEST: %s\n", name); \
    printf("═══════════════════════════════════════════════════════════\n"); \
    results.total++

#define ASSERT_TRUE(condition, message) \
    do { \
        if (condition) { \
            printf("  ✅ PASS: %s\n", message); \
            results.passed++; \
        } else { \
            printf("  ❌ FAIL: %s\n", message); \
            results.failed++; \
        } \
    } while(0)

#define ASSERT_FALSE(condition, message) \
    ASSERT_TRUE(!(condition), message)

#define ASSERT_EQUAL(actual, expected, message) \
    do { \
        if ((actual) == (expected)) { \
            printf("  ✅ PASS: %s (got %d)\n", message, actual); \
            results.passed++; \
        } else { \
            printf("  ❌ FAIL: %s (expected %d, got %d)\n", \
                   message, expected, actual); \
            results.failed++; \
        } \
    } while(0)

#define ASSERT_NULL(pointer, message) \
    do { \
        if ((pointer) == NULL) { \
            printf("  ✅ PASS: %s\n", message); \
            results.passed++; \
        } else { \
            printf("  ❌ FAIL: %s (expected NULL, got %p)\n", \
                   message, (void*)(pointer)); \
            results.failed++; \
        } \
    } while(0)

#define ASSERT_NOT_NULL(pointer, message) \
    do { \
        if ((pointer) != NULL) { \
            printf("  ✅ PASS: %s\n", message); \
            results.passed++; \
        } else { \
            printf("  ❌ FAIL: %s (got NULL)\n", message); \
            results.failed++; \
        } \
    } while(0)

// ═══════════════════════════════════════════════════════════════════
//                    TEST FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Test creation and destruction of 4×4 board
 */
TestResults test_create_4x4_board(void) {
    TEST_START();
    TEST_CASE("Create and Destroy 4×4 Board");
    
    SudokuBoard *board = sudoku_board_create_size(2);
    ASSERT_NOT_NULL(board, "Board created successfully");
    
    if (board != NULL) {
        ASSERT_EQUAL(sudoku_board_get_subgrid_size(board), 2, 
                    "Subgrid size is 2");
        ASSERT_EQUAL(sudoku_board_get_board_size(board), 4, 
                    "Board size is 4");
        ASSERT_EQUAL(sudoku_board_get_total_cells(board), 16, 
                    "Total cells is 16");
        ASSERT_EQUAL(sudoku_board_get_clues(board), 0, 
                    "Initial clues is 0");
        ASSERT_EQUAL(sudoku_board_get_empty(board), 16, 
                    "Initial empty is 16");
        
        sudoku_board_destroy(board);
        printf("  ✅ Board destroyed successfully\n");
    }
    
    TEST_END();
}

/**
 * @brief Test creation and destruction of 9×9 board (default)
 */
TestResults test_create_9x9_board(void) {
    TEST_START();
    TEST_CASE("Create and Destroy 9×9 Board (Default)");
    
    // Test both creation methods
    SudokuBoard *board1 = sudoku_board_create_size(3);
    ASSERT_NOT_NULL(board1, "Board created with create_size(3)");
    
    SudokuBoard *board2 = sudoku_board_create();
    ASSERT_NOT_NULL(board2, "Board created with create() default");
    
    if (board1 != NULL && board2 != NULL) {
        ASSERT_EQUAL(sudoku_board_get_board_size(board1),
                    sudoku_board_get_board_size(board2),
                    "Both methods create same size board");
    }
    
    if (board1 != NULL) {
        ASSERT_EQUAL(sudoku_board_get_subgrid_size(board1), 3, 
                    "Subgrid size is 3");
        ASSERT_EQUAL(sudoku_board_get_board_size(board1), 9, 
                    "Board size is 9");
        ASSERT_EQUAL(sudoku_board_get_total_cells(board1), 81, 
                    "Total cells is 81");
        sudoku_board_destroy(board1);
    }
    
    if (board2 != NULL) {
        sudoku_board_destroy(board2);
    }
    
    TEST_END();
}

/**
 * @brief Test creation and destruction of 16×16 board
 */
TestResults test_create_16x16_board(void) {
    TEST_START();
    TEST_CASE("Create and Destroy 16×16 Board");
    
    SudokuBoard *board = sudoku_board_create_size(4);
    ASSERT_NOT_NULL(board, "Board created successfully");
    
    if (board != NULL) {
        ASSERT_EQUAL(sudoku_board_get_subgrid_size(board), 4, 
                    "Subgrid size is 4");
        ASSERT_EQUAL(sudoku_board_get_board_size(board), 16, 
                    "Board size is 16");
        ASSERT_EQUAL(sudoku_board_get_total_cells(board), 256, 
                    "Total cells is 256");
        
        sudoku_board_destroy(board);
        printf("  ✅ Board destroyed successfully\n");
    }
    
    TEST_END();
}

/**
 * @brief Test creation and destruction of 25×25 board
 */
TestResults test_create_25x25_board(void) {
    TEST_START();
    TEST_CASE("Create and Destroy 25×25 Board");
    
    SudokuBoard *board = sudoku_board_create_size(5);
    ASSERT_NOT_NULL(board, "Board created successfully");
    
    if (board != NULL) {
        ASSERT_EQUAL(sudoku_board_get_subgrid_size(board), 5, 
                    "Subgrid size is 5");
        ASSERT_EQUAL(sudoku_board_get_board_size(board), 25, 
                    "Board size is 25");
        ASSERT_EQUAL(sudoku_board_get_total_cells(board), 625, 
                    "Total cells is 625");
        
        sudoku_board_destroy(board);
        printf("  ✅ Board destroyed successfully\n");
    }
    
    TEST_END();
}

/**
 * @brief Test rejection of invalid board sizes
 */
TestResults test_invalid_sizes(void) {
    TEST_START();
    TEST_CASE("Reject Invalid Board Sizes");
    
    // Too small
    SudokuBoard *too_small = sudoku_board_create_size(1);
    ASSERT_NULL(too_small, "Rejected subgrid_size=1 (too small)");
    
    // Too large
    SudokuBoard *too_large = sudoku_board_create_size(10);
    ASSERT_NULL(too_large, "Rejected subgrid_size=10 (too large)");
    
    // Zero
    SudokuBoard *zero = sudoku_board_create_size(0);
    ASSERT_NULL(zero, "Rejected subgrid_size=0");
    
    // Negative
    SudokuBoard *negative = sudoku_board_create_size(-3);
    ASSERT_NULL(negative, "Rejected subgrid_size=-3 (negative)");
    
    TEST_END();
}

/**
 * @brief Test cell set/get operations with dynamic memory
 */
TestResults test_cell_operations(void) {
    TEST_START();
    TEST_CASE("Cell Set/Get Operations");
    
    SudokuBoard *board = sudoku_board_create_size(3);
    ASSERT_NOT_NULL(board, "Board created");
    
    if (board != NULL) {
        // Test setting and getting various cells
        bool set_result;
        
        set_result = sudoku_board_set_cell(board, 0, 0, 5);
        ASSERT_TRUE(set_result, "Set cell (0,0) = 5");
        ASSERT_EQUAL(sudoku_board_get_cell(board, 0, 0), 5, 
                    "Get cell (0,0) returns 5");
        
        set_result = sudoku_board_set_cell(board, 8, 8, 9);
        ASSERT_TRUE(set_result, "Set cell (8,8) = 9");
        ASSERT_EQUAL(sudoku_board_get_cell(board, 8, 8), 9, 
                    "Get cell (8,8) returns 9");
        
        set_result = sudoku_board_set_cell(board, 4, 4, 1);
        ASSERT_TRUE(set_result, "Set cell (4,4) = 1");
        ASSERT_EQUAL(sudoku_board_get_cell(board, 4, 4), 1, 
                    "Get cell (4,4) returns 1");
        
        // Test boundary validation
        set_result = sudoku_board_set_cell(board, -1, 0, 5);
        ASSERT_FALSE(set_result, "Rejected negative row");
        
        set_result = sudoku_board_set_cell(board, 0, 20, 5);
        ASSERT_FALSE(set_result, "Rejected out-of-bounds column");
        
        set_result = sudoku_board_set_cell(board, 0, 0, 15);
        ASSERT_FALSE(set_result, "Rejected invalid value (> board_size)");
        
        sudoku_board_destroy(board);
    }
    
    TEST_END();
}

/**
 * @brief Test statistics update functionality
 */
TestResults test_statistics_update(void) {
    TEST_START();
    TEST_CASE("Statistics Update");
    
    SudokuBoard *board = sudoku_board_create_size(3);
    ASSERT_NOT_NULL(board, "Board created");
    
    if (board != NULL) {
        // Initially empty
        ASSERT_EQUAL(sudoku_board_get_clues(board), 0, 
                    "Initial clues = 0");
        ASSERT_EQUAL(sudoku_board_get_empty(board), 81, 
                    "Initial empty = 81");
        
        // Add some cells
        sudoku_board_set_cell(board, 0, 0, 1);
        sudoku_board_set_cell(board, 1, 1, 2);
        sudoku_board_set_cell(board, 2, 2, 3);
        sudoku_board_set_cell(board, 3, 3, 4);
        sudoku_board_set_cell(board, 4, 4, 5);
        
        // Update statistics
        sudoku_board_update_stats(board);
        
        ASSERT_EQUAL(sudoku_board_get_clues(board), 5, 
                    "Clues updated to 5");
        ASSERT_EQUAL(sudoku_board_get_empty(board), 76, 
                    "Empty updated to 76");
        
        // Verify invariant: clues + empty = total_cells
        int clues = sudoku_board_get_clues(board);
        int empty = sudoku_board_get_empty(board);
        int total = sudoku_board_get_total_cells(board);
        ASSERT_EQUAL(clues + empty, total, 
                    "Invariant: clues + empty = total_cells");
        
        sudoku_board_destroy(board);
    }
    
    TEST_END();
}

/**
 * @brief Test board initialization
 */
TestResults test_board_init(void) {
    TEST_START();
    TEST_CASE("Board Initialization");
    
    SudokuBoard *board = sudoku_board_create_size(3);
    ASSERT_NOT_NULL(board, "Board created");
    
    if (board != NULL) {
        // Add some values
        sudoku_board_set_cell(board, 0, 0, 5);
        sudoku_board_set_cell(board, 1, 1, 7);
        sudoku_board_update_stats(board);
        
        ASSERT_EQUAL(sudoku_board_get_clues(board), 2, 
                    "Board has 2 clues before init");
        
        // Re-initialize
        sudoku_board_init(board);
        
        ASSERT_EQUAL(sudoku_board_get_cell(board, 0, 0), 0, 
                    "Cell (0,0) cleared to 0");
        ASSERT_EQUAL(sudoku_board_get_cell(board, 1, 1), 0, 
                    "Cell (1,1) cleared to 0");
        ASSERT_EQUAL(sudoku_board_get_clues(board), 0, 
                    "Clues reset to 0");
        ASSERT_EQUAL(sudoku_board_get_empty(board), 81, 
                    "Empty reset to total_cells");
        
        sudoku_board_destroy(board);
    }
    
    TEST_END();
}

/**
 * @brief Test multiple boards simultaneously (memory independence)
 */
TestResults test_multiple_boards(void) {
    TEST_START();
    TEST_CASE("Multiple Boards Simultaneously");
    
    SudokuBoard *board4 = sudoku_board_create_size(2);
    SudokuBoard *board9 = sudoku_board_create_size(3);
    SudokuBoard *board16 = sudoku_board_create_size(4);
    
    ASSERT_NOT_NULL(board4, "4×4 board created");
    ASSERT_NOT_NULL(board9, "9×9 board created");
    ASSERT_NOT_NULL(board16, "16×16 board created");
    
    if (board4 != NULL && board9 != NULL && board16 != NULL) {
        // Set different values in each board
        sudoku_board_set_cell(board4, 0, 0, 1);
        sudoku_board_set_cell(board9, 0, 0, 5);
        sudoku_board_set_cell(board16, 0, 0, 9);
        
        // Verify values are independent
        ASSERT_EQUAL(sudoku_board_get_cell(board4, 0, 0), 1, 
                    "4×4 board has correct value");
        ASSERT_EQUAL(sudoku_board_get_cell(board9, 0, 0), 5, 
                    "9×9 board has correct value");
        ASSERT_EQUAL(sudoku_board_get_cell(board16, 0, 0), 9, 
                    "16×16 board has correct value");
        
        // Verify sizes are correct
        ASSERT_EQUAL(sudoku_board_get_board_size(board4), 4, 
                    "4×4 board has correct size");
        ASSERT_EQUAL(sudoku_board_get_board_size(board9), 9, 
                    "9×9 board has correct size");
        ASSERT_EQUAL(sudoku_board_get_board_size(board16), 16, 
                    "16×16 board has correct size");
    }
    
    // Clean up all boards
    if (board4 != NULL) sudoku_board_destroy(board4);
    if (board9 != NULL) sudoku_board_destroy(board9);
    if (board16 != NULL) sudoku_board_destroy(board16);
    printf("  ✅ All boards destroyed successfully\n");
    
    TEST_END();
}

// ═══════════════════════════════════════════════════════════════════
//                    MAIN TEST RUNNER
// ═══════════════════════════════════════════════════════════════════

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                                                           ║\n");
    printf("║     PHASE 2A: DYNAMIC MEMORY ARCHITECTURE TEST SUITE     ║\n");
    printf("║                                                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    
    TestResults total = {0, 0, 0};
    TestResults result;
    
    // Run all tests
    result = test_create_4x4_board();
    total.passed += result.passed;
    total.failed += result.failed;
    total.total += result.total;
    
    result = test_create_9x9_board();
    total.passed += result.passed;
    total.failed += result.failed;
    total.total += result.total;
    
    result = test_create_16x16_board();
    total.passed += result.passed;
    total.failed += result.failed;
    total.total += result.total;
    
    result = test_create_25x25_board();
    total.passed += result.passed;
    total.failed += result.failed;
    total.total += result.total;
    
    result = test_invalid_sizes();
    total.passed += result.passed;
    total.failed += result.failed;
    total.total += result.total;
    
    result = test_cell_operations();
    total.passed += result.passed;
    total.failed += result.failed;
    total.total += result.total;
    
    result = test_statistics_update();
    total.passed += result.passed;
    total.failed += result.failed;
    total.total += result.total;
    
    result = test_board_init();
    total.passed += result.passed;
    total.failed += result.failed;
    total.total += result.total;
    
    result = test_multiple_boards();
    total.passed += result.passed;
    total.failed += result.failed;
    total.total += result.total;
    
    // Print summary
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                     TEST SUMMARY                          ║\n");
    printf("╠═══════════════════════════════════════════════════════════╣\n");
    printf("║ Total Tests:  %-3d                                        ║\n", total.total);
    printf("║ Passed:       %-3d  ✅                                    ║\n", total.passed);
    printf("║ Failed:       %-3d  ❌                                    ║\n", total.failed);
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    
    if (total.failed == 0) {
        printf("\n🎉 ¡ÉXITO! Todos los tests de Phase 2A pasaron.\n\n");
        printf("✅ La arquitectura de memoria dinámica funciona correctamente\n");
        printf("✅ Todos los tamaños de tablero (4×4, 9×9, 16×16, 25×25) soportados\n");
        printf("✅ No hay memory leaks (verificar con Valgrind para confirmar)\n\n");
        printf("📋 Próximos pasos:\n");
        printf("   1. Ejecutar: valgrind --leak-check=full ./test_phase2a\n");
        printf("   2. Verificar que no hay memory leaks\n");
        printf("   3. Commit: git commit -m \"feat(board): implement dynamic memory - Phase 2A\"\n");
        printf("   4. Proceder a Phase 2B: Adaptar validation.c y backtracking.c\n\n");
        return 0;
    } else {
        printf("\n⚠️  %d test(s) fallaron. Revisar implementación.\n\n", total.failed);
        return 1;
    }
}

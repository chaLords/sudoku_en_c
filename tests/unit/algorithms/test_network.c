/**
 * @file test_network.c
 * @brief Unit tests for constraint network
 * @author Gonzalo Ramírez
 * @date 2025-11
 * 
 * UBICACIÓN: tests/unit/algorithms/test_network.c
 * 
 * ARQUITECTURA v3.0:
 *   - constraint_network_create(): Solo crea estructura de datos
 *   - ac3_enforce_consistency(): Propaga restricciones
 * 
 * COMPILAR:
 *   cd build && cmake .. && make test_network
 * 
 * EJECUTAR:
 *   ./test_network
 *   valgrind --leak-check=full ./test_network
 */

#include "sudoku/algorithms/network.h"
#include "sudoku/algorithms/ac3.h"
#include "sudoku/core/board.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

// ═══════════════════════════════════════════════════════════════════
//                    TEST UTILITIES
// ═══════════════════════════════════════════════════════════════════

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) \
    printf("\n[TEST] %s\n", name); \
    tests_run++;

#define ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf("  ❌ FAILED: %s\n", message); \
            printf("     at line %d\n", __LINE__); \
            return; \
        } \
    } while(0)

#define PASS() \
    do { \
        printf("  ✓ PASSED\n"); \
        tests_passed++; \
    } while(0)

// ═══════════════════════════════════════════════════════════════════
//                    BASIC NETWORK TESTS
// ═══════════════════════════════════════════════════════════════════

void test_network_create_destroy(void) {
    TEST("Network create and destroy");
    
    // Create 9×9 board
    SudokuBoard *board = sudoku_board_create();
    ASSERT(board != NULL, "Board creation failed");
    
    // Create network
    ConstraintNetwork *net = constraint_network_create(board);
    ASSERT(net != NULL, "Network creation failed");
    
    // Verify basic properties
    int size = constraint_network_get_board_size(net);
    ASSERT(size == 9, "Board size should be 9");
    
    // Clean up
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

void test_empty_board_domains(void) {
    TEST("Empty board has full domains");
    
    SudokuBoard *board = sudoku_board_create();
    ConstraintNetwork *net = constraint_network_create(board);
    
    // All cells should have domain {1..9}
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            int size = constraint_network_domain_size(net, r, c);
            ASSERT(size == 9, "Empty cell should have full domain");
            
            // Check all values present
            for (int v = 1; v <= 9; v++) {
                bool has = constraint_network_has_value(net, r, c, v);
                ASSERT(has, "Empty cell should have all values");
            }
        }
    }
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

void test_filled_cell_singleton(void) {
    TEST("Filled cell has singleton domain");
    
    SudokuBoard *board = sudoku_board_create();
    
    // Place value 5 at (3,4)
    sudoku_board_set_cell(board, 3, 4, 5);
    
    ConstraintNetwork *net = constraint_network_create(board);
    
    // Check that cell has singleton domain {5}
    int size = constraint_network_domain_size(net, 3, 4);
    ASSERT(size == 1, "Filled cell should have singleton domain");
    
    bool has_5 = constraint_network_has_value(net, 3, 4, 5);
    ASSERT(has_5, "Filled cell should contain its value");
    
    bool has_7 = constraint_network_has_value(net, 3, 4, 7);
    ASSERT(!has_7, "Filled cell should not contain other values");
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

/**
 * Test constraint propagation using AC-3 algorithm
 * 
 * ARQUITECTURA v3.0:
 *   - constraint_network_create() solo crea la estructura
 *   - ac3_enforce_consistency() propaga las restricciones
 * 
 * Esto sigue el Single Responsibility Principle (SRP):
 *   - Network: estructura de datos
 *   - AC-3: propagación de restricciones
 */
void test_constraint_propagation(void) {
    TEST("Constraint propagation with AC-3");
    
    SudokuBoard *board = sudoku_board_create();
    
    // Place 5 at (0,0)
    sudoku_board_set_cell(board, 0, 0, 5);
    
    ConstraintNetwork *net = constraint_network_create(board);
    ASSERT(net != NULL, "Network creation failed");
    
    // ═══════════════════════════════════════════════════════════════
    // Propagar restricciones con AC-3 (arquitectura v3.0)
    // ═══════════════════════════════════════════════════════════════
    AC3Statistics stats;
    bool consistent = ac3_enforce_consistency(net, &stats);
    ASSERT(consistent, "Network should be arc consistent");
    
    printf("  📊 AC-3 stats: %d revisions, %d propagations\n", 
           stats.revisions, stats.propagations);
    
    // All cells in row 0 should not have 5 in domain
    for (int c = 1; c < 9; c++) {
        bool has_5 = constraint_network_has_value(net, 0, c, 5);
        ASSERT(!has_5, "Row neighbor should not have conflicting value");
    }
    
    // All cells in column 0 should not have 5 in domain
    for (int r = 1; r < 9; r++) {
        bool has_5 = constraint_network_has_value(net, r, 0, 5);
        ASSERT(!has_5, "Column neighbor should not have conflicting value");
    }
    
    // All cells in top-left subgrid should not have 5 in domain
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            if (r == 0 && c == 0) continue;  // Skip the filled cell
            
            bool has_5 = constraint_network_has_value(net, r, c, 5);
            ASSERT(!has_5, "Subgrid neighbor should not have conflicting value");
        }
    }
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

// ═══════════════════════════════════════════════════════════════════
//                    DOMAIN MANIPULATION TESTS
// ═══════════════════════════════════════════════════════════════════

void test_remove_value(void) {
    TEST("Remove value from domain");
    
    SudokuBoard *board = sudoku_board_create();
    ConstraintNetwork *net = constraint_network_create(board);
    
    // Initially cell (5,5) has all values
    int initial_size = constraint_network_domain_size(net, 5, 5);
    ASSERT(initial_size == 9, "Should start with full domain");
    
    // Remove value 7
    bool removed = constraint_network_remove_value(net, 5, 5, 7);
    ASSERT(removed, "Value should be removed");
    
    // Check size decreased
    int new_size = constraint_network_domain_size(net, 5, 5);
    ASSERT(new_size == 8, "Domain size should decrease by 1");
    
    // Check 7 no longer in domain
    bool has_7 = constraint_network_has_value(net, 5, 5, 7);
    ASSERT(!has_7, "Removed value should not be in domain");
    
    // Try removing again (should return false)
    removed = constraint_network_remove_value(net, 5, 5, 7);
    ASSERT(!removed, "Removing again should return false");
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

void test_assign_value(void) {
    TEST("Assign value to create singleton domain");
    
    SudokuBoard *board = sudoku_board_create();
    ConstraintNetwork *net = constraint_network_create(board);
    
    // Assign value 3 to cell (4,4)
    constraint_network_assign_value(net, 4, 4, 3);
    
    // Check singleton domain
    int size = constraint_network_domain_size(net, 4, 4);
    ASSERT(size == 1, "Assigned cell should have singleton domain");
    
    bool has_3 = constraint_network_has_value(net, 4, 4, 3);
    ASSERT(has_3, "Assigned value should be in domain");
    
    bool has_5 = constraint_network_has_value(net, 4, 4, 5);
    ASSERT(!has_5, "Other values should not be in domain");
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

void test_restore_domain(void) {
    TEST("Restore full domain");
    
    SudokuBoard *board = sudoku_board_create();
    ConstraintNetwork *net = constraint_network_create(board);
    
    // Remove some values
    constraint_network_remove_value(net, 2, 3, 5);
    constraint_network_remove_value(net, 2, 3, 7);
    constraint_network_remove_value(net, 2, 3, 9);
    
    int reduced_size = constraint_network_domain_size(net, 2, 3);
    ASSERT(reduced_size == 6, "Domain should be reduced");
    
    // Restore
    constraint_network_restore_domain(net, 2, 3);
    
    int restored_size = constraint_network_domain_size(net, 2, 3);
    ASSERT(restored_size == 9, "Domain should be fully restored");
    
    bool has_7 = constraint_network_has_value(net, 2, 3, 7);
    ASSERT(has_7, "Previously removed value should be back");
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

// ═══════════════════════════════════════════════════════════════════
//                    NEIGHBOR TESTS
// ═══════════════════════════════════════════════════════════════════

void test_neighbors_count(void) {
    TEST("Verify neighbor counts");
    
    SudokuBoard *board = sudoku_board_create();
    ConstraintNetwork *net = constraint_network_create(board);
    
    // For 9×9: typically 20 neighbors (8 row + 8 col + 4 subgrid)
    int count;
    constraint_network_get_neighbors(net, 4, 4, &count);
    
    ASSERT(count == 20, "Center cell should have 20 neighbors");
    
    // Corner cell also has 20 neighbors
    constraint_network_get_neighbors(net, 0, 0, &count);
    ASSERT(count == 20, "Corner cell should have 20 neighbors");
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

void test_neighbors_unique(void) {
    TEST("Neighbors list has no duplicates");
    
    SudokuBoard *board = sudoku_board_create();
    ConstraintNetwork *net = constraint_network_create(board);
    
    int count;
    const SudokuPosition *neighbors = 
        constraint_network_get_neighbors(net, 4, 4, &count);
    
    // Check no duplicates
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            bool duplicate = (neighbors[i].row == neighbors[j].row &&
                            neighbors[i].col == neighbors[j].col);
            ASSERT(!duplicate, "Neighbors should be unique");
        }
    }
    
    // Check self not included
    for (int i = 0; i < count; i++) {
        bool is_self = (neighbors[i].row == 4 && neighbors[i].col == 4);
        ASSERT(!is_self, "Cell should not be its own neighbor");
    }
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

// ═══════════════════════════════════════════════════════════════════
//                    ADVANCED TESTS
// ═══════════════════════════════════════════════════════════════════

void test_domain_empty_detection(void) {
    TEST("Detect empty domain");
    
    SudokuBoard *board = sudoku_board_create();
    ConstraintNetwork *net = constraint_network_create(board);
    
    // Remove all values from a cell
    for (int v = 1; v <= 9; v++) {
        constraint_network_remove_value(net, 3, 3, v);
    }
    
    bool is_empty = constraint_network_domain_empty(net, 3, 3);
    ASSERT(is_empty, "Domain should be empty");
    
    int size = constraint_network_domain_size(net, 3, 3);
    ASSERT(size == 0, "Empty domain should have size 0");
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

void test_total_possibilities(void) {
    TEST("Count total possibilities");
    
    SudokuBoard *board = sudoku_board_create();
    
    // Empty 9×9 board: 81 cells × 9 values = 729 possibilities
    ConstraintNetwork *net1 = constraint_network_create(board);
    int total1 = constraint_network_total_possibilities(net1);
    ASSERT(total1 == 729, "Empty board should have 729 possibilities");
    constraint_network_destroy(net1);
    
    // Fill one cell: singleton domain reduces count by 8
    // (from 9 to 1 for that cell)
    sudoku_board_set_cell(board, 0, 0, 5);
    ConstraintNetwork *net2 = constraint_network_create(board);
    int total2 = constraint_network_total_possibilities(net2);
    
    // Without AC-3 propagation: 1 + 80*9 = 721
    // The filled cell has domain {5} = 1 value
    ASSERT(total2 == 721, "Filled cell should reduce total to 721");
    
    constraint_network_destroy(net2);
    sudoku_board_destroy(board);
    
    PASS();
}

void test_4x4_board(void) {
    TEST("Network works with 4×4 board");
    
    // Create 4×4 board (subgrid_size = 2)
    SudokuBoard *board = sudoku_board_create_size(2);
    ASSERT(board != NULL, "4×4 board creation failed");
    
    ConstraintNetwork *net = constraint_network_create(board);
    ASSERT(net != NULL, "Network creation for 4×4 failed");
    
    int size = constraint_network_get_board_size(net);
    ASSERT(size == 4, "Board size should be 4");
    
    // Check domain sizes for empty board
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            int domain_size = constraint_network_domain_size(net, r, c);
            ASSERT(domain_size == 4, "4×4 cells should have 4 possibilities");
        }
    }
    
    // For 4×4: each cell has 7 neighbors (3 row + 3 col + 1 subgrid)
    int count;
    constraint_network_get_neighbors(net, 0, 0, &count);
    ASSERT(count == 7, "4×4 corner cell should have 7 neighbors");
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

// ═══════════════════════════════════════════════════════════════════
//                    MAIN TEST RUNNER
// ═══════════════════════════════════════════════════════════════════

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║   CONSTRAINT NETWORK UNIT TESTS                       ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    
    // Basic tests
    test_network_create_destroy();
    test_empty_board_domains();
    test_filled_cell_singleton();
    test_constraint_propagation();
    
    // Domain manipulation
    test_remove_value();
    test_assign_value();
    test_restore_domain();
    
    // Neighbor tests
    test_neighbors_count();
    test_neighbors_unique();
    
    // Advanced tests
    test_domain_empty_detection();
    test_total_possibilities();
    test_4x4_board();
    
    // Summary
    printf("\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("  TEST RESULTS: %d/%d passed\n", tests_passed, tests_run);
    printf("═══════════════════════════════════════════════════════\n");
    
    if (tests_passed == tests_run) {
        printf("✅ ALL TESTS PASSED!\n\n");
        return 0;
    } else {
        printf("❌ SOME TESTS FAILED\n\n");
        return 1;
    }
}

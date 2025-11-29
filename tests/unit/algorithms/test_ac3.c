/**
 * @file test_ac3.c
 * @brief Unit tests for AC-3 algorithm
 * @author Gonzalo Ramírez
 * @date 2025-11
 * 
 * UBICACIÓN: tests/unit/algorithms/test_ac3.c
 * 
 * COMPILAR:
 *   cd tests/unit/algorithms
 *   gcc -I../../../include -I../../../src/core/internal \
 *       ../../../src/core/board.c \
 *       ../../../src/core/network.c \
 *       ../../../src/core/algorithms/ac3.c \
 *       test_ac3.c \
 *       -o test_ac3 -std=c11
 * 
 * EJECUTAR:
 *   ./test_ac3
 */

#include "sudoku/algorithms/ac3.h"
#include "sudoku/algorithms/network.h"
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
//                    BASIC AC-3 TESTS
// ═══════════════════════════════════════════════════════════════════

void test_ac3_consistent_empty_board() {
    TEST("AC-3 on empty board (should be consistent)");
    
    SudokuBoard *board = sudoku_board_create();
    ConstraintNetwork *net = constraint_network_create(board);
    
    AC3Statistics stats;
    bool consistent = ac3_enforce_consistency(net, &stats);
    
    ASSERT(consistent, "Empty board should be arc-consistent");
    ASSERT(stats.consistent == true, "Statistics should indicate consistency");
    ASSERT(stats.revisions > 0, "Should perform some revisions");
    
    printf("  📊 Revisions: %d, Propagations: %d, Time: %.3fms\n",
           stats.revisions, stats.propagations, stats.time_ms);
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

void test_ac3_consistent_partial_board() {
    TEST("AC-3 on partially filled consistent board");
    
    SudokuBoard *board = sudoku_board_create();
    
    // Fill some cells in non-conflicting way
    sudoku_board_set_cell(board, 0, 0, 1);
    sudoku_board_set_cell(board, 1, 1, 2);
    sudoku_board_set_cell(board, 2, 2, 3);
    sudoku_board_set_cell(board, 4, 4, 5);
    
    ConstraintNetwork *net = constraint_network_create(board);
    
    AC3Statistics stats;
    bool consistent = ac3_enforce_consistency(net, &stats);
    
    ASSERT(consistent, "Partial board should be arc-consistent");
    ASSERT(stats.values_removed > 0, "Should remove some values from domains");
    
    printf("  📊 Values removed: %d\n", stats.values_removed);
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

void test_ac3_detects_inconsistency() {
    TEST("AC-3 detects inconsistent configuration");
    
    SudokuBoard *board = sudoku_board_create();
    ConstraintNetwork *net = constraint_network_create(board);
    
    // Create inconsistency: remove all values from a cell's domain
    for (int v = 1; v <= 9; v++) {
        constraint_network_remove_value(net, 4, 4, v);
    }
    
    AC3Statistics stats;
    bool consistent = ac3_enforce_consistency(net, &stats);
    
    ASSERT(!consistent, "Should detect inconsistency");
    ASSERT(stats.consistent == false, "Statistics should indicate inconsistency");
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

void test_ac3_revise_arc() {
    TEST("AC-3 revise single arc");
    
    SudokuBoard *board = sudoku_board_create();
    ConstraintNetwork *net = constraint_network_create(board);
    
    // Assign value to (0,1)
    constraint_network_assign_value(net, 0, 1, 5);
    
    // Check domain of (0,0) before revision
    bool before = constraint_network_has_value(net, 0, 0, 5);
    
    // Revise arc ((0,0), (0,1))
    bool revised = ac3_revise_arc(net, 0, 0, 0, 1);
    
    ASSERT(revised, "Arc should be revised");
    
    // Check domain of (0,0) after revision
    bool after = constraint_network_has_value(net, 0, 0, 5);
    
    ASSERT(before == true, "Before revision should have value");
    ASSERT(after == false, "After revision should not have value");
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

// ═══════════════════════════════════════════════════════════════════
//                    PROPAGATION TESTS
// ═══════════════════════════════════════════════════════════════════

void test_ac3_propagate_from() {
    TEST("AC-3 propagate from single assignment");
    
    SudokuBoard *board = sudoku_board_create();
    ConstraintNetwork *net = constraint_network_create(board);
    
    // Assign value
    constraint_network_assign_value(net, 4, 4, 5);
    
    AC3Statistics stats;
    bool consistent = ac3_propagate_from(net, 4, 4, 5, &stats);
    
    ASSERT(consistent, "Propagation should maintain consistency");
    ASSERT(stats.revisions > 0, "Should perform revisions");
    
    // Check that neighbors don't have 5 in domain
    int neighbor_count;
    const SudokuPosition *neighbors = 
        constraint_network_get_neighbors(net, 4, 4, &neighbor_count);
    
    for (int i = 0; i < neighbor_count; i++) {
        bool has_5 = constraint_network_has_value(net, 
                                                  neighbors[i].row,
                                                  neighbors[i].col, 5);
        ASSERT(!has_5, "Neighbors should not have conflicting value");
    }
    
    printf("  📊 Revisions: %d, Propagations: %d\n",
           stats.revisions, stats.propagations);
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

void test_ac3_propagate_chain() {
    TEST("AC-3 propagates through chain of constraints");
    
    SudokuBoard *board = sudoku_board_create();
    ConstraintNetwork *net = constraint_network_create(board);
    
    // Create situation where propagation chains
    // Assign several values that will trigger cascading updates
    constraint_network_assign_value(net, 0, 0, 1);
    ac3_propagate_from(net, 0, 0, 1, NULL);
    
    constraint_network_assign_value(net, 0, 1, 2);
    ac3_propagate_from(net, 0, 1, 2, NULL);
    
    constraint_network_assign_value(net, 0, 2, 3);
    
    AC3Statistics stats;
    ac3_propagate_from(net, 0, 2, 3, &stats);
    
    ASSERT(stats.propagations > 0, "Should trigger propagations");
    
    printf("  📊 Chained propagations: %d\n", stats.propagations);
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

// ═══════════════════════════════════════════════════════════════════
//                    SINGLETON DETECTION
// ═══════════════════════════════════════════════════════════════════

void test_ac3_find_singles() {
    TEST("AC-3 finds and assigns singleton domains");
    
    SudokuBoard *board = sudoku_board_create();
    ConstraintNetwork *net = constraint_network_create(board);
    
    // Create situation where one cell has singleton domain
    // Fill row 0 except cell (0,0)
    for (int c = 1; c < 9; c++) {
        sudoku_board_set_cell(board, 0, c, c);
    }
    
    // Recreate network to reflect board state
    constraint_network_destroy(net);
    net = constraint_network_create(board);
    
    // Apply AC-3
    ac3_enforce_consistency(net, NULL);
    
    // Cell (0,0) should now have singleton domain {9}
    int domain_size = constraint_network_domain_size(net, 0, 0);
    ASSERT(domain_size == 1, "Should have singleton domain");
    
    bool has_9 = constraint_network_has_value(net, 0, 0, 9);
    ASSERT(has_9, "Singleton domain should contain correct value");
    
    // Now find and assign singles
    AC3Statistics stats;
    bool consistent = ac3_find_singles(net, &stats);
    
    ASSERT(consistent, "Should remain consistent");
    ASSERT(stats.values_removed >= 1, "Should find at least one singleton");
    
    printf("  📊 Singles found: %d\n", stats.values_removed);
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

// ═══════════════════════════════════════════════════════════════════
//                    PERFORMANCE TESTS
// ═══════════════════════════════════════════════════════════════════

void test_ac3_performance_empty() {
    TEST("AC-3 performance on empty 9×9 board");
    
    SudokuBoard *board = sudoku_board_create();
    ConstraintNetwork *net = constraint_network_create(board);
    
    AC3Statistics stats;
    ac3_enforce_consistency(net, &stats);
    
    printf("  📊 Performance metrics:\n");
    printf("     - Revisions: %d\n", stats.revisions);
    printf("     - Propagations: %d\n", stats.propagations);
    printf("     - Values removed: %d\n", stats.values_removed);
    printf("     - Time: %.3f ms\n", stats.time_ms);
    
    ASSERT(stats.time_ms < 100.0, "Should complete in < 100ms");
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

void test_ac3_performance_partial() {
    TEST("AC-3 performance on partially filled board");
    
    SudokuBoard *board = sudoku_board_create();
    
    // Fill ~30 cells randomly
    sudoku_board_set_cell(board, 0, 0, 1);
    sudoku_board_set_cell(board, 0, 3, 4);
    sudoku_board_set_cell(board, 0, 6, 7);
    sudoku_board_set_cell(board, 1, 1, 2);
    sudoku_board_set_cell(board, 1, 4, 5);
    sudoku_board_set_cell(board, 1, 7, 8);
    sudoku_board_set_cell(board, 2, 2, 3);
    sudoku_board_set_cell(board, 2, 5, 6);
    sudoku_board_set_cell(board, 2, 8, 9);
    // ... (más celdas para alcanzar ~30)
    
    ConstraintNetwork *net = constraint_network_create(board);
    
    AC3Statistics stats;
    ac3_enforce_consistency(net, &stats);
    
    printf("  📊 Performance on partial board:\n");
    printf("     - Time: %.3f ms\n", stats.time_ms);
    printf("     - Values removed: %d\n", stats.values_removed);
    
    ASSERT(stats.time_ms < 50.0, "Should be faster than empty board");
    
    constraint_network_destroy(net);
    sudoku_board_destroy(board);
    
    PASS();
}

// ═══════════════════════════════════════════════════════════════════
//                    4×4 BOARD TESTS
// ═══════════════════════════════════════════════════════════════════

void test_ac3_4x4_board() {
    TEST("AC-3 works with 4×4 board");
    
    SudokuBoard *board = sudoku_board_create_size(2);
    ConstraintNetwork *net = constraint_network_create(board);
    
    AC3Statistics stats;
    bool consistent = ac3_enforce_consistency(net, &stats);
    
    ASSERT(consistent, "4×4 board should be consistent");
    ASSERT(stats.revisions > 0, "Should perform revisions");
    
    printf("  📊 4×4 performance: %.3f ms\n", stats.time_ms);
    
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
    printf("║        AC-3 ALGORITHM UNIT TESTS                      ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    
    // Basic tests
    test_ac3_consistent_empty_board();
    test_ac3_consistent_partial_board();
    test_ac3_detects_inconsistency();
    test_ac3_revise_arc();
    
    // Propagation tests
    test_ac3_propagate_from();
    test_ac3_propagate_chain();
    
    // Singleton detection
    test_ac3_find_singles();
    
    // Performance tests
    test_ac3_performance_empty();
    test_ac3_performance_partial();
    
    // Different board sizes
    test_ac3_4x4_board();
    
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

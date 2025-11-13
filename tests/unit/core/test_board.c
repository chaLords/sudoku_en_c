#include <stdio.h>
#include <stdbool.h>
#include "sudoku/core/board.h"
#include "sudoku/core/types.h"

/* ================================================================
                   FUNCIONES AUXILIARES DE TEST
   ================================================================ */

/**
 * @brief Imprime el tablero en formato legible
 */
void print_test_board(const SudokuBoard *board) {
    printf("\nCurrent board state:\n");
    for(int i = 0; i < SUDOKU_SIZE; i++) {
        for(int j = 0; j < SUDOKU_SIZE; j++) {
            if(sudoku_board_get_cell(board, i, j) == 0) {
                printf(". ");
            } else {
                printf("%d ", sudoku_board_get_cell(board, i, j));
            }
            if((j + 1) % 3 == 0 && j < 8) printf("| ");
        }
        printf("\n");
        if((i + 1) % 3 == 0 && i < 8) {
            printf("------+-------+------\n");
        }
    }
    printf("\n");
}

/**
 * @brief Contador de tests
 */
typedef struct {
    int passed;
    int failed;
    int total;
} TestResults;

TestResults results = {0, 0, 0};

/**
 * @brief Macro para assertions
 */
#define TEST_ASSERT(condition, message) do { \
    results.total++; \
    if(condition) { \
        printf("  [PASS] %s\n", message); \
        results.passed++; \
    } else { \
        printf("  [FAIL] %s\n", message); \
        results.failed++; \
    } \
} while(0)

/* ================================================================
                        TESTS DE board.h
   ================================================================ */

/**
 * @brief Test 1: sudoku_board_init()
 */
void test_board_init(void) {
    printf("\n===============================================================\n");
    printf("TEST 1: sudoku_board_init() - Board Initialization\n");
    printf("===============================================================\n");
    
    SudokuBoard *board = sudoku_board_create();
if (board == NULL) {
    printf("  [FAIL] Could not create board\n");
    return;
}    
    /* Verificar que todas las celdas estan en 0 */
    bool all_zeros = true;
    for(int i = 0; i < SUDOKU_SIZE; i++) {
        for(int j = 0; j < SUDOKU_SIZE; j++) {
            if(sudoku_board_get_cell(board, i, j) != 0) {
                all_zeros = false;
                break;
            }
        }
    }
    
    TEST_ASSERT(all_zeros, "All cells initialized to 0");
    TEST_ASSERT(sudoku_board_get_clues(board) == 0, "Clues counter is 0");
    TEST_ASSERT(sudoku_board_get_empty(board) == TOTAL_CELLS, "Empty counter is 81");
    
    print_test_board(board);
    sudoku_board_destroy(board);
}

/**
 * @brief Test 2: sudoku_board_update_stats()
 */
void test_board_update_stats(void) {
    printf("\n===============================================================\n");
    printf("TEST 2: sudoku_board_update_stats() - Stats Update\n");
    printf("===============================================================\n");
    
  SudokuBoard *board = sudoku_board_create();
if (board == NULL) {
    printf("  [FAIL] Could not create board\n");
    return;
}    
    /* Colocar algunos numeros */
    sudoku_board_set_cell(board, 0, 0, 5);
    sudoku_board_set_cell(board, 0, 1, 3);
    sudoku_board_set_cell(board, 0, 2, 7);
    sudoku_board_set_cell(board, 4, 4, 9);    
    /* Actualizar estadisticas */
    sudoku_board_update_stats(board);
    
    TEST_ASSERT(sudoku_board_get_clues(board) == 4, "Clues counter is 4");
    TEST_ASSERT(sudoku_board_get_empty(board) == 77, "Empty counter is 77 (81-4)");    
    
    printf("  Board after placing 4 numbers:\n");
    printf("  Clues: %d, Empty: %d\n", sudoku_board_get_clues(board), sudoku_board_get_empty(board));
    print_test_board(board);
    sudoku_board_destroy(board);
}

/**
 * @brief Test 3: Crear subcuadricula
 */
void test_subgrid_create(void) {
    printf("\n===============================================================\n");
    printf("TEST 3: sudoku_subgrid_create() - SubGrid Creation\n");
    printf("===============================================================\n");
    
    /* Probar todas las subcuadriculas (0-8) */
    for(int idx = 0; idx < 9; idx++) {
        SudokuSubGrid sg = sudoku_subgrid_create(idx);
        
        int expected_row = (idx / 3) * 3;
        int expected_col = (idx % 3) * 3;
        
        bool correct_base = (sg.base.row == expected_row && 
                            sg.base.col == expected_col);
        
        printf("  SubGrid %d: base=(%d,%d) index=%d ", 
               idx, sg.base.row, sg.base.col, sg.index);
        
        if(correct_base && sg.index == idx) {
            printf("[OK]\n");
            results.passed++;
        } else {
            printf("[ERROR]\n");
            results.failed++;
        }
        results.total++;
    }
}

/**
 * @brief Test 4: Obtener posicion dentro de subcuadricula
 */
void test_subgrid_get_position(void) {
    printf("\n===============================================================\n");
    printf("TEST 4: sudoku_subgrid_get_position() - Position Mapping\n");
    printf("===============================================================\n");
    
    SudokuSubGrid sg = sudoku_subgrid_create(4);  /* Centro (base: 3,3) */
    
    /* Test: cell_index 0 debe dar posicion (3,3) */
    SudokuPosition pos0 = sudoku_subgrid_get_position(&sg, 0);
    TEST_ASSERT(pos0.row == 3 && pos0.col == 3, 
                "Cell 0 in subgrid 4 is at (3,3)");
    
    /* Test: cell_index 4 debe dar posicion (4,4) - centro del tablero */
    SudokuPosition pos4 = sudoku_subgrid_get_position(&sg, 4);
    TEST_ASSERT(pos4.row == 4 && pos4.col == 4, 
                "Cell 4 in subgrid 4 is at (4,4)");
    
    /* Test: cell_index 8 debe dar posicion (5,5) */
    SudokuPosition pos8 = sudoku_subgrid_get_position(&sg, 8);
    TEST_ASSERT(pos8.row == 5 && pos8.col == 5, 
                "Cell 8 in subgrid 4 is at (5,5)");
    
    printf("  Mapping visualization for SubGrid 4:\n");
    printf("  Cell 0->(3,3), Cell 4->(4,4), Cell 8->(5,5)\n");
}

/**
 * @brief Test 5: Llenar subcuadricula con numeros
 */
void test_subgrid_fill(void) {
    printf("\n===============================================================\n");
    printf("TEST 5: Filling SubGrid - Manual Test\n");
    printf("===============================================================\n");
    
    SudokuBoard *board = sudoku_board_create();
    if (board == NULL) {
    printf("  [FAIL] Could not create board\n");
    return;
    }    
    SudokuSubGrid sg = sudoku_subgrid_create(0);  /* Top-left (0,0) */
    
    /* Llenar manualmente con numeros 1-9 */
    int numbers[9] = {5, 3, 7, 6, 2, 1, 9, 8, 4};
    for(int i = 0; i < 9; i++) {
        SudokuPosition pos = sudoku_subgrid_get_position(&sg, i);
        sudoku_board_set_cell(board, pos.row, pos.col, numbers[i]);    }
    
    /* Verificar que se lleno correctamente */
    bool filled_correctly = true;
    for(int i = 0; i < 9; i++) {
        SudokuPosition pos = sudoku_subgrid_get_position(&sg, i);
        if(sudoku_board_get_cell(board, pos.row, pos.col) != numbers[i]){
            filled_correctly = false;
            break;
        }
    }
    
    TEST_ASSERT(filled_correctly, "SubGrid 0 filled with correct numbers");
    
    printf("  SubGrid 0 contents:\n");
    printf("  %d %d %d\n", sudoku_board_get_cell(board, 0, 0), sudoku_board_get_cell(board, 0, 1), sudoku_board_get_cell(board, 0, 2));
    printf("  %d %d %d\n", sudoku_board_get_cell(board, 1, 0), sudoku_board_get_cell(board, 1, 1), sudoku_board_get_cell(board, 1, 2));
    printf("  %d %d %d\n", sudoku_board_get_cell(board, 2, 0), sudoku_board_get_cell(board, 2, 1), sudoku_board_get_cell(board, 2, 2));    
    print_test_board(board);
    sudoku_board_destroy(board);
}

/* ================================================================
                    MAIN - EJECUTOR DE TESTS
   ================================================================ */

int main(void) {
    printf("===============================================================\n");
    printf("       BOARD MODULE TEST - Modular Architecture Demo\n");
    printf("===============================================================\n");
    
    /* Ejecutar todos los tests */
    test_board_init();
    test_board_update_stats();
    test_subgrid_create();
    test_subgrid_get_position();
    test_subgrid_fill();
    
    /* Resumen final */
    printf("\n===============================================================\n");
    printf("                    TEST SUMMARY\n");
    printf("===============================================================\n");
    printf("  Total tests:  %d\n", results.total);
    printf("  Passed:       %d\n", results.passed);
    printf("  Failed:       %d\n", results.failed);
    
    if(results.failed == 0) {
        printf("\n  *** ALL TESTS PASSED ***\n");
    } else {
        printf("\n  *** SOME TESTS FAILED ***\n");
    }
    printf("===============================================================\n");
    
    return results.failed > 0 ? 1 : 0;
}

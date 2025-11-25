/**
 * @file test_phase1.c
 * @brief Test básico de tamaños configurables - Fase 1
 * @author Gonzalo Ramírez
 * @date 2025-11-18
 * 
 * Este test verifica que la implementación de tamaños configurables
 * funciona correctamente para las operaciones básicas de board.
 * 
 * Compile:
 *   gcc test_phase1.c src/core/board.c -I./include -o test_phase1 -std=c11
 * 
 * Run:
 *   ./test_phase1
 */

#include <stdio.h>
#include <stdlib.h>
#include "sudoku/core/board.h"

int main(void) {
    printf("═══════════════════════════════════════════════\n");
    printf("  FASE 1: Test de Tamaños Configurables\n");
    printf("═══════════════════════════════════════════════\n\n");
    
    int tests_passed = 0;
    int tests_total = 0;
    
    // ═══════════════════════════════════════════════════════════
    //  Test 1: Crear board 4×4
    // ═══════════════════════════════════════════════════════════
    printf("Test 1: Crear board 4×4 (subgrid 2×2)...\n");
    tests_total++;
    SudokuBoard *board4 = sudoku_board_create_size(2);
    if (board4 != NULL) {
        printf("  ✅ Board creado\n");
        printf("     Subgrid size: %d\n", sudoku_board_get_subgrid_size(board4));
        printf("     Board size: %d×%d\n", 
               sudoku_board_get_board_size(board4),
               sudoku_board_get_board_size(board4));
        printf("     Total cells: %d\n", sudoku_board_get_total_cells(board4));
        
        if (sudoku_board_get_board_size(board4) == 4 &&
            sudoku_board_get_total_cells(board4) == 16) {
            tests_passed++;
            printf("  ✅ Dimensiones correctas\n");
        } else {
            printf("  ❌ Dimensiones incorrectas\n");
        }
        
        sudoku_board_destroy(board4);
    } else {
        printf("  ❌ Error al crear board\n");
    }
    printf("\n");
    
    // ═══════════════════════════════════════════════════════════
    //  Test 2: Crear board 9×9 (default)
    // ═══════════════════════════════════════════════════════════
    printf("Test 2: Crear board 9×9 (default)...\n");
    tests_total++;
    SudokuBoard *board9 = sudoku_board_create();
    if (board9 != NULL) {
        printf("  ✅ Board creado\n");
        printf("     Subgrid size: %d\n", sudoku_board_get_subgrid_size(board9));
        printf("     Board size: %d×%d\n", 
               sudoku_board_get_board_size(board9),
               sudoku_board_get_board_size(board9));
        printf("     Total cells: %d\n", sudoku_board_get_total_cells(board9));
        
        if (sudoku_board_get_board_size(board9) == 9 &&
            sudoku_board_get_total_cells(board9) == 81) {
            tests_passed++;
            printf("  ✅ Dimensiones correctas\n");
        } else {
            printf("  ❌ Dimensiones incorrectas\n");
        }
        
        sudoku_board_destroy(board9);
    } else {
        printf("  ❌ Error al crear board\n");
    }
    printf("\n");
    
    // ═══════════════════════════════════════════════════════════
    //  Test 3: Crear board 16×16
    // ═══════════════════════════════════════════════════════════
    printf("Test 3: Crear board 16×16 (subgrid 4×4)...\n");
    tests_total++;
    SudokuBoard *board16 = sudoku_board_create_size(4);
    if (board16 != NULL) {
        printf("  ✅ Board creado\n");
        printf("     Subgrid size: %d\n", sudoku_board_get_subgrid_size(board16));
        printf("     Board size: %d×%d\n", 
               sudoku_board_get_board_size(board16),
               sudoku_board_get_board_size(board16));
        printf("     Total cells: %d\n", sudoku_board_get_total_cells(board16));
        
        if (sudoku_board_get_board_size(board16) == 16 &&
            sudoku_board_get_total_cells(board16) == 256) {
            tests_passed++;
            printf("  ✅ Dimensiones correctas\n");
        } else {
            printf("  ❌ Dimensiones incorrectas\n");
        }
        
        sudoku_board_destroy(board16);
    } else {
        printf("  ❌ Error al crear board\n");
    }
    printf("\n");
    
    // ═══════════════════════════════════════════════════════════
    //  Test 4: Set/Get cell
    // ═══════════════════════════════════════════════════════════
    printf("Test 4: Set/Get cell...\n");
    tests_total++;
    SudokuBoard *board = sudoku_board_create();
    if (board != NULL) {
        if (sudoku_board_set_cell(board, 0, 0, 5)) {
            int value = sudoku_board_get_cell(board, 0, 0);
            if (value == 5) {
                tests_passed++;
                printf("  ✅ Set/Get funcionando correctamente\n");
            } else {
                printf("  ❌ Get devolvió valor incorrecto: %d\n", value);
            }
        } else {
            printf("  ❌ Error en set_cell\n");
        }
        sudoku_board_destroy(board);
    }
    printf("\n");
    
    // ═══════════════════════════════════════════════════════════
    //  Test 5: Validación de tamaño inválido
    // ═══════════════════════════════════════════════════════════
    printf("Test 5: Validación de tamaño inválido...\n");
    tests_total++;
    SudokuBoard *invalid = sudoku_board_create_size(10);  // Demasiado grande
    if (invalid == NULL) {
        tests_passed++;
        printf("  ✅ Correctamente rechazó tamaño inválido\n");
    } else {
        printf("  ❌ Aceptó tamaño inválido\n");
        sudoku_board_destroy(invalid);
    }
    printf("\n");
    
    // ═══════════════════════════════════════════════════════════
    //  Test 6: Init y Update Stats
    // ═══════════════════════════════════════════════════════════
    printf("Test 6: Init y Update Stats...\n");
    tests_total++;
    SudokuBoard *board_stats = sudoku_board_create();
    if (board_stats != NULL) {
        // Inicialmente vacío
        if (sudoku_board_get_clues(board_stats) == 0 &&
            sudoku_board_get_empty(board_stats) == 81) {
            
            // Agregar algunas celdas
            sudoku_board_set_cell(board_stats, 0, 0, 1);
            sudoku_board_set_cell(board_stats, 1, 1, 2);
            sudoku_board_set_cell(board_stats, 2, 2, 3);
            
            // Actualizar estadísticas
            sudoku_board_update_stats(board_stats);
            
            if (sudoku_board_get_clues(board_stats) == 3 &&
                sudoku_board_get_empty(board_stats) == 78) {
                tests_passed++;
                printf("  ✅ Stats actualizan correctamente\n");
            } else {
                printf("  ❌ Stats incorrectas: clues=%d, empty=%d\n",
                       sudoku_board_get_clues(board_stats),
                       sudoku_board_get_empty(board_stats));
            }
        } else {
            printf("  ❌ Board no inició vacío\n");
        }
        sudoku_board_destroy(board_stats);
    }
    printf("\n");
    
    // ═══════════════════════════════════════════════════════════
    //  RESUMEN
    // ═══════════════════════════════════════════════════════════
    printf("═══════════════════════════════════════════════\n");
    printf("  RESUMEN: %d/%d tests pasaron\n", tests_passed, tests_total);
    printf("═══════════════════════════════════════════════\n");
    
    if (tests_passed == tests_total) {
        printf("\n🎉 ¡FASE 1 COMPLETADA! Todos los tests pasaron.\n\n");
        printf("Próximos pasos:\n");
        printf("1. git add include/sudoku/core/types.h\n");
        printf("2. git add include/sudoku/core/board.h\n");
        printf("3. git add src/core/board.c\n");
        printf("4. git commit -m \"feat(board): implement configurable board sizes - Phase 1\"\n");
        printf("5. git push\n\n");
        return 0;
    } else {
        printf("\n⚠️  %d/%d tests fallaron. Revisar implementación.\n\n",
               tests_total - tests_passed, tests_total);
        return 1;
    }
}

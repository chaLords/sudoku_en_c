/**
 * @file phase2.c (ADAPTED FOR CONFIGURABLE BOARD SIZES)
 * @brief Phase 2: Heuristic elimination of cells without alternatives
 * @author Gonzalo Ramírez
 * @date 2025-11-21
 * 
 * ARCHITECTURAL CHANGES FOR CONFIGURABLE SIZES:
 * 
 * 1. REPLACED: All SUDOKU_SIZE constants in loops
 *    WITH: Dynamic board->board_size
 *    
 * 2. REPLACED: SUBGRID_SIZE in subgrid calculations
 *    WITH: Dynamic board->subgrid_size
 *    
 * 3. ENHANCED: hasAlternative() now handles any board size
 *    
 * KEY ALGORITHMIC INSIGHT: The "no alternatives" heuristic is
 * size-independent. A number locked to a position in a 4×4 board
 * is just as locked as in a 16×16 board - the logic is identical,
 * only the iteration bounds change.
 */

#include "elimination_internal.h"
//#include "board_internal.h"
#include "events_internal.h"
#include "sudoku/core/validation.h"
#include "sudoku/core/board.h"
#include <stdbool.h>

/**
 * @brief Check if a number has alternative valid positions
 * 
 * CRITICAL ADAPTATIONS FOR CONFIGURABLE SIZES:
 * 
 * This function is the heart of Phase 2. It must search three constraint
 * regions (row, column, subgrid) to determine if a number can legally
 * be placed anywhere other than its current position.
 * 
 * ADAPTATION STRATEGY:
 * 1. Replace SUDOKU_SIZE with board->board_size in row/column loops
 * 2. Replace SUBGRID_SIZE with board->subgrid_size in subgrid loops
 * 3. Replace manual subgrid calculation with dynamic calculation
 * 
 * MATHEMATICAL FOUNDATION:
 * - Row contains board_size cells [0..board_size-1]
 * - Column contains board_size cells [0..board_size-1]
 * - Subgrid contains subgrid_size × subgrid_size cells
 * 
 * These relationships hold for any valid Sudoku board dimensions.
 * 
 * @param[in,out] board Board to analyze (temporarily modified, then restored)
 * @param[in] pos Position containing the number to check
 * @param[in] num The number to check for alternatives
 * @return true if number has at least one alternative position
 * 
 * Time Complexity: O(N²) worst case, O(N) typical with early exit
 *                  where N = board_size
 */
bool hasAlternative(SudokuBoard *board, const SudokuPosition *pos, int num) {
    // ✅ ADAPTACIÓN 1: Obtener tamaños dinámicos del tablero
    // Estos valores son necesarios para todos los bucles de búsqueda
    int board_size = board->board_size;      // N para tablero N×N
    int subgrid_size = board->subgrid_size;  // k para subcuadrículas k×k
    
    // Save original value and temporarily empty the cell
    int original_value = board->cells[pos->row][pos->col];
    board->cells[pos->row][pos->col] = 0;
    
    int alternatives = 0;
    
    // ═══════════════════════════════════════════════════════════════
    //                    SEARCH IN ROW
    // ═══════════════════════════════════════════════════════════════
    // 
    // ✅ ADAPTACIÓN 2: Reemplazar SUDOKU_SIZE con board_size
    // ANTES: for (int col = 0; col < SUDOKU_SIZE && alternatives == 0; col++)
    // AHORA: for (int col = 0; col < board_size && alternatives == 0; col++)
    //
    // RAZONAMIENTO: Una fila en un tablero N×N tiene N columnas.
    // Para 4×4: columnas 0-3
    // Para 9×9: columnas 0-8
    // Para 16×16: columnas 0-15
    for (int col = 0; col < board_size && alternatives == 0; col++) {
        // Skip the original position and non-empty cells
        if (col != pos->col && board->cells[pos->row][col] == 0) {
            SudokuPosition test_pos = {pos->row, col};
            if (sudoku_is_safe_position(board, &test_pos, num)) {
                alternatives++;  // Found alternative in row
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                    SEARCH IN COLUMN
    // ═══════════════════════════════════════════════════════════════
    //
    // ✅ ADAPTACIÓN 3: Reemplazar SUDOKU_SIZE con board_size
    // ANTES: for (int row = 0; row < SUDOKU_SIZE && alternatives == 0; row++)
    // AHORA: for (int row = 0; row < board_size && alternatives == 0; row++)
    //
    // RAZONAMIENTO: Una columna en un tablero N×N tiene N filas.
    for (int row = 0; row < board_size && alternatives == 0; row++) {
        // Skip the original position and non-empty cells
        if (row != pos->row && board->cells[row][pos->col] == 0) {
            SudokuPosition test_pos = {row, pos->col};
            if (sudoku_is_safe_position(board, &test_pos, num)) {
                alternatives++;  // Found alternative in column
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                    SEARCH IN SUBGRID
    // ═══════════════════════════════════════════════════════════════
    
    if (alternatives == 0) {
        // ✅ ADAPTACIÓN 4: Cálculo dinámico del inicio de subcuadrícula
        // ANTES: int subgrid_row_start = pos->row - (pos->row % SUBGRID_SIZE);
        // AHORA: int subgrid_row_start = pos->row - (pos->row % subgrid_size);
        //
        // EXPLICACIÓN MATEMÁTICA:
        // Para encontrar la esquina superior izquierda de una subcuadrícula,
        // redondeamos hacia abajo a la subcuadrícula más cercana.
        //
        // Ejemplo con 9×9 (subgrid_size=3):
        // - Celda en fila 5: 5 - (5 % 3) = 5 - 2 = 3 (inicio correcto)
        // - Celda en fila 7: 7 - (7 % 3) = 7 - 1 = 6 (inicio correcto)
        //
        // Ejemplo con 16×16 (subgrid_size=4):
        // - Celda en fila 10: 10 - (10 % 4) = 10 - 2 = 8 (inicio correcto)
        // - Celda en fila 3: 3 - (3 % 4) = 3 - 3 = 0 (inicio correcto)
        //
        // Esta fórmula funciona para cualquier tamaño de tablero.
        int subgrid_row_start = pos->row - (pos->row % subgrid_size);
        int subgrid_col_start = pos->col - (pos->col % subgrid_size);
        
        // ✅ ADAPTACIÓN 5: Iterar sobre dimensiones de subcuadrícula dinámicas
        // ANTES: for (int i = 0; i < SUBGRID_SIZE; i++)
        // AHORA: for (int i = 0; i < subgrid_size; i++)
        //
        // RAZONAMIENTO: Una subcuadrícula k×k tiene k filas y k columnas.
        // Para 4×4: subgrid es 2×2
        // Para 9×9: subgrid es 3×3
        // Para 16×16: subgrid es 4×4
        for (int i = 0; i < subgrid_size && alternatives == 0; i++) {
            for (int j = 0; j < subgrid_size && alternatives == 0; j++) {
                int row = subgrid_row_start + i;
                int col = subgrid_col_start + j;
                
                // Skip the original position and non-empty cells
                if ((row != pos->row || col != pos->col) && 
                    board->cells[row][col] == 0) {
                    SudokuPosition test_pos = {row, col};
                    if (sudoku_is_safe_position(board, &test_pos, num)) {
                        alternatives++;  // Found alternative in subgrid
                    }
                }
            }
        }
    }
    
    // Restore original value (CRITICAL for correctness!)
    board->cells[pos->row][pos->col] = original_value;
    
    return alternatives > 0;
}

/**
 * @brief Phase 2: Remove numbers without alternative positions
 * 
 * ADAPTATIONS FOR CONFIGURABLE SIZES:
 * 
 * The main loop structure remains unchanged, but now hasAlternative()
 * works with any board size. The function is already size-agnostic at
 * a high level because it iterates over subgrids using the index array
 * and sudoku_subgrid_create(), both of which handle variable sizes.
 * 
 * The key improvement is that hasAlternative() now correctly checks
 * all constraint regions regardless of board dimensions.
 * 
 * @param[in,out] board Board with Phase 1 eliminations applied
 * @param[in] index Array of subgrid indices
 * @param[in] count Number of subgrids to process
 * @return Number of cells successfully removed
 * 
 * Time Complexity: O(k² × N²) where k² = subgrid count, N = board size
 *                  This is because we check k² subgrids, and for each
 *                  filled cell, hasAlternative() is O(N²) worst case.
 */
int phase2Elimination(SudokuBoard *board, const int *index, int count) {
    // Emit phase start event
    emit_event(SUDOKU_EVENT_PHASE2_START, board, 2, 0);
    
    // ✅ ADAPTACIÓN 6: Obtener board_size para el bucle de celdas
    int board_size = board->board_size;
    
    int removed = 0;
    
    // Process each subgrid in shuffled order
    for (int idx = 0; idx < count; idx++) {
        SudokuSubGrid subgrid = sudoku_subgrid_create(index[idx]);
        
        // ✅ ADAPTACIÓN 7: Iterar hasta board_size en lugar de SUDOKU_SIZE
        // ANTES: for (int cell_idx = 0; cell_idx < SUDOKU_SIZE; cell_idx++)
        // AHORA: for (int cell_idx = 0; cell_idx < board_size; cell_idx++)
        //
        // RAZONAMIENTO: Cada subcuadrícula contiene board_size celdas numeradas 0..(board_size-1)
        for (int cell_idx = 0; cell_idx < board_size; cell_idx++) {
            SudokuPosition pos = sudoku_subgrid_get_position(&subgrid, cell_idx);
            
            // Only consider filled cells
            if (board->cells[pos.row][pos.col] != 0) {
                int num = board->cells[pos.row][pos.col];
                
                // Check if this number has any alternative valid positions
                // hasAlternative() now works correctly for any board size
                if (!hasAlternative(board, &pos, num)) {
                    // No alternatives: safe to remove
                    board->cells[pos.row][pos.col] = 0;
                    removed++;
                    
                    // Emit cell selected event
                    emit_event_cell(SUDOKU_EVENT_PHASE2_CELL_SELECTED,
                                    board, 2, removed, pos.row, pos.col, num);
                    
                    break;  // Only one removal per subgrid per round
                }
            }
        }
    }
    
    // Emit phase complete event
    emit_event(SUDOKU_EVENT_PHASE2_COMPLETE, board, 2, removed);
    
    return removed;
}

/**
 * EDUCATIONAL NOTES ON COMPLEXITY SCALING:
 * 
 * 1. HOW DOES PHASE 2 SCALE WITH BOARD SIZE?
 *    
 *    For a 9×9 board:
 *    - 9 subgrids to process
 *    - Each has ~9 filled cells after Phase 1
 *    - hasAlternative() checks ~27 cells worst case
 *    - Total: 9 × 9 × 27 = 2,187 cell checks
 *    
 *    For a 16×16 board:
 *    - 16 subgrids to process
 *    - Each has ~16 filled cells after Phase 1
 *    - hasAlternative() checks ~48 cells worst case (16+16+16)
 *    - Total: 16 × 16 × 48 = 12,288 cell checks
 *    
 *    Scaling factor: approximately N³ where N = board_size
 *    This is acceptable because early exit optimizations reduce
 *    actual work significantly below worst-case bounds.
 * 
 * 2. WHY IS THE MODULO OPERATION CORRECT?
 *    
 *    The expression `pos->row - (pos->row % subgrid_size)` computes
 *    integer division and floors to the nearest subgrid boundary.
 *    
 *    Mathematical proof:
 *    Let r = pos->row, k = subgrid_size
 *    Define q = floor(r / k), the subgrid index
 *    Then r = q×k + (r % k) by definition of modulo
 *    So r - (r % k) = q×k, which is the start of subgrid q
 *    
 *    Example: row=7, subgrid_size=3
 *    7 % 3 = 1, so 7-1 = 6 (start of third subgrid)
 *    Check: floor(7/3) = 2, and 2×3 = 6 ✓
 * 
 * 3. WHY DOES hasAlternative() RESTORE THE CELL?
 *    
 *    We temporarily set the cell to 0 to ask: "if this cell were empty,
 *    could the number go somewhere else?" This requires making the cell
 *    appear empty during the isSafePosition() checks.
 *    
 *    However, we must restore it because:
 *    a) We haven't decided to remove it yet (only checking)
 *    b) Other iterations need to see the original board state
 *    c) Forgetting to restore causes board corruption
 *    
 *    This pattern (modify, test, restore) is common in backtracking
 *    and heuristic search algorithms.
 */

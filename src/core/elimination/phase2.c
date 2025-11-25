/**
 * @file phase2.c (ADAPTED FOR CONFIGURABLE BOARD SIZES)
 * @brief Phase 2: Heuristic elimination of numbers without alternatives
 * @author Gonzalo Ramírez
 * @date 2025-11-21
 * 
 * ARCHITECTURAL CHANGES FOR CONFIGURABLE SIZES:
 * 
 * 1. REPLACED: All hardcoded SUDOKU_SIZE (9) constants
 *    WITH: Dynamic board->board_size access
 *    
 * 2. REPLACED: Hardcoded SUBGRID_SIZE (3) constants
 *    WITH: Dynamic calculation sqrt(board_size)
 *    
 * 3. MAINTAINED: Event-driven architecture (no printf)
 * 4. MAINTAINED: Original algorithm logic unchanged
 * 
 * CRITICAL INSIGHT: The hasAlternative() function is the heart of Phase 2.
 * Its logic involves checking row, column, and subgrid for alternative
 * positions. All three searches must use dynamic sizes.
 * 
 * MATHEMATICAL FOUNDATION:
 * For an N×N board with √N×√N subgrids:
 * - Row search: positions 0 to N-1 in same row
 * - Column search: positions 0 to N-1 in same column
 * - Subgrid search: positions in the √N×√N region containing the cell
 * 
 * The subgrid calculation uses: base = pos - (pos % √N)
 * This formula works for any perfect square board size.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "algorithms_internal.h"
#include "elimination_internal.h"
#include "events_internal.h"
#include "sudoku/core/validation.h"
#include "sudoku/core/board.h"

/**
 * @brief Check if a number has alternative valid positions
 * 
 * ADAPTATION FOR CONFIGURABLE SIZES:
 * 
 * This function must check three constraint regions (row, column, subgrid)
 * for alternative placement positions. All loop bounds and calculations
 * are now derived from the board's actual dimensions.
 * 
 * ALGORITHM EXPLANATION:
 * 1. Temporarily remove the number from its current position
 * 2. For each empty cell in the row: check if number could go there
 * 3. For each empty cell in the column: check if number could go there
 * 4. For each empty cell in the subgrid: check if number could go there
 * 5. Restore the original number
 * 6. Return true if ANY alternative was found, false otherwise
 * 
 * EARLY EXIT OPTIMIZATION:
 * The loops terminate as soon as one alternative is found (alternatives > 0).
 * This transforms worst-case O(3N) to average-case O(k) where k << N.
 * 
 * @param board The Sudoku board
 * @param pos Position of the number to check
 * @param num The number to check for alternatives
 * @return true if the number has at least one alternative position
 * 
 * @pre board != NULL && pos != NULL
 * @pre board->cells[pos->row][pos->col] == num (or temporarily 0)
 */
bool hasAlternative(SudokuBoard *board, const SudokuPosition *pos, int num) {
    // ✅ ADAPTACIÓN 1: Obtener dimensiones dinámicas del tablero
    // ANTES: Usaba constantes SUDOKU_SIZE (9) y SUBGRID_SIZE (3)
    // AHORA: Consulta las dimensiones reales del tablero
    int board_size = sudoku_board_get_board_size(board);
    int subgrid_size = sudoku_board_get_subgrid_size(board);
    
    // Step 1: Temporarily remove the number
    // This allows isSafePosition to consider this cell as "available"
    int temp = board->cells[pos->row][pos->col];
    board->cells[pos->row][pos->col] = 0;
    
    int alternatives = 0;
    
    // ═══════════════════════════════════════════════════════════════
    // SEARCH IN ROW: Check all other empty cells in the same row
    // ═══════════════════════════════════════════════════════════════
    // ✅ ADAPTACIÓN 2: Usar board_size en lugar de SUDOKU_SIZE
    for (int col = 0; col < board_size && alternatives == 0; col++) {
        // Skip the current position itself
        if (col != pos->col && board->cells[pos->row][col] == 0) {
            SudokuPosition test_pos = {pos->row, col};
            if (sudoku_is_safe_position(board, &test_pos, num)) {
                alternatives++;
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    // SEARCH IN COLUMN: Check all other empty cells in the same column
    // ═══════════════════════════════════════════════════════════════
    // ✅ ADAPTACIÓN 3: Usar board_size en lugar de SUDOKU_SIZE
    for (int row = 0; row < board_size && alternatives == 0; row++) {
        // Skip the current position itself
        if (row != pos->row && board->cells[row][pos->col] == 0) {
            SudokuPosition test_pos = {row, pos->col};
            if (sudoku_is_safe_position(board, &test_pos, num)) {
                alternatives++;
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    // SEARCH IN SUBGRID: Check all other empty cells in the same subgrid
    // ═══════════════════════════════════════════════════════════════
    if (alternatives == 0) {
        // ✅ ADAPTACIÓN 4: Cálculo dinámico de la base de la subcuadrícula
        // ANTES: rowStart = pos->row - (pos->row % 3)
        // AHORA: rowStart = pos->row - (pos->row % subgrid_size)
        //
        // PRUEBA MATEMÁTICA de por qué esto funciona:
        // Sea r = pos->row, k = subgrid_size
        // Por definición de módulo: r = q*k + (r % k) donde q = floor(r/k)
        // Entonces: r - (r % k) = q*k = inicio de la subcuadrícula q
        //
        // Ejemplo para 9×9 (k=3): row=7 → 7-(7%3) = 7-1 = 6 (fila 6)
        // Ejemplo para 16×16 (k=4): row=7 → 7-(7%4) = 7-3 = 4 (fila 4)
        int row_start = pos->row - (pos->row % subgrid_size);
        int col_start = pos->col - (pos->col % subgrid_size);
        
        // ✅ ADAPTACIÓN 5: Usar subgrid_size para límites de iteración
        for (int i = 0; i < subgrid_size && alternatives == 0; i++) {
            for (int j = 0; j < subgrid_size && alternatives == 0; j++) {
                int r = row_start + i;
                int c = col_start + j;
                
                // Skip the current position itself
                if ((r != pos->row || c != pos->col) && board->cells[r][c] == 0) {
                    SudokuPosition test_pos = {r, c};
                    if (sudoku_is_safe_position(board, &test_pos, num)) {
                        alternatives++;
                    }
                }
            }
        }
    }
    
    // Step 5: Restore the original number
    // CRITICAL: Forgetting this causes board corruption!
    board->cells[pos->row][pos->col] = temp;
    
    return alternatives > 0;
}

/**
 * @brief Phase 2: Remove numbers that have no alternative positions
 * 
 * ALGORITHM OVERVIEW:
 * 
 * Phase 2 exploits a key insight: if a number can ONLY go in its current
 * position within its constraint regions (row, column, subgrid), then
 * removing it cannot introduce multiple solutions. The solver will be
 * forced to place that exact number there.
 * 
 * This is called "naked single elimination" in Sudoku solving terminology.
 * 
 * ADAPTATIONS FOR CONFIGURABLE SIZES:
 * - All loop bounds use board_size instead of SUDOKU_SIZE (9)
 * - Subgrid iteration works for any perfect square board
 * 
 * TIME COMPLEXITY ANALYSIS:
 * 
 * For an N×N board with √N×√N subgrids:
 * - Outer loop: N subgrids (technically (N/√N)² = N subgrids)
 * - Inner loop: N cells per subgrid
 * - hasAlternative: O(3N) worst case = O(N)
 * - Total per round: O(N × N × N) = O(N³)
 * 
 * Typical rounds: 3-5 before convergence
 * Total Phase 2: O(5 × N³) = O(N³)
 * 
 * @param board Board with partially filled solution
 * @param index Array of subgrid indices to process
 * @param count Number of subgrids to process
 * @return Number of cells removed in this round
 * 
 * @note This function should be called in a loop until it returns 0
 */
int phase2Elimination(SudokuBoard *board, const int *index, int count) {
    // Emit phase 2 start event
    emit_event(SUDOKU_EVENT_PHASE2_START, board, 2, 0);
    
    // ✅ ADAPTACIÓN: Obtener tamaño dinámico del tablero
    int board_size = sudoku_board_get_board_size(board);
    
    int removed = 0;
    
    // Process each subgrid in the order specified
    for (int idx = 0; idx < count; idx++) {
        SudokuSubGrid subgrid = sudoku_subgrid_create(index[idx], board->subgrid_size);
        
        // ✅ ADAPTACIÓN: Usar board_size para iterar sobre celdas de subcuadrícula
        // RAZONAMIENTO: Cada subcuadrícula contiene board_size celdas
        // Para 9×9: 9 celdas por subcuadrícula (3×3)
        // Para 16×16: 16 celdas por subcuadrícula (4×4)
        for (int cell_idx = 0; cell_idx < board_size; cell_idx++) {
            SudokuPosition pos = sudoku_subgrid_get_position(&subgrid, cell_idx);
            
            // Only process filled cells
            if (board->cells[pos.row][pos.col] != 0) {
                int num = board->cells[pos.row][pos.col];
                
                // Check if this number has NO alternatives
                if (!hasAlternative(board, &pos, num)) {
                    // Safe to remove: number can only go here
                    int removed_value = board->cells[pos.row][pos.col];
                    board->cells[pos.row][pos.col] = 0;
                    removed++;
                    
                    // Emit cell selected event
                    emit_event_cell(SUDOKU_EVENT_PHASE2_CELL_SELECTED, board, 2,
                                    removed, pos.row, pos.col, removed_value);
                    
                    // Only remove one per subgrid per round
                    // This prevents over-aggressive elimination
                    break;
                }
            }
        }
    }
    
    // Emit phase 2 complete event
    emit_event(SUDOKU_EVENT_PHASE2_COMPLETE, board, 2, removed);
    
    return removed;
}

/**
 * EDUCATIONAL NOTES ON PHASE 2 ADAPTATIONS:
 * 
 * 1. WHY THE ALGORITHM ITSELF DOESN'T CHANGE:
 *    
 *    The core logic of Phase 2 is size-agnostic:
 *    "Remove cells whose numbers have no alternative positions"
 *    
 *    This statement is true for 4×4, 9×9, 16×16, and any other size.
 *    The only changes needed are:
 *    - Loop bounds: 9 → board_size
 *    - Subgrid calculations: 3 → subgrid_size
 * 
 * 2. HOW DOES PHASE 2 SCALE WITH BOARD SIZE?
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
 * 3. WHY IS THE MODULO OPERATION CORRECT?
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
 * 4. WHY DOES hasAlternative() RESTORE THE CELL?
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

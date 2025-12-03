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
/*
 * NUEVO EN v3.0: Includes para eliminación inteligente
 */
#include "elimination_config.h"
#include "density_scoring.h"
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
 * @brief PHASE 2 SMART: Intelligent elimination guided by density scoring
 * 
 * This is an enhanced version of phase2Elimination() that uses density analysis
 * to make strategic decisions about which subgrids to process first. Instead of
 * processing subgrids in a fixed order, we calculate a "score" for each subgrid
 * and process them in order of priority.
 * 
 * WHAT MAKES THIS "SMART":
 * 
 * The original phase2Elimination() processes subgrids in the order you give it,
 * which is typically just the natural order from zero to eight. This works, but
 * it doesn't take advantage of information we can extract from the board state.
 * 
 * The smart version analyzes each subgrid before processing and asks:
 * - How full is this subgrid? (density)
 * - How many cells can potentially be eliminated? (candidates)
 * 
 * Based on these metrics, we can prioritize our efforts. For EASY and MEDIUM
 * puzzles, we want to process high-density subgrids first because they're safer.
 * For HARD and EXPERT puzzles, we process them randomly to increase difficulty.
 * 
 * CONCEPTUAL ANALOGY:
 * 
 * Imagine you're a chef preparing a complex meal with many dishes. You could
 * cook them in arbitrary order (original approach), or you could prioritize
 * based on cooking time and complexity (smart approach). The smart chef finishes
 * faster and produces better results because they work strategically.
 * 
 * Similarly, this function works strategically by processing subgrids in an
 * order that maximizes the quality and difficulty of the resulting puzzle.
 * 
 * @param board The current board state (will be modified)
 * @param config Configuration that controls behavior based on difficulty
 * @return Number of cells eliminated in this round
 * 
 * @example
 * ```c
 * // Create configuration for EASY puzzle
 * SudokuEliminationConfig config = sudoku_elimination_config_create(DIFFICULTY_EASY);
 * 
 * // Run Phase 2 smart in a loop until no more eliminations possible
 * int total_removed = 0;
 * int removed;
 * do {
 *     removed = phase2EliminationSmart(board, &config);
 *     total_removed += removed;
 * } while (removed > 0 && total_removed < 35);
 * 
 * printf("Phase 2 eliminated %d cells total\n", total_removed);
 * ```
 * 
 * @note This function should be called in a loop, just like the original
 *       phase2Elimination(), because removing cells can create new opportunities
 *       for removal in subsequent rounds.
 */
int phase2EliminationSmart(SudokuBoard *board, 
                          const SudokuEliminationConfig *config) {
    
    /*
     * ═══════════════════════════════════════════════════════════════
     * STEP 1: EMIT START EVENT
     * ═══════════════════════════════════════════════════════════════
     * 
     * The event system allows external code (like tests or UI) to observe
     * what's happening during generation. This is a professional pattern
     * called "observer pattern" or "event-driven architecture".
     * 
     * WHY EVENTS MATTER:
     * Instead of scattering printf() statements throughout the code (which
     * would clutter the logic and make it hard to turn off), we emit events
     * that interested parties can listen to. If nobody's listening, the
     * events are essentially free (just a function call that returns quickly).
     */
    emit_event(SUDOKU_EVENT_PHASE2_START, board, 2, 0);
    
    /*
     * ═══════════════════════════════════════════════════════════════
     * STEP 2: CALCULATE SCORES FOR ALL SUBGRIDS
     * ═══════════════════════════════════════════════════════════════
     * 
     * This is where the "intelligence" begins. Instead of blindly processing
     * subgrids in arbitrary order, we first analyze them all and create a
     * priority ranking.
     * 
     * WHAT THE SCORING DOES:
     * For each of the nine subgrids (in a 9×9 board), we calculate:
     * 1. Density: What percentage of cells are filled?
     * 2. Candidates: How many cells could potentially be eliminated?
     * 
     * These two numbers together form a "score" that tells us how attractive
     * this subgrid is for elimination purposes.
     */
    int num_subgrids;
    SubGridScore *scores = sudoku_score_subgrids(board, config, &num_subgrids);
    
    /*
     * DEFENSIVE PROGRAMMING: Check for allocation failure
     * 
     * If malloc() failed inside sudoku_score_subgrids(), we get NULL back.
     * Rather than crashing with a segmentation fault (which would happen if
     * we tried to use a NULL pointer), we handle it gracefully by returning
     * zero eliminations.
     * 
     * This is called "fail-safe" behavior: if something goes wrong, we
     * degrade gracefully rather than catastrophically.
     */
    if (scores == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed in phase2EliminationSmart\n");
        return 0;
    }
    
    /*
     * ═══════════════════════════════════════════════════════════════
     * STEP 3: SORT SUBGRIDS BY SCORE (CONDITIONAL)
     * ═══════════════════════════════════════════════════════════════
     * 
     * Here's where we make a strategic decision based on difficulty level.
     * 
     * FOR EASY AND MEDIUM PUZZLES:
     * We sort the subgrids so that high-density ones come first. This means
     * we'll process the "safest" subgrids first, where "safe" means they have
     * more constraints and are less likely to create multiple solutions when
     * we eliminate cells from them.
     * 
     * Think of it like removing blocks from a Jenga tower: you want to remove
     * blocks from the top (well-supported, high density) rather than from the
     * bottom (risky, low density).
     * 
     * FOR HARD AND EXPERT PUZZLES:
     * We DON'T sort them. This leaves them in whatever order they were
     * calculated, which is effectively random. This increases difficulty
     * because we might end up removing cells from low-density subgrids,
     * creating a more challenging puzzle.
     * 
     * THE QSORT FUNCTION:
     * qsort() is a standard C library function that sorts an array. It needs
     * to know:
     * - The array to sort (scores)
     * - How many elements (num_subgrids)
     * - The size of each element (sizeof(SubGridScore))
     * - How to compare elements (sudoku_compare_subgrid_scores_desc)
     * 
     * The comparison function determines the sort order. Our comparison function
     * is defined in density_scoring.c and compares by density first, then by
     * candidate count as a tiebreaker.
     */
    if (config->prioritize_high_density) {
        qsort(scores, num_subgrids, sizeof(SubGridScore), 
              sudoku_compare_subgrid_scores_desc);
    }
    
    /*
     * ═══════════════════════════════════════════════════════════════
     * STEP 4: PROCESS SUBGRIDS IN PRIORITY ORDER
     * ═══════════════════════════════════════════════════════════════
     * 
     * Now we iterate through the subgrids in the order determined by sorting
     * (or in the original order if we didn't sort). For each subgrid, we
     * attempt to eliminate one cell that has no alternatives.
     * 
     * IMPORTANT LOOP INVARIANT:
     * We eliminate AT MOST one cell per subgrid per round. This maintains
     * balance across the board and prevents any single subgrid from becoming
     * too sparse too quickly.
     */
    int removed = 0;
    
    for (int idx = 0; idx < num_subgrids; idx++) {
        /*
         * Get the actual subgrid index from the score structure.
         * If we sorted, this gives us subgrids in density order.
         * If we didn't sort, this gives us natural order (0, 1, 2, ..., 8).
         */
        int subgrid_idx = scores[idx].subgrid_idx;
        
        /*
         * Create a SubGrid object for convenient iteration
         * 
         * The SubGrid abstraction hides the messy arithmetic of converting
         * from subgrid index to actual board positions. We just iterate
         * from 0 to board_size and let the SubGrid handle the position
         * calculations.
         */
        SudokuSubGrid sg = sudoku_subgrid_create(subgrid_idx, board->subgrid_size);
        int board_size = sudoku_board_get_board_size(board);
        
        /*
         * INNER LOOP: Search for a cell to eliminate
         * 
         * We iterate through each cell in this subgrid, looking for one that:
         * 1. Contains a number (is filled)
         * 2. Has no alternatives elsewhere
         * 
         * When we find such a cell, we eliminate it and break out of the loop,
         * ensuring we only remove one cell per subgrid.
         */
        for (int i = 0; i < board_size; i++) {
            SudokuPosition pos = sudoku_subgrid_get_position(&sg, i);
            int num = board->cells[pos.row][pos.col];
            
            // Skip empty cells
            if (num == 0) continue;
            
            /*
             * CHECK FOR ALTERNATIVES:
             * 
             * This is the core logic of Phase 2. We ask: "If we remove this
             * number, could it go anywhere else in the same row, column, or
             * subgrid?"
             * 
             * The hasAlternative() function (from the original phase2.c)
             * temporarily removes the number and checks if it can be placed
             * elsewhere while respecting Sudoku rules.
             * 
             * If it returns false (no alternatives), this number is "locked"
             * into this position, making it safe to eliminate from a puzzle
             * generation perspective. The solver will be forced to deduce
             * that this number must go here.
             */
            if (!hasAlternative(board, &pos, num)) {
                /*
                 * ELIMINATE THE CELL:
                 * 
                 * Set the cell to zero, which represents an empty cell.
                 * This is the actual elimination action.
                 */
                board->cells[pos.row][pos.col] = 0;
                removed++;
                
                /*
                 * EMIT CELL EVENT:
                 * 
                 * Notify observers that we eliminated a cell. This event
                 * includes detailed information:
                 * - Which phase (2)
                 * - How many cells removed so far (removed)
                 * - The position (pos.row, pos.col)
                 * - The number that was there (num)
                 * 
                 * This allows external code to log, display, or analyze
                 * the elimination process in real-time.
                 */
                emit_event_cell(SUDOKU_EVENT_PHASE2_CELL_SELECTED, 
                                      board, 2, removed, 
                                      pos.row, pos.col, num);
                
                /*
                 * BREAK: One per subgrid per round
                 * 
                 * This break is critical. It ensures we only remove one cell
                 * from this subgrid before moving to the next. This maintains
                 * balance and prevents over-elimination from any single region.
                 * 
                 * Why is balance important? If we removed all possible cells
                 * from one subgrid in a single round, we might make that region
                 * extremely sparse while others remain dense, creating an
                 * unbalanced puzzle that's frustrating to solve.
                 */
                break;
            }
        }
    }
    
    /*
     * ═══════════════════════════════════════════════════════════════
     * STEP 5: CLEANUP AND RETURN
     * ═══════════════════════════════════════════════════════════════
     * 
     * MEMORY MANAGEMENT:
     * We allocated the scores array with malloc() at the beginning, so we
     * MUST free it now to avoid a memory leak. Every malloc() must have a
     * matching free(), and this is that match.
     * 
     * This is one of the responsibilities of C programming: manual memory
     * management. Languages like Python or Java do this automatically
     * (garbage collection), but in C, we must be explicit.
     */
    free(scores);
    
    /*
     * EMIT COMPLETION EVENT:
     * 
     * Signal that this round of Phase 2 has completed and report how many
     * cells were eliminated.
     */
    emit_event(SUDOKU_EVENT_PHASE2_COMPLETE, board, 2, removed);
    
    /*
     * RETURN RESULT:
     * 
     * The return value tells the caller how many cells were eliminated in
     * this round. If this is zero, it means we couldn't find any more cells
     * to eliminate, and the caller should stop calling this function.
     * 
     * Typical usage pattern:
     * ```c
     * int removed;
     * do {
     *     removed = phase2EliminationSmart(board, config);
     * } while (removed > 0);
     * ```
     * 
     * This continues calling Phase 2 until it returns zero (no more work).
     */
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

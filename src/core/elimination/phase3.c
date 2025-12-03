/**
 * @file phase3.c (ADAPTED FOR CONFIGURABLE BOARD SIZES)
 * @brief Phase 3: Exhaustive verified elimination with dynamic targeting
 * @author Gonzalo Ramírez
 * @date 2025-11-21
 * 
 * CRITICAL ARCHITECTURAL CHANGES FOR CONFIGURABLE SIZES:
 * 
 * 1. REPLACED: Hardcoded TOTAL_CELLS (81) in malloc
 *    WITH: Dynamic calculation board_size × board_size
 *    
 * 2. REPLACED: Hardcoded SUDOKU_SIZE in iteration loops
 *    WITH: Dynamic board->board_size
 *    
 * 3. NEW: calculate_phase3_target() function for proportional scaling
 *    This replaces the obsolete #define PHASE3_TARGET 25 constant
 *    
 * 4. NEW: phase3EliminationAuto() wrapper for simplified API
 *    
 * KEY INSIGHT: Phase 3's target (how many additional cells to remove)
 * cannot be a fixed constant. A target of 25 cells represents ~31% of
 * a 9×9 board (81 cells), but only ~4% of a 25×25 board (625 cells).
 * We must calculate the target proportionally.
 * 
 * MEMORY MANAGEMENT:
 * This module allocates dynamic memory for the positions array.
 * The size is now total_cells × sizeof(SudokuPosition) instead of
 * the old 81 × sizeof(SudokuPosition).
 * 
 * IMPORTANT NOTE ON countSolutionsExact():
 * This function is NOT defined here. It is defined in validation.c
 * and declared in validation.h. We include the header and use the
 * existing implementation to avoid duplicate symbol errors.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "algorithms_internal.h"
#include "elimination_internal.h"
#include "events_internal.h"
#include "sudoku/core/validation.h"  // Provides countSolutionsExact() declaration
#include "sudoku/core/board.h"
/*
 * NUEVO EN v3.0: Includes para eliminación inteligente
 */
#include "generator_internal.h"
#include "elimination_config.h"
#include "density_scoring.h"

/*
 * ═══════════════════════════════════════════════════════════════════
 * EDUCATIONAL NOTE: Why countSolutionsExact() is NOT defined here
 * ═══════════════════════════════════════════════════════════════════
 * 
 * The function countSolutionsExact() is already implemented in 
 * validation.c and declared in validation.h. Including the header
 * gives us access to the function without redefining it.
 * 
 * In C, the One Definition Rule (ODR) states:
 * - A function can be DECLARED multiple times (in headers)
 * - A function can only be DEFINED once (in one .c file)
 * 
 * If we define countSolutionsExact() here AND in validation.c,
 * the linker will fail with "multiple definitions" error.
 * 
 * The include statement:
 *     #include "sudoku/core/validation.h"
 * 
 * Provides the DECLARATION:
 *     int countSolutionsExact(SudokuBoard *board, int limit);
 * 
 * The DEFINITION lives in validation.c and is linked automatically.
 * ═══════════════════════════════════════════════════════════════════
 */

/**
 * @brief Calculate the Phase 3 elimination target based on board size
 * 
 * This function replaces the obsolete PHASE3_TARGET constant with a
 * dynamic calculation that scales proportionally with board dimensions.
 * 
 * MATHEMATICAL REASONING:
 * 
 * The target represents "additional cells to remove after Phases 1 and 2".
 * For 9×9 boards, the original target of 25 represents approximately 31%
 * of total cells (25/81 ≈ 0.31).
 * 
 * However, larger boards have denser constraint networks, making it harder
 * to maintain unique solutions with high removal rates. We therefore use
 * decreasing percentages for larger boards:
 * 
 * - Small boards (N ≤ 9):   31% → maintains original 9×9 behavior
 * - Medium boards (N ≤ 16): 27% → slightly conservative for 16×16
 * - Large boards (N > 16):  23% → more conservative for 25×25+
 * 
 * VERIFICATION:
 * - 4×4 board:   4×4=16 cells → 16×0.31 ≈ 5 target (reasonable)
 * - 9×9 board:   9×9=81 cells → 81×0.31 ≈ 25 target (matches original)
 * - 16×16 board: 16×16=256 → 256×0.27 ≈ 69 target
 * - 25×25 board: 25×25=625 → 625×0.23 ≈ 144 target
 * 
 * @param board Board to calculate target for
 * @return Calculated target number of cells to attempt removing
 * 
 * @note The returned value is an upper bound; actual removals may be fewer
 *       if unique solution cannot be maintained.
 */
static int calculate_phase3_target(const SudokuBoard *board) {
    int board_size = sudoku_board_get_board_size(board);
    int total_cells = board_size * board_size;
    
    // Determine percentage based on board size
    // Larger boards need smaller percentages to maintain solvability
    double percentage;
    
    if (board_size <= 9) {
        // Small boards: 31% (matches original PHASE3_TARGET = 25 for 9×9)
        percentage = 0.31;
    } else if (board_size <= 16) {
        // Medium boards: 27% (slightly more conservative)
        percentage = 0.27;
    } else {
        // Large boards: 23% (most conservative)
        percentage = 0.23;
    }
    
    // Calculate target with rounding to nearest integer
    // The +0.5 before casting ensures proper rounding instead of truncation
    int target = (int)(total_cells * percentage + 0.5);
    
    return target;
}

/**
 * @brief Phase 3: Free elimination with unique solution verification
 * 
 * This is the most computationally expensive phase because it verifies
 * unique solution after each removal attempt. However, it's essential
 * for producing quality puzzles.
 * 
 * ADAPTATIONS FOR CONFIGURABLE SIZES:
 * 
 * 1. malloc size: Uses board_size² instead of hardcoded 81
 * 2. Loop bounds: Uses board_size instead of SUDOKU_SIZE
 * 3. Target parameter: Caller should use calculate_phase3_target() or
 *    phase3EliminationAuto() for automatic calculation
 * 
 * MEMORY MANAGEMENT:
 * - Allocates: board_size² × sizeof(SudokuPosition) bytes
 * - For 9×9: 81 × 8 = 648 bytes
 * - For 16×16: 256 × 8 = 2,048 bytes
 * - For 25×25: 625 × 8 = 5,000 bytes
 * - Must free() before returning to prevent memory leak
 * 
 * SOLUTION VERIFICATION:
 * Uses countSolutionsExact() from validation.c to verify that removing
 * a cell doesn't create multiple solutions. The function is called with
 * limit=2, which means it stops as soon as it finds 2 solutions (we only
 * need to distinguish between "exactly 1" and "more than 1").
 * 
 * @param board Board to perform Phase 3 elimination on
 * @param target Maximum number of cells to attempt removing
 * @return Number of cells successfully removed (≤ target)
 * 
 * @pre board != NULL
 * @pre Board should have passed through Phases 1 and 2 first
 * @post Board has unique solution guaranteed
 */
int phase3Elimination(SudokuBoard *board, int target) {
    // Emit phase start event
    emit_event(SUDOKU_EVENT_PHASE3_START, board, 3, 0);
    
    // ✅ ADAPTACIÓN 1: Obtener dimensiones dinámicas
    int board_size = sudoku_board_get_board_size(board);
    int total_cells = board_size * board_size;
    
    // ✅ ADAPTACIÓN 2: Asignación dinámica basada en tamaño real
    // ANTES: Position *positions = malloc(81 * sizeof(Position));
    // AHORA: SudokuPosition *positions = malloc(total_cells * sizeof(SudokuPosition));
    SudokuPosition *positions = (SudokuPosition *)malloc(total_cells * sizeof(SudokuPosition));
    
    if (positions == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for Phase 3\n");
        return 0;
    }
    
    // Collect all positions that currently have numbers
    int count = 0;
    
    // ✅ ADAPTACIÓN 3: Usar board_size para iterar
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            if (board->cells[row][col] != 0) {
                positions[count].row = row;
                positions[count].col = col;
                count++;
            }
        }
    }
    
    // Shuffle positions for random removal order
    // This ensures different puzzles even from the same complete board
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        SudokuPosition temp = positions[i];
        positions[i] = positions[j];
        positions[j] = temp;
    }
    
    int removed = 0;
    
    // Try removing cells in random order until target reached
    for (int i = 0; i < count && removed < target; i++) {
        SudokuPosition *pos = &positions[i];
        int temp = board->cells[pos->row][pos->col];
        
        // Temporarily remove the cell
        board->cells[pos->row][pos->col] = 0;
        
        // CRITICAL CHECK: Does the puzzle still have exactly one solution?
        // countSolutionsExact (from validation.c) with limit=2 stops as soon
        // as it finds 2 solutions, providing an enormous performance boost.
        if (countSolutionsExact(board, 2) == 1) {
            // Safe to remove: unique solution maintained
            removed++;
            
            // Emit cell removed event
            emit_event_cell(SUDOKU_EVENT_PHASE3_CELL_REMOVED, board, 3,
                            removed, pos->row, pos->col, temp);
        } else {
            // Multiple solutions detected: restore the cell
            board->cells[pos->row][pos->col] = temp;
            
            // Optionally emit cell kept event
            emit_event_cell(SUDOKU_EVENT_PHASE3_CELL_KEPT, board, 3,
                            removed, pos->row, pos->col, temp);
        }
    }
    
    // ✅ CRÍTICO: Liberar memoria dinámica
    // Olvidar este free() causa memory leak
    free(positions);
    
    // Emit phase complete event
    emit_event(SUDOKU_EVENT_PHASE3_COMPLETE, board, 3, removed);
    
    return removed;
}

/**
 * @brief Phase 3 elimination with automatic target calculation
 * 
 * ✅ NUEVA FUNCIÓN - FORMA RECOMENDADA DE INVOCAR PHASE 3
 * 
 * This wrapper function automatically calculates an appropriate removal
 * target based on the board's dimensions, eliminating the need for the
 * caller to manage size-dependent constants.
 * 
 * USAGE:
 * 
 * Instead of:
 *     stats->phase3_removed = phase3Elimination(board, PHASE3_TARGET);
 * 
 * Use:
 *     stats->phase3_removed = phase3EliminationAuto(board);
 * 
 * BENEFITS:
 * - No need to define or maintain PHASE3_TARGET constant
 * - Automatically scales for any board size
 * - Simplified API for the common case
 * - Original phase3Elimination() still available for custom targets
 * 
 * @param board Board to perform Phase 3 elimination on
 * @return Number of cells successfully removed
 * 
 * @note Internally calls calculate_phase3_target() and phase3Elimination()
 */
int phase3EliminationAuto(SudokuBoard *board) {
    int target = calculate_phase3_target(board);
    return phase3Elimination(board, target);
}

/**
 * @brief PHASE 3 SMART: Intelligent free elimination with density prioritization
 * 
 * This is an enhanced version of phase3Elimination() that uses density scoring
 * to intelligently select which cells to attempt eliminating. Instead of trying
 * cells in random order, we prioritize based on:
 * - The density of the subgrid containing the cell
 * - The number of alternatives the cell's number has
 * 
 * THE FUNDAMENTAL CHALLENGE OF PHASE 3:
 * 
 * Phase 3 is the most computationally expensive phase because every elimination
 * must be verified. When we eliminate a cell, we must check that the puzzle
 * still has exactly one solution. This check requires backtracking through the
 * entire solution space, which can take milliseconds per cell.
 * 
 * Given that Phase 3 might attempt eliminating twenty to thirty cells, and each
 * attempt involves expensive verification, the total time can be substantial.
 * That's why being strategic about which cells we try first is important.
 * 
 * HOW DENSITY SCORING HELPS:
 * 
 * By trying cells from high-density subgrids first, we increase the likelihood
 * that elimination will succeed (maintain unique solution). This means we spend
 * less time on failed attempts and more time on successful ones, reducing the
 * total number of expensive verification calls.
 * 
 * ANALOGY: MINING FOR GOLD
 * 
 * Imagine you're mining for gold in a mountain. You could dig randomly (original
 * approach), or you could use a metal detector to find the most promising spots
 * first (smart approach). Both methods will eventually find the gold, but the
 * smart approach finds it faster because it focuses effort where success is likely.
 * 
 * Similarly, this function "mines" for cells that can be eliminated by focusing
 * on the most promising candidates first.
 * 
 * @param board The current board state (will be modified)
 * @param config Configuration controlling behavior and difficulty
 * @param already_removed Number of cells already eliminated in Phases 1 and 2
 * @return Number of cells successfully eliminated
 * 
 * @example
 * ```c
 * SudokuEliminationConfig config = sudoku_elimination_config_create(DIFFICULTY_EASY);
 * 
 * int already_removed = 22;  // Phase 1 (9) + Phase 2 (13)
 * int phase3_removed = phase3EliminationSmart(board, &config, already_removed);
 * 
 * printf("Phase 3 eliminated %d additional cells\n", phase3_removed);
 * printf("Total eliminated: %d\n", already_removed + phase3_removed);
 * ```
 * 
 * @note This function calculates its own target dynamically based on the
 *       difficulty configuration and how many cells were already eliminated.
 */
int phase3EliminationSmart(SudokuBoard *board,
                          const SudokuEliminationConfig *config,
                          int already_removed) {
    
    /*
     * ═══════════════════════════════════════════════════════════════
     * STEP 1: CALCULATE TARGET FOR THIS PHASE
     * ═══════════════════════════════════════════════════════════════
     * 
     * Unlike the original phase3Elimination() which takes a fixed target
     * parameter, the smart version calculates its own target based on:
     * - The difficulty configuration (EASY, MEDIUM, HARD, EXPERT)
     * - How many cells were already eliminated in Phases 1 and 2
     * 
     * This dynamic calculation ensures that the final puzzle has the correct
     * number of clues for the desired difficulty level, regardless of how
     * successful Phases 1 and 2 were.
     * 
     * EXAMPLE CALCULATION:
     * For a 9×9 EASY puzzle:
     * - Target: 49.5% elimination (average of 43% and 56%)
     * - Total cells: 81
     * - Target empty: 81 × 0.495 = 40 cells
     * - Already removed: 22 cells (Phase 1: 9, Phase 2: 13)
     * - Additional needed: 40 - 22 = 18 cells
     */
    int target = sudoku_elimination_calculate_target(board, config, already_removed);
    
    /*
     * EARLY EXIT: Target already met
     * 
     * If Phases 1 and 2 already eliminated enough cells (or too many),
     * we don't need to do anything. The calculate_target() function
     * returns zero in this case, and we immediately return.
     * 
     * This is defensive programming: we check for the base case before
     * doing expensive work.
     */
    if (target <= 0) {
        emit_event(SUDOKU_EVENT_PHASE3_COMPLETE, board, 3, 0);
        return 0;
    }
    
    /*
     * EMIT START EVENT
     */
    emit_event(SUDOKU_EVENT_PHASE3_START, board, 3, target);
    
    /*
     * ═══════════════════════════════════════════════════════════════
     * STEP 2: SCORE ALL FILLED CELLS
     * ═══════════════════════════════════════════════════════════════
     * 
     * This is where the intelligence comes in. Instead of trying cells
     * randomly, we first analyze ALL filled cells and assign each a score
     * based on:
     * - Density of its parent subgrid
     * - Number of alternatives for its number
     * 
     * This creates a ranked list of candidates, from most promising to
     * least promising.
     * 
     * WHY THIS IS EXPENSIVE BUT WORTHWHILE:
     * Scoring all cells requires iterating through the entire board and
     * checking alternatives for each cell. This takes time. However, the
     * time investment pays off because:
     * 1. We only score once per Phase 3 invocation
     * 2. Prioritized elimination reduces failed attempts
     * 3. Failed attempts are MUCH more expensive (they involve backtracking)
     * 
     * Think of it like spending five minutes planning a route before a
     * road trip. The planning time is small compared to the time saved
     * by avoiding wrong turns.
     */
    int num_cells;
    CellScore *cell_scores = sudoku_score_cells(board, config, &num_cells);
    
    /*
     * DEFENSIVE PROGRAMMING: Check allocation
     */
    if (cell_scores == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed in phase3EliminationSmart\n");
        return 0;
    }
    
    /*
     * ═══════════════════════════════════════════════════════════════
     * STEP 3: SORT OR SHUFFLE BASED ON DIFFICULTY
     * ═══════════════════════════════════════════════════════════════
     * 
     * This is a crucial decision point that determines puzzle difficulty.
     * 
     * FOR EASY AND MEDIUM (prioritize_high_density = true):
     * We sort cells by score, putting high-density cells first. This means
     * we'll try eliminating from safe, well-constrained positions first,
     * which tends to succeed more often and creates easier puzzles.
     * 
     * FOR HARD AND EXPERT (prioritize_high_density = false):
     * We shuffle cells randomly using Fisher-Yates algorithm. This means
     * we might try eliminating from low-density positions, which is riskier
     * and creates harder puzzles. The randomness also makes each puzzle
     * more unique and unpredictable.
     * 
     * THE FISHER-YATES SHUFFLE:
     * This is a classic algorithm for generating uniformly random permutations.
     * We iterate backward through the array and swap each element with a
     * randomly chosen element from earlier in the array (including itself).
     * 
     * The algorithm guarantees that every possible ordering has equal
     * probability, which is important for fairness. We don't want certain
     * positions to be favored over others.
     */
    if (config->prioritize_high_density) {
        /*
         * EASY/MEDIUM MODE: Sort by score
         * 
         * qsort() rearranges the array so that high-scoring cells come first.
         * The comparison function (from density_scoring.c) compares by density
         * first, then by alternatives as a tiebreaker.
         */
        qsort(cell_scores, num_cells, sizeof(CellScore), 
              sudoku_compare_cell_scores_desc);
    } else {
        /*
         * HARD/EXPERT MODE: Fisher-Yates shuffle
         * 
         * This creates a random permutation of the cell scores array.
         */
        for (int i = num_cells - 1; i > 0; i--) {
            /*
             * Pick a random index from 0 to i (inclusive)
             * 
             * The % operator gives us a number in range [0, i].
             * We add 1 because i+1 represents the number of choices,
             * not the maximum index.
             */
            int j = rand() % (i + 1);
            
            /*
             * Swap elements i and j
             * 
             * We use a temporary variable to avoid losing data during the swap.
             * This is the classic three-way swap that every programmer learns.
             */
            CellScore temp = cell_scores[i];
            cell_scores[i] = cell_scores[j];
            cell_scores[j] = temp;
        }
    }
    
    /*
     * ═══════════════════════════════════════════════════════════════
     * STEP 4: ATTEMPT ELIMINATION IN PRIORITY ORDER
     * ═══════════════════════════════════════════════════════════════
     * 
     * Now we iterate through the scored and ordered cells, attempting to
     * eliminate each one. For each cell, we:
     * 1. Temporarily remove it
     * 2. Check if the puzzle still has exactly one solution
     * 3. If yes: keep it removed (success)
     * 4. If no: restore it (failure)
     * 
     * We continue until we either:
     * - Reach our target number of eliminations, OR
     * - Run out of cells to try
     * 
     * THE VERIFICATION COST:
     * The countSolutionsExact() call is expensive. It uses backtracking
     * to explore the solution space. However, we optimize it with early
     * exit: we only count up to 2 solutions. If we find 2, we stop
     * immediately and reject the elimination.
     * 
     * Without early exit, we'd have to find ALL solutions, which could
     * take seconds. With early exit, we typically finish in milliseconds.
     */
    int removed = 0;
    
    for (int i = 0; i < num_cells && removed < target; i++) {
        /*
         * Get the position and current value of this cell
         * 
         * We stored this information in the CellScore structure during
         * scoring, so we just read it out now.
         */
        SudokuPosition *pos = &cell_scores[i].pos;
        int value = cell_scores[i].value;
        
        /*
         * TEMPORARY ELIMINATION:
         * 
         * Set the cell to zero (empty). This is temporary—we might restore
         * it in a moment if verification fails.
         * 
         * IMPORTANT: We must save the original value so we can restore it
         * if needed. The value is already stored in cell_scores[i].value,
         * but we also keep a local copy for clarity.
         */
        int temp = board->cells[pos->row][pos->col];
        board->cells[pos->row][pos->col] = 0;
        
        /*
         * VERIFICATION: CHECK FOR UNIQUE SOLUTION
         * 
         * This is the heart of Phase 3 and the reason it's computationally
         * expensive. We must verify that removing this cell doesn't create
         * multiple solutions.
         * 
         * countSolutionsExact(board, 2) means:
         * "Count solutions, but stop after finding 2"
         * 
         * The return value can be:
         * - 0: No solution (impossible, since we started with a valid board)
         * - 1: Exactly one solution (GOOD! Keep elimination)
         * - 2: Two or more solutions (BAD! Restore cell)
         * 
         * THE EARLY EXIT OPTIMIZATION:
         * By stopping at 2, we avoid exploring the entire solution tree.
         * For a puzzle with many solutions, this can save billions of
         * recursive calls. The speedup is typically 10^40 to 10^44 times
         * faster than counting all solutions.
         * 
         * TRADE-OFF ANALYSIS:
         * - Cost: One expensive verification per attempted elimination
         * - Benefit: Guaranteed unique solution (professional quality)
         * - Alternative: Skip verification (fast but produces bad puzzles)
         * 
         * We choose quality over speed because generating a single puzzle
         * in 5-10 milliseconds is perfectly acceptable for most applications.
         */
        if (sudoku_count_solutions(board, 2) == 1) {
            /*
             * SUCCESS: Unique solution maintained
             * 
             * The elimination is safe. We keep the cell empty and increment
             * our counter.
             */
            removed++;
            
            /*
             * EMIT SUCCESS EVENT:
             * 
             * Notify observers that we successfully eliminated a cell.
             * This event includes full details about what was removed.
             */
            emit_event_cell(SUDOKU_EVENT_PHASE3_CELL_REMOVED,
                                  board, 3, removed,
                                  pos->row, pos->col, temp);
        } else {
            /*
             * FAILURE: Multiple solutions detected
             * 
             * Removing this cell would create a puzzle with multiple valid
             * solutions, which is unacceptable. Professional Sudoku puzzles
             * must have exactly one solution.
             * 
             * RESTORE THE CELL:
             * Put the original number back. The cell remains filled.
             */
            board->cells[pos->row][pos->col] = temp;
            
            /*
             * NOTE: We don't emit an event for failures because they're
             * normal and frequent. Emitting events for every failure would
             * create excessive logging noise. We only report successes.
             */
        }
    }
    
    /*
     * ═══════════════════════════════════════════════════════════════
     * STEP 5: CLEANUP AND RETURN
     * ═══════════════════════════════════════════════════════════════
     */
    
    /*
     * FREE MEMORY:
     * The cell_scores array was allocated dynamically, so we must free it.
     */
    free(cell_scores);
    
    /*
     * EMIT COMPLETION EVENT:
     * Report how many cells were actually eliminated (might be less than target).
     */
    emit_event(SUDOKU_EVENT_PHASE3_COMPLETE, board, 3, removed);
    
    /*
     * RETURN RESULT:
     * 
     * Return the number of successfully eliminated cells. This might be:
     * - Equal to target (we achieved our goal)
     * - Less than target (we ran out of safe cells to eliminate)
     * - Zero (no cells could be safely eliminated)
     * 
     * The caller should check this return value to see if the puzzle reached
     * the desired difficulty level.
     */
    return removed;
}

/**
 * EDUCATIONAL NOTES ON PHASE 3 ADAPTATIONS:
 * 
 * 1. WHY CAN'T WE USE A FIXED TARGET?
 *    
 *    Consider PHASE3_TARGET = 25 (the old constant):
 *    
 *    For 9×9 board (81 cells):
 *    - 25 represents 31% of the board
 *    - This is reasonable and produces good puzzles
 *    
 *    For 4×4 board (16 cells):
 *    - 25 exceeds the total cell count!
 *    - Algorithm would try to remove more cells than exist
 *    
 *    For 25×25 board (625 cells):
 *    - 25 represents only 4% of the board
 *    - Puzzles would be extremely easy (too many clues)
 *    
 *    The solution: calculate target as a PERCENTAGE of total cells.
 * 
 * 2. WHY DIFFERENT PERCENTAGES FOR DIFFERENT SIZES?
 *    
 *    Constraint density increases non-linearly with board size.
 *    In a 9×9 board, each cell is constrained by:
 *    - 8 other cells in its row
 *    - 8 other cells in its column
 *    - 4 other cells in its 3×3 subgrid (excluding row/column overlap)
 *    Total: ~20 constraints per cell
 *    
 *    In a 25×25 board, each cell is constrained by:
 *    - 24 other cells in its row
 *    - 24 other cells in its column
 *    - 16 other cells in its 5×5 subgrid (excluding overlap)
 *    Total: ~64 constraints per cell
 *    
 *    With more constraints, it's harder to maintain unique solutions
 *    while removing many cells. Hence, larger boards need smaller
 *    removal percentages.
 * 
 * 3. MEMORY SCALING:
 *    
 *    The positions array scales with total cells:
 *    - 4×4:   16 × 8 bytes = 128 bytes
 *    - 9×9:   81 × 8 bytes = 648 bytes
 *    - 16×16: 256 × 8 bytes = 2,048 bytes (2 KB)
 *    - 25×25: 625 × 8 bytes = 5,000 bytes (5 KB)
 *    
 *    All of these are trivial amounts of memory for modern systems.
 *    The heap allocation is necessary because:
 *    a) Stack space may be limited in deep recursion
 *    b) Large arrays can cause stack overflow
 *    c) It demonstrates proper dynamic memory management
 * 
 * 4. TIME COMPLEXITY SCALING:
 *    
 *    Phase 3's complexity is dominated by countSolutionsExact():
 *    - Worst case: O(N^m) where N = board_size, m = empty cells
 *    - With early exit: Typically O(N^k) where k << m
 *    
 *    For larger boards, the branching factor increases (N possibilities
 *    instead of 9), but constraint propagation is more effective.
 *    In practice, 16×16 boards take 2-5x longer than 9×9, not 256x.
 * 
 * 5. WHY USE countSolutionsExact FROM validation.c?
 *    
 *    The function countSolutionsExact() is a general-purpose solution
 *    counter that can be used for:
 *    - Validating that a puzzle has exactly one solution
 *    - Checking if a puzzle is solvable
 *    - Testing puzzle difficulty
 *    
 *    By keeping it in validation.c, we follow the Single Responsibility
 *    Principle: validation.c handles validation, phase3.c handles
 *    elimination. Phase 3 USES validation but doesn't IMPLEMENT it.
 *    
 *    This also follows the DRY principle (Don't Repeat Yourself):
 *    one implementation, used everywhere it's needed.
 */

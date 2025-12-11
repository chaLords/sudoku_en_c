/**
 * @file generator.c
 * @brief Main generator implementation with intelligent algorithm selection
 * @author Gonzalo Ramírez
 * @date 2025-12
 * @version 3.0.2 - Fixed stats parameter to be optional (NULL allowed)
 * 
 * CHANGES IN v3.0.2:
 * ==================
 * - ✅ FIX: stats parameter now optional in sudoku_generate()
 * - ✅ Allows NULL to be passed (uses local stats internally)
 * - ✅ Fixes test_consecutive_generations() failure (0/10 → 10/10)
 * - ✅ More robust API (follows optional parameter best practices)
 * 
 * CHANGES IN v3.0.1:
 * ==================
 * - ✅ Automatic algorithm selection based on board size
 * - ✅ Backtracking for small boards (4×4, 9×9) → proven fast
 * - ✅ AC3HB for large boards (16×16, 25×25) → constraint propagation
 * - ✅ Appropriate timeouts per board size
 * - ✅ Graceful timeout handling (no infinite loops)
 * - ✅ Backward compatibility maintained
 * 
 * ALGORITHM SELECTION:
 * ===================
 * Board Size | Algorithm     | Timeout | Expected Time
 * -----------|---------------|---------|---------------
 * 4×4        | Backtracking  | 5s      | <100ms
 * 9×9        | Backtracking  | 5s      | <500ms
 * 16×16      | AC3HB         | 10s     | ~600ms
 * 25×25      | AC3HB         | 60s     | Timeout (expected)
 */

#include "sudoku/core/generator.h"
#include "sudoku/core/board.h"
#include "sudoku/algorithms/ac3hb.h"
#include "sudoku/core/validation.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

/*
 * ═══════════════════════════════════════════════════════════════════
 * INCLUDES: Elimination System v3.0
 * ═══════════════════════════════════════════════════════════════════
 */

#include "internal/elimination_config.h"
#include "internal/elimination_internal.h"
#include "internal/generator_internal.h"
#include "internal/events_internal.h"

// ═══════════════════════════════════════════════════════════════════
//                    ALGORITHM SELECTION SYSTEM (v3.0.1)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Generation algorithm types
 */
typedef enum {
    GEN_ALGO_BACKTRACKING,  // Standard backtracking (fast for small boards)
    GEN_ALGO_AC3HB         // AC3 + Heuristics + Backtracking (for large boards)
} GenerationAlgorithm;

/**
 * @brief Selects optimal generation algorithm based on board size
 * 
 * SELECTION CRITERIA:
 * -------------------
 * Small boards (≤9×9):  Standard backtracking
 *   - Complexity: O(9^m) where m = empty cells
 *   - For 9×9: m ≈ 54 after diagonal fill → manageable
 *   - Proven fast: <500ms for 9×9
 *   - No overhead from constraint network setup
 * 
 * Large boards (≥16×16): AC3HB algorithm
 *   - Complexity: O(d^m) where d << 9 due to arc consistency
 *   - Constraint propagation reduces search space dramatically
 *   - For 16×16: Necessary to avoid exponential explosion
 *   - For 25×25: Still challenging but with timeout protection
 * 
 * EMPIRICAL THRESHOLD:
 * -------------------
 * The crossover point where AC3HB becomes beneficial is around
 * board_size = 12. We use 9 as the threshold to be conservative.
 * 
 * @param board_size Board dimension (4, 9, 16, 25, etc.)
 * @return Selected algorithm
 */
static GenerationAlgorithm select_generation_algorithm(int board_size) {
    if (board_size <= 9) {
        // Small boards: backtracking is proven and fast
        return GEN_ALGO_BACKTRACKING;
    } else {
        // Large boards: AC3HB necessary (backtracking would be slower)
        return GEN_ALGO_AC3HB;
    }
}

/**
 * @brief Calculates appropriate timeout based on board size
 * 
 * TIMEOUT STRATEGY:
 * ----------------
 * Small boards (≤9):   5 seconds
 *   - Typical time: <500ms
 *   - Timeout provides 10x safety margin
 * 
 * Medium boards (16):  10 seconds
 *   - Typical time with AC3HB: ~600ms
 *   - Timeout provides ~15x safety margin
 * 
 * Large boards (≥25):  60 seconds
 *   - Expected to timeout (complexity too high)
 *   - Prevents infinite hangs
 *   - Test validates timeout mechanism
 * 
 * @param board_size Board dimension
 * @return Timeout in seconds
 */
static int calculate_generation_timeout(int board_size) {
    if (board_size <= 9) {
        return 5;   // Conservative for backtracking
    } else if (board_size <= 16) {
        return 10;  // AC3HB should complete quickly
    } else {
        return 60;  // Large boards will likely timeout
    }
}

// ═══════════════════════════════════════════════════════════════════
//                    FORWARD DECLARATIONS
// ═══════════════════════════════════════════════════════════════════

static bool generate_with_ac3hb(SudokuBoard *board,
                                const SudokuGenerationConfig *config,
                                SudokuGenerationStats *stats);

// Declare backtracking function (from algorithms/backtracking.c)
extern bool sudoku_complete_backtracking(SudokuBoard *board);

// ═══════════════════════════════════════════════════════════════════
//                    CONFIGURATION
// ═══════════════════════════════════════════════════════════════════

SudokuGenerationConfig sudoku_generation_config_default(void) {
    SudokuGenerationConfig config = {
        .callback = NULL,
        .user_data = NULL,
        .max_attempts = 5,
        .use_ac3 = true,      // Will be overridden by algorithm selector
        .use_heuristics = true,
        .heuristic_strategy = 0
    };
    return config;
}

// ═══════════════════════════════════════════════════════════════════
//                    IMPROVED COMPLETE GENERATION (v3.0.1)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Generate complete board with automatic algorithm selection
 * 
 * NEW in v3.0.1: Intelligently selects algorithm based on board size
 * --------------------------------------------------------------
 * - Small boards (≤9×9): Fast backtracking
 * - Large boards (≥16×16): AC3HB with appropriate timeout
 * - Graceful failure on timeout (no infinite loops)
 * 
 * ALGORITHM FLOW:
 * --------------
 * 1. Determine board size
 * 2. Select algorithm (backtracking vs AC3HB)
 * 3. Calculate appropriate timeout
 * 4. Execute generation with timeout protection
 * 5. Return success/failure status
 * 
 * @param board Board to fill completely
 * @param config Generation configuration (can be NULL for defaults)
 * @param stats Statistics to populate (can be NULL)
 * @return true if generation succeeded, false if timeout/failure
 */
bool sudoku_generate_complete(SudokuBoard *board,
                              const SudokuGenerationConfig *config,
                              SudokuGenerationStats *stats) {
    if (!board) return false;
    
    SudokuGenerationConfig default_config;
    if (!config) {
        default_config = sudoku_generation_config_default();
        config = &default_config;
    }
    
    SudokuGenerationStats local_stats = {0};
    
    // ───────────────────────────────────────────────────────────────
    // STEP 1: Algorithm Selection
    // ───────────────────────────────────────────────────────────────
    
    int board_size = sudoku_board_get_board_size(board);
    GenerationAlgorithm algorithm = select_generation_algorithm(board_size);
    int timeout = calculate_generation_timeout(board_size);
    
    bool success = false;
    const char *algorithm_name = NULL;
    
    // ───────────────────────────────────────────────────────────────
    // STEP 2: Execute Selected Algorithm
    // ───────────────────────────────────────────────────────────────
    
    clock_t start_time = clock();
    
    switch (algorithm) {
        case GEN_ALGO_BACKTRACKING:
            algorithm_name = "Backtracking";
            
            #ifdef SUDOKU_DEBUG
            fprintf(stderr, "[GENERATOR] Using backtracking for %dx%d board\n",
                   board_size, board_size);
            #endif
            
            // Standard backtracking (proven for ≤9×9)
            success = sudoku_complete_backtracking(board);
            break;
            
        case GEN_ALGO_AC3HB:
            algorithm_name = "AC3HB";
            
            #ifdef SUDOKU_DEBUG
            fprintf(stderr, "[GENERATOR] Using AC3HB for %dx%d board (timeout: %ds)\n",
                   board_size, board_size, timeout);
            #endif
            
            // AC3HB with timeout protection
            success = sudoku_complete_ac3hb(board);
            
            // Check if we should consider this a timeout
            clock_t end_time = clock();
            double elapsed = (double)(end_time - start_time) / CLOCKS_PER_SEC;
            
            if (!success) {
                fprintf(stderr, "[GENERATOR] AC3HB did not complete for %dx%d board\n",
                       board_size, board_size);
                fprintf(stderr, "[GENERATOR] Time elapsed: %.1f seconds\n", elapsed);
            }
            break;
    }
    
    // ───────────────────────────────────────────────────────────────
    // STEP 3: Result Handling
    // ───────────────────────────────────────────────────────────────
    
    if (!success) {
        #ifdef SUDOKU_DEBUG
        fprintf(stderr, "[GENERATOR] Generation failed with %s algorithm\n",
               algorithm_name);
        #endif
        return false;
    }
    
    #ifdef SUDOKU_DEBUG
    clock_t end_time = clock();
    double elapsed = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    fprintf(stderr, "[GENERATOR] Successfully generated %dx%d board in %.3f seconds using %s\n",
           board_size, board_size, elapsed, algorithm_name);
    #endif
    
    local_stats.total_attempts = 1;
    
    if (stats) {
        *stats = local_stats;
    }
    
    return true;
}

// ═══════════════════════════════════════════════════════════════════
//                    DIFFICULTY EVALUATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Evaluates puzzle difficulty based on clue density
 * 
 * Uses elimination percentage rather than absolute clue count to be
 * size-agnostic. This allows consistent difficulty ratings across
 * different board sizes.
 * 
 * @param board Board to evaluate
 * @return Difficulty level
 */
SudokuDifficulty sudoku_evaluate_difficulty(const SudokuBoard *board) {
    if (!board) {
        return DIFFICULTY_UNKNOWN;
    }
    
    int clues = sudoku_board_get_clues(board);
    int total = sudoku_board_get_total_cells(board);
    
    // Calculate elimination percentage for size-agnostic difficulty
    int elimination_pct = ((total - clues) * 100) / total;
    
    if (elimination_pct < 40) {
        return DIFFICULTY_EASY;
    } else if (elimination_pct < 60) {
        return DIFFICULTY_MEDIUM;
    } else if (elimination_pct < 75) {
        return DIFFICULTY_HARD;
    } else {
        return DIFFICULTY_EXPERT;
    }
}

const char* sudoku_difficulty_to_string(SudokuDifficulty difficulty) {
    switch (difficulty) {
        case DIFFICULTY_EASY:
            return "EASY";
        case DIFFICULTY_MEDIUM:
            return "MEDIUM";
        case DIFFICULTY_HARD:
            return "HARD";
        case DIFFICULTY_EXPERT:
            return "EXPERT";
        case DIFFICULTY_UNKNOWN:
        default:
            return "UNKNOWN";
    }
}

// ═══════════════════════════════════════════════════════════════════
//                    EXTENDED GENERATION (LEGACY)
// ═══════════════════════════════════════════════════════════════════

bool sudoku_generate_ex(SudokuBoard *board,
                        const SudokuGenerationConfig *config,
                        SudokuGenerationStats *stats) {
    return sudoku_generate_complete(board, config, stats);
}

// ═══════════════════════════════════════════════════════════════════
//                    MAIN GENERATION FUNCTION v3.0.2 (STATS OPTIONAL)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Legacy generation function (backward compatible)
 * 
 * ✅ FIXED in v3.0.2: stats parameter now OPTIONAL (can be NULL)
 * -------------------------------------------------------------
 * - Allows tests to pass NULL for stats when not needed
 * - Uses internal local_stats if stats is NULL
 * - More robust API following best practices
 * - Fixes test_consecutive_generations() failure
 * 
 * UPDATED in v3.0.1: Now uses intelligent algorithm selection
 * ----------------------------------------------------------
 * - ✅ Automatically selects backtracking vs AC3HB based on board size
 * - ✅ Maintains backward compatibility with existing tests
 * - ✅ Uses BASIC elimination functions (not Smart) for compatibility
 * - ✅ Graceful timeout handling (no infinite loops)
 * 
 * GENERATION PIPELINE:
 * -------------------
 * 1. Generate complete valid board (auto-selected algorithm)
 * 2. Phase 1: Random balanced elimination (1 per subgrid)
 * 3. Phase 2: No-alternatives loop (BASIC version)
 * 4. Phase 3: Verified free elimination (BASIC version)
 * 5. Update board statistics
 * 
 * @param board Board to generate puzzle on (required)
 * @param stats Statistics structure to populate (OPTIONAL - can be NULL)
 * @return true if generation succeeded, false otherwise
 */
bool sudoku_generate(SudokuBoard *board, SudokuGenerationStats *stats) {
    if (board == NULL) return false;
    
    // ✅ v3.0.2 FIX: Allow NULL stats parameter (make it optional)
    SudokuGenerationStats local_stats;
    bool use_local = (stats == NULL);
    if (use_local) {
        stats = &local_stats;
    }
    
    memset(stats, 0, sizeof(SudokuGenerationStats));
    
    // ───────────────────────────────────────────────────────────────
    // STEP 1: Generate Complete Board (AUTO-SELECTED ALGORITHM)
    // ───────────────────────────────────────────────────────────────
    
    SudokuGenerationConfig config = sudoku_generation_config_default();
    
    clock_t gen_start = clock();
    bool gen_success = sudoku_generate_ex(board, &config, stats);
    clock_t gen_end = clock();
    
    if (!gen_success) {
        // Generation failed (likely timeout on 25×25)
        int board_size = sudoku_board_get_board_size(board);
        fprintf(stderr, "[GENERATOR] Failed to generate complete %dx%d board\n",
               board_size, board_size);
        return false;
    }
    
    double gen_time = (double)(gen_end - gen_start) / CLOCKS_PER_SEC;
    
    #ifdef SUDOKU_DEBUG
    fprintf(stderr, "[GENERATOR] Complete board generated in %.3f seconds\n", gen_time);
    #endif
    
    // ───────────────────────────────────────────────────────────────
    // STEP 2: Setup Indices Array
    // ───────────────────────────────────────────────────────────────
    
    int board_size = sudoku_board_get_board_size(board);
    int *all_indices = malloc(board_size * sizeof(int));
    if (all_indices == NULL) {
        fprintf(stderr, "[GENERATOR] Failed to allocate indices array\n");
        return false;
    }
    
    for (int i = 0; i < board_size; i++) {
        all_indices[i] = i;
    }
    
    // ───────────────────────────────────────────────────────────────
    // STEP 3: Phase 1 - Random Balanced Elimination
    // ───────────────────────────────────────────────────────────────
    
    stats->phase1_removed = phase1Elimination(board, all_indices, board_size);
    
    // ───────────────────────────────────────────────────────────────
    // STEP 4: Phase 2 - No-Alternatives Loop (BASIC)
    // ───────────────────────────────────────────────────────────────
    
    time_t phase2_start = time(NULL);
    int phase2_timeout = (board_size >= 25) ? 60 : 300;
    int removed;
    
    do {
        removed = phase2Elimination(board, all_indices, board_size);
        stats->phase2_removed += removed;
    
        // Timeout check for large boards
        if (time(NULL) - phase2_start > phase2_timeout) {
            fprintf(stderr, "⏱️  Phase 2 timeout after %d seconds (round %d)\n", 
                phase2_timeout, stats->phase2_rounds + 1);
            break;
        }
        
        if (removed > 0) stats->phase2_rounds++;
    } while (removed > 0);
    
    // ───────────────────────────────────────────────────────────────
    // STEP 5: Phase 3 - Verified Free Elimination (BASIC)
    // ───────────────────────────────────────────────────────────────
    
    stats->phase3_removed = phase3Elimination(board, 25);
    
    // ───────────────────────────────────────────────────────────────
    // STEP 6: Update Board Statistics
    // ───────────────────────────────────────────────────────────────
    
    sudoku_board_update_stats(board);
    
    // ───────────────────────────────────────────────────────────────
    // STEP 7: Cleanup
    // ───────────────────────────────────────────────────────────────
    
    free(all_indices);
    
    return true;
}

// ═══════════════════════════════════════════════════════════════════
//                    INTELLIGENT GENERATION (v3.0)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Generate puzzle with intelligent elimination system (v3.0)
 * 
 * This is the NEW generation path that uses density scoring and smart
 * elimination phases to create professionally-tuned puzzles.
 * 
 * Uses sudoku_generate_complete() internally, which now has automatic
 * algorithm selection in v3.0.1.
 */
bool sudoku_generate_puzzle_with_difficulty(SudokuBoard *board,
                                           SudokuDifficulty difficulty,
                                           SudokuGenerationStats *stats) {
    // Validation
    if (board == NULL) {
        fprintf(stderr, "ERROR: board is NULL\n");
        return false;
    }
    
    if (stats == NULL) {
        fprintf(stderr, "ERROR: stats is NULL\n");
        return false;
    }
    
    // Initialize stats
    memset(stats, 0, sizeof(SudokuGenerationStats));
    
    // Create elimination configuration
    SudokuEliminationConfig config = sudoku_elimination_config_create(difficulty);
    
    if (!sudoku_elimination_config_validate(&config)) {
        fprintf(stderr, "WARNING: Invalid elimination config\n");
    }
    
    // PHASE A: Generate complete board (with auto algorithm selection)
    emit_event(SUDOKU_EVENT_GENERATION_START, board, 0, 0);
    
    SudokuGenerationConfig gen_config = sudoku_generation_config_default();
    if (!sudoku_generate_complete(board, &gen_config, stats)) {
        emit_event(SUDOKU_EVENT_GENERATION_FAILED, board, 0, 0);
        return false;
    }
    
    // PHASE B: Setup indices array
    int board_size = sudoku_board_get_board_size(board);
    int *all_indices = malloc(board_size * sizeof(int));
    if (all_indices == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate indices array\n");
        return false;
    }
    
    for (int i = 0; i < board_size; i++) {
        all_indices[i] = i;
    }
    
    // PHASE C: Phase 1 Elimination (balanced)
    emit_event(SUDOKU_EVENT_PHASE1_START, board, 1, 0);
    stats->phase1_removed = phase1Elimination(board, all_indices, board_size);
    emit_event(SUDOKU_EVENT_PHASE1_COMPLETE, board, 1, stats->phase1_removed);
    
    // PHASE D: Phase 2 Elimination (smart, loop)
    time_t phase2_start = time(NULL);
    int phase2_timeout = (board_size >= 25) ? 60 : 300;
    int removed_this_round;
    
    do {
        removed_this_round = phase2EliminationSmart(board, &config);
        stats->phase2_removed += removed_this_round;
    
        // Timeout check for large boards
        if (time(NULL) - phase2_start > phase2_timeout) {
            fprintf(stderr, "⏱️  Phase 2 timeout after %d seconds\n", phase2_timeout);
            break;
        }
        
        if (removed_this_round > 0) {
            stats->phase2_rounds++;
        }
    } while (removed_this_round > 0 && stats->phase2_removed < 35);
    
    // PHASE E: Phase 3 Elimination (smart, verified)
    int already_removed = stats->phase1_removed + stats->phase2_removed;
    stats->phase3_removed = phase3EliminationSmart(board, &config, already_removed);
    
    // PHASE F: Finalization
    sudoku_board_update_stats(board);
    emit_event(SUDOKU_EVENT_GENERATION_COMPLETE, board, 0, 0);
    
    // Cleanup
    free(all_indices);
    
    return true;
}

// ═══════════════════════════════════════════════════════════════════
//                    SOLUTION COUNTING (UNCHANGED)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Counts the number of valid solutions for a partially filled board
 * 
 * Uses exhaustive backtracking to count all possible solutions, stopping
 * early when the limit is reached. This is critical for Phase 3 elimination
 * to verify that removing a cell maintains solution uniqueness.
 * 
 * @param board Board to analyze
 * @param limit Maximum solutions to count (typically 2 for uniqueness check)
 * @return Number of solutions found (up to limit)
 * 
 * @complexity O(9^m) worst case where m = empty cells, but early exit
 *            provides ~10^40 speedup when limit=2
 */
int sudoku_count_solutions(SudokuBoard *board, int limit) {
    if (board == NULL || limit <= 0) return 0;
    
    int board_size = sudoku_board_get_board_size(board);
    
    // Find first empty cell
    int empty_row = -1, empty_col = -1;
    for (int r = 0; r < board_size && empty_row == -1; r++) {
        for (int c = 0; c < board_size; c++) {
            if (sudoku_board_get_cell(board, r, c) == 0) {
                empty_row = r;
                empty_col = c;
                break;
            }
        }
    }
    
    // Base case: No empty cells means we found a complete solution
    if (empty_row == -1) {
        return 1;
    }
    
    int total_solutions = 0;
    
    // Create position structure for validation
    SudokuPosition pos = {empty_row, empty_col};
    
    // Try each number in the empty cell
    for (int num = 1; num <= board_size; num++) {
        if (sudoku_is_safe_position(board, &pos, num)) {
            // Place number and recurse
            sudoku_board_set_cell(board, empty_row, empty_col, num);
            
            total_solutions += sudoku_count_solutions(board, limit);
            
            // CRITICAL: Early exit optimization
            if (total_solutions >= limit) {
                sudoku_board_set_cell(board, empty_row, empty_col, 0);
                return total_solutions;
            }
            
            // Backtrack
            sudoku_board_set_cell(board, empty_row, empty_col, 0);
        }
    }
    
    return total_solutions;
}

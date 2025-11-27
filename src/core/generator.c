/**
 * @file generator.c
 * @brief Main API for generating Sudoku puzzles
 * @author Gonzalo Ramírez
 * @date 2025-11-26
 * @version 3.0.2 - Added AC3HB branching architecture
 * 
 * This file provides the main API for generating Sudoku puzzles
 * using TWO algorithm paths:
 * 
 * 1. CLASSIC PATH (v2.2.1 compatible):
 *    - Fisher-Yates for diagonal subgrids
 *    - Standard backtracking for completion
 *    - Three-phase elimination
 * 
 * 2. AC3HB PATH (NEW in v3.0 - currently stub):
 *    - Fisher-Yates for diagonal subgrids
 *    - AC-3 constraint propagation
 *    - Backtracking with MRV/LCV heuristics
 *    - Three-phase elimination
 * 
 * CHANGELOG v3.0.2:
 * - Added branching architecture for AC3HB path
 * - Refactored existing code into generate_classic()
 * - Created stub for generate_with_ac3hb()
 * - Maintained 100% backward compatibility
 * 
 * VERSION 3.0.1 BUGFIX (maintained):
 * - Retry loop for diagonal filling + backtracking
 * - Fixes ~70% failure rate on 4×4 boards
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sudoku/core/generator.h"
#include "sudoku/core/board.h"
#include "sudoku/core/types.h"
#include "internal/board_internal.h"
#include "internal/algorithms_internal.h"
#include "internal/elimination_internal.h"
#include "internal/events_internal.h"

// ═══════════════════════════════════════════════════════════════════
//                    FORWARD DECLARATIONS (PRIVATE)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Generate puzzle using classic Fisher-Yates + Backtracking
 * 
 * This is the EXISTING v2.2.1 algorithm extracted into a separate function.
 * Maintains 100% backward compatibility with previous versions.
 */
static bool generate_classic(SudokuBoard *board,
                            const SudokuGenerationConfig *config,
                            SudokuGenerationStats *stats);

/**
 * @brief Generate puzzle using AC3HB algorithm (NEW in v3.0)
 * 
 * This is the NEW algorithm path that uses constraint propagation
 * and heuristic search. Currently implemented as stub.
 * 
 * @note Currently delegates to generate_classic()
 * @todo Implement full AC3HB path (Week 3 of roadmap)
 */
static bool generate_with_ac3hb(SudokuBoard *board,
                               const SudokuGenerationConfig *config,
                               SudokuGenerationStats *stats);

// ═══════════════════════════════════════════════════════════════════
//                    PUBLIC API IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Generate a playable Sudoku puzzle with unique solution
 * 
 * NEW in v3.0.2: This function now acts as a dispatcher that chooses
 * between two algorithm paths based on configuration:
 * 
 * - If config->use_ac3 == true  → Use AC3HB path (optimized for large boards)
 * - If config->use_ac3 == false → Use Classic path (v2.2.1 compatible)
 * - If config == NULL           → Use Classic path (default, safe)
 * 
 * @param[out] board Pointer to the board structure to fill
 * @param[in] config Configuration including algorithm selection
 * @param[out] stats Pointer to store generation statistics, or NULL
 * @return true if generation succeeded, false on failure
 */
bool sudoku_generate_ex(SudokuBoard *board,
                        const SudokuGenerationConfig *config,
                        SudokuGenerationStats *stats) {
    
    // ═══════════════════════════════════════════════════════════════
    // ✨ NEW v3.0.2: ALGORITHM PATH SELECTION
    // ═══════════════════════════════════════════════════════════════
    
    /*
     * ARCHITECTURAL DECISION: Branching Strategy
     * 
     * This dispatcher separates concerns:
     * - Public API remains stable
     * - Internal implementation can vary
     * - Configuration controls behavior
     * 
     * USAGE EXAMPLES:
     * 
     * // Default (Classic path):
     * sudoku_generate_ex(&board, NULL, &stats);
     * 
     * // Explicit Classic path:
     * SudokuGenerationConfig config = {.use_ac3 = false};
     * sudoku_generate_ex(&board, &config, &stats);
     * 
     * // AC3HB path (NEW):
     * SudokuGenerationConfig config = {.use_ac3 = true};
     * sudoku_generate_ex(&board, &config, &stats);
     */
    
    if (config != NULL && config->use_ac3) {
        // ✨ NEW: AC3HB generation path
        // Expected speedup: 30-60× for large boards
        return generate_with_ac3hb(board, config, stats);
    } else {
        // ✅ EXISTING: Classic Fisher-Yates + Backtracking
        // 100% backward compatible
        return generate_classic(board, config, stats);
    }
}

// ═══════════════════════════════════════════════════════════════════
//                    CLASSIC ALGORITHM PATH (v2.2.1)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief CLASSIC generation path - Fisher-Yates + Standard Backtracking
 * 
 * This function contains the ENTIRE v2.2.1 generation algorithm,
 * extracted from the original sudoku_generate_ex() for modularity.
 * 
 * NO CHANGES to algorithm logic - pure refactoring for branching.
 */
static bool generate_classic(SudokuBoard *board,
                            const SudokuGenerationConfig *config,
                            SudokuGenerationStats *stats) {
    
    // ═══════════════════════════════════════════════════════════════
    // STEP 0: Initialize board and extract dimensions
    // ═══════════════════════════════════════════════════════════════
    
    sudoku_board_init(board);
    
    int board_size = sudoku_board_get_board_size(board);
    int num_subgrids = board_size;
    
    // Initialize event system
    if (config != NULL && config->callback != NULL) {
        events_init(config->callback, config->user_data);
    } else {
        events_init(NULL, NULL);
    }
    
    emit_event(SUDOKU_EVENT_GENERATION_START, NULL, 0, 0);
    
    // Initialize statistics
    if (stats) {
        stats->phase1_removed = 0;
        stats->phase2_removed = 0;
        stats->phase2_rounds = 0;
        stats->phase3_removed = 0;
        stats->total_attempts = 1;
    }
    
    // ═══════════════════════════════════════════════════════════════
    // STEPS 1-2: Fill diagonal + Complete with backtracking (WITH RETRY)
    // ═══════════════════════════════════════════════════════════════
    
    /*
     * ✅ BUGFIX v3.0.1: Retry loop for small boards
     * 
     * Fisher-Yates can create incompletable configurations on small boards.
     * 4×4 has ~70% failure probability. We retry until success.
     */
    
    int max_attempts;
    switch (board_size) {
        case 4:
            max_attempts = 20;
            break;
        case 9:
            max_attempts = 5;
            break;
        case 16:
            max_attempts = 10;
            break;
        case 25:
            max_attempts = 15;
            break;
        default:
            max_attempts = 10;
            break;
    }
    
    bool generation_successful = false;
    int attempt;
    
    for (attempt = 0; attempt < max_attempts && !generation_successful; attempt++) {
        if (attempt > 0) {
            sudoku_board_init(board);
        }
        
        // STEP 1: Fill diagonal subgrids with Fisher-Yates
        fillDiagonal(board);
        
        // Emit diagonal complete event
        emit_event(SUDOKU_EVENT_DIAGONAL_FILL_COMPLETE, board, 0, 0);
        
        // STEP 2: Complete remaining cells with backtracking
        if (sudoku_complete_backtracking(board)) {
            generation_successful = true;
            
            // Emit backtracking complete event
            emit_event(SUDOKU_EVENT_BACKTRACK_COMPLETE, board, 0, 0);
            
            if (stats) {
                stats->total_attempts = attempt + 1;
            }
        }
    }
    
    if (!generation_successful) {
        fprintf(stderr, "❌ Error: Failed to complete board (size %d×%d) after %d attempts\n",
                board_size, board_size, max_attempts);
        emit_event(SUDOKU_EVENT_GENERATION_FAILED, board, attempt, 0);
        return false;
    }
    
    // ═══════════════════════════════════════════════════════════════
    // STEP 3: Allocate dynamic array for subgrid indices
    // ═══════════════════════════════════════════════════════════════
    
    int *subgrid_indices = (int *)malloc(num_subgrids * sizeof(int));
    if (subgrid_indices == NULL) {
        fprintf(stderr, "❌ Error: Memory allocation failed for subgrid indices\n");
        return false;
    }
    
    for (int i = 0; i < num_subgrids; i++) {
        subgrid_indices[i] = i;
    }
    
    // ═══════════════════════════════════════════════════════════════
    // PHASE 1: Remove one random number from each subgrid
    // ═══════════════════════════════════════════════════════════════
    
    sudoku_generate_permutation(subgrid_indices, num_subgrids, 0);
    
    int removed1 = phase1Elimination(board, subgrid_indices, num_subgrids);
    
    if (stats) {
        stats->phase1_removed = removed1;
    }
    
    // ═══════════════════════════════════════════════════════════════
    // PHASE 2: Remove numbers without alternatives (iterative)
    // ═══════════════════════════════════════════════════════════════
    
    sudoku_generate_permutation(subgrid_indices, num_subgrids, 0);
    
    int total_removed2 = 0;
    int rounds = 0;
    int removed_this_round;
    
    do {
        removed_this_round = phase2Elimination(board, subgrid_indices, num_subgrids);
        total_removed2 += removed_this_round;
        
        if (removed_this_round > 0) {
            rounds++;
            
            // Emit round complete event
            emit_event(SUDOKU_EVENT_PHASE2_ROUND_START, board, removed_this_round, rounds);
            
            sudoku_generate_permutation(subgrid_indices, num_subgrids, 0);
        }
    } while (removed_this_round > 0);
    
    if (stats) {
        stats->phase2_removed = total_removed2;
        stats->phase2_rounds = rounds;
    }
    
    // Emit phase 2 complete event
    emit_event(SUDOKU_EVENT_PHASE2_COMPLETE, board, total_removed2, rounds);
    
    // ═══════════════════════════════════════════════════════════════
    // CLEANUP: Free dynamically allocated memory
    // ═══════════════════════════════════════════════════════════════
    
    free(subgrid_indices);
    subgrid_indices = NULL;
    
    // ═══════════════════════════════════════════════════════════════
    // PHASE 3: Free elimination with uniqueness verification
    // ═══════════════════════════════════════════════════════════════
    
    int removed3 = phase3EliminationAuto(board);
    
    if (stats) {
        stats->phase3_removed = removed3;
    }
    
    // Emit phase 3 complete event
    emit_event(SUDOKU_EVENT_PHASE3_COMPLETE, board, removed3, 0);
    
    // ═══════════════════════════════════════════════════════════════
    // FINALIZATION: Update statistics and emit completion event
    // ═══════════════════════════════════════════════════════════════
    
    sudoku_board_update_stats(board);
    
    emit_event(SUDOKU_EVENT_GENERATION_COMPLETE, board, 0, 0);
    
    return true;
}

// ═══════════════════════════════════════════════════════════════════
//                    AC3HB ALGORITHM PATH (v3.0)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief AC3HB generation path - Constraint Propagation + Heuristic Search
 * 
 * ⚠️ CURRENT STATUS: STUB IMPLEMENTATION
 * 
 * This function is a placeholder for the future AC3HB implementation.
 * For now, it simply calls generate_classic() to maintain functionality.
 * 
 * PLANNED IMPLEMENTATION (Week 3 of roadmap):
 * 
 * 1. Fisher-Yates diagonal filling (SAME as classic)
 * 2. Build constraint network (NEW)
 * 3. AC-3 constraint propagation (NEW)
 * 4. Backtracking with heuristics (NEW)
 * 5. Three-phase elimination (SAME as classic)
 * 
 * EXPECTED BENEFITS:
 * - 30-60× speedup for 16×16 boards
 * - Makes 25×25 generation practical
 * 
 * @note Currently forwards to generate_classic()
 * @todo Implement full AC3HB algorithm (Week 3 roadmap)
 */
static bool generate_with_ac3hb(SudokuBoard *board,
                               const SudokuGenerationConfig *config,
                               SudokuGenerationStats *stats) {
    
    /*
     * ⚠️ STUB IMPLEMENTATION
     * 
     * For now, delegate to classic algorithm.
     * This allows compilation and testing while AC3HB is being developed.
     */
    
    // TODO: Implement AC3HB path
    // Week 1: AC-3 algorithm
    // Week 2: Heuristics (MRV, LCV, Degree)
    // Week 3: Integration
    
    return generate_classic(board, config, stats);
}

// ═══════════════════════════════════════════════════════════════════
//                    SIMPLE API (BACKWARD COMPATIBLE)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Generate a Sudoku puzzle (simple version without callbacks)
 * 
 * Convenience wrapper that calls sudoku_generate_ex() with no callback.
 * Maintains backward compatibility with existing code.
 */
bool sudoku_generate(SudokuBoard *board, SudokuGenerationStats *stats) {
    return sudoku_generate_ex(board, NULL, stats);
}

// ═══════════════════════════════════════════════════════════════════
//                    DIFFICULTY-TARGETED GENERATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Generate a Sudoku puzzle targeting a specific difficulty level
 * 
 * @note Currently generates normally - difficulty targeting not yet implemented
 * @todo Adjust elimination parameters based on target difficulty
 */
bool sudoku_generate_with_difficulty(SudokuBoard *board,
                                    SudokuDifficulty difficulty,
                                    SudokuGenerationStats *stats) {
    bool result = sudoku_generate(board, stats);
    
    (void)difficulty;  // Suppress unused parameter warning
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════
//                    DIFFICULTY EVALUATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Evaluate the difficulty level of a puzzle
 * 
 * Analyzes based on clue percentage. Thresholds scale with board size:
 * - EASY:   ≥55% filled
 * - MEDIUM: ≥43% filled
 * - HARD:   ≥31% filled
 * - EXPERT: <31% filled
 */
SudokuDifficulty sudoku_evaluate_difficulty(const SudokuBoard *board) {
    int clues = sudoku_board_get_clues(board);
    int board_size = sudoku_board_get_board_size(board);
    int total_cells = board_size * board_size;
    
    int easy_threshold = (int)(total_cells * 0.55 + 0.5);
    int medium_threshold = (int)(total_cells * 0.43 + 0.5);
    int hard_threshold = (int)(total_cells * 0.31 + 0.5);
    
    if (clues >= easy_threshold) {
        return SUDOKU_EASY;
    } else if (clues >= medium_threshold) {
        return SUDOKU_MEDIUM;
    } else if (clues >= hard_threshold) {
        return SUDOKU_HARD;
    } else {
        return SUDOKU_EXPERT;
    }
}

/**
 * @brief Convert difficulty level to human-readable string
 */
const char* sudoku_difficulty_to_string(SudokuDifficulty difficulty) {
    switch (difficulty) {
        case SUDOKU_EASY:
            return "EASY";
        case SUDOKU_MEDIUM:
            return "MEDIUM";
        case SUDOKU_HARD:
            return "HARD";
        case SUDOKU_EXPERT:
            return "EXPERT";
        default:
            return "UNKNOWN";
    }
}

/*
 * ═══════════════════════════════════════════════════════════════════
 *                    VERSION 3.0.2 CHANGELOG
 * ═══════════════════════════════════════════════════════════════════
 * 
 * NEW: Added AC3HB branching architecture
 * 
 * ARCHITECTURAL CHANGES:
 * - Refactored sudoku_generate_ex() into dispatcher
 * - Extracted v2.2.1 logic into generate_classic()
 * - Created generate_with_ac3hb() stub
 * 
 * BACKWARD COMPATIBILITY:
 * - NULL config or config->use_ac3 = false → Classic path
 * - config->use_ac3 = true → AC3HB path (stub)
 * - All existing code works unchanged
 * 
 * NEXT STEPS:
 * Week 1: Implement AC-3 algorithm
 * Week 2: Implement heuristics
 * Week 3: Complete generate_with_ac3hb()
 * Week 4: Testing and optimization
 * 
 * ═══════════════════════════════════════════════════════════════════
 */

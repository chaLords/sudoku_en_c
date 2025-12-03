/**
 * @file generator.c
 * @brief Main generator implementation
 * @author Gonzalo Ramírez
 * @date 2025-12
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
 * NUEVO EN v3.0: Sistema de Eliminación Inteligente
 * ═══════════════════════════════════════════════════════════════════
 */

#include "internal/elimination_config.h"
#include "internal/elimination_internal.h"
#include "internal/generator_internal.h"
#include "internal/events_internal.h"

// ═══════════════════════════════════════════════════════════════════
//                    FORWARD DECLARATIONS
// ═══════════════════════════════════════════════════════════════════

static bool generate_with_ac3hb(SudokuBoard *board,
                                const SudokuGenerationConfig *config,
                                SudokuGenerationStats *stats);

// ═══════════════════════════════════════════════════════════════════
//                    CONFIGURATION
// ═══════════════════════════════════════════════════════════════════

SudokuGenerationConfig sudoku_generation_config_default(void) {
    SudokuGenerationConfig config = {
        .callback = NULL,
        .user_data = NULL,
        .max_attempts = 5,
        .use_ac3 = true,
        .use_heuristics = true,
        .heuristic_strategy = 0
    };
    return config;
}

// ═══════════════════════════════════════════════════════════════════
//                    MAIN GENERATION FUNCTION
// ═══════════════════════════════════════════════════════════════════

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
    
    bool success = false;
    if (config->use_ac3) {
        success = sudoku_complete_ac3hb(board);
    } else {
        success = sudoku_complete_backtracking(board);
    }
    
    if (!success) {
        return false;
    }
    
    local_stats.total_attempts = 1;
    
    if (stats) {
        *stats = local_stats;
    }
    
    return true;
}

// ═══════════════════════════════════════════════════════════════════
//                    DIFFICULTY EVALUATION
// ═══════════════════════════════════════════════════════════════════

SudokuDifficulty sudoku_evaluate_difficulty(const SudokuBoard *board) {
    if (!board) {
        return DIFFICULTY_UNKNOWN;
    }
    
    int clues = sudoku_board_get_clues(board);
    
    if (clues >= 45) {
        return DIFFICULTY_EASY;
    } else if (clues >= 35) {
        return DIFFICULTY_MEDIUM;
    } else if (clues >= 25) {
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

/**
 * @brief Legacy generation function (backward compatible)
 * 
 * This is the original API that existing code depends on. It uses the
 * BASIC elimination functions (not Smart) to maintain compatibility
 * with existing tests and behavior.
 * 
 * @param board Board to generate puzzle on
 * @param stats Statistics structure to populate
 * @return true if generation succeeded, false otherwise
 */
bool sudoku_generate(SudokuBoard *board, SudokuGenerationStats *stats) {
    if (board == NULL || stats == NULL) return false;
    
    memset(stats, 0, sizeof(SudokuGenerationStats));
    
    // Step 1: Generate complete board using default config
    SudokuGenerationConfig config = sudoku_generation_config_default();
    if (!sudoku_generate_ex(board, &config, stats)) {
        return false;
    }
    
    // Step 2: Setup indices array for elimination phases
    int board_size = sudoku_board_get_board_size(board);
    int *all_indices = malloc(board_size * sizeof(int));
    if (all_indices == NULL) {
        return false;
    }
    
    for (int i = 0; i < board_size; i++) {
        all_indices[i] = i;
    }
    
    // Step 3: Phase 1 - Random balanced elimination
    stats->phase1_removed = phase1Elimination(board, all_indices, board_size);
    
    // Step 4: Phase 2 - No-alternatives loop (BASIC, not Smart)
    int removed;
    do {
        removed = phase2Elimination(board, all_indices, board_size);
        stats->phase2_removed += removed;
        if (removed > 0) stats->phase2_rounds++;
    } while (removed > 0);
    
    // Step 5: Phase 3 - Verified free elimination (BASIC, not Smart)
    int already_removed = stats->phase1_removed + stats->phase2_removed;
    stats->phase3_removed = phase3Elimination(board, 25);
    
    // Step 6: Update board statistics
    sudoku_board_update_stats(board);
    
    // Step 7: Cleanup
    free(all_indices);
    
    return true;
}// ═══════════════════════════════════════════════════════════════════
//                    INTELLIGENT GENERATION (v3.0)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Generate puzzle with intelligent elimination system (v3.0)
 * 
 * This is the NEW generation path that uses density scoring and smart
 * elimination phases to create professionally-tuned puzzles.
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
    
    // PHASE A: Generate complete board
    emit_event(SUDOKU_EVENT_GENERATION_START, board, 0, 0);
    
    if (!sudoku_complete_ac3hb(board)) {
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
    int removed_this_round;
    do {
        removed_this_round = phase2EliminationSmart(board, &config);
        stats->phase2_removed += removed_this_round;
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
    
    // Try each number 1-9 in the empty cell
    for (int num = 1; num <= board_size; num++) {
        if (sudoku_is_safe_position(board, &pos, num)) {
            // Place number and recurse
            sudoku_board_set_cell(board, empty_row, empty_col, num);
            
            total_solutions += sudoku_count_solutions(board, limit);
            
            // CRITICAL: Early exit optimization
            // If we've found enough solutions, stop searching
            if (total_solutions >= limit) {
                sudoku_board_set_cell(board, empty_row, empty_col, 0);
                return total_solutions;
            }
            
            // Backtrack: remove the number
            sudoku_board_set_cell(board, empty_row, empty_col, 0);
        }
    }
    
    return total_solutions;
}

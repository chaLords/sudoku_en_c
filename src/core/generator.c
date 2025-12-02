/**
 * @file generator.c
 * @brief Main generator implementation
 * @author Gonzalo Ramírez
 * @date 2025-12
 */

#include "sudoku/core/generator.h"
#include "sudoku/core/board.h"
#include "sudoku/algorithms/ac3hb.h"  // ← AGREGAR ESTE INCLUDE
#include <stdio.h>
#include <time.h>

// ═══════════════════════════════════════════════════════════════════
//                    FORWARD DECLARATIONS
// ═══════════════════════════════════════════════════════════════════

// Declaración de función interna (implementada en otro archivo)
static bool generate_with_ac3hb(SudokuBoard *board,
                                const SudokuGenerationConfig *config,
                                SudokuGenerationStats *stats);
/**
 * @brief Get default generation configuration
 * 
 * Returns a configuration with sensible defaults:
 * - No event callback
 * - 5 maximum attempts
 * - AC3 enabled
 * - Heuristics enabled
 * - Default heuristic strategy
 */
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

/**
 * @brief Simple generation function with default configuration
 * 
 * This is a convenience wrapper around sudoku_generate_ex() that uses
 * default configuration settings. Provided for backward compatibility
 * with existing test code and for simple use cases.
 * 
 * @param board Board to fill with generated puzzle
 * @param stats Optional statistics structure (can be NULL)
 * @return true if generation succeeded, false otherwise
 * 
 * @note For advanced configuration (custom callbacks, AC3 settings,
 *       heuristics), use sudoku_generate_ex() instead.
 */
bool sudoku_generate(SudokuBoard *board, SudokuGenerationStats *stats) {
    SudokuGenerationConfig config = sudoku_generation_config_default();
    return sudoku_generate_ex(board, &config, stats);
}

// ═══════════════════════════════════════════════════════════════════
//                    MAIN GENERATION FUNCTION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Generate a complete valid Sudoku board
 */
bool sudoku_generate_complete(SudokuBoard *board,
                              const SudokuGenerationConfig *config,
                              SudokuGenerationStats *stats) {
    if (!board) return false;
    
    // Use default config if not provided
    SudokuGenerationConfig default_config;
    if (!config) {
        default_config = sudoku_generation_config_default();
        config = &default_config;
    }
    
    // Initialize stats
    SudokuGenerationStats local_stats = {0};
    clock_t start = clock();
    
    // Select strategy
    bool success = false;
    // ✅ CÓDIGO CORREGIDO:
    if (config->use_ac3) {
        // Usar AC3HB
        if (!sudoku_complete_ac3hb(board)) {
            return false;
        }
    } else {
        // Usar backtracking clásico
        if (!sudoku_complete_backtracking(board)) {
            return false;
        }
    }
        
    // ✅ CORRECTO:
    // No hay campo time_ms general en SudokuGenerationStats
    // Solo hay ac3_time_ms y heuristic_time_ms
    // Y en lugar de attempts_used, usar total_attempts
    local_stats.total_attempts = 1;    // Copy stats if requested

    if (stats) {
        *stats = local_stats;
    }
    
    return success;
}

// ═══════════════════════════════════════════════════════════════════
//                    DIFFICULTY EVALUATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Evaluate puzzle difficulty based on clue count
 */
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

/**
 * @brief Convert difficulty enum to string
 */
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
//                    EXTENDED GENERATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Extended generation function (wrapper for compatibility)
 * 
 * This function is equivalent to sudoku_generate_complete() but
 * uses the naming convention expected by some CLI tools.
 */
bool sudoku_generate_ex(SudokuBoard *board,
                        const SudokuGenerationConfig *config,
                        SudokuGenerationStats *stats) {
    // Simply call the main generation function
    return sudoku_generate_complete(board, config, stats);
}

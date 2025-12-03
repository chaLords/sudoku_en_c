/**
 * @file elimination_config.c
 * @brief Implementation of elimination configuration system
 */

#include "elimination_config.h"
#include "sudoku/core/board.h"
#include <stdio.h>

/**
 * IMPLEMENTATION NOTE: Configuration Percentages
 * 
 * The percentages in this function were carefully derived from the original
 * 9×9 three-phase elimination system. Here's the mathematical derivation:
 * 
 * ORIGINAL 9×9 SYSTEM:
 * - Professional easy Sudoku: 36-46 clues
 * - For 81 cells, this means eliminating 35-45 cells
 * - Percentage: (35/81) to (45/81) = 43% to 56%
 * 
 * SIMILARLY:
 * - MEDIUM: 32-35 clues → eliminate 46-49 → 57% to 60%
 * - HARD: 28-31 clues → eliminate 50-53 → 62% to 65%
 * - EXPERT: 22-27 clues → eliminate 54-59 → 67% to 73%
 * 
 * These percentages now apply proportionally to ANY board size.
 */

SudokuEliminationConfig sudoku_elimination_config_create(SudokuDifficulty difficulty) {
    // Create an empty configuration structure
    SudokuEliminationConfig config;
    
    // Store the difficulty level (useful for debugging and logging)
    config.difficulty = difficulty;
    
    // Always use density scoring for optimal results
    config.use_density_scoring = true;
    
    // Configure based on difficulty level
    switch(difficulty) {
        case DIFFICULTY_EASY:
            /*
             * EASY CONFIGURATION
             * 
             * Target: 36-46 clues for 9×9 (professional easy standard)
             * Elimination: 43-56% of cells
             * 
             * STRATEGY:
             * - Prioritize high-density subgrids (safer elimination)
             * - More conservative approach
             * - Leaves more solving paths for the player
             * 
             * EXPECTED RESULTS:
             * - 4×4: 7-9 clues
             * - 9×9: 36-46 clues
             * - 16×16: 113-146 clues
             * - 25×25: 275-356 clues
             */
            config.min_elimination_percent = 0.43f;
            config.max_elimination_percent = 0.56f;
            config.prioritize_high_density = true;
            break;
            
        case DIFFICULTY_MEDIUM:
            /*
             * MEDIUM CONFIGURATION
             * 
             * Target: 32-35 clues for 9×9 (professional medium standard)
             * Elimination: 57-60% of cells
             * 
             * STRATEGY:
             * - Still prioritize high-density subgrids
             * - More aggressive than EASY but still strategic
             * - Balanced difficulty
             * 
             * EXPECTED RESULTS:
             * - 4×4: 6-7 clues
             * - 9×9: 32-35 clues
             * - 16×16: 102-110 clues
             * - 25×25: 250-269 clues
             */
            config.min_elimination_percent = 0.57f;
            config.max_elimination_percent = 0.60f;
            config.prioritize_high_density = true;
            break;
            
        case DIFFICULTY_HARD:
            /*
             * HARD CONFIGURATION
             * 
             * Target: 28-31 clues for 9×9 (professional hard standard)
             * Elimination: 62-65% of cells
             * 
             * STRATEGY:
             * - NO density prioritization (random order)
             * - More unpredictable elimination pattern
             * - Challenges advanced players
             * 
             * EXPECTED RESULTS:
             * - 4×4: 5-6 clues
             * - 9×9: 28-31 clues
             * - 16×16: 90-97 clues
             * - 25×25: 219-244 clues
             */
            config.min_elimination_percent = 0.62f;
            config.max_elimination_percent = 0.65f;
            config.prioritize_high_density = false;  // Random order
            break;
            
        case DIFFICULTY_EXPERT:
            /*
             * EXPERT CONFIGURATION
             * 
             * Target: 22-27 clues for 9×9 (professional expert standard)
             * Elimination: 67-73% of cells
             * 
             * STRATEGY:
             * - NO density prioritization (maximum randomness)
             * - Very aggressive elimination
             * - Minimal clues while maintaining unique solution
             * - For expert solvers only
             * 
             * EXPECTED RESULTS:
             * - 4×4: 4-5 clues
             * - 9×9: 22-27 clues
             * - 16×16: 69-84 clues
             * - 25×25: 169-206 clues
             */
            config.min_elimination_percent = 0.67f;
            config.max_elimination_percent = 0.73f;
            config.prioritize_high_density = false;  // Random order
            break;
            
        case DIFFICULTY_UNKNOWN:
        default:
            /*
             * DEFAULT TO MEDIUM
             * 
             * If an invalid difficulty is passed (shouldn't happen in normal use),
             * we default to MEDIUM as a safe middle ground.
             * 
             * This is defensive programming: instead of crashing or producing
             * nonsense, we degrade gracefully to a reasonable default.
             */
            config.min_elimination_percent = 0.57f;
            config.max_elimination_percent = 0.60f;
            config.prioritize_high_density = true;
            
            // Log a warning (in production code, this would use a proper logging system)
            fprintf(stderr, "WARNING: Unknown difficulty level, defaulting to MEDIUM\n");
            break;
    }
    
    return config;
}

int sudoku_elimination_calculate_target(const SudokuBoard *board,
                                       const SudokuEliminationConfig *config,
                                       int already_removed) {
    /*
     * STEP 1: Get board dimensions
     * 
     * For a 9×9 board: board_size = 9
     * For a 16×16 board: board_size = 16
     */
    int board_size = sudoku_board_get_board_size(board);
    
    /*
     * STEP 2: Calculate total cells
     * 
     * This is simply N², where N is the board size.
     * 
     * Examples:
     * - 4×4: 16 cells
     * - 9×9: 81 cells
     * - 16×16: 256 cells
     * - 25×25: 625 cells
     */
    int total_cells = board_size * board_size;
    
    /*
     * STEP 3: Calculate target percentage
     * 
     * We use the AVERAGE of min and max percentages as our target.
     * This gives us a single target number to aim for.
     * 
     * WHY AVERAGE:
     * The min/max range represents acceptable variation due to board structure.
     * The average is a reasonable middle ground that Phase 3 should try to hit.
     * 
     * Example for EASY:
     * target_percent = (0.43 + 0.56) / 2.0 = 0.495 (49.5%)
     */
    float target_percent = (config->min_elimination_percent + 
                           config->max_elimination_percent) / 2.0f;
    
    /*
     * STEP 4: Calculate target number of empty cells
     * 
     * Convert the percentage to an absolute number of cells.
     * We cast to int to get a whole number (can't eliminate 0.5 cells).
     * 
     * Example for 9×9 EASY:
     * target_empty = (int)(81 × 0.495) = (int)(40.095) = 40 cells
     */
    int target_empty = (int)(total_cells * target_percent);
    
    /*
     * STEP 5: Calculate how many MORE cells to eliminate
     * 
     * Phase 1 and Phase 2 have already eliminated some cells.
     * Phase 3 needs to eliminate the difference.
     * 
     * Example:
     * - target_empty = 40 cells total
     * - already_removed = 22 cells (9 in Phase 1, 13 in Phase 2)
     * - additional = 40 - 22 = 18 cells more
     */
    int additional = target_empty - already_removed;
    
    /*
     * STEP 6: Return result (minimum 0)
     * 
     * If Phases 1 and 2 already eliminated enough (or too many) cells,
     * we return 0, indicating Phase 3 should not attempt further elimination.
     * 
     * This is defensive programming: we never return negative numbers.
     */
    return (additional > 0) ? additional : 0;
}

bool sudoku_elimination_config_validate(const SudokuEliminationConfig *config) {
    /*
     * VALIDATION CHECK 1: Non-negative minimum
     * 
     * You can't eliminate a negative number of cells.
     * If min_percent < 0, something is seriously wrong.
     */
    if (config->min_elimination_percent < 0.0f) {
        fprintf(stderr, "ERROR: min_elimination_percent cannot be negative (got %.2f)\n",
                config->min_elimination_percent);
        return false;
    }
    
    /*
     * VALIDATION CHECK 2: Maximum doesn't exceed 100%
     * 
     * You can't eliminate more than all the cells.
     * If max_percent > 1.0, something is wrong.
     */
    if (config->max_elimination_percent > 1.0f) {
        fprintf(stderr, "ERROR: max_elimination_percent cannot exceed 1.0 (got %.2f)\n",
                config->max_elimination_percent);
        return false;
    }
    
    /*
     * VALIDATION CHECK 3: Valid range
     * 
     * The minimum must not exceed the maximum.
     * This is a fundamental property of any range.
     */
    if (config->min_elimination_percent > config->max_elimination_percent) {
        fprintf(stderr, "ERROR: min_elimination_percent (%.2f) exceeds max (%.2f)\n",
                config->min_elimination_percent,
                config->max_elimination_percent);
        return false;
    }
    
    /*
     * VALIDATION CHECK 4: Minimum difficulty threshold
     * 
     * Eliminating less than 20% of cells produces puzzles that are too easy
     * to be interesting. This is a soft limit based on empirical observation.
     * 
     * For 9×9: 20% = 16 cells eliminated → 65 clues (extremely easy)
     */
    if (config->min_elimination_percent < 0.20f) {
        fprintf(stderr, "WARNING: min_elimination_percent (%.2f) is very low, "
                        "puzzle may be trivially easy\n",
                config->min_elimination_percent);
        // This is a warning, not an error, so we continue
    }
    
    /*
     * VALIDATION CHECK 5: Maximum difficulty threshold
     * 
     * Eliminating more than 85% of cells often leads to puzzles with
     * multiple solutions or no solution. This is a safety limit.
     * 
     * For 9×9: 85% = 69 cells eliminated → 12 clues (often unsolvable uniquely)
     */
    if (config->max_elimination_percent > 0.85f) {
        fprintf(stderr, "WARNING: max_elimination_percent (%.2f) is very high, "
                        "puzzle may have multiple solutions\n",
                config->max_elimination_percent);
        // This is a warning, not an error, so we continue
    }
    
    // All critical checks passed
    return true;
}

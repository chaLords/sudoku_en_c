/**
 * @file elimination_config.h
 * @brief Configuration system for intelligent cell elimination with difficulty levels
 * 
 * This module provides a professional configuration system that adapts elimination
 * strategies based on desired puzzle difficulty. The percentages are carefully
 * calculated to maintain proportionality across different board sizes (4×4, 9×9, 16×16, 25×25).
 * 
 * DESIGN PHILOSOPHY:
 * Instead of hardcoding elimination targets, we use percentage-based configuration
 * that scales naturally with board size. This makes the code maintainable and
 * extensible for future board sizes.
 * 
 * PERCENTAGE BASIS (from original 9×9 system):
 * - EASY:   36-46 clues (eliminates 43-56% of cells)
 * - MEDIUM: 32-35 clues (eliminates 57-60% of cells)
 * - HARD:   28-31 clues (eliminates 62-65% of cells)
 * - EXPERT: 22-27 clues (eliminates 67-73% of cells)
 * 
 * These percentages were derived from the original 3-phase system behavior
 * on 9×9 boards and are now applied proportionally to all board sizes.
 * 
 * @author Gonzalo Ramírez (@chaLords)
 * @date 2025-01-XX
 * @version 3.0.0
 */

#ifndef SUDOKU_ELIMINATION_CONFIG_H
#define SUDOKU_ELIMINATION_CONFIG_H

#include "sudoku/core/types.h"
#include <stdbool.h>

/**
 * @brief Configuration structure for elimination behavior
 * 
 * This structure encapsulates all parameters that control how the elimination
 * system behaves. By bundling these parameters together, we follow the 
 * "data cohesion" principle from software engineering: related data belongs together.
 * 
 * EXAMPLE USAGE:
 * ```c
 * EliminationConfig config = sudoku_elimination_config_create(DIFFICULTY_EASY);
 * // config.min_elimination_percent = 0.43f
 * // config.max_elimination_percent = 0.56f
 * // config.prioritize_high_density = true
 * ```
 */
typedef struct {
    /**
     * @brief The target difficulty level
     * 
     * This is primarily for documentation purposes. The actual behavior is
     * controlled by the percentage fields below, but storing the difficulty
     * helps with debugging and logging.
     */
    SudokuDifficulty difficulty;
    
    /**
     * @brief Minimum percentage of cells to eliminate (0.0 to 1.0)
     * 
     * INTERPRETATION:
     * - 0.43 means "eliminate at least 43% of the board"
     * - For a 9×9 board (81 cells): 81 × 0.43 = 35 cells minimum
     * - For a 16×16 board (256 cells): 256 × 0.43 = 110 cells minimum
     * 
     * WHY MINIMUM/MAXIMUM RANGES:
     * Sudoku generation has inherent randomness. Some boards naturally allow
     * more elimination than others due to their structure. The range gives
     * flexibility while maintaining difficulty targets.
     */
    float min_elimination_percent;
    
    /**
     * @brief Maximum percentage of cells to eliminate (0.0 to 1.0)
     * 
     * INTERPRETATION:
     * - 0.56 means "eliminate at most 56% of the board"
     * - For a 9×9 board: 81 × 0.56 = 45 cells maximum
     * - For a 16×16 board: 256 × 0.56 = 143 cells maximum
     * 
     * The actual elimination percentage will fall between min and max based
     * on the board's structure and verification results.
     */
    float max_elimination_percent;
    
    /**
     * @brief Whether to use density-based scoring for cell selection
     * 
     * WHAT IS DENSITY SCORING:
     * Density scoring calculates how "full" each subgrid is and uses this
     * information to prioritize which cells to eliminate. Cells in denser
     * subgrids are easier to eliminate while maintaining solution uniqueness.
     * 
     * WHY IT MATTERS:
     * - With scoring: More strategic, produces higher-quality puzzles
     * - Without scoring: Random order, less predictable results
     * 
     * Currently always set to true, but kept as a flag for future flexibility.
     */
    bool use_density_scoring;
    
    /**
     * @brief Whether to prioritize high-density subgrids first
     * 
     * STRATEGIC DIFFERENCE:
     * - true (EASY/MEDIUM): Process high-density subgrids first
     *   → Safer elimination, maintains more solving paths
     *   → Results in easier puzzles with more clues
     * 
     * - false (HARD/EXPERT): Process subgrids in random order
     *   → More aggressive elimination, fewer solving paths
     *   → Results in harder puzzles with fewer clues
     * 
     * ANALOGY:
     * Think of building a house. High-density prioritization is like removing
     * non-structural walls first (safe). Random order is like removing walls
     * randomly, which might remove load-bearing ones (risky, harder to solve).
     */
    bool prioritize_high_density;
    
} SudokuEliminationConfig;

/**
 * @brief Creates an elimination configuration for a specific difficulty level
 * 
 * This is the primary "factory function" for creating configurations. It
 * encapsulates all the knowledge about what percentages and settings are
 * appropriate for each difficulty level.
 * 
 * FACTORY PATTERN:
 * Instead of making the user manually set all fields, we provide a function
 * that creates correctly-configured objects. This is a fundamental design
 * pattern in professional software engineering.
 * 
 * @param difficulty The target difficulty level (EASY, MEDIUM, HARD, EXPERT)
 * @return A fully-configured EliminationConfig structure
 * 
 * @example
 * ```c
 * // Create configuration for an easy puzzle
 * SudokuEliminationConfig config = sudoku_elimination_config_create(DIFFICULTY_EASY);
 * 
 * // The config is now ready to use with phase2EliminationSmart() and phase3EliminationSmart()
 * phase2EliminationSmart(board, &config);
 * ```
 * 
 * @note This function never fails - it always returns a valid configuration.
 *       If an unknown difficulty is passed, it defaults to MEDIUM.
 */
SudokuEliminationConfig sudoku_elimination_config_create(SudokuDifficulty difficulty);

/**
 * @brief Calculates the dynamic elimination target for Phase 3
 * 
 * Phase 3 is responsible for filling the gap between what Phases 1 and 2
 * achieved and what the difficulty configuration requires. This function
 * performs that calculation.
 * 
 * CALCULATION LOGIC:
 * 1. Determine total cells in the board (N²)
 * 2. Calculate target percentage (average of min and max)
 * 3. Compute total cells to eliminate: N² × target_percent
 * 4. Subtract already eliminated cells
 * 5. Return the difference (minimum 0)
 * 
 * EXAMPLE:
 * ```
 * Board: 9×9 (81 cells)
 * Difficulty: EASY (43-56% elimination)
 * Already removed: 22 cells (Phase 1: 9, Phase 2: 13)
 * 
 * Calculation:
 * - total_cells = 81
 * - target_percent = (0.43 + 0.56) / 2 = 0.495 (49.5%)
 * - target_empty = 81 × 0.495 = 40 cells
 * - additional = 40 - 22 = 18 cells
 * 
 * Result: Phase 3 should attempt to eliminate 18 more cells
 * ```
 * 
 * @param board The current board state
 * @param config The elimination configuration with target percentages
 * @param already_removed Number of cells already eliminated in Phases 1 and 2
 * @return Number of additional cells Phase 3 should attempt to eliminate (minimum 0)
 * 
 * @note If already_removed exceeds the target, this function returns 0,
 *       indicating that the puzzle has already reached sufficient difficulty.
 */
int sudoku_elimination_calculate_target(const SudokuBoard *board,
                                       const SudokuEliminationConfig *config,
                                       int already_removed);

/**
 * @brief Validates that an elimination configuration is sensible
 * 
 * This is a defensive programming function that catches configuration errors
 * early. It's better to fail fast with a clear error message than to produce
 * nonsensical puzzles.
 * 
 * VALIDATION CHECKS:
 * 1. min_percent >= 0.0 (can't eliminate negative cells)
 * 2. max_percent <= 1.0 (can't eliminate more than 100%)
 * 3. min_percent <= max_percent (range must be valid)
 * 4. min_percent >= 0.20 (eliminating less than 20% produces trivial puzzles)
 * 5. max_percent <= 0.85 (eliminating more than 85% risks no solution)
 * 
 * @param config The configuration to validate
 * @return true if configuration is valid, false otherwise
 * 
 * @example
 * ```c
 * SudokuEliminationConfig config = sudoku_elimination_config_create(DIFFICULTY_EASY);
 * 
 * if (!sudoku_elimination_config_validate(&config)) {
 *     fprintf(stderr, "ERROR: Invalid elimination configuration!\n");
 *     return false;
 * }
 * ```
 * 
 * @note This function is primarily for debugging and development. In production,
 *       configs created by sudoku_elimination_config_create() are always valid.
 */
bool sudoku_elimination_config_validate(const SudokuEliminationConfig *config);

#endif /* SUDOKU_ELIMINATION_CONFIG_H */

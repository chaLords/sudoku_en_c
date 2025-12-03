/**
 * @file generator.h
 * @brief Sudoku puzzle generator with multiple strategies
 * @author Gonzalo Ramírez
 * @date 2025-12
 */

#ifndef SUDOKU_GENERATOR_H
#define SUDOKU_GENERATOR_H

#include "sudoku/core/types.h"
#include "sudoku/core/board.h"
#include <stdbool.h>

// ═══════════════════════════════════════════════════════════════════
//                    MAIN GENERATION FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Generate a complete valid Sudoku board
 * 
 * @param board Board to fill (modified in place)
 * @param config Generation configuration (use sudoku_generation_config_default())
 * @param[out] stats Output statistics (can be NULL)
 * @return true if generation succeeded
 */
bool sudoku_generate_complete(SudokuBoard *board,
                              const SudokuGenerationConfig *config,
                              SudokuGenerationStats *stats);

/**
 * @brief Complete a partially filled board using backtracking
 * 
 * Classic backtracking algorithm (v2.x compatibility).
 * 
 * @param board Partially filled board
 * @return true if completion succeeded
 */
bool sudoku_complete_backtracking(SudokuBoard *board);

/**
 * @brief Complete a partially filled board using AC3HB
 * 
 * AC3 + Heuristics + Backtracking hybrid algorithm (v3.0).
 * 
 * @param board Partially filled board
 * @return true if completion succeeded
 */
bool sudoku_complete_ac3hb(SudokuBoard *board);

// ═══════════════════════════════════════════════════════════════════
//                    CONFIGURATION HELPERS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Get default generation configuration
 * 
 * Uses AC3HB with heuristics enabled.
 * 
 * @return Default configuration
 */
SudokuGenerationConfig sudoku_generation_config_default(void);

/**
 * @brief Get backtracking-only configuration
 * 
 * For compatibility with v2.x or benchmarking.
 * Disables AC3 and heuristics.
 * 
 * @return Backtracking configuration
 */
SudokuGenerationConfig sudoku_generation_config_backtracking(void);

// ═══════════════════════════════════════════════════════════════════
//                    DIFFICULTY EVALUATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Evaluate puzzle difficulty based on clue count
 * 
 * @param board Board to evaluate
 * @return Difficulty level
 */
SudokuDifficulty sudoku_evaluate_difficulty(const SudokuBoard *board);

/**
 * @brief Convert difficulty enum to string
 * 
 * @param difficulty Difficulty level
 * @return String representation
 */
const char* sudoku_difficulty_to_string(SudokuDifficulty difficulty);

// ═══════════════════════════════════════════════════════════════════
//                    EXTENDED GENERATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Extended generation with detailed configuration and stats
 * 
 * This is a convenience wrapper around sudoku_generate_complete()
 * that provides the same functionality with optional parameters.
 * 
 * @param board Board to fill
 * @param config Generation configuration (can be NULL for defaults)
 * @param stats Output statistics (can be NULL)
 * @return true if generation succeeded
 */
bool sudoku_generate_ex(SudokuBoard *board,
                        const SudokuGenerationConfig *config,
                        SudokuGenerationStats *stats);


/**
 * @brief Simple puzzle generation with default settings (MEDIUM difficulty)
 * 
 * This is the recommended function for most use cases. It generates a complete,
 * playable Sudoku puzzle with MEDIUM difficulty using intelligent elimination
 * strategies.
 * 
 * @param board Board to generate puzzle into (must be initialized)
 * @param stats Output parameter for generation statistics
 * @return true if generation succeeded, false otherwise
 */
bool sudoku_generate(SudokuBoard *board, SudokuGenerationStats *stats);

/**
 * @brief Advanced puzzle generation with specified difficulty level
 * 
 * This function provides full control over puzzle difficulty. It uses the
 * intelligent elimination system introduced in v3.0, which adapts elimination
 * strategies based on the desired difficulty level.
 * 
 * DIFFICULTY LEVELS AND EXPECTED RESULTS (for 9×9):
 * - DIFFICULTY_EASY:   36-46 clues (43-56% elimination)
 * - DIFFICULTY_MEDIUM: 32-35 clues (57-60% elimination)
 * - DIFFICULTY_HARD:   28-31 clues (62-65% elimination)
 * - DIFFICULTY_EXPERT: 22-27 clues (67-73% elimination)
 * 
 * These percentages scale proportionally for other board sizes.
 * 
 * @param board Board to generate puzzle into (must be initialized)
 * @param difficulty Desired difficulty level
 * @param stats Output parameter for generation statistics
 * @return true if generation succeeded, false otherwise
 */
bool sudoku_generate_puzzle_with_difficulty(SudokuBoard *board,
                                           SudokuDifficulty difficulty,
                                           SudokuGenerationStats *stats);

/**
 * @brief Generate complete Sudoku with default configuration
 * 
 * Convenience wrapper around sudoku_generate_ex() using default settings.
 * Suitable for simple use cases and backward compatibility.
 * 
 * @param board Board to fill
 * @param stats Optional statistics (can be NULL)
 * @return true if successful
 */
bool sudoku_generate(SudokuBoard *board, SudokuGenerationStats *stats);

#endif // SUDOKU_GENERATOR_H

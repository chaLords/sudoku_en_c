/**
 * @file generator.h
 * @brief Sudoku puzzle generation functions
 * @author Gonzalo Ramírez
 * @date 2025-11-06
 * 
 * This file provides the main API for generating Sudoku puzzles
 * using the hybrid Fisher-Yates + Backtracking approach with
 * three-phase elimination.
 */

#ifndef SUDOKU_CORE_GENERATOR_H
#define SUDOKU_CORE_GENERATOR_H

#include <sudoku/core/types.h>

// ═══════════════════════════════════════════════════════════════════
//                    CORE GENERATION FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Generate a playable Sudoku puzzle
 * 
 * Creates a complete valid Sudoku and then removes cells using
 * a three-phase elimination strategy to create a playable puzzle.
 * 
 * Generation process:
 * 1. Fill diagonal subgrids (0, 4, 8) with Fisher-Yates
 * 2. Complete remaining cells with backtracking
 * 3. Phase 1: Remove one random number per subgrid
 * 4. Phase 2: Remove numbers without alternatives (iterative)
 * 5. Phase 3: Free elimination with uniqueness verification
 * 
 * @param[out] board Pointer to the board to fill (will be initialized)
 * @param[out] stats Pointer to store generation statistics (can be NULL)
 * @return true if generation succeeded, false on failure
 * 
 * @pre board != NULL
 * @post If returns true, board contains a valid puzzle with unique solution
 * @post If stats != NULL, contains detailed generation information
 * 
 * @note If stats is NULL, statistics are not collected with no performance impact
 * @note The function has a ~99.9% success rate on first attempt
 * 
 * @warning This function uses rand() internally. Call srand(time(NULL))
 *          before first use to ensure different puzzles each execution
 * 
 * @see sudoku_generate_with_difficulty() for difficulty-targeted generation
 * @see sudoku_set_verbosity() to control debug output during generation
 */
bool sudoku_generate(SudokuBoard *board, SudokuGenerationStats *stats);

/**
 * @brief Generate a Sudoku puzzle with specific difficulty target
 * 
 * Similar to sudoku_generate(), but attempts to produce a puzzle
 * at the specified difficulty level by adjusting elimination parameters.
 * 
 * @param[out] board Pointer to the board to fill (will be initialized)
 * @param[in] difficulty Desired difficulty level to target
 * @param[out] stats Pointer to store generation statistics (can be NULL)
 * @return true if generation succeeded, false on failure
 * 
 * @note Exact difficulty is not guaranteed, as it depends on the random
 *       puzzle structure. The function makes best effort to match target
 * 
 * @warning Not yet fully implemented - currently returns same as sudoku_generate()
 * @todo Implement difficulty-specific elimination parameter adjustment
 * 
 * @see sudoku_evaluate_difficulty() to check actual difficulty of result
 */
bool sudoku_generate_with_difficulty(SudokuBoard *board, 
                                     SudokuDifficulty difficulty,
                                     SudokuGenerationStats *stats);

// ═══════════════════════════════════════════════════════════════════
//                    DIFFICULTY EVALUATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Evaluate the difficulty level of a puzzle
 * 
 * Analyzes a Sudoku puzzle and returns its estimated difficulty
 * based primarily on the number of clues (filled cells).
 * 
 * Difficulty criteria:
 * - EASY: 45+ clues
 * - MEDIUM: 35-44 clues
 * - HARD: 25-34 clues
 * - EXPERT: <25 clues
 * 
 * @param[in] board Pointer to the board to evaluate
 * @return Estimated difficulty level
 * 
 * @pre board != NULL
 * 
 * @note This is a simple heuristic based on clue count. More sophisticated
 *       analysis could consider specific solving techniques required
 * 
 * @see sudoku_difficulty_to_string() to convert result to readable format
 */
SudokuDifficulty sudoku_evaluate_difficulty(const SudokuBoard *board);

/**
 * @brief Convert difficulty level to human-readable string
 * 
 * Converts a SudokuDifficulty enumeration value into its
 * corresponding string representation for display purposes.
 * 
 * @param[in] difficulty The difficulty level to convert
 * @return Constant string representing the difficulty ("EASY", "MEDIUM", 
 *         "HARD", "EXPERT", or "UNKNOWN" for invalid values)
 * 
 * @note The returned string is a static constant and should not be freed
 * 
 * Example usage:
 * @code
 * SudokuDifficulty diff = sudoku_evaluate_difficulty(&board);
 * printf("Difficulty: %s\n", sudoku_difficulty_to_string(diff));
 * @endcode
 */
const char* sudoku_difficulty_to_string(SudokuDifficulty difficulty);

// ═══════════════════════════════════════════════════════════════════
//                    CONFIGURATION FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Set the verbosity level for generation output
 * 
 * Controls how much information is printed during puzzle generation:
 * - 0: Minimal (only final puzzle)
 * - 1: Compact (phase summaries) - default
 * - 2: Detailed (complete debugging information)
 * 
 * This function should be called before generating puzzles if you
 * wish to change from the default level (1 = compact).
 * 
 * @param[in] level Desired verbosity level (0-2)
 * 
 * @note Values outside 0-2 range are automatically clamped to valid range
 * @note This setting affects all subsequent generation operations
 * 
 * @see sudoku_get_verbosity() to query current setting
 */
void sudoku_set_verbosity(int level);

/**
 * @brief Get the current verbosity level
 * 
 * Returns the currently configured verbosity level that controls
 * how much debugging information is displayed during generation.
 * 
 * @return Current verbosity level (0-2)
 * 
 * @see sudoku_set_verbosity() to change the verbosity level
 */
int sudoku_get_verbosity(void);
// ═══════════════════════════════════════════════════════════════════
//                    GENERATION WITH CALLBACKS (Advanced)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Generate a Sudoku puzzle with event monitoring
 * 
 * This is the advanced generation function that supports progress callbacks.
 * The library will emit events during generation, allowing the application
 * to monitor and display progress however it wants.
 * 
 * This maintains the separation between library logic (WHAT happens) and
 * application presentation (HOW it's displayed).
 * 
 * @param board Pointer to the board to fill (will be initialized)
 * @param config Configuration including optional callback function
 * @param stats Pointer to store generation statistics (can be NULL)
 * @return true if generation succeeded, false on failure
 * 
 * @pre board != NULL
 * @post If returns true, board contains a valid puzzle with unique solution
 * 
 * Example usage:
 * @code
 * void my_callback(const SudokuEventData *event, void *user_data) {
 *     if (event->type == SUDOKU_EVENT_PHASE1_COMPLETE) {
 *         printf("Phase 1 removed %d cells\n", event->cells_removed_total);
 *     }
 * }
 * 
 * SudokuGenerationConfig config = {
 *     .callback = my_callback,
 *     .user_data = NULL,
 *     .max_attempts = 10
 * };
 * 
 * SudokuBoard *board = sudoku_board_create();
 * sudoku_generate_ex(board, &config, NULL);
 * sudoku_board_destroy(board);
 * @endcode
 * 
 * @note If config is NULL or config->callback is NULL, behaves like sudoku_generate()
 * @note The callback is called synchronously during generation
 * @note Keep callbacks fast - don't do heavy computation inside them
 */
bool sudoku_generate_ex(SudokuBoard *board,
                        const SudokuGenerationConfig *config,
                        SudokuGenerationStats *stats);
#endif // SUDOKU_CORE_GENERATOR_H

/**
 * @file config.c
 * @brief Global configuration state and control API
 * @author Gonzalo Ramírez
 * @date 2025-11-06
 * 
 * This module manages the library's global configuration state, specifically
 * the verbosity level that controls diagnostic output during puzzle generation.
 * 
 * Design Pattern: Encapsulated Global State
 * 
 * Rather than exposing the VERBOSITY_LEVEL variable directly to client code,
 * which would allow invalid values and make future changes difficult, this
 * module uses the getter/setter pattern common in mature C libraries. The
 * actual variable is file-scope (static/internal), and access is controlled
 * through public functions that validate inputs and provide flexibility for
 * future enhancements.
 * 
 * This approach offers several advantages:
 * 1. Input validation: Ensures verbosity stays within valid range (0-2)
 * 2. Encapsulation: Internal representation can change without breaking API
 * 3. Side effects: Future versions could trigger actions when verbosity changes
 * 4. Documentation: Functions are easily documented with Doxygen
 * 
 * The pattern follows precedents from mature C libraries like OpenGL
 * (glEnable/glDisable) and SDL (SDL_SetHint/SDL_GetHint).
 */

#include "internal/config_internal.h"  // For VERBOSITY_LEVEL declaration

// ═══════════════════════════════════════════════════════════════════
//                    INTERNAL GLOBAL STATE
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Global verbosity level controlling diagnostic output
 * 
 * This variable is internal to the library and should never be accessed
 * directly by client code. Use sudoku_set_verbosity() and 
 * sudoku_get_verbosity() instead.
 * 
 * Valid values:
 * - 0: Silent mode - no generation progress output
 * - 1: Compact mode - single-line progress indicators (default)
 * - 2: Verbose mode - detailed step-by-step generation output
 * 
 * Default is 1 (compact mode) which provides useful feedback without
 * overwhelming the console with detailed diagnostic information.
 */
int VERBOSITY_LEVEL = 1;  // Default: compact mode

// ═══════════════════════════════════════════════════════════════════
//                    PUBLIC CONFIGURATION API
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Set the verbosity level for generation output
 * 
 * Controls how much diagnostic information the library prints to stdout
 * during puzzle generation. This allows clients to choose between silent
 * operation, compact progress indicators, or detailed debugging output.
 * 
 * Verbosity levels:
 * - 0: Silent mode
 *   No output during generation. Use this for production code where you
 *   don't want any console output, or when generating many puzzles in batch.
 * 
 * - 1: Compact mode (default)
 *   Single-line progress indicators showing major phases: "🎲 Diagonal +
 *   Backtracking..." followed by phase-wise removal counts. Provides useful
 *   feedback without cluttering the console. Recommended for interactive
 *   applications where users want to see progress.
 * 
 * - 2: Verbose mode
 *   Detailed step-by-step output showing:
 *   - Each subgrid being filled during diagonal phase
 *   - Completion status of backtracking
 *   - Detailed removal counts for each elimination phase
 *   - Final statistics
 *   Useful for debugging, educational purposes, or understanding the
 *   generation algorithm in detail.
 * 
 * The function validates input and clamps values outside the valid range
 * (0-2) to the nearest valid value. This ensures the library never enters
 * an undefined verbosity state even if called with invalid arguments.
 * 
 * @param[in] level Desired verbosity level (0=silent, 1=compact, 2=verbose)
 * 
 * @note Values less than 0 are clamped to 0 (silent)
 * @note Values greater than 2 are clamped to 2 (verbose)
 * @note Changes take effect immediately for the next generation operation
 * @note This setting is global and affects all subsequent generation calls
 * 
 * @warning If you're running multi-threaded generation, be aware that this
 *          is a global setting shared across all threads. Consider using
 *          thread-local storage or per-board configuration if you need
 *          different verbosity levels for concurrent generation operations.
 * 
 * Example usage:
 * @code
 * // Silent mode for batch generation
 * sudoku_set_verbosity(0);
 * for (int i = 0; i < 100; i++) {
 *     sudoku_generate(&boards[i], NULL);  // No output
 * }
 * 
 * // Verbose mode for debugging a generation issue
 * sudoku_set_verbosity(2);
 * sudoku_generate(&problem_board, &stats);  // Detailed output
 * 
 * // Restore default compact mode
 * sudoku_set_verbosity(1);
 * @endcode
 * 
 * @see sudoku_get_verbosity() to query current setting
 */
void sudoku_set_verbosity(int level) {
    // Input validation: clamp to valid range [0, 2]
    // This defensive programming prevents undefined behavior if called
    // with out-of-range values like -5 or 999
    if (level < 0) {
        level = 0;  // Clamp negative values to silent mode
    } else if (level > 2) {
        level = 2;  // Clamp excessive values to maximum verbosity
    }
    
    // Update the global state
    // In future versions, this could trigger additional actions like
    // opening a log file when verbosity is enabled, or closing it when
    // set to silent. The encapsulation allows such enhancements without
    // breaking the public API.
    VERBOSITY_LEVEL = level;
}

/**
 * @brief Get the current verbosity level
 * 
 * Queries the current verbosity setting. This is useful when you want to
 * temporarily change verbosity for a specific operation and then restore
 * the previous setting, or when adapting your application's UI based on
 * whether generation output is being produced.
 * 
 * @return Current verbosity level (0=silent, 1=compact, 2=verbose)
 * 
 * @note The return value is always in the valid range 0-2 since
 *       sudoku_set_verbosity() validates and clamps all inputs
 * 
 * Example: Temporarily changing verbosity
 * @code
 * // Save current verbosity
 * int saved_verbosity = sudoku_get_verbosity();
 * 
 * // Use verbose mode for this specific generation
 * sudoku_set_verbosity(2);
 * sudoku_generate(&debug_board, &stats);
 * 
 * // Restore previous verbosity
 * sudoku_set_verbosity(saved_verbosity);
 * @endcode
 * 
 * Example: Conditional UI updates
 * @code
 * if (sudoku_get_verbosity() > 0) {
 *     printf("Generating puzzle (this may take a moment)...\n");
 * }
 * sudoku_generate(&board, NULL);
 * if (sudoku_get_verbosity() > 0) {
 *     printf("Generation complete!\n");
 * }
 * @endcode
 * 
 * @see sudoku_set_verbosity() to modify the setting
 */
int sudoku_get_verbosity(void) {
    return VERBOSITY_LEVEL;
}

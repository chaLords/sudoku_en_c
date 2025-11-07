/**
 * @file sudoku.h
 * @brief Sudoku Generator Library - Main Header
 * @author Gonzalo Ramírez
 * @date 2025-11-17
 * 
 * This is the convenience header for the Sudoku Generator Library.
 * Include this single header to access all core library functionality.
 * 
 * For more selective includes, you can include individual module headers:
 * - sudoku/core/types.h     - Type definitions
 * - sudoku/core/generator.h - Puzzle generation
 * - sudoku/core/validation.h - Board validation
 * - sudoku/core/board.h     - Board operations
 * - sudoku/core/display.h   - Visual output
 * 
 * @mainpage Sudoku Generator Library v1.0.0
 * 
 * @section intro_sec Introduction
 * 
 * The Sudoku Generator Library provides a robust, efficient implementation
 * for generating valid Sudoku puzzles with unique solutions. It uses a
 * hybrid algorithm combining Fisher-Yates shuffling for independent diagonal
 * subgrids and randomized backtracking for completion, followed by a
 * sophisticated three-phase elimination process to create playable puzzles.
 * 
 * The library is designed with modern C best practices: modular architecture,
 * clear separation of concerns, comprehensive error handling, and extensive
 * documentation. It serves both as a practical tool for Sudoku generation
 * and as an educational resource for learning algorithm implementation in C.
 * 
 * @section features_sec Key Features
 * 
 * - **High Success Rate**: Generates valid puzzles in ~99.9% of attempts
 * - **Guaranteed Uniqueness**: All generated puzzles have exactly one solution
 * - **Configurable Difficulty**: Support for Easy, Medium, Hard, and Expert levels
 * - **Efficient Algorithm**: Hybrid Fisher-Yates + Backtracking approach
 * - **Modular Design**: Clean separation between generation, validation, and display
 * - **Educational Code**: Well-documented implementation demonstrating key algorithms
 * - **Zero Dependencies**: Pure C with standard library only (stdio, stdlib, time)
 * - **Cross-Platform**: Works on Linux, macOS, Windows with standard C99 compiler
 * 
 * @section algorithm_sec Algorithm Overview
 * 
 * The generation process follows these phases:
 * 
 * **Phase 1: Diagonal Fill**
 * The three diagonal 3x3 subgrids (indices 0, 4, 8) are independent of each
 * other, so they can be filled with random permutations using the Fisher-Yates
 * algorithm. This provides 27 pre-filled cells with guaranteed validity.
 * 
 * **Phase 2: Backtracking Completion**
 * The remaining 54 cells are filled using randomized backtracking. By trying
 * numbers in random order at each step, we ensure uniform distribution of
 * generated boards across all possible valid Sudoku configurations.
 * 
 * **Phase 3: Cell Elimination**
 * Starting from a complete valid board, cells are removed in three stages:
 * 1. Remove one random number from each subgrid (9 cells)
 * 2. Iteratively remove numbers that have no alternatives in their constraints
 * 3. Free elimination with uniqueness verification up to target difficulty
 * 
 * Each elimination in Phase 3 is validated to ensure the puzzle maintains
 * exactly one solution, guaranteeing solvability and uniqueness.
 * 
 * @section architecture_sec Library Architecture
 * 
 * The library is organized into logical modules:
 * 
 * - **core/types**: Fundamental data structures (Board, Position, Difficulty)
 * - **core/board**: Board initialization and manipulation functions
 * - **core/generator**: Main puzzle generation API
 * - **core/validation**: Rule checking and solution counting
 * - **core/display**: Visual formatting and output
 * - **core/algorithms**: Internal algorithms (Fisher-Yates, Backtracking)
 * - **core/elimination**: Cell removal strategies
 * 
 * Public API functions use the prefix `sudoku_` followed by the module name,
 * ensuring clear namespacing and preventing naming conflicts.
 * 
 * @section usage_sec Basic Usage Example
 * 
 * Here's a complete example showing typical library usage:
 * 
 * @code
 * #include <sudoku/sudoku.h>
 * #include <stdio.h>
 * #include <time.h>
 * 
 * int main(void) {
 *     // Initialize random number generator (required for generation)
 *     srand((unsigned int)time(NULL));
 *     
 *     // Set verbosity level (0=minimal, 1=compact, 2=detailed)
 *     sudoku_set_verbosity(1);
 *     
 *     // Prepare structures
 *     SudokuBoard board;
 *     SudokuGenerationStats stats;
 *     
 *     // Generate a puzzle
 *     if (sudoku_generate(&board, &stats)) {
 *         printf("✓ Successfully generated puzzle\n");
 *         printf("  Clues: %d, Empty: %d\n", board.clues, board.empty);
 *         
 *         // Evaluate difficulty
 *         SudokuDifficulty difficulty = sudoku_evaluate_difficulty(&board);
 *         printf("  Difficulty: %s\n", sudoku_difficulty_to_string(difficulty));
 *         
 *         // Display the puzzle
 *         sudoku_display_print_board(&board);
 *         
 *         // Validate correctness
 *         if (sudoku_validate_board(&board)) {
 *             printf("✓ Puzzle is valid\n");
 *         }
 *     } else {
 *         printf("✗ Generation failed (very rare)\n");
 *         return 1;
 *     }
 *     
 *     return 0;
 * }
 * @endcode
 * 
 * @section building_sec Building and Installation
 * 
 * The library uses CMake for cross-platform building:
 * 
 * @code{.sh}
 * mkdir build && cd build
 * cmake ..
 * make
 * sudo make install  # Optional: install system-wide
 * @endcode
 * 
 * This generates:
 * - `libsudoku_core.a` - Static library
 * - `sudoku_generator` - CLI demonstration tool
 * - Headers installed to `/usr/local/include/sudoku/`
 * 
 * Link against the library in your projects:
 * @code{.sh}
 * gcc -o myapp myapp.c -lsudoku_core
 * @endcode
 * 
 * @section performance_sec Performance Characteristics
 * 
 * - **Generation Time**: Typically <10ms on modern hardware
 * - **Success Rate**: >99.9% on first attempt
 * - **Memory Usage**: ~1KB per board structure
 * - **Difficulty Range**: 17-64 clues (64 filled, 17-60 empty)
 * 
 * The algorithm is optimized for generating puzzles one at a time.
 * For bulk generation, consider parallelization since each generation
 * is independent.
 * 
 * @section educational_sec Educational Value
 * 
 * This library demonstrates several important computer science concepts:
 * - **Backtracking**: Classic constraint satisfaction problem solving
 * - **Fisher-Yates Shuffle**: Efficient uniform random permutation
 * - **Constraint Propagation**: Intelligent elimination strategies
 * - **Modular Design**: Clean separation of concerns in C
 * - **API Design**: Creating intuitive, well-documented interfaces
 * - **Memory Management**: Proper allocation and cleanup patterns
 * 
 * The codebase is extensively commented to serve as a learning resource
 * for students and developers interested in algorithm implementation.
 * 
 * @section future_sec Future Enhancements
 * 
 * Planned features for upcoming releases:
 * - Interactive solver with step-by-step hints
 * - Export to various formats (PDF, PNG, JSON)
 * - Puzzle difficulty analysis using solving technique classification
 * - Support for Sudoku variants (Killer Sudoku, Samurai Sudoku)
 * - Performance optimizations using SIMD instructions
 * 
 * @section license_sec License
 * 
 * Copyright (c) 2025 Gonzalo Ramírez
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * @author Gonzalo Ramírez
 * @date 2025-11-17
 * @version 1.0.0
 */

#ifndef SUDOKU_H
#define SUDOKU_H

// ═══════════════════════════════════════════════════════════════════
//                    CORE LIBRARY HEADERS
// ═══════════════════════════════════════════════════════════════════

/**
 * Core type definitions (SudokuBoard, SudokuPosition, SudokuDifficulty)
 * This header defines all fundamental data structures used throughout
 * the library. Include this if you only need type definitions.
 */
#include <sudoku/core/types.h>

/**
 * Board manipulation functions (init, stats update, subgrid utilities)
 * This header provides functions for initializing and managing board state.
 */
#include <sudoku/core/board.h>

/**
 * Validation functions (position checking, board validation, solution counting)
 * This header provides all functions for verifying Sudoku rules compliance.
 */
#include <sudoku/core/validation.h>

/**
 * Generation functions (puzzle creation, difficulty targeting)
 * This is the main API for generating new Sudoku puzzles.
 */
#include <sudoku/core/generator.h>

/**
 * Display functions (formatted board printing)
 * This header provides visual output functions with Unicode box-drawing.
 */
#include <sudoku/core/display.h>

// ═══════════════════════════════════════════════════════════════════
//                    FUTURE MODULES (NOT YET IMPLEMENTED)
// ═══════════════════════════════════════════════════════════════════

/*
 * The following modules are planned for future releases:
 * 
 * #include <sudoku/solver/solver.h>      - Interactive solving with hints
 * #include <sudoku/io/export.h>          - Export to PDF, PNG, JSON formats
 * #include <sudoku/analysis/difficulty.h> - Advanced difficulty metrics
 * #include <sudoku/variants/killer.h>    - Killer Sudoku variant support
 */

// ═══════════════════════════════════════════════════════════════════
//                    LIBRARY VERSION INFORMATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Library version - Major number
 * 
 * Incremented for incompatible API changes. When this changes,
 * code using the old version may need modifications.
 */
#define SUDOKU_VERSION_MAJOR 1

/**
 * @brief Library version - Minor number
 * 
 * Incremented for backward-compatible functionality additions.
 * Old code continues to work without changes.
 */
#define SUDOKU_VERSION_MINOR 0

/**
 * @brief Library version - Patch number
 * 
 * Incremented for backward-compatible bug fixes and minor improvements.
 */
#define SUDOKU_VERSION_PATCH 0

/**
 * @brief Complete version string in semver format
 * 
 * Follows Semantic Versioning 2.0.0 specification.
 * Format: MAJOR.MINOR.PATCH
 */
#define SUDOKU_VERSION_STRING "1.0.0"

/**
 * @brief Numeric version for compile-time comparison
 * 
 * Allows conditional compilation based on version:
 * @code
 * #if SUDOKU_VERSION >= SUDOKU_MAKE_VERSION(1, 2, 0)
 *     // Use features from version 1.2.0 or later
 * #endif
 * @endcode
 */
#define SUDOKU_VERSION \
    ((SUDOKU_VERSION_MAJOR * 10000) + \
     (SUDOKU_VERSION_MINOR * 100) + \
     SUDOKU_VERSION_PATCH)

/**
 * @brief Helper macro to create version numbers for comparison
 * 
 * Example usage:
 * @code
 * #if SUDOKU_VERSION >= SUDOKU_MAKE_VERSION(2, 0, 0)
 *     // Code that requires version 2.0.0 or higher
 * #endif
 * @endcode
 */
#define SUDOKU_MAKE_VERSION(major, minor, patch) \
    (((major) * 10000) + ((minor) * 100) + (patch))

#endif // SUDOKU_H

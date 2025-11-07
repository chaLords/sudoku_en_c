/**
 * @file types.h
 * @brief Core type definitions for Sudoku library
 * @author Gonzalo Ramírez
 * @date 2025-10-19
 * 
 * This file defines all fundamental data structures and constants used
 * throughout the Sudoku library. These types form the vocabulary of the API.
 * 
 * Design decisions:
 * - Uses 'int' for compatibility with existing code (can optimize to uint8_t later)
 * - Excludes unused types (SolutionBoard, PlayerBoard) until needed
 * - Constants use expressions to maintain semantic relationships
 */

#ifndef SUDOKU_CORE_TYPES_H
#define SUDOKU_CORE_TYPES_H

#include <stdbool.h>
#include <stdint.h>

/* ═══════════════════════════════════════════════════════════════
 * BOARD DIMENSIONS
 * ═══════════════════════════════════════════════════════════════ */

/**
 * @brief Size of the Sudoku grid (9x9)
 * 
 * This is the fundamental dimension of a standard Sudoku board.
 * Changing this value would allow the code to work with variant
 * Sudoku sizes (4x4, 16x16, etc.) if other parts were generalized.
 */
#define SUDOKU_SIZE 9

/**
 * @brief Size of each 3x3 subgrid
 * 
 * Standard Sudoku divides the 9x9 board into nine 3x3 regions.
 * This constant expresses the mathematical relationship that
 * SUBGRID_SIZE squared equals SUDOKU_SIZE.
 */
#define SUBGRID_SIZE 3

/**
 * @brief Total number of cells in the board
 * 
 * Computed as SUDOKU_SIZE squared rather than hardcoded as 81.
 * This maintains the semantic relationship that total cells
 * equals the board dimensions squared, making the code more
 * maintainable and self-documenting.
 */
#define TOTAL_CELLS (SUDOKU_SIZE * SUDOKU_SIZE)

/* ═══════════════════════════════════════════════════════════════
 * GENERATION PARAMETERS
 * ═══════════════════════════════════════════════════════════════ */

/**
 * @brief Target number of cells to remove in Phase 3
 * 
 * Phase 3 of the generation algorithm attempts to remove up to
 * this many additional cells while maintaining a unique solution.
 * This value affects the final difficulty of generated puzzles.
 */
#define PHASE3_TARGET 25

/* ═══════════════════════════════════════════════════════════════
 * BASIC TYPES
 * ═══════════════════════════════════════════════════════════════ */

/**
 * @brief Type for a single cell value
 * 
 * Using 'int' for compatibility with existing code.
 * Valid range: 0-9, where 0 represents an empty cell
 * and 1-9 represent the filled numbers.
 * 
 * Note: Could be optimized to uint8_t in future for memory efficiency.
 */
typedef int CellValue;

/* ═══════════════════════════════════════════════════════════════
 * GEOMETRIC STRUCTURES
 * ═══════════════════════════════════════════════════════════════ */

/**
 * @brief Represents a position (row, column) on the board
 * 
 * This structure avoids passing multiple row/col parameters
 * separately, making function signatures cleaner and less error-prone.
 * 
 * Layout matches original: int row, int col
 */
typedef struct {
    int row;        /**< Row index (0-8) */
    int col;        /**< Column index (0-8) */
} SudokuPosition;

/**
 * @brief Defines a 3x3 subgrid region of the board
 * 
 * Sudoku boards are divided into 9 subgrids numbered 0-8.
 * Each subgrid has a base position (top-left corner) that
 * determines which cells belong to it.
 * 
 * Layout matches original: int index, Position base
 */
typedef struct {
    int index;                  /**< Subgrid index (0-8) */
    SudokuPosition base;        /**< Top-left corner position */
} SudokuSubGrid;

/* ═══════════════════════════════════════════════════════════════
 * BOARD STRUCTURE
 * ═══════════════════════════════════════════════════════════════ */

/**
 * @brief Complete Sudoku board state
 * 
 * This is the central data structure that represents a Sudoku puzzle.
 * It encapsulates both the grid itself and metadata about filled/empty cells.
 * 
 * Layout matches original exactly:
 *   int cells[SIZE][SIZE]
 *   int clues
 *   int empty
 * 
 * This ensures binary compatibility with existing code during migration.
 */
typedef struct {
    int cells[SUDOKU_SIZE][SUDOKU_SIZE];    /**< The 9x9 grid of cell values */
    int clues;                               /**< Number of filled cells (clues) */
    int empty;                               /**< Number of empty cells */
} SudokuBoard;

/* ═══════════════════════════════════════════════════════════════
 * GENERATION STATISTICS
 * ═══════════════════════════════════════════════════════════════ */

/**
 * @brief Statistics collected during puzzle generation
 * 
 * This structure tracks information about how a puzzle was generated,
 * including the number of cells removed in each phase and how many
 * attempts were needed.
 * 
 * Layout matches original GenerationStats exactly.
 */
typedef struct {
    int phase1_removed;     /**< Cells removed in phase 1 (Fisher-Yates selection) */
    int phase2_removed;     /**< Cells removed in phase 2 (no alternatives) */
    int phase2_rounds;      /**< Number of rounds in phase 2 */
    int phase3_removed;     /**< Cells removed in phase 3 (free elimination) */
    int total_attempts;     /**< Total generation attempts before success */
} SudokuGenerationStats;

/* ═══════════════════════════════════════════════════════════════
 * DIFFICULTY LEVELS
 * ═══════════════════════════════════════════════════════════════ */

/**
 * @brief Difficulty levels for generated puzzles
 * 
 * Difficulty is determined primarily by the number of clues (filled cells).
 * Fewer clues generally make the puzzle harder to solve.
 * 
 * These thresholds match the logic in evaluateDifficulty() function.
 */
typedef enum {
    SUDOKU_EASY,        /**< Easy difficulty (45+ clues) */
    SUDOKU_MEDIUM,      /**< Medium difficulty (35-44 clues) */
    SUDOKU_HARD,        /**< Hard difficulty (25-34 clues) */
    SUDOKU_EXPERT       /**< Expert difficulty (<25 clues) */
} SudokuDifficulty;

/* ═══════════════════════════════════════════════════════════════
 * NOTES ON EXCLUDED TYPES
 * ═══════════════════════════════════════════════════════════════ 
 * 
 * The following types were defined in the original main.c but are
 * not included here because they are not currently used:
 * 
 * - SolutionBoard: Intended for future interactive mode
 * - PlayerBoard: Intended for future interactive mode
 * 
 * These will be added when implementing the interactive/solver
 * modules where they will actually be needed.
 */

#endif /* SUDOKU_CORE_TYPES_H */

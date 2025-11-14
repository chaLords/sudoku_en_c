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
 * @brief Opaque type for Sudoku board
 * 
 * This is an opaque pointer type. Clients cannot see the internal
 * structure and must use the provided API functions to interact
 * with boards.
 * 
 * Boards must be created with sudoku_board_create() and destroyed
 * with sudoku_board_destroy() to ensure proper resource management.
 * 
 * @note In previous versions, this structure was fully visible.
 *       This change improves encapsulation and future compatibility.
 */
typedef struct SudokuBoard SudokuBoard;

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
 * EVENT SYSTEM FOR GENERATION MONITORING
 * ═══════════════════════════════════════════════════════════════ */

/**
 * @brief Types of events that occur during puzzle generation
 * 
 * The library emits these events to notify the application about
 * significant milestones during the generation process. The application
 * can choose to handle these events (via callbacks) or ignore them.
 * 
 * This allows the library to remain presentation-agnostic - it doesn't
 * care HOW events are displayed, only WHAT happened.
 */
typedef enum {
    SUDOKU_EVENT_GENERATION_START,       /**< Generation process started */
    SUDOKU_EVENT_DIAGONAL_FILL_START,    /**< Filling diagonal subgrids */
    SUDOKU_EVENT_DIAGONAL_FILL_COMPLETE, /**< Diagonal filled successfully */
    SUDOKU_EVENT_BACKTRACK_START,        /**< Backtracking phase started */
    SUDOKU_EVENT_BACKTRACK_COMPLETE,     /**< Board completion successful */
    SUDOKU_EVENT_PHASE1_START,           /**< Phase 1 elimination started */
    SUDOKU_EVENT_PHASE1_CELL_SELECTED,   /**< A cell was selected for removal in phase 1 */
    SUDOKU_EVENT_PHASE1_COMPLETE,        /**< Phase 1 finished */
    SUDOKU_EVENT_PHASE2_START,           /**< Phase 2 elimination started */
    SUDOKU_EVENT_PHASE2_ROUND_START,     /**< New phase 2 iteration started */
    SUDOKU_EVENT_PHASE2_CELL_SELECTED,   /**< A cell was selected for removal in phase 2 */
    SUDOKU_EVENT_PHASE2_ROUND_COMPLETE,  /**< Phase 2 round finished */
    SUDOKU_EVENT_PHASE2_COMPLETE,        /**< Phase 2 finished */
    SUDOKU_EVENT_PHASE3_START,           /**< Phase 3 elimination started */
    SUDOKU_EVENT_PHASE3_CELL_TESTING,    /**< Testing if a cell can be removed */
    SUDOKU_EVENT_PHASE3_CELL_REMOVED,    /**< A cell was successfully removed */
    SUDOKU_EVENT_PHASE3_CELL_KEPT,       /**< A cell must be kept (removal would break uniqueness) */
    SUDOKU_EVENT_PHASE3_COMPLETE,        /**< Phase 3 finished */
    SUDOKU_EVENT_GENERATION_COMPLETE,    /**< Entire generation succeeded */
    SUDOKU_EVENT_GENERATION_FAILED       /**< Generation failed */
} SudokuEventType;

/**
 * @brief Data associated with a generation event
 * 
 * Contains contextual information about what happened during generation.
 * Not all fields are meaningful for all event types - check the event
 * type to know which fields contain valid data.
 * 
 * The board pointer is always valid and points to the current state,
 * but it's read-only and only valid during the callback execution.
 */
typedef struct {
    SudokuEventType type;       /**< Type of event that occurred */
    const SudokuBoard *board;   /**< Current board state (read-only, temporary) */
    
    /* Phase information */
    int phase_number;           /**< Which elimination phase (1, 2, or 3) */
    int cells_removed_total;    /**< Total cells removed so far in this phase */
    int round_number;           /**< Round/iteration number (mainly for phase 2) */
    
    /* Cell-specific information (for cell-related events) */
    int row;                    /**< Row of the cell in question (-1 if not applicable) */
    int col;                    /**< Column of the cell in question (-1 if not applicable) */
    int value;                  /**< Value that was in the cell (0 if not applicable) */
    
} SudokuEventData;

/**
 * @brief Callback function signature for generation events
 * 
 * Applications provide a function matching this signature to receive
 * notifications about generation progress. The library calls this function
 * whenever something significant happens.
 * 
 * The callback should be fast and non-blocking. Don't perform heavy
 * computations or I/O operations inside the callback, as it runs
 * synchronously with the generation process.
 * 
 * @param event Pointer to event data describing what happened
 * @param user_data Custom pointer provided by the application when
 *                  registering the callback (can be NULL)
 * 
 * Example callback:
 * @code
 * void my_callback(const SudokuEventData *event, void *user_data) {
 *     if (event->type == SUDOKU_EVENT_PHASE1_COMPLETE) {
 *         printf("Phase 1 removed %d cells\n", event->cells_removed_total);
 *     }
 * }
 * @endcode
 */
typedef void (*SudokuEventCallback)(const SudokuEventData *event, void *user_data);

/**
 * @brief Configuration for puzzle generation
 * 
 * Allows customizing the generation process, including registering
 * a callback to monitor progress.
 */
typedef struct {
    SudokuEventCallback callback;  /**< Optional callback function (can be NULL) */
    void *user_data;               /**< Custom data passed to callback (can be NULL) */
    int max_attempts;              /**< Maximum generation attempts (0 = unlimited) */
} SudokuGenerationConfig;

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

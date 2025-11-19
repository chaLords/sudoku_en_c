/**
 * @file types.h
 * @brief Core type definitions for Sudoku game
 * @author Gonzalo Ramírez
 * @date 2025-11-18
 * 
 * This header defines the fundamental data structures used throughout
 * the Sudoku library. The main structure, SudokuBoard, now supports
 * configurable board sizes (not just 9×9).
 * 
 * Supported sizes:
 * - 2×2 subgrids → 4×4 board (16 cells)
 * - 3×3 subgrids → 9×9 board (81 cells) [default]
 * - 4×4 subgrids → 16×16 board (256 cells)
 * - 5×5 subgrids → 25×25 board (625 cells)
 */

#ifndef SUDOKU_TYPES_H
#define SUDOKU_TYPES_H

#include <stdbool.h>

// ═══════════════════════════════════════════════════════════════════
//                    DEFAULT SIZE CONSTANTS
// ═══════════════════════════════════════════════════════════════════

/**
 * Default subgrid size (3×3) for classic Sudoku
 * Used when creating boards without specifying size
 */
#define SUDOKU_DEFAULT_SUBGRID_SIZE 3

/**
 * Default board size (9×9) for classic Sudoku
 * Calculated as SUDOKU_DEFAULT_SUBGRID_SIZE²
 */
#define SUDOKU_DEFAULT_BOARD_SIZE 9

/**
 * Default total cells (81) for classic Sudoku
 * Calculated as SUDOKU_DEFAULT_BOARD_SIZE²
 */
#define SUDOKU_DEFAULT_TOTAL_CELLS 81

// ═══════════════════════════════════════════════════════════════════
//                    GENERATION PARAMETERS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Target number of cells to remove in Phase 3
 * 
 * Phase 3 of the generation algorithm attempts to remove up to
 * this many additional cells while maintaining a unique solution.
 * 
 * @deprecated This constant is hardcoded for 9×9 boards and will be
 *             replaced with a dynamic calculation in Phase 3 refactoring.
 *             For configurable sizes, Phase 3 should remove approximately
 *             30% of total_cells instead of a fixed value.
 * 
 * TODO (Phase 3): Replace with dynamic calculation:
 *       phase3_target = (board->total_cells * 3) / 10;  // 30% of cells
 */
#define PHASE3_TARGET 25

// ═══════════════════════════════════════════════════════════════════
//                    BACKWARD COMPATIBILITY (Deprecated)
// ═══════════════════════════════════════════════════════════════════

/**
 * @deprecated Use board->board_size instead of this constant
 * 
 * This constant is maintained for backward compatibility with code
 * that directly uses SUDOKU_SIZE. New code should use the board's
 * dynamic size fields instead.
 */
#define SUDOKU_SIZE SUDOKU_DEFAULT_BOARD_SIZE

/**
 * @deprecated Use board->subgrid_size instead of this constant
 */
#define SUBGRID_SIZE SUDOKU_DEFAULT_SUBGRID_SIZE

/**
 * @deprecated Use board->total_cells instead of this constant
 */
#define TOTAL_CELLS SUDOKU_DEFAULT_TOTAL_CELLS

// ═══════════════════════════════════════════════════════════════════
//                    POSITION STRUCTURE
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Represents a position on the Sudoku board
 * 
 * Zero-indexed coordinates where (0,0) is the top-left cell
 * and (board_size-1, board_size-1) is the bottom-right cell.
 */
typedef struct {
    int row;    ///< Row index (0 to board_size-1)
    int col;    ///< Column index (0 to board_size-1)
} SudokuPosition;

// ═══════════════════════════════════════════════════════════════════
//                    BOARD STRUCTURE (Configurable Size)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Main Sudoku board structure with configurable dimensions
 * 
 * This structure now supports boards of different sizes, not just 9×9.
 * The board is represented as a 2D array allocated dynamically based
 * on the subgrid size.
 * 
 * Memory management:
 * - Create with: sudoku_board_create_size(subgrid_size)
 * - Destroy with: sudoku_board_destroy(board)
 * 
 * Example sizes:
 * - subgrid_size=2: 4×4 board (16 cells)
 * - subgrid_size=3: 9×9 board (81 cells)
 * - subgrid_size=4: 16×16 board (256 cells)
 * 
 * @note The cells array is dynamically allocated on the heap
 * @note This is now an opaque structure - use accessor functions
 */
typedef struct {
    // ─────────────────────────────────────────────────────────────
    //  Dimension Configuration
    // ─────────────────────────────────────────────────────────────
    
    /**
     * Size of each subgrid (e.g., 3 for 3×3 subgrids in classic Sudoku)
     * Valid range: 2-5 (larger sizes may be computationally expensive)
     */
    int subgrid_size;
    
    /**
     * Size of the board (subgrid_size²)
     * For classic Sudoku: 3² = 9
     * For mini Sudoku: 2² = 4
     * For mega Sudoku: 4² = 16
     */
    int board_size;
    
    /**
     * Total number of cells in the board (board_size²)
     * For classic Sudoku: 9² = 81
     * For mini Sudoku: 4² = 16
     * For mega Sudoku: 16² = 256
     */
    int total_cells;
    
    // ─────────────────────────────────────────────────────────────
    //  Board Data (Dynamic Allocation)
    // ─────────────────────────────────────────────────────────────
    
    /**
     * 2D array representing the board cells [board_size][board_size]
     * 
     * Values:
     * - 0: Empty cell
     * - 1 to board_size: Valid numbers
     * 
     * Memory: Dynamically allocated on the heap
     * Access: board->cells[row][col]
     * 
     * @note Do not access directly if structure is opaque
     * @note Use sudoku_board_get_cell() and sudoku_board_set_cell()
     */
    int **cells;
    
    // ─────────────────────────────────────────────────────────────
    //  Board Statistics
    // ─────────────────────────────────────────────────────────────
    
    /**
     * Number of filled cells (non-zero values)
     * Range: 0 to total_cells
     */
    int clues;
    
    /**
     * Number of empty cells (zero values)
     * Range: 0 to total_cells
     * Invariant: clues + empty = total_cells
     */
    int empty;
} SudokuBoard;

// ═══════════════════════════════════════════════════════════════════
//                    SUBGRID STRUCTURE
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Represents a single subgrid within the board
 * 
 * In classic 9×9 Sudoku, there are 9 subgrids of 3×3 cells each.
 * In general, there are board_size subgrids of subgrid_size² cells each.
 * 
 * Subgrids are indexed in row-major order (left to right, top to bottom).
 * For classic Sudoku:
 * @code
 *  0 | 1 | 2
 * ---+---+---
 *  3 | 4 | 5
 * ---+---+---
 *  6 | 7 | 8
 * @endcode
 */
typedef struct {
    int index;              ///< Subgrid index (0 to board_size-1)
    SudokuPosition base;    ///< Top-left corner position of this subgrid
} SudokuSubGrid;

// ═══════════════════════════════════════════════════════════════════
//                    GENERATION STATISTICS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Statistics collected during puzzle generation
 * 
 * Tracks how many cells were removed in each elimination phase,
 * providing insight into the generation process and difficulty.
 */
typedef struct {
    int phase1_removed;     ///< Cells removed in Phase 1 (Fisher-Yates selection)
    int phase2_removed;     ///< Cells removed in Phase 2 (heuristic elimination)
    int phase2_rounds;      ///< Number of rounds Phase 2 took
    int phase3_removed;     ///< Cells removed in Phase 3 (exhaustive verification)
    int total_attempts;     /// SE AGREGO PERO TENGO DUDA... LO PIDIO generator.c
} SudokuGenerationStats;

// ═══════════════════════════════════════════════════════════════════
//                    DIFFICULTY LEVELS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Difficulty classification for generated puzzles
 * 
 * Based on the number of clues remaining after generation.
 * Thresholds may need adjustment for non-standard board sizes.
 */
typedef enum {
    SUDOKU_EASY,        ///< Many clues (45+ for 9×9)
    SUDOKU_MEDIUM,      ///< Moderate clues (35-44 for 9×9)
    SUDOKU_HARD,        ///< Few clues (25-34 for 9×9)
    SUDOKU_EXPERT,      ///< Very few clues (<25 for 9×9)
    SUDOKU_INVALID      ///< Invalid board state
} SudokuDifficulty;

// ═══════════════════════════════════════════════════════════════════
//                    EVENT SYSTEM FOR GENERATION MONITORING
// ═══════════════════════════════════════════════════════════════════

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
    SUDOKU_EVENT_GENERATION_START,        /**< Generation process started */
    SUDOKU_EVENT_DIAGONAL_FILL_START,     /**< Filling diagonal subgrids */
    SUDOKU_EVENT_DIAGONAL_FILL_COMPLETE,  /**< Diagonal filled successfully */
    SUDOKU_EVENT_BACKTRACK_START,         /**< Backtracking phase started */
    SUDOKU_EVENT_BACKTRACK_COMPLETE,      /**< Board completion successful */
    SUDOKU_EVENT_PHASE1_START,            /**< Phase 1 elimination started */
    SUDOKU_EVENT_PHASE1_CELL_SELECTED,    /**< A cell was selected for removal in phase 1 */
    SUDOKU_EVENT_PHASE1_COMPLETE,         /**< Phase 1 finished */
    SUDOKU_EVENT_PHASE2_START,            /**< Phase 2 elimination started */
    SUDOKU_EVENT_PHASE2_ROUND_START,      /**< New phase 2 iteration started */
    SUDOKU_EVENT_PHASE2_CELL_SELECTED,    /**< A cell was selected for removal in phase 2 */
    SUDOKU_EVENT_PHASE2_ROUND_COMPLETE,   /**< Phase 2 round finished */
    SUDOKU_EVENT_PHASE2_COMPLETE,         /**< Phase 2 finished */
    SUDOKU_EVENT_PHASE3_START,            /**< Phase 3 elimination started */
    SUDOKU_EVENT_PHASE3_CELL_TESTING,     /**< Testing if a cell can be removed */
    SUDOKU_EVENT_PHASE3_CELL_REMOVED,     /**< A cell was successfully removed */
    SUDOKU_EVENT_PHASE3_CELL_KEPT,        /**< A cell must be kept (removal would break uniqueness) */
    SUDOKU_EVENT_PHASE3_COMPLETE,         /**< Phase 3 finished */
    SUDOKU_EVENT_GENERATION_COMPLETE,     /**< Entire generation succeeded */
    SUDOKU_EVENT_GENERATION_FAILED        /**< Generation failed */
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
    SudokuEventType type;       ///< Type of event that occurred
    const SudokuBoard *board;   ///< Current board state (read-only, temporary)
    
    // Phase information
    int phase_number;           ///< Which elimination phase (1, 2, or 3)
    int cells_removed_total;    ///< Total cells removed so far in this phase
    int round_number;           ///< Round/iteration number (mainly for phase 2)
    
    // Cell-specific information (for cell-related events)
    int row;                    ///< Row of the cell (-1 if not applicable)
    int col;                    ///< Column of the cell (-1 if not applicable)
    int value;                  ///< Value that was in the cell (0 if not applicable)
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
    SudokuEventCallback callback;  ///< Optional callback function (can be NULL)
    void *user_data;               ///< Custom data passed to callback (can be NULL)
    int max_attempts;
} SudokuGenerationConfig;

#endif // SUDOKU_TYPES_H

/**
 * @file types.h
 * @brief Core type definitions for Sudoku game
 * @author Gonzalo Ramírez
 * @date 2025-11-18
 * @version 3.0.0 - Adapted for configurable board sizes
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
 * 
 * VERSION 3.0 CHANGES:
 * - Removed PHASE3_TARGET constant (now calculated dynamically)
 * - Added comprehensive documentation for all structures
 * - Maintained backward compatibility with deprecated constants
 */

#ifndef SUDOKU_TYPES_H
#define SUDOKU_TYPES_H

#include <stdbool.h>

// ═══════════════════════════════════════════════════════════════════
//                    DEFAULT SIZE CONSTANTS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Default subgrid size (3×3) for classic Sudoku
 * 
 * Used when creating boards without specifying size.
 * The board size is calculated as subgrid_size².
 */
#define SUDOKU_DEFAULT_SUBGRID_SIZE 3

/**
 * @brief Default board size (9×9) for classic Sudoku
 * 
 * Calculated as SUDOKU_DEFAULT_SUBGRID_SIZE² = 3² = 9
 */
#define SUDOKU_DEFAULT_BOARD_SIZE 9

/**
 * @brief Default total cells (81) for classic Sudoku
 * 
 * Calculated as SUDOKU_DEFAULT_BOARD_SIZE² = 9² = 81
 */
#define SUDOKU_DEFAULT_TOTAL_CELLS 81

// ═══════════════════════════════════════════════════════════════════
//                    GENERATION PARAMETERS (v3.0)
// ═══════════════════════════════════════════════════════════════════

/*
 * REMOVED: PHASE3_TARGET constant (v3.0+)
 * ════════════════════════════════════════════════════════════════════
 * 
 * The target for Phase 3 elimination is now calculated DYNAMICALLY
 * based on board size using calculate_phase3_target() in phase3.c.
 * 
 * This change was necessary because a fixed target (25 cells) does
 * not scale appropriately across different board sizes:
 * 
 * | Board Size | Total Cells | Old Target | Problem                    |
 * |------------|-------------|------------|----------------------------|
 * | 4×4        | 16          | 25         | 156% of cells (impossible) |
 * | 9×9        | 81          | 25         | 31% of cells (correct)     |
 * | 16×16      | 256         | 25         | 10% of cells (too easy)    |
 * | 25×25      | 625         | 25         | 4% of cells (trivial)      |
 * 
 * The new dynamic calculation uses PROPORTIONAL targets:
 * 
 * | Board Size | Percentage | Calculated Target |
 * |------------|------------|-------------------|
 * | 4×4        | 31%        | ~5 cells          |
 * | 9×9        | 31%        | ~25 cells         |
 * | 16×16      | 27%        | ~69 cells         |
 * | 25×25      | 23%        | ~144 cells        |
 * 
 * For 9×9 boards, this produces IDENTICAL behavior to the old constant.
 * 
 * Migration guide:
 * - Old code: phase3Elimination(board, PHASE3_TARGET)
 * - New code: phase3EliminationAuto(board)
 * 
 * See phase3.c:calculate_phase3_target() for implementation details.
 * 
 * Previous definition (kept for historical reference only):
 * #define PHASE3_TARGET 25
 */

// ═══════════════════════════════════════════════════════════════════
//                    BACKWARD COMPATIBILITY (Deprecated)
// ═══════════════════════════════════════════════════════════════════

/**
 * @deprecated Use board->board_size instead of this constant
 * 
 * This constant is maintained for backward compatibility with code
 * that directly uses SUDOKU_SIZE. New code should use the board's
 * dynamic size fields instead.
 * 
 * @warning Will be removed in a future major version
 */
#define SUDOKU_SIZE SUDOKU_DEFAULT_BOARD_SIZE

/**
 * @deprecated Use board->subgrid_size instead of this constant
 * 
 * This constant is maintained for backward compatibility.
 * New code should query the board's subgrid_size field.
 * 
 * @warning Will be removed in a future major version
 */
#define SUBGRID_SIZE SUDOKU_DEFAULT_SUBGRID_SIZE

/**
 * @deprecated Use board->total_cells instead of this constant
 * 
 * This constant is maintained for backward compatibility.
 * New code should query the board's total_cells field.
 * 
 * @warning Will be removed in a future major version
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
 * 
 * This structure is used extensively throughout the library to
 * pass coordinate pairs as a single unit, improving code clarity
 * and reducing parameter count in function signatures.
 * 
 * Memory footprint: 8 bytes (two 4-byte integers)
 * 
 * Example usage:
 * @code
 * SudokuPosition pos = {3, 5};  // Row 3, Column 5
 * int value = sudoku_board_get_cell(board, &pos);
 * @endcode
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
 * This structure supports boards of different sizes, not just 9×9.
 * The board is represented as a 2D array allocated dynamically based
 * on the subgrid size.
 * 
 * MEMORY MANAGEMENT:
 * - Create with: sudoku_board_create_size(subgrid_size)
 * - Destroy with: sudoku_board_destroy(board)
 * - For default 9×9: sudoku_board_create() is available
 * 
 * SIZE RELATIONSHIPS:
 * - subgrid_size = k (e.g., 3 for classic Sudoku)
 * - board_size = k² (e.g., 9 for classic Sudoku)
 * - total_cells = k⁴ (e.g., 81 for classic Sudoku)
 * - Number of subgrids = k² (same as board_size)
 * 
 * Example sizes:
 * | subgrid_size | board_size | total_cells | Subgrids |
 * |--------------|------------|-------------|----------|
 * | 2            | 4          | 16          | 4        |
 * | 3            | 9          | 81          | 9        |
 * | 4            | 16         | 256         | 16       |
 * | 5            | 25         | 625         | 25       |
 * 
 * @note The cells array is dynamically allocated on the heap
 * @note Prefer using accessor functions over direct field access
 * 
 * @see sudoku_board_create_size() for creating boards of specific sizes
 * @see sudoku_board_destroy() for proper cleanup
 */
typedef struct {
    // ─────────────────────────────────────────────────────────────
    //  Dimension Configuration
    // ─────────────────────────────────────────────────────────────
    
    /**
     * @brief Size of each subgrid (k in k×k subgrids)
     * 
     * For classic Sudoku: 3 (3×3 subgrids)
     * Valid range: 2-5 (larger sizes may be computationally expensive)
     * 
     * This is the fundamental parameter from which all other
     * dimensions are derived: board_size = subgrid_size²
     */
    int subgrid_size;
    
    /**
     * @brief Size of the board (subgrid_size²)
     * 
     * Represents both the width and height of the square board,
     * as well as the number of subgrids in the board.
     * 
     * Examples:
     * - Mini Sudoku:    2² = 4  (4×4 board)
     * - Classic Sudoku: 3² = 9  (9×9 board)
     * - Mega Sudoku:    4² = 16 (16×16 board)
     * - Giant Sudoku:   5² = 25 (25×25 board)
     */
    int board_size;
    
    /**
     * @brief Total number of cells in the board (board_size²)
     * 
     * This is a convenience field to avoid repeated calculation.
     * Invariant: total_cells == board_size * board_size
     * 
     * Examples:
     * - Mini Sudoku:    4² = 16 cells
     * - Classic Sudoku: 9² = 81 cells
     * - Mega Sudoku:    16² = 256 cells
     * - Giant Sudoku:   25² = 625 cells
     */
    int total_cells;
    
    // ─────────────────────────────────────────────────────────────
    //  Board Data (Dynamic Allocation)
    // ─────────────────────────────────────────────────────────────
    
    /**
     * @brief 2D array representing the board cells [board_size][board_size]
     * 
     * Cell values:
     * - 0: Empty cell (needs to be filled by solver/player)
     * - 1 to board_size: Valid numbers
     * 
     * Memory layout:
     * - Pointer to array of row pointers (heap allocated)
     * - Each row is a contiguous array of integers (heap allocated)
     * - Total memory: board_size * sizeof(int*) + board_size² * sizeof(int)
     * 
     * Access patterns:
     * - Direct: board->cells[row][col] (fast, but bypasses encapsulation)
     * - Safe: sudoku_board_get_cell(board, &pos) (recommended)
     * 
     * @note Allocated by sudoku_board_create_size()
     * @note Freed by sudoku_board_destroy()
     * @warning Do not free individual rows or the cells pointer directly
     */
    int **cells;
    
    // ─────────────────────────────────────────────────────────────
    //  Board Statistics
    // ─────────────────────────────────────────────────────────────
    
    /**
     * @brief Number of filled cells (non-zero values)
     * 
     * Range: 0 to total_cells
     * 
     * This field is updated by sudoku_board_update_stats() and
     * should be called after any sequence of cell modifications
     * to keep statistics accurate.
     * 
     * @see sudoku_board_update_stats() to refresh this value
     * @see sudoku_board_get_clues() for accessor function
     */
    int clues;
    
    /**
     * @brief Number of empty cells (zero values)
     * 
     * Range: 0 to total_cells
     * Invariant: clues + empty == total_cells
     * 
     * This field is updated by sudoku_board_update_stats().
     * 
     * @see sudoku_board_update_stats() to refresh this value
     * @see sudoku_board_get_empty() for accessor function
     */
    int empty;
} SudokuBoard;

// ═══════════════════════════════════════════════════════════════════
//                    SUBGRID STRUCTURE
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Represents a single subgrid within the board
 * 
 * In Sudoku, each row, column, and SUBGRID must contain each number
 * exactly once. This structure identifies a specific subgrid and
 * provides its location on the board.
 * 
 * Subgrids are indexed in row-major order (left to right, top to bottom):
 * 
 * For classic 9×9 Sudoku (9 subgrids of 3×3):
 * @code
 *  0 | 1 | 2
 * ---+---+---
 *  3 | 4 | 5
 * ---+---+---
 *  6 | 7 | 8
 * @endcode
 * 
 * For 16×16 Sudoku (16 subgrids of 4×4):
 * @code
 *  0 |  1 |  2 |  3
 * ---+----+----+---
 *  4 |  5 |  6 |  7
 * ---+----+----+---
 *  8 |  9 | 10 | 11
 * ---+----+----+---
 * 12 | 13 | 14 | 15
 * @endcode
 * 
 * The base position allows calculating any cell within the subgrid:
 * cell(i) = base + (i / subgrid_size, i % subgrid_size)
 * 
 * @see sudoku_subgrid_create() to construct from index
 */
typedef struct {
    int index;              ///< Subgrid index (0 to board_size-1)
    SudokuPosition base;    ///< Top-left corner position of this subgrid
    int subgrid_size;       ///< Size of this subgrid (k in k×k), needed for position calculations
} SudokuSubGrid;
// ═══════════════════════════════════════════════════════════════════
//                    GENERATION STATISTICS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Statistics collected during puzzle generation
 * 
 * Tracks how many cells were removed in each elimination phase,
 * providing insight into the generation process and resulting
 * difficulty. This information is useful for:
 * 
 * - Debugging generation issues
 * - Understanding puzzle characteristics
 * - Tuning difficulty parameters
 * - Performance monitoring
 * 
 * Typical values for 9×9 puzzles:
 * - phase1_removed: 9 (always, one per subgrid)
 * - phase2_removed: 15-25 (varies with puzzle structure)
 * - phase2_rounds: 3-5 (iterations until convergence)
 * - phase3_removed: 0-25 (up to PHASE3_TARGET equivalent)
 * - Total empty cells: 45-57 typically
 * 
 * @see sudoku_generate_ex() which populates this structure
 */
typedef struct {
    /**
     * @brief Cells removed in Phase 1 (Fisher-Yates selection)
     * 
     * Phase 1 removes exactly one random cell from each subgrid,
     * ensuring uniform initial distribution of empty cells.
     * 
     * For board_size N, this value equals N (one per subgrid).
     */
    int phase1_removed;
    
    /**
     * @brief Cells removed in Phase 2 (no-alternatives elimination)
     * 
     * Phase 2 iteratively removes cells whose values cannot be
     * placed elsewhere in their row, column, or subgrid. This
     * maintains solution uniqueness without expensive verification.
     * 
     * Cumulative total across all rounds.
     */
    int phase2_removed;
    
    /**
     * @brief Number of rounds Phase 2 required
     * 
     * Phase 2 runs in a loop until no more cells can be removed.
     * This counter tracks how many iterations were needed.
     * 
     * Typical range: 3-5 rounds for 9×9 puzzles.
     */
    int phase2_rounds;
    
    /**
     * @brief Cells removed in Phase 3 (verified free elimination)
     * 
     * Phase 3 attempts to remove additional cells while verifying
     * that the puzzle retains a unique solution. This is the most
     * computationally expensive phase.
     * 
     * The target number of removals is calculated proportionally
     * based on board size (see phase3.c:calculate_phase3_target()).
     */
    int phase3_removed;
    
    /**
     * @brief Total generation attempts made
     * 
     * Normally 1, but may be higher if generation fails and retries.
     * Failure is extremely rare (<0.1% for 9×9 boards) but possible
     * for edge cases or larger board sizes.
     * 
     * If this value exceeds 1, it indicates the generator encountered
     * a difficult configuration and had to restart.
     */
    int total_attempts;
} SudokuGenerationStats;

// ═══════════════════════════════════════════════════════════════════
//                    DIFFICULTY LEVELS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Difficulty classification for generated puzzles
 * 
 * Based primarily on the percentage of clues remaining after
 * generation. VERSION 3.0 uses proportional thresholds that
 * scale correctly across all board sizes.
 * 
 * Threshold percentages (v3.0):
 * | Difficulty | Clue % | 9×9 Clues | 16×16 Clues | 25×25 Clues |
 * |------------|--------|-----------|-------------|-------------|
 * | EASY       | ≥55%   | ≥45       | ≥141        | ≥344        |
 * | MEDIUM     | 43-54% | 35-44     | 110-140     | 269-343     |
 * | HARD       | 31-42% | 25-34     | 79-109      | 194-268     |
 * | EXPERT     | <31%   | <25       | <79         | <194        |
 * 
 * @note These thresholds are heuristics based on clue count only.
 *       Actual solving difficulty also depends on which techniques
 *       are required (naked pairs, X-wings, etc.).
 * 
 * @see sudoku_evaluate_difficulty() for the evaluation function
 * @see sudoku_difficulty_to_string() for string conversion
 */
typedef enum {
    SUDOKU_EASY,        ///< Many clues (≥55% of cells filled)
    SUDOKU_MEDIUM,      ///< Moderate clues (43-54% filled)
    SUDOKU_HARD,        ///< Few clues (31-42% filled)
    SUDOKU_EXPERT,      ///< Very few clues (<31% filled)
    SUDOKU_INVALID      ///< Invalid board state (error condition)
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
 * This event-driven architecture allows the library to remain
 * presentation-agnostic. The library doesn't care HOW events are
 * displayed (console, GUI, logged, etc.), only WHAT happened.
 * 
 * Event categories:
 * - Generation lifecycle: START, COMPLETE, FAILED
 * - Algorithm phases: DIAGONAL_FILL, BACKTRACK, PHASE1/2/3
 * - Cell operations: CELL_SELECTED, CELL_REMOVED, CELL_KEPT
 * 
 * @see SudokuEventCallback for how to handle events
 * @see SudokuEventData for event payload structure
 */
typedef enum {
    // ─────────────────────────────────────────────────────────────
    //  Generation Lifecycle Events
    // ─────────────────────────────────────────────────────────────
    SUDOKU_EVENT_GENERATION_START,        /**< Generation process started */
    SUDOKU_EVENT_GENERATION_COMPLETE,     /**< Entire generation succeeded */
    SUDOKU_EVENT_GENERATION_FAILED,       /**< Generation failed (rare) */
    
    // ─────────────────────────────────────────────────────────────
    //  Board Filling Events (Phase A)
    // ─────────────────────────────────────────────────────────────
    SUDOKU_EVENT_DIAGONAL_FILL_START,     /**< Filling diagonal subgrids */
    SUDOKU_EVENT_DIAGONAL_FILL_COMPLETE,  /**< Diagonal filled successfully */
    SUDOKU_EVENT_BACKTRACK_START,         /**< Backtracking phase started */
    SUDOKU_EVENT_BACKTRACK_COMPLETE,      /**< Board completion successful */
    
    // ─────────────────────────────────────────────────────────────
    //  Phase 1 Elimination Events
    // ─────────────────────────────────────────────────────────────
    SUDOKU_EVENT_PHASE1_START,            /**< Phase 1 elimination started */
    SUDOKU_EVENT_PHASE1_CELL_SELECTED,    /**< A cell selected for removal */
    SUDOKU_EVENT_PHASE1_COMPLETE,         /**< Phase 1 finished */
    
    // ─────────────────────────────────────────────────────────────
    //  Phase 2 Elimination Events
    // ─────────────────────────────────────────────────────────────
    SUDOKU_EVENT_PHASE2_START,            /**< Phase 2 elimination started */
    SUDOKU_EVENT_PHASE2_ROUND_START,      /**< New phase 2 round started */
    SUDOKU_EVENT_PHASE2_CELL_SELECTED,    /**< A cell selected for removal */
    SUDOKU_EVENT_PHASE2_ROUND_COMPLETE,   /**< Phase 2 round finished */
    SUDOKU_EVENT_PHASE2_COMPLETE,         /**< Phase 2 finished (all rounds) */
    
    // ─────────────────────────────────────────────────────────────
    //  Phase 3 Elimination Events
    // ─────────────────────────────────────────────────────────────
    SUDOKU_EVENT_PHASE3_START,            /**< Phase 3 elimination started */
    SUDOKU_EVENT_PHASE3_CELL_TESTING,     /**< Testing if cell can be removed */
    SUDOKU_EVENT_PHASE3_CELL_REMOVED,     /**< Cell successfully removed */
    SUDOKU_EVENT_PHASE3_CELL_KEPT,        /**< Cell kept (removal breaks uniqueness) */
    SUDOKU_EVENT_PHASE3_COMPLETE          /**< Phase 3 finished */
} SudokuEventType;

/**
 * @brief Data associated with a generation event
 * 
 * Contains contextual information about what happened during generation.
 * Not all fields are meaningful for all event types - check the event
 * type to determine which fields contain valid data.
 * 
 * Field validity by event category:
 * | Field              | Lifecycle | Phase Start/End | Cell Events |
 * |--------------------|-----------|-----------------|-------------|
 * | type               | ✓         | ✓               | ✓           |
 * | board              | ✓         | ✓               | ✓           |
 * | phase_number       | ✗         | ✓               | ✓           |
 * | cells_removed_total| ✗         | ✓               | ✓           |
 * | round_number       | ✗         | Phase 2 only    | Phase 2 only|
 * | row, col, value    | ✗         | ✗               | ✓           |
 * 
 * @note The board pointer is read-only and only valid during callback
 */
typedef struct {
    SudokuEventType type;       ///< Type of event that occurred
    const SudokuBoard *board;   ///< Current board state (read-only, temporary)
    
    // ─────────────────────────────────────────────────────────────
    //  Phase Information
    // ─────────────────────────────────────────────────────────────
    
    int phase_number;           ///< Which elimination phase (1, 2, or 3)
    int cells_removed_total;    ///< Total cells removed so far in this phase
    int round_number;           ///< Round/iteration number (mainly for phase 2)
    
    // ─────────────────────────────────────────────────────────────
    //  Cell-Specific Information
    // ─────────────────────────────────────────────────────────────
    
    int row;                    ///< Row of the cell (-1 if not applicable)
    int col;                    ///< Column of the cell (-1 if not applicable)
    int value;                  ///< Value that was in the cell (0 if N/A)
} SudokuEventData;

/**
 * @brief Callback function signature for generation events
 * 
 * Applications provide a function matching this signature to receive
 * notifications about generation progress. The library calls this
 * function synchronously whenever something significant happens.
 * 
 * IMPORTANT GUIDELINES:
 * - Callbacks should be fast and non-blocking
 * - Do not perform heavy I/O or computation inside callbacks
 * - Do not modify the board through the callback
 * - The event data pointer is only valid during the callback
 * 
 * @param event Pointer to event data describing what happened
 * @param user_data Custom pointer provided when registering callback
 * 
 * Example implementation:
 * @code
 * void my_progress_callback(const SudokuEventData *event, void *user_data) {
 *     switch (event->type) {
 *         case SUDOKU_EVENT_PHASE1_COMPLETE:
 *             printf("Phase 1: removed %d cells\n", event->cells_removed_total);
 *             break;
 *         case SUDOKU_EVENT_PHASE3_CELL_REMOVED:
 *             printf("Removed %d from (%d,%d)\n", 
 *                    event->value, event->row, event->col);
 *             break;
 *         default:
 *             break;  // Ignore other events
 *     }
 * }
 * @endcode
 * 
 * @see sudoku_generate_ex() for how to register callbacks
 */
typedef void (*SudokuEventCallback)(const SudokuEventData *event, void *user_data);

/**
 * @brief Configuration for puzzle generation
 * 
 * Allows customizing the generation process, including registering
 * a callback to monitor progress.
 * 
 * All fields are optional and can be set to NULL/0 for defaults.
 * 
 * Example usage:
 * @code
 * SudokuGenerationConfig config = {
 *     .callback = my_progress_callback,
 *     .user_data = &my_context,
 *     .max_attempts = 5
 * };
 * sudoku_generate_ex(&board, &config, &stats);
 * @endcode
 */
typedef struct {
    /**
     * @brief Optional callback function for progress monitoring
     * 
     * Set to NULL to disable event notifications.
     * When set, the function is called for each generation event.
     */
    SudokuEventCallback callback;
    
    /**
     * @brief Custom data passed to callback function
     * 
     * Can be NULL. This pointer is passed unchanged to every
     * callback invocation, allowing the callback to access
     * application-specific context.
     */
    void *user_data;
    
    /**
     * @brief Maximum generation attempts before giving up
     * 
     * If generation fails (extremely rare), the generator will
     * retry up to this many times before returning failure.
     * 
     * Set to 0 to use the default value (typically 5).
     */
    int max_attempts;
} SudokuGenerationConfig;

#endif // SUDOKU_TYPES_H

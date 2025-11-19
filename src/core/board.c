/**
 * @file board.c 
 * @brief Implementation of Sudoku board operations with configurable sizes
 * @author Gonzalo Ramírez
 * @date 2025-11-18
 * 
 * This module provides the foundational layer for all Sudoku board operations.
 * Now supports configurable board sizes through dynamic memory allocation.
 * 
 * BREAKING CHANGES from v2.2.x:
 * - SudokuBoard structure changed from static to dynamic allocation
 * - cells array is now int** instead of int[9][9]
 * - Stack allocation no longer supported - must use sudoku_board_create()
 * - Board dimensions are now runtime-configurable
 */

#include <stdlib.h>                         // For malloc(), calloc(), free()
#include <stdio.h>                          // For fprintf()
#include <assert.h>                         // For assert()
#include "sudoku/core/board.h"              // Our public header
#include "sudoku/core/types.h"              // For structure definitions
#include "internal/board_internal.h"        // For internal function declarations
#include "internal/algorithms_internal.h"   // For sudoku_generate_permutation()

// ═══════════════════════════════════════════════════════════════════
//                    PRIVATE: MEMORY ALLOCATION HELPERS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Allocate a 2D integer array dynamically
 * 
 * Creates a 2D array using pointer-to-pointer technique. First allocates
 * an array of row pointers, then allocates each row. Uses calloc() for
 * rows to initialize all cells to zero automatically.
 * 
 * Memory layout:
 * @code
 *   array → [ptr0] → [0][0][0]...[0]  (row 0)
 *           [ptr1] → [0][0][0]...[0]  (row 1)
 *           [ptr2] → [0][0][0]...[0]  (row 2)
 *           ...
 * @endcode
 * 
 * @param[in] rows Number of rows to allocate
 * @param[in] cols Number of columns per row
 * @return Pointer to allocated 2D array, or NULL on allocation failure
 * 
 * @note Uses calloc() for rows → cells are initialized to 0
 * @note On failure, frees any partially allocated memory before returning NULL
 * @note Caller must free with free_2d_array()
 * 
 * Example:
 * @code
 * int **cells = allocate_2d_array(9, 9);
 * if (cells != NULL) {
 *     cells[0][0] = 5;  // Access like normal 2D array
 *     free_2d_array(cells, 9);
 * }
 * @endcode
 * 
 * @internal This is a private helper, not exposed in public API
 */
static int** allocate_2d_array(int rows, int cols) {
    // Allocate array of row pointers
    int **array = (int**)malloc(rows * sizeof(int*));
    if (array == NULL) {
        fprintf(stderr, "Error: Failed to allocate row pointers (%d rows)\n", rows);
        return NULL;
    }
    
    // Allocate each row
    for (int i = 0; i < rows; i++) {
        // Using calloc() to initialize cells to 0 automatically
        array[i] = (int*)calloc(cols, sizeof(int));
        
        if (array[i] == NULL) {
            // Allocation failed - clean up what we've allocated so far
            fprintf(stderr, "Error: Failed to allocate row %d of %d\n", i, rows);
            
            // Free all previously allocated rows
            for (int j = 0; j < i; j++) {
                free(array[j]);
            }
            
            // Free the row pointer array
            free(array);
            
            return NULL;
        }
    }
    
    return array;
}

/**
 * @brief Free a 2D integer array
 * 
 * Releases all memory associated with a 2D array allocated by
 * allocate_2d_array(). First frees each row, then frees the
 * array of row pointers.
 * 
 * @param[in] array Pointer to 2D array to free (can be NULL)
 * @param[in] rows Number of rows in the array
 * 
 * @note Safe to call with NULL array (does nothing)
 * @note rows must match the value used in allocate_2d_array()
 * 
 * Example:
 * @code
 * int **cells = allocate_2d_array(9, 9);
 * // ... use cells ...
 * free_2d_array(cells, 9);
 * @endcode
 * 
 * @internal This is a private helper, not exposed in public API
 */
static void free_2d_array(int **array, int rows) {
    if (array == NULL) {
        return;  // Nothing to free
    }
    
    // Free each row
    for (int i = 0; i < rows; i++) {
        free(array[i]);
    }
    
    // Free the array of row pointers
    free(array);
}

// ═══════════════════════════════════════════════════════════════════
//                    PUBLIC API: MEMORY MANAGEMENT
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Create a Sudoku board with specific subgrid size
 * 
 * Allocates and initializes a new board with configurable dimensions.
 * This is the primary way to create boards of different sizes.
 * 
 * The function performs the following steps:
 * 1. Validates the subgrid size (must be 2-5)
 * 2. Allocates the SudokuBoard structure
 * 3. Calculates derived dimensions (board_size, total_cells)
 * 4. Allocates the 2D cells array
 * 5. Initializes all cells to 0 and sets metadata
 * 
 * @param[in] subgrid_size Size of subgrids (valid: 2, 3, 4, 5)
 * @return Pointer to newly created board, or NULL on error
 * 
 * @note Returns NULL if:
 *       - subgrid_size is out of range (< 2 or > 5)
 *       - Memory allocation fails
 * @note Caller must free with sudoku_board_destroy()
 * @note All cells are initialized to 0 (empty)
 * 
 * Memory requirements:
 * - 2×2 (4×4):   ~120 bytes
 * - 3×3 (9×9):   ~400 bytes
 * - 4×4 (16×16): ~1.3 KB
 * - 5×5 (25×25): ~3.2 KB
 * 
 * Performance:
 * - Allocation is O(n²) where n = board_size
 * - Typically takes < 1ms even for 25×25 boards
 * 
 * Example:
 * @code
 * // Create a 16×16 Sudoku board (4×4 subgrids)
 * SudokuBoard *board = sudoku_board_create_size(4);
 * if (board == NULL) {
 *     fprintf(stderr, "Failed to create board\n");
 *     return 1;
 * }
 * 
 * printf("Created %d×%d board with %d cells\n",
 *        board->board_size, board->board_size, board->total_cells);
 * 
 * // Use board...
 * sudoku_board_destroy(board);
 * @endcode
 * 
 * @see sudoku_board_create() for default 9×9 board
 * @see sudoku_board_destroy() to free memory
 */
SudokuBoard* sudoku_board_create_size(int subgrid_size) {
    // ─────────────────────────────────────────────────────────────
    //  Step 1: Validate subgrid size
    // ─────────────────────────────────────────────────────────────
    
    if (subgrid_size < 2 || subgrid_size > 5) {
        fprintf(stderr, "Error: Invalid subgrid size %d (must be 2-5)\n", subgrid_size);
        fprintf(stderr, "       Valid sizes:\n");
        fprintf(stderr, "       - 2: Creates 4×4 board (16 cells)\n");
        fprintf(stderr, "       - 3: Creates 9×9 board (81 cells) [classic]\n");
        fprintf(stderr, "       - 4: Creates 16×16 board (256 cells)\n");
        fprintf(stderr, "       - 5: Creates 25×25 board (625 cells)\n");
        return NULL;
    }
    
    // ─────────────────────────────────────────────────────────────
    //  Step 2: Allocate board structure
    // ─────────────────────────────────────────────────────────────
    
    SudokuBoard *board = (SudokuBoard*)malloc(sizeof(SudokuBoard));
    if (board == NULL) {
        fprintf(stderr, "Error: Failed to allocate SudokuBoard structure\n");
        return NULL;
    }
    
    // ─────────────────────────────────────────────────────────────
    //  Step 3: Calculate and store dimensions
    // ─────────────────────────────────────────────────────────────
    
    board->subgrid_size = subgrid_size;
    board->board_size = subgrid_size * subgrid_size;
    board->total_cells = board->board_size * board->board_size;
    
    // ─────────────────────────────────────────────────────────────
    //  Step 4: Allocate cells array
    // ─────────────────────────────────────────────────────────────
    
    board->cells = allocate_2d_array(board->board_size, board->board_size);
    if (board->cells == NULL) {
        fprintf(stderr, "Error: Failed to allocate %d×%d cells array\n",
                board->board_size, board->board_size);
        free(board);  // Clean up the board structure
        return NULL;
    }
    
    // ─────────────────────────────────────────────────────────────
    //  Step 5: Initialize metadata
    // ─────────────────────────────────────────────────────────────
    
    // Cells are already 0 (calloc in allocate_2d_array)
    board->clues = 0;
    board->empty = board->total_cells;
    
    return board;
}

/**
 * @brief Create a board with default size (9×9)
 * 
 * Convenience wrapper for creating a classic 9×9 Sudoku board.
 * This maintains backward compatibility with code that expects
 * the standard board size.
 * 
 * Equivalent to: sudoku_board_create_size(3)
 * 
 * @return Pointer to newly created 9×9 board, or NULL on error
 * 
 * @note Caller must free with sudoku_board_destroy()
 * @note This is the recommended way to create a standard Sudoku board
 * 
 * Example:
 * @code
 * // Create classic 9×9 board
 * SudokuBoard *board = sudoku_board_create();
 * if (board != NULL) {
 *     // Board is ready to use
 *     sudoku_board_destroy(board);
 * }
 * @endcode
 * 
 * @see sudoku_board_create_size() for non-standard sizes
 */
SudokuBoard* sudoku_board_create(void) {
    return sudoku_board_create_size(SUDOKU_DEFAULT_SUBGRID_SIZE);
}

/**
 * @brief Destroy a Sudoku board and free its memory
 * 
 * Releases all memory associated with a dynamically allocated board:
 * 1. Frees the 2D cells array (each row, then the row pointer array)
 * 2. Frees the board structure itself
 * 
 * After calling this function, the board pointer is no longer valid
 * and must not be dereferenced.
 * 
 * @param[in] board Pointer to board to destroy (can be NULL)
 * 
 * @post If board was not NULL, all its memory is freed
 * @post Board pointer is no longer valid
 * 
 * @note Safe to call with NULL (does nothing, like standard free())
 * @note Do NOT call on stack-allocated boards (only heap-allocated)
 * @note After calling, set pointer to NULL to avoid use-after-free
 * 
 * Example:
 * @code
 * SudokuBoard *board = sudoku_board_create();
 * // ... use board ...
 * sudoku_board_destroy(board);
 * board = NULL;  // Good practice: prevent use-after-free
 * @endcode
 * 
 * @see sudoku_board_create() and sudoku_board_create_size()
 */
void sudoku_board_destroy(SudokuBoard *board) {
    if (board == NULL) {
        return;  // Nothing to free
    }
    
    // Free the 2D cells array
    free_2d_array(board->cells, board->board_size);
    
    // Free the board structure itself
    free(board);
}

// ═══════════════════════════════════════════════════════════════════
//                    PUBLIC API: BOARD INITIALIZATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Initialize a board to completely empty state
 * 
 * Sets all cells to zero (representing empty cells) and resets
 * metadata counters. This is automatically called by
 * sudoku_board_create_size(), but can be called manually to
 * reset an existing board.
 * 
 * The function now uses the board's dynamic board_size instead
 * of a hardcoded constant, making it work for any board size.
 * 
 * @param[out] board Pointer to the board structure to initialize
 * 
 * @pre board != NULL
 * @pre board->cells != NULL (memory must be allocated)
 * @post All cells contain 0 (empty)
 * @post board->clues == 0
 * @post board->empty == board->total_cells
 * 
 * @note This function has no return value - initialization cannot fail
 * @note Time complexity: O(n²) where n = board_size
 * 
 * Example:
 * @code
 * SudokuBoard *board = sudoku_board_create();
 * // ... use board ...
 * sudoku_board_init(board);  // Reset to empty state
 * // Board is now ready for new generation
 * @endcode
 */
void sudoku_board_init(SudokuBoard *board) {
    // Zero out all cells using dynamic board size
    for(int i = 0; i < board->board_size; i++) {
        for(int j = 0; j < board->board_size; j++) {
            board->cells[i][j] = 0;
        }
    }
    
    // Initialize metadata to reflect empty board state
    board->clues = 0;
    board->empty = board->total_cells;
}

/**
 * @brief Recalculate and update board statistics
 * 
 * Scans the entire board to count filled and empty cells, updating
 * the board's metadata fields. This should be called after any series
 * of cell modifications to ensure statistics remain accurate.
 * 
 * The function now uses the board's dynamic board_size instead
 * of a hardcoded constant.
 * 
 * @param[in,out] board Pointer to the board to update
 * 
 * @pre board != NULL
 * @post board->clues == number of non-zero cells
 * @post board->empty == board->total_cells - board->clues
 * 
 * @note Time complexity: O(n²) where n = board_size
 * @note This is safe to call multiple times (idempotent)
 * 
 * Common usage pattern:
 * @code
 * // After modifying several cells during generation/elimination
 * sudoku_board_set_cell(board, 3, 4, 7);
 * sudoku_board_set_cell(board, 5, 2, 0);  // Removing a cell
 * // ... more modifications ...
 * sudoku_board_update_stats(board);  // Refresh statistics
 * printf("Board has %d clues\n", board->clues);
 * @endcode
 */
void sudoku_board_update_stats(SudokuBoard *board) {
    int count = 0;
    
    // Scan entire board counting filled cells (using dynamic size)
    for(int i = 0; i < board->board_size; i++) {
        for(int j = 0; j < board->board_size; j++) {
            if(board->cells[i][j] != 0) {
                count++;  // Found a clue (non-empty cell)
            }
        }
    }
    
    // Update metadata based on count
    board->clues = count;
    board->empty = board->total_cells - count;
}

// ═══════════════════════════════════════════════════════════════════
//                    PUBLIC API: CELL ACCESS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Get the value of a specific cell
 * 
 * Retrieves the current value at the specified board position.
 * Provides read-only access to cell data through a controlled interface.
 * 
 * @param[in] board Pointer to the board
 * @param[in] row Row index (0 to board_size-1)
 * @param[in] col Column index (0 to board_size-1)
 * @return Cell value (0-board_size), where 0 represents empty cell
 * 
 * @pre board != NULL
 * @pre 0 <= row < board->board_size
 * @pre 0 <= col < board->board_size
 * 
 * @note Violating preconditions results in assertion failure in debug builds
 *       and undefined behavior in release builds
 * @note This function does not modify the board (const parameter)
 * 
 * Example:
 * @code
 * int value = sudoku_board_get_cell(board, 4, 5);
 * if (value == 0) {
 *     printf("Cell at (4,5) is empty\n");
 * } else {
 *     printf("Cell at (4,5) contains %d\n", value);
 * }
 * @endcode
 * 
 * @see sudoku_board_set_cell() to modify cell values
 */
int sudoku_board_get_cell(const SudokuBoard *board, int row, int col) {
    // Defensive programming: validate inputs in debug builds
    assert(board != NULL);
    assert(row >= 0 && row < board->board_size);
    assert(col >= 0 && col < board->board_size);
    
    return board->cells[row][col];
}

/**
 * @brief Set the value of a specific cell
 * 
 * Updates the cell at the specified position with a new value.
 * Provides controlled write access with parameter validation.
 * 
 * Important: This function does NOT perform Sudoku rule validation.
 * It only validates parameter ranges. Use sudoku_is_safe_position()
 * before calling this if you need to verify the move is legal.
 * 
 * Important: This function does NOT automatically update board statistics.
 * Call sudoku_board_update_stats() after a series of modifications
 * if you need accurate clues/empty counts.
 * 
 * @param[in,out] board Pointer to the board to modify
 * @param[in] row Row index (0 to board_size-1)
 * @param[in] col Column index (0 to board_size-1)
 * @param[in] value New cell value (0 to board_size)
 * @return true if the cell was successfully updated, false if any
 *         parameter was invalid
 * 
 * @pre board != NULL (enforced by return value)
 * @post If returns true, board->cells[row][col] == value
 * @post If returns false, board is not modified
 * 
 * @note Unlike get_cell(), this uses return value for validation rather
 *       than assertions, allowing graceful error handling
 * @note Board statistics are NOT automatically updated
 * @note Valid values: 0 (empty) to board_size (maximum number)
 * 
 * Example:
 * @code
 * if (sudoku_board_set_cell(board, 3, 4, 7)) {
 *     printf("Successfully set cell (3,4) to 7\n");
 * } else {
 *     printf("Invalid parameters\n");
 * }
 * @endcode
 * 
 * @see sudoku_board_get_cell() to read cell values
 * @see sudoku_board_update_stats() to refresh statistics
 * @see sudoku_is_safe_position() to validate Sudoku rules
 */
bool sudoku_board_set_cell(SudokuBoard *board, int row, int col, int value) {
    // Comprehensive parameter validation
    if (board == NULL ||
        row < 0 || row >= board->board_size ||
        col < 0 || col >= board->board_size ||
        value < 0 || value > board->board_size) {
        return false;  // Invalid parameters - operation rejected
    }
    
    // Parameters valid - update the cell
    board->cells[row][col] = value;
    return true;  // Success
}

// ═══════════════════════════════════════════════════════════════════
//                    PUBLIC API: STATISTICS ACCESS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Get the number of filled cells (clues) in the board
 * 
 * Returns the cached count of non-zero cells. This count is maintained
 * by sudoku_board_init() and sudoku_board_update_stats().
 * 
 * @param[in] board Pointer to the board
 * @return Number of filled cells (0 to total_cells)
 * 
 * @pre board != NULL
 * 
 * @note This returns a cached value. After modifying cells with
 *       sudoku_board_set_cell(), call sudoku_board_update_stats()
 *       to ensure this value is accurate
 * 
 * Example:
 * @code
 * int clues = sudoku_board_get_clues(board);
 * printf("Puzzle has %d clues\n", clues);
 * @endcode
 * 
 * @see sudoku_board_get_empty() for the count of empty cells
 * @see sudoku_board_update_stats() to refresh cached statistics
 */
int sudoku_board_get_clues(const SudokuBoard *board) {
    assert(board != NULL);
    return board->clues;
}

/**
 * @brief Get the number of empty cells in the board
 * 
 * Returns the cached count of zero-valued cells.
 * 
 * @param[in] board Pointer to the board
 * @return Number of empty cells (0 to total_cells)
 * 
 * @pre board != NULL
 * @note Invariant: get_clues() + get_empty() == total_cells
 * 
 * Example:
 * @code
 * int remaining = sudoku_board_get_empty(board);
 * printf("The puzzle has %d cells left to fill\n", remaining);
 * @endcode
 */
int sudoku_board_get_empty(const SudokuBoard *board) {
    assert(board != NULL);
    return board->empty;
}

// ═══════════════════════════════════════════════════════════════════
//                    PUBLIC API: DIMENSION QUERIES (NEW)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Get the subgrid size
 * 
 * Returns the size of each subgrid (the n in n×n subgrids).
 * This is the fundamental dimension that determines board size.
 * 
 * @param[in] board Pointer to the board
 * @return Subgrid size (2, 3, 4, or 5)
 * 
 * @pre board != NULL
 * 
 * Example:
 * @code
 * int subgrid = sudoku_board_get_subgrid_size(board);
 * printf("Board has %d×%d subgrids\n", subgrid, subgrid);
 * @endcode
 */
int sudoku_board_get_subgrid_size(const SudokuBoard *board) {
    assert(board != NULL);
    return board->subgrid_size;
}

/**
 * @brief Get the board size
 * 
 * Returns the dimensions of the board (the n in n×n board).
 * This is calculated as subgrid_size².
 * 
 * @param[in] board Pointer to the board
 * @return Board size (4, 9, 16, or 25)
 * 
 * @pre board != NULL
 * 
 * Example:
 * @code
 * int size = sudoku_board_get_board_size(board);
 * printf("Board is %d×%d\n", size, size);
 * @endcode
 */
int sudoku_board_get_board_size(const SudokuBoard *board) {
    assert(board != NULL);
    return board->board_size;
}

/**
 * @brief Get total number of cells
 * 
 * Returns the total number of cells in the board.
 * This is calculated as board_size².
 * 
 * @param[in] board Pointer to the board
 * @return Total cells (16, 81, 256, or 625)
 * 
 * @pre board != NULL
 * 
 * Example:
 * @code
 * int total = sudoku_board_get_total_cells(board);
 * printf("Board has %d cells total\n", total);
 * @endcode
 */
int sudoku_board_get_total_cells(const SudokuBoard *board) {
    assert(board != NULL);
    return board->total_cells;
}

// ═══════════════════════════════════════════════════════════════════
//                    PUBLIC API: SUBGRID GEOMETRY
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Create a subgrid structure from an index
 * 
 * Constructs a SudokuSubGrid structure representing one of the subgrids
 * that compose the Sudoku board. Subgrids are indexed in row-major order
 * (left to right, top to bottom).
 * 
 * For 9×9 board (3×3 subgrids):
 * @code
 *  0 | 1 | 2
 * ---+---+---
 *  3 | 4 | 5
 * ---+---+---
 *  6 | 7 | 8
 * @endcode
 * 
 * The function calculates the top-left corner (base position) of the
 * subgrid using integer arithmetic.
 * 
 * @param[in] index Subgrid index (0 to board_size-1)
 * @return Initialized SudokuSubGrid structure with index and base position
 * 
 * @pre 0 <= index <= board_size-1
 * @post return.index == index
 * @post return.base contains valid top-left corner coordinates
 * 
 * @note The calculation uses integer division to map index to grid position
 * 
 * Example: Creating center subgrid of 9×9 board (index 4)
 * @code
 * SudokuSubGrid center = sudoku_subgrid_create(4);
 * // center.index == 4
 * // center.base.row == 3, center.base.col == 3
 * @endcode
 */
SudokuSubGrid sudoku_subgrid_create(int index) {
    SudokuSubGrid sg;
    sg.index = index;
    
    // Calculate top-left corner of this subgrid
    // For 9×9: index 4 → (4/3)*3 = 3 (row), (4%3)*3 = 3 (col)
    // For 16×16: index 7 → (7/4)*4 = 4 (row), (7%4)*4 = 12 (col)
    sg.base.row = (index / SUBGRID_SIZE) * SUBGRID_SIZE;
    sg.base.col = (index % SUBGRID_SIZE) * SUBGRID_SIZE;
    
    return sg;
}

/**
 * @brief Get absolute board position from subgrid-relative cell index
 * 
 * Converts a cell index within a subgrid to absolute board coordinates.
 * This allows treating each subgrid as its own mini-grid with positions
 * numbered 0 to (subgrid_size²-1).
 * 
 * Cell indices within a 3×3 subgrid:
 * @code
 *  0 1 2
 *  3 4 5
 *  6 7 8
 * @endcode
 * 
 * The function maps these indices to absolute positions by adding the
 * subgrid's base offset to the relative position within the subgrid.
 * 
 * @param[in] sg Pointer to the subgrid structure defining base position
 * @param[in] cell_index Cell index within subgrid (0 to subgrid_size²-1)
 * @return Absolute position on the full board
 * 
 * @pre sg != NULL
 * @pre 0 <= cell_index < subgrid_size²
 * @post return contains valid board coordinates
 * 
 * Example: Center subgrid (base 3,3), cell 4 (center of subgrid)
 * @code
 * SudokuSubGrid center = sudoku_subgrid_create(4);
 * SudokuPosition pos = sudoku_subgrid_get_position(&center, 4);
 * // pos.row == 3 + (4/3) = 4
 * // pos.col == 3 + (4%3) = 4
 * // Result: position (4,4) which is the center of the entire board
 * @endcode
 */
SudokuPosition sudoku_subgrid_get_position(const SudokuSubGrid *sg, int cell_index) {
    SudokuPosition pos;
    
    // Calculate position relative to subgrid's top-left corner
    pos.row = sg->base.row + (cell_index / SUBGRID_SIZE);
    pos.col = sg->base.col + (cell_index % SUBGRID_SIZE);
    
    return pos;
}

// ═══════════════════════════════════════════════════════════════════
//                    INTERNAL: GENERATION HELPERS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Indices of the three diagonal subgrids
 * 
 * For 9×9 board: {0, 4, 8} (top-left, center, bottom-right)
 * For 4×4 board: {0, 3} would be diagonal (but we use all appropriate ones)
 * 
 * These subgrids are independent - placing numbers in one cannot
 * conflict with the others since they share no rows, columns, or regions.
 * 
 * @note For non-9×9 boards, this might need adjustment
 */
static const int DIAGONAL_INDICES[3] = {0, 4, 8};

/**
 * @brief Fill a specific subgrid with random numbers using Fisher-Yates
 * 
 * Internal helper function used during diagonal initialization phase of
 * puzzle generation. Fills all cells of the specified subgrid with a
 * random permutation of numbers 1 to board_size.
 * 
 * @param[in,out] board Pointer to the board being generated
 * @param[in] sg Pointer to the subgrid structure to fill
 * 
 * @pre board != NULL && sg != NULL
 * @pre sg represents one of the diagonal subgrids for conflict-free filling
 * @post All cells in the subgrid contain unique numbers 1 to board_size
 * 
 * @note This function is silent - progress events are emitted by caller
 * @note Uses sudoku_generate_permutation() for Fisher-Yates shuffle
 * 
 * @internal This is an internal helper not exposed in public API
 */
void fillSubGrid(SudokuBoard *board, const SudokuSubGrid *sg) {
    // Generate random permutation of 1 to board_size
    int numbers[board->board_size];  // VLA for flexibility
    sudoku_generate_permutation(numbers, board->board_size, 1);
    
    // Place each number in the corresponding cell of the subgrid
    for(int i = 0; i < board->board_size; i++) {
        SudokuPosition pos = sudoku_subgrid_get_position(sg, i);
        board->cells[pos.row][pos.col] = numbers[i];
    }
}

/**
 * @brief Fill the three diagonal subgrids with random permutations
 * 
 * First phase of the puzzle generation algorithm. Fills the diagonal
 * subgrids with random permutations.
 * 
 * For 9×9: Fills subgrids 0, 4, 8 (top-left, center, bottom-right)
 * 
 * These subgrids are independent because they share no rows, columns,
 * or regions with each other. This allows us to fill them without
 * validation, which is efficient and ensures uniform distribution.
 * 
 * @param[in,out] board Pointer to the board to initialize
 * 
 * @pre board != NULL
 * @pre board is initialized (typically via sudoku_board_init)
 * @post Diagonal subgrids contain random valid permutations
 * 
 * @note This function is silent - events emitted from generator.c
 * @note For non-9×9 boards, this may need adjustment
 * 
 * @internal This is an internal helper not exposed in public API
 */
void fillDiagonal(SudokuBoard *board) {
    // Fill each of the three independent diagonal subgrids
    for(int i = 0; i < 3; i++) {
        SudokuSubGrid sg = sudoku_subgrid_create(DIAGONAL_INDICES[i]);
        fillSubGrid(board, &sg);
    }
}

/**
 * @file board.c
 * @brief Implementation of Sudoku board operations with dynamic memory
 * @author Gonzalo Ramírez
 * @date 2025-11-18
 * 
 * PHASE 2A REFACTORING: DYNAMIC MEMORY ARCHITECTURE
 * ==================================================
 * 
 * This module now implements fully dynamic memory allocation for boards,
 * enabling support for multiple board sizes (4×4, 9×9, 16×16, 25×25).
 * 
 * KEY CHANGES FROM v2.2.x:
 * - Replaced static int cells[9][9] with dynamic int **cells
 * - Added dimension tracking (subgrid_size, board_size, total_cells)
 * - Implemented proper memory allocation and deallocation patterns
 * - Maintained identical API surface for backward compatibility
 * 
 * CRITICAL MEMORY MANAGEMENT RULES:
 * - Every sudoku_board_create*() MUST pair with sudoku_board_destroy()
 * - Deallocation must happen in REVERSE order of allocation
 * - Always check malloc() return values for NULL (allocation failure)
 */

#include "sudoku/core/board.h"
#include "internal/board_internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// ═══════════════════════════════════════════════════════════════════
//                    MEMORY MANAGEMENT
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Allocate the 2D dynamic array for cells
 * 
 * This helper function encapsulates the two-step allocation process
 * required for dynamic 2D arrays in C:
 * 
 * Step 1: Allocate array of row pointers
 * Step 2: For each row, allocate array of column values
 * 
 * MEMORY LAYOUT CREATED:
 * 
 * board->cells → [ptr_row0] → [cell][cell][cell]...[cell]
 *                [ptr_row1] → [cell][cell][cell]...[cell]
 *                [ptr_row2] → [cell][cell][cell]...[cell]
 *                ...
 *                [ptr_rowN] → [cell][cell][cell]...[cell]
 * 
 * WHY TWO STEPS?
 * Because C doesn't support variable-length 2D arrays natively. The
 * pointer-to-pointer approach gives us the same [][] syntax while
 * allowing runtime size determination.
 * 
 * @param board Board structure to allocate cells for
 * @param size Dimension of the board (both rows and columns)
 * @return true if allocation succeeded, false if malloc failed
 * 
 * @note On failure, no partial allocations remain (cleanup is performed)
 * @note Time complexity: O(n) where n = size
 * @note Memory allocated: size * sizeof(int*) + size * size * sizeof(int)
 */
static bool allocate_cells(SudokuBoard *board, int size) {
    // STEP 1: Allocate array of row pointers
    // This creates space for 'size' pointers, each will point to a row
    board->cells = (int**)malloc(size * sizeof(int*));
    if (board->cells == NULL) {
        // Allocation failed - probably out of memory
        return false;
    }
    
    // STEP 2: Allocate each individual row
    // Each row is an array of 'size' integers
    for (int i = 0; i < size; i++) {
        board->cells[i] = (int*)malloc(size * sizeof(int));
        if (board->cells[i] == NULL) {
            // Allocation failed for this row
            // CRITICAL: We must clean up the rows we already allocated
            // to avoid memory leaks
            
            // Free all previously allocated rows
            for (int j = 0; j < i; j++) {
                free(board->cells[j]);
            }
            
            // Free the row pointer array
            free(board->cells);
            board->cells = NULL;
            
            return false;
        }
    }
    
    // Success! All rows allocated successfully
    return true;
}

/**
 * @brief Free the 2D dynamic array of cells
 * 
 * This function performs the inverse operation of allocate_cells().
 * It's CRITICAL that we free in the reverse order of allocation:
 * 
 * 1. Free each row (the arrays pointed to by row pointers)
 * 2. Free the array of row pointers itself
 * 
 * WHY THIS ORDER?
 * If we freed board->cells first, we'd lose the pointers to the
 * individual rows, making them unreachable and causing memory leaks.
 * 
 * MEMORY LEAK SCENARIO (WRONG ORDER):
 * 
 * free(board->cells);     // ← Lose row pointers!
 * // Now cells[0], cells[1], ... are orphaned in memory
 * // We have no way to free them → MEMORY LEAK
 * 
 * CORRECT ORDER (THIS FUNCTION):
 * 
 * for each row: free(cells[i]);  // ← Free each row
 * free(board->cells);             // ← Then free pointer array
 * 
 * @param board Board whose cells should be freed
 * 
 * @note Safe to call even if cells is NULL (does nothing)
 * @note After this call, board->cells is set to NULL for safety
 */
static void free_cells(SudokuBoard *board) {
    if (board->cells == NULL) {
        return;  // Nothing to free
    }
    
    // Free each row individually
    // IMPORTANT: We use board->board_size, not a hardcoded value
    for (int i = 0; i < board->board_size; i++) {
        free(board->cells[i]);
    }
    
    // Free the array of row pointers
    free(board->cells);
    
    // Set to NULL to avoid double-free or use-after-free bugs
    board->cells = NULL;
}

/**
 * @brief Create a Sudoku board with specific subgrid size
 * 
 * This is the primary constructor for SudokuBoard. It allocates all
 * necessary memory and initializes the structure to represent an empty
 * board of the requested size.
 * 
 * ALLOCATION PERFORMED:
 * 1. SudokuBoard structure itself: sizeof(SudokuBoard)
 * 2. Array of row pointers: board_size * sizeof(int*)
 * 3. Each row array: board_size * sizeof(int), repeated board_size times
 * 
 * TOTAL MEMORY: sizeof(SudokuBoard) + board_size * sizeof(int*) + 
 *               board_size² * sizeof(int)
 * 
 * Examples:
 * - 4×4 board: ~88 bytes (struct) + 16 bytes (ptrs) + 64 bytes (cells) = ~168 bytes
 * - 9×9 board: ~88 bytes + 36 bytes + 324 bytes = ~448 bytes
 * - 16×16 board: ~88 bytes + 64 bytes + 1024 bytes = ~1176 bytes
 * 
 * @param subgrid_size Size of subgrids (valid: 2, 3, 4, 5)
 * @return Pointer to newly created board, or NULL on error
 * 
 * @post If successful, returned board is fully initialized and empty
 * @post If failed (returns NULL), no memory leaks occur
 * 
 * @note Caller MUST call sudoku_board_destroy() when done
 * @note Returns NULL if subgrid_size is invalid or allocation fails
 */
SudokuBoard* sudoku_board_create_size(int subgrid_size) {
    // VALIDATION: Check for valid subgrid size
    // Valid Sudoku sizes: 2 (4×4), 3 (9×9), 4 (16×16), 5 (25×25)
    // Larger sizes are theoretically valid but computationally expensive
    if (subgrid_size < 2 || subgrid_size > 5) {
        fprintf(stderr, "Error: Invalid subgrid size %d (valid: 2-5)\n", 
                subgrid_size);
        return NULL;
    }
    
    // STEP 1: Allocate the board structure itself
    SudokuBoard *board = (SudokuBoard*)malloc(sizeof(SudokuBoard));
    if (board == NULL) {
        fprintf(stderr, "Error: Failed to allocate SudokuBoard structure\n");
        return NULL;
    }
    
    // STEP 2: Calculate and store dimensions
    // These calculations define the board geometry
    board->subgrid_size = subgrid_size;
    board->board_size = subgrid_size * subgrid_size;
    board->total_cells = board->board_size * board->board_size;
    
    // STEP 3: Allocate the dynamic 2D array for cells
    if (!allocate_cells(board, board->board_size)) {
        // Allocation failed - free the board structure and return NULL
        fprintf(stderr, "Error: Failed to allocate cells array for %dx%d board\n",
                board->board_size, board->board_size);
        free(board);
        return NULL;
    }
    
    // STEP 4: Initialize the board to empty state
    // This sets all cells to 0 and updates statistics
    sudoku_board_init(board);
    
    // Success! Return the fully initialized board
    return board;
}

/**
 * @brief Create a board with default size (9×9)
 * 
 * Convenience wrapper for the classic 9×9 Sudoku. This maintains
 * backward compatibility with code that doesn't need configurable sizes.
 * 
 * Implementation is trivial: just delegates to sudoku_board_create_size(3)
 * 
 * @return Pointer to newly created 9×9 board, or NULL on error
 */
SudokuBoard* sudoku_board_create(void) {
    return sudoku_board_create_size(3);  // 3×3 subgrids → 9×9 board
}

/**
 * @brief Destroy a Sudoku board and free all associated memory
 * 
 * This is the destructor for SudokuBoard. It releases ALL memory
 * associated with the board, including the 2D cells array and the
 * board structure itself.
 * 
 * DEALLOCATION ORDER:
 * 1. Free each row of cells: free(cells[i])
 * 2. Free array of row pointers: free(cells)
 * 3. Free board structure: free(board)
 * 
 * This order is CRITICAL - we must free inner structures before
 * outer structures to avoid memory leaks.
 * 
 * @param board Pointer to board to destroy (can be NULL)
 * 
 * @post All memory associated with board is freed
 * @post board pointer is invalid after this call (caller should set to NULL)
 * 
 * @note Safe to call with NULL pointer (does nothing)
 * @note DO NOT call on stack-allocated boards (only heap-allocated)
 * @note After calling, the pointer is dangling - set it to NULL in caller
 */
void sudoku_board_destroy(SudokuBoard *board) {
    if (board == NULL) {
        return;  // Nothing to destroy
    }
    
    // Free the 2D cells array (handles NULL gracefully)
    free_cells(board);
    
    // Free the board structure itself
    free(board);
    
    // Note: We don't set board to NULL here because we can't modify
    // the caller's pointer. The caller should do: 
    // sudoku_board_destroy(board); board = NULL;
}

// ═══════════════════════════════════════════════════════════════════
//                    BOARD INITIALIZATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Initialize a board to empty state (all cells = 0)
 * 
 * This function resets the board to a clean empty state. It's called
 * automatically by sudoku_board_create_size(), but can also be called
 * manually to reset an existing board.
 * 
 * OPERATIONS PERFORMED:
 * 1. Set all cells to 0 (empty)
 * 2. Set clues = 0 (no filled cells)
 * 3. Set empty = total_cells (all cells empty)
 * 
 * @param board Pointer to board to initialize
 * 
 * @pre board != NULL
 * @pre board->cells != NULL (memory must be allocated)
 * @post All cells contain 0
 * @post board->clues == 0
 * @post board->empty == board->total_cells
 * 
 * @note Time complexity: O(n²) where n = board_size
 */
void sudoku_board_init(SudokuBoard *board) {
    assert(board != NULL);
    assert(board->cells != NULL);
    
    // Zero out all cells
    // NOTE: We use board->board_size, NOT a hardcoded constant
    for (int i = 0; i < board->board_size; i++) {
        for (int j = 0; j < board->board_size; j++) {
            board->cells[i][j] = 0;
        }
    }
    
    // Update statistics to reflect empty board
    board->clues = 0;
    board->empty = board->total_cells;
}

/**
 * @brief Recalculate board statistics by scanning all cells
 * 
 * This function performs a complete scan of the board to count filled
 * and empty cells, updating the clues and empty fields. Call this after
 * manually modifying cells to ensure statistics remain accurate.
 * 
 * ALGORITHM:
 * 1. Initialize counter to 0
 * 2. Scan every cell in the board
 * 3. Increment counter for each non-zero cell
 * 4. Set clues = counter
 * 5. Set empty = total_cells - counter
 * 
 * @param board Pointer to board to update
 * 
 * @pre board != NULL
 * @post board->clues == actual count of non-zero cells
 * @post board->empty == board->total_cells - board->clues
 * 
 * @note Time complexity: O(n²) where n = board_size
 * @note Safe to call multiple times (idempotent)
 */
void sudoku_board_update_stats(SudokuBoard *board) {
    assert(board != NULL);
    
    int count = 0;
    
    // Count all non-zero cells
    // NOTE: Uses board->board_size, works for any size board
    for (int i = 0; i < board->board_size; i++) {
        for (int j = 0; j < board->board_size; j++) {
            if (board->cells[i][j] != 0) {
                count++;
            }
        }
    }
    
    // Update statistics
    board->clues = count;
    board->empty = board->total_cells - count;
}

// ═══════════════════════════════════════════════════════════════════
//                    CELL ACCESS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Get the value of a specific cell
 * 
 * Simple accessor function with bounds checking via assertions.
 * 
 * @param board Pointer to the board
 * @param row Row index (0 to board_size-1)
 * @param col Column index (0 to board_size-1)
 * @return Cell value (0 = empty, 1 to board_size = filled)
 */
int sudoku_board_get_cell(const SudokuBoard *board, int row, int col) {
    assert(board != NULL);
    assert(row >= 0 && row < board->board_size);
    assert(col >= 0 && col < board->board_size);
    
    return board->cells[row][col];
}

/**
 * @brief Set the value of a specific cell
 * 
 * Updates the cell with bounds checking. Does NOT validate Sudoku rules
 * or automatically update statistics - those are separate operations.
 * 
 * @param board Pointer to board to modify
 * @param row Row index (0 to board_size-1)
 * @param col Column index (0 to board_size-1)
 * @param value New value (0 to board_size)
 * @return true if successful, false if parameters invalid
 */
bool sudoku_board_set_cell(SudokuBoard *board, int row, int col, int value) {
    // Validation checks
    if (board == NULL) return false;
    if (row < 0 || row >= board->board_size) return false;
    if (col < 0 || col >= board->board_size) return false;
    if (value < 0 || value > board->board_size) return false;
    
    // Set the value
    board->cells[row][col] = value;
    return true;
}

// ═══════════════════════════════════════════════════════════════════
//                    STATISTICS ACCESS
// ═══════════════════════════════════════════════════════════════════

int sudoku_board_get_clues(const SudokuBoard *board) {
    assert(board != NULL);
    return board->clues;
}

int sudoku_board_get_empty(const SudokuBoard *board) {
    assert(board != NULL);
    return board->empty;
}

// ═══════════════════════════════════════════════════════════════════
//                    DIMENSION QUERIES
// ═══════════════════════════════════════════════════════════════════

int sudoku_board_get_subgrid_size(const SudokuBoard *board) {
    assert(board != NULL);
    return board->subgrid_size;
}

int sudoku_board_get_board_size(const SudokuBoard *board) {
    assert(board != NULL);
    return board->board_size;
}

int sudoku_board_get_total_cells(const SudokuBoard *board) {
    assert(board != NULL);
    return board->total_cells;
}

// ═══════════════════════════════════════════════════════════════════
//                    SUBGRID GEOMETRY
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Creates a SubGrid structure from an index and subgrid size
 * 
 * @param index Subgrid index (0 to board_size-1)
 * @param subgrid_size Size of subgrid (k in k×k subgrids)
 * @return SudokuSubGrid with calculated base position
 * 
 * PHASE 2C FIX: Now accepts subgrid_size parameter instead of using
 * hardcoded SUBGRID_SIZE constant. This enables correct operation
 * for all board sizes (4×4, 9×9, 16×16, 25×25).
 */
SudokuSubGrid sudoku_subgrid_create(int index, int subgrid_size) {
    SudokuSubGrid sg;
    sg.index = index;
    sg.subgrid_size = subgrid_size;  // Store for use in get_position
    
    // Calculate base position using dynamic subgrid_size
    sg.base.row = (index / subgrid_size) * subgrid_size;
    sg.base.col = (index % subgrid_size) * subgrid_size;
    
    return sg;
}
/**
 * @brief Gets the absolute board position of a cell within a subgrid
 * 
 * @param sg Pointer to the SubGrid structure
 * @param cell_index Index of cell within subgrid (0 to subgrid_size²-1)
 * @return SudokuPosition with absolute row and column on the board
 * 
 * PHASE 2C FIX: Now uses sg->subgrid_size instead of hardcoded constant.
 * This correctly calculates positions for all board sizes.
 * 
 * Example for 16×16 board (subgrid_size=4):
 *   cell_index=15 → row offset = 15/4 = 3, col offset = 15%4 = 3
 *   With base (12,12): position = (12+3, 12+3) = (15,15) ✓
 */
SudokuPosition sudoku_subgrid_get_position(const SudokuSubGrid *sg, int cell_index) {
    SudokuPosition pos;
    
    // Use stored subgrid_size for correct calculation on all board sizes
    pos.row = sg->base.row + (cell_index / sg->subgrid_size);
    pos.col = sg->base.col + (cell_index % sg->subgrid_size);
    
    return pos;
}

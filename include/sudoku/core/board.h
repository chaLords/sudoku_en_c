/**
 * @file board.h
 * @brief Public API for Sudoku board operations
 * @author Gonzalo Ramírez
 * @date 2025-11-18
 * 
 * This header provides the public interface for creating, manipulating,
 * and querying Sudoku boards. Now supports configurable board sizes.
 */

#ifndef SUDOKU_CORE_BOARD_H
#define SUDOKU_CORE_BOARD_H

#include <sudoku/core/types.h>
#include <stdbool.h>
#
// ═══════════════════════════════════════════════════════════════════
//                    MEMORY MANAGEMENT
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Create a board with specific subgrid size
 * 
 * Allocates a new Sudoku board with the specified dimensions.
 * The board size will be subgrid_size², and total cells will be
 * (subgrid_size²)².
 * 
 * Examples:
 * - subgrid_size=2 → 4×4 board (16 cells)
 * - subgrid_size=3 → 9×9 board (81 cells)
 * - subgrid_size=4 → 16×16 board (256 cells)
 * 
 * @param[in] subgrid_size Size of subgrids (valid: 2-5)
 * @return Pointer to newly created board, or NULL on error
 * 
 * @note Caller must free with sudoku_board_destroy()
 * @note Valid sizes: 2, 3, 4, 5 (larger may be computationally expensive)
 * @note Returns NULL if allocation fails or size is invalid
 * 
 * @warning Do NOT use stack allocation for SudokuBoard anymore
 * 
 * Example:
 * @code
 * // Create a 16×16 Sudoku board
 * SudokuBoard *board = sudoku_board_create_size(4);
 * if (board == NULL) {
 *     fprintf(stderr, "Failed to create board\n");
 *     return 1;
 * }
 * // Use board...
 * sudoku_board_destroy(board);
 * @endcode
 * 
 * @see sudoku_board_create() for default 9×9 board
 * @see sudoku_board_destroy() to free memory
 */
SudokuBoard* sudoku_board_create_size(int subgrid_size);

/**
 * @brief Create a board with default size (9×9)
 * 
 * Convenience wrapper for creating a classic 9×9 Sudoku board.
 * Equivalent to calling sudoku_board_create_size(3).
 * 
 * @return Pointer to newly created 9×9 board, or NULL on error
 * 
 * @note This maintains backward compatibility with code expecting 9×9
 * @note Caller must free with sudoku_board_destroy()
 * 
 * Example:
 * @code
 * // Create classic 9×9 board
 * SudokuBoard *board = sudoku_board_create();
 * if (board != NULL) {
 *     // Use board...
 *     sudoku_board_destroy(board);
 * }
 * @endcode
 * 
 * @see sudoku_board_create_size() for non-standard sizes
 */
SudokuBoard* sudoku_board_create(void);

/**
 * @brief Destroy a Sudoku board and free its memory
 * 
 * Releases all memory associated with a dynamically allocated board,
 * including the 2D cells array and the board structure itself.
 * 
 * @param[in] board Pointer to board to destroy (can be NULL)
 * 
 * @post Board pointer is no longer valid after this call
 * 
 * @note Safe to call with NULL (does nothing)
 * @note Do NOT call on stack-allocated boards
 * @note After calling, set pointer to NULL to avoid use-after-free
 * 
 * Example:
 * @code
 * SudokuBoard *board = sudoku_board_create();
 * // ... use board ...
 * sudoku_board_destroy(board);
 * board = NULL;  // Good practice
 * @endcode
 * 
 * @see sudoku_board_create() and sudoku_board_create_size()
 */
void sudoku_board_destroy(SudokuBoard *board);

// ═══════════════════════════════════════════════════════════════════
//                    BOARD INITIALIZATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Initialize a board to empty state
 * 
 * Resets all cells to zero and updates statistics. This is automatically
 * called by sudoku_board_create_size(), but can be called manually to
 * reset an existing board.
 * 
 * @param[out] board Pointer to board to initialize
 * 
 * @pre board != NULL
 * @pre board->cells != NULL (memory must be allocated)
 * @post All cells are 0 (empty)
 * @post board->clues == 0
 * @post board->empty == board->total_cells
 * 
 * Example:
 * @code
 * // Reset an existing board
 * sudoku_board_init(board);
 * // Board is now empty and ready for new generation
 * @endcode
 */
void sudoku_board_init(SudokuBoard *board);

/**
 * @brief Recalculate and update board statistics
 * 
 * Scans the entire board to count filled and empty cells, updating
 * the clues and empty fields. Call this after manually modifying
 * cells to keep statistics accurate.
 * 
 * @param[in,out] board Pointer to board to update
 * 
 * @pre board != NULL
 * @post board->clues == count of non-zero cells
 * @post board->empty == board->total_cells - board->clues
 * 
 * @note Time complexity: O(n²) where n = board_size
 * @note Safe to call multiple times (idempotent)
 * 
 * Example:
 * @code
 * // After manual modifications
 * sudoku_board_set_cell(board, 0, 0, 5);
 * sudoku_board_set_cell(board, 1, 1, 7);
 * sudoku_board_update_stats(board);  // Refresh statistics
 * @endcode
 */
void sudoku_board_update_stats(SudokuBoard *board);

// ═══════════════════════════════════════════════════════════════════
//                    CELL ACCESS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Get the value of a specific cell
 * 
 * @param[in] board Pointer to the board
 * @param[in] row Row index (0 to board_size-1)
 * @param[in] col Column index (0 to board_size-1)
 * @return Cell value (0 = empty, 1 to board_size = filled)
 * 
 * @pre board != NULL
 * @pre 0 <= row < board->board_size
 * @pre 0 <= col < board->board_size
 * 
 * Example:
 * @code
 * int value = sudoku_board_get_cell(board, 4, 5);
 * if (value == 0) {
 *     printf("Cell is empty\n");
 * }
 * @endcode
 */
int sudoku_board_get_cell(const SudokuBoard *board, int row, int col);

/**
 * @brief Set the value of a specific cell
 * 
 * Updates the cell at the specified position. Does NOT validate
 * according to Sudoku rules - use sudoku_is_safe_position() first
 * if validation is needed.
 * 
 * @param[in,out] board Pointer to board to modify
 * @param[in] row Row index (0 to board_size-1)
 * @param[in] col Column index (0 to board_size-1)
 * @param[in] value New value (0 to board_size)
 * @return true if successful, false if parameters invalid
 * 
 * @pre board != NULL (checked at runtime)
 * @post If returns true, board->cells[row][col] == value
 * @post If returns false, board is unchanged
 * 
 * @note Does NOT automatically update statistics - call
 *       sudoku_board_update_stats() if needed
 * @note Does NOT validate Sudoku rules - just sets the value
 * 
 * Example:
 * @code
 * if (sudoku_board_set_cell(board, 3, 4, 7)) {
 *     printf("Cell set successfully\n");
 * }
 * @endcode
 */
bool sudoku_board_set_cell(SudokuBoard *board, int row, int col, int value);

// ═══════════════════════════════════════════════════════════════════
//                    STATISTICS ACCESS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Get the number of filled cells
 * 
 * @param[in] board Pointer to board
 * @return Number of non-zero cells (0 to total_cells)
 * 
 * @pre board != NULL
 * 
 * @note Returns cached value - call sudoku_board_update_stats()
 *       first if cells were modified manually
 */
int sudoku_board_get_clues(const SudokuBoard *board);

/**
 * @brief Get the number of empty cells
 * 
 * @param[in] board Pointer to board
 * @return Number of zero-valued cells (0 to total_cells)
 * 
 * @pre board != NULL
 * @note Invariant: get_clues() + get_empty() == total_cells
 */
int sudoku_board_get_empty(const SudokuBoard *board);

// ═══════════════════════════════════════════════════════════════════
//                    DIMENSION QUERIES (NEW)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Get the subgrid size
 * 
 * @param[in] board Pointer to board
 * @return Subgrid size (e.g., 3 for classic Sudoku)
 * 
 * @pre board != NULL
 * 
 * Example:
 * @code
 * int subgrid = sudoku_board_get_subgrid_size(board);
 * printf("Subgrids are %d×%d\n", subgrid, subgrid);
 * @endcode
 */
int sudoku_board_get_subgrid_size(const SudokuBoard *board);

/**
 * @brief Get the board size
 * 
 * @param[in] board Pointer to board
 * @return Board size (subgrid_size², e.g., 9 for classic Sudoku)
 * 
 * @pre board != NULL
 * 
 * Example:
 * @code
 * int size = sudoku_board_get_board_size(board);
 * printf("Board is %d×%d\n", size, size);
 * @endcode
 */
int sudoku_board_get_board_size(const SudokuBoard *board);

/**
 * @brief Get total number of cells
 * 
 * @param[in] board Pointer to board
 * @return Total cells (board_size², e.g., 81 for classic Sudoku)
 * 
 * @pre board != NULL
 * 
 * Example:
 * @code
 * int total = sudoku_board_get_total_cells(board);
 * printf("Board has %d cells\n", total);
 * @endcode
 */
int sudoku_board_get_total_cells(const SudokuBoard *board);

// ═══════════════════════════════════════════════════════════════════
//                    SUBGRID GEOMETRY
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Create a subgrid structure from an index
 * 
 * @param[in] index Subgrid index (0 to board_size-1)
 * @return Initialized SudokuSubGrid with index and base position
 * 
 * @pre 0 <= index < board_size
 * @post return.index == index
 * @post return.base contains valid top-left corner
 * 
 * @note For 9×9 board: index 0 = top-left, index 8 = bottom-right
 */
SudokuSubGrid sudoku_subgrid_create(int index, int subgrid_size);
/**
 * @brief Get absolute board position from subgrid-relative cell index
 * 
 * @param[in] sg Pointer to subgrid structure
 * @param[in] cell_index Cell index within subgrid (0 to subgrid_size²-1)
 * @return Absolute position on the board
 * 
 * @pre sg != NULL
 * @pre 0 <= cell_index < sg->subgrid_size²
 */
SudokuPosition sudoku_subgrid_get_position(const SudokuSubGrid *sg, int cell_index);

#endif // SUDOKU_CORE_BOARD_H

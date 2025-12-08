/**
 * @file board_internal.h
 * @brief Internal board operations not exposed in the public API
 * @author Gonzalo Ramírez
 * @date 2025-01-19
 * 
 * This header contains private functions for board manipulation that are
 * used internally by the generator and solver algorithms. These functions
 * are not part of the public API and should not be used by external code.
 * 
 * PHASE 2A REFACTORING:
 * - Transformed SudokuBoard from static arrays to dynamic memory allocation
 * - Added dimension tracking fields (subgrid_size, board_size, total_cells)
 * - This enables true multi-size board support (4×4, 9×9, 16×16, 25×25)
 */

#ifndef SUDOKU_BOARD_INTERNAL_H
#define SUDOKU_BOARD_INTERNAL_H

#include "sudoku/core/types.h"
#include <stdbool.h>

/**
 * @brief Finds the first empty cell in the board using row-major traversal
 * 
 * This function performs a linear search through the board from left to right,
 * top to bottom, looking for the first cell that contains a zero (empty cell).
 * This traversal strategy is optimal for backtracking algorithms because it
 * maintains a consistent and predictable order of cell exploration.
 * 
 * The function is used internally by the backtracking algorithm during board
 * completion. It represents a design decision: we could search for empty cells
 * in different orders (column-major, spiral, most-constrained-first), but
 * row-major order provides a good balance between simplicity and performance.
 *
 * @param board Pointer to the Sudoku board to search (not modified)
 * @param pos Pointer to a Position structure where the coordinates will be stored
 * @return true if an empty cell was found (pos contains its coordinates)
 * @return false if the board is complete (no empty cells exist)
 * 
 * @pre board != NULL && pos != NULL
 * @post If true is returned, pos->row and pos->col contain valid coordinates
 * @post The board structure is never modified by this function
 * 
 * @note This is an internal function - not part of the public API
 * @note Time complexity: O(n²) where n = board_size, but typically terminates early
 * @note Space complexity: O(1) - no additional memory allocation
 */
bool sudoku_find_empty_cell(const SudokuBoard *board, SudokuPosition *pos);

// Funciones de llenado de tablero (internas)
void fillSubGrid(SudokuBoard *board, const SudokuSubGrid *sg);
void fillDiagonal(SudokuBoard *board);

// Actualización de contadores (wrapper)
void sudoku_board_update_stats(SudokuBoard *board);

/**
 * @brief Complete internal definition of SudokuBoard
 * 
 * PHASE 2A: DYNAMIC MEMORY ARCHITECTURE
 * =======================================
 * 
 * This structure now supports configurable board sizes through dynamic
 * memory allocation. The key changes from v2.2.x:
 * 
 * BEFORE (v2.2.x - Static Array):
 *   int cells[SUDOKU_SIZE][SUDOKU_SIZE];  // Fixed 9×9, 324 bytes
 * 
 * AFTER (v3.0 - Dynamic Array):
 *   int **cells;                            // Pointer to pointer
 *   int subgrid_size;                       // Size of subgrids (2-5)
 *   int board_size;                         // Full board size (4-25)
 *   int total_cells;                        // Total cells in board
 * 
 * MEMORY LAYOUT EXAMPLE (4×4 board):
 * ===================================
 * 
 * board->cells → [ptr0] → [1][2][3][4]  ← Row 0
 *                [ptr1] → [5][6][7][8]  ← Row 1
 *                [ptr2] → [9][0][0][0]  ← Row 2
 *                [ptr3] → [0][0][0][0]  ← Row 3
 * 
 * ALLOCATION STRATEGY:
 * ====================
 * 1. Allocate array of row pointers: malloc(board_size * sizeof(int*))
 * 2. For each row, allocate column array: malloc(board_size * sizeof(int))
 * 
 * DEALLOCATION STRATEGY:
 * ======================
 * 1. Free each row individually: free(cells[i]) for i in [0, board_size)
 * 2. Free the row pointer array: free(cells)
 * 
 * CLIENT CODE COMPATIBILITY:
 * ==========================
 * The syntax board->cells[row][col] remains IDENTICAL to static arrays.
 * This is the beauty of C's pointer arithmetic - the [][] operator works
 * seamlessly with both static and dynamic 2D arrays.
 * 
 * WHY DYNAMIC ALLOCATION?
 * =======================
 * 1. Flexibility: Support any valid Sudoku size at runtime
 * 2. Memory efficiency: Only allocate what's needed
 * 3. Scalability: Can handle very large boards (25×25 = 625 cells)
 * 4. Professional practice: Standard approach for variable-size data
 * 
 * EDUCATIONAL NOTE:
 * =================
 * This transformation from static to dynamic arrays is a fundamental
 * skill in systems programming. The key insight is that int** creates
 * a "pointer to pointer" structure that mimics 2D array syntax while
 * allowing runtime size determination.
 */
struct SudokuBoard {
    // ═══════════════════════════════════════════════════════════
    //  DYNAMIC MEMORY: The 2D Cell Grid
    // ═══════════════════════════════════════════════════════════
    
    /**
     * @brief Dynamic 2D array of cell values
     * 
     * This is allocated as an array of pointers, where each pointer
     * points to a row of integers. Access syntax remains identical
     * to static arrays: cells[row][col]
     * 
     * Memory ownership: This structure owns this memory and is
     * responsible for freeing it in sudoku_board_destroy()
     * 
     * Valid values: 0 (empty) or 1 to board_size (filled)
     */
    int **cells;
    
    // ═══════════════════════════════════════════════════════════
    //  DIMENSION TRACKING: Board Geometry
    // ═══════════════════════════════════════════════════════════
    
    /**
     * @brief Size of each subgrid (2, 3, 4, or 5)
     * 
     * Examples:
     * - subgrid_size = 2 → 2×2 subgrids in 4×4 board
     * - subgrid_size = 3 → 3×3 subgrids in 9×9 board (classic)
     * - subgrid_size = 4 → 4×4 subgrids in 16×16 board
     * - subgrid_size = 5 → 5×5 subgrids in 25×25 board
     */
    int subgrid_size;
    
    /**
     * @brief Full size of the board (subgrid_size²)
     * 
     * This is the number of rows and columns in the board.
     * 
     * Formula: board_size = subgrid_size × subgrid_size
     * 
     * Examples:
     * - subgrid_size = 2 → board_size = 4
     * - subgrid_size = 3 → board_size = 9
     * - subgrid_size = 4 → board_size = 16
     */
    int board_size;
    
    /**
     * @brief Total number of cells in the board (board_size²)
     * 
     * This is a cached value for performance - we could always
     * compute it as board_size * board_size, but storing it
     * avoids repeated multiplication in hot paths.
     * 
     * Formula: total_cells = board_size × board_size
     * 
     * Examples:
     * - board_size = 4 → total_cells = 16
     * - board_size = 9 → total_cells = 81
     * - board_size = 16 → total_cells = 256
     * 
     * Invariant: clues + empty = total_cells (always)
     */
    int total_cells;
    
    // ═══════════════════════════════════════════════════════════
    //  STATISTICS: Cell Counts (Cached for Performance)
    // ═══════════════════════════════════════════════════════════
    
    /**
     * @brief Number of filled cells (non-zero values)
     * 
     * This is updated by sudoku_board_update_stats() and represents
     * the count of cells containing values from 1 to board_size.
     * 
     * Valid range: [0, total_cells]
     */
    int clues;
    
    /**
     * @brief Number of empty cells (zero values)
     * 
     * This is the complement of clues. It represents the count of
     * cells that still need to be filled.
     * 
     * Valid range: [0, total_cells]
     * Invariant: empty = total_cells - clues
     */
    int empty;
  /**
     * @brief Registry of forced cells (filled during AC3HB generation)
     * 
     * Este registry se crea durante sudoku_complete_ac3hb() y se usa
     * en phase3EliminationSmart() para protección inteligente.
     * 
     * LIFECYCLE:
     * - Creado: Al inicio de sudoku_complete_ac3hb()
     * - Llenado: Durante propagación AC3 y backtracking
     * - Usado: En phase3EliminationSmart()
     * - Destruido: Al final de sudoku_generate() o en error paths
     * 
     * Puede ser NULL si:
     * - Board no fue generado con AC3HB
     * - Generación falló
     * - Board fue creado manualmente
     */
    ForcedCellsRegistry *forced_cells;
};

#endif // SUDOKU_BOARD_INTERNAL_H

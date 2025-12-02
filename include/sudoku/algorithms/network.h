/**
 * @file network.h
 * @brief Constraint network for Sudoku CSP representation
 * @author Gonzalo Ramírez
 * @date 2025-11
 * 
 * This module provides a constraint network data structure that represents
 * a Sudoku puzzle as a Constraint Satisfaction Problem (CSP). The network
 * maintains domain information for each cell and the neighbor relationships
 * that define the Sudoku constraints.
 * 
 * DESIGN PRINCIPLES:
 * - Opaque structure (implementation hidden)
 * - Efficient bit-vector domain representation
 * - Dynamic memory for variable board sizes
 * - Clean separation from board representation
 */

#ifndef SUDOKU_CONSTRAINT_NETWORK_H
#define SUDOKU_CONSTRAINT_NETWORK_H

#include "sudoku/core/types.h"
#include <stdbool.h>
#include <stdint.h>

// ═══════════════════════════════════════════════════════════════════
//                    OPAQUE TYPE DECLARATIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Opaque pointer to constraint network structure
 * 
 * This type hides the implementation details of the network from client
 * code, allowing internal changes without breaking API compatibility.
 */
typedef struct ConstraintNetwork ConstraintNetwork;

// ═══════════════════════════════════════════════════════════════════
//                    DOMAIN REPRESENTATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Efficient domain representation using bit vectors
 * 
 * Each bit position represents whether a value is possible:
 * - Bit 0 = value 1 possible
 * - Bit 1 = value 2 possible
 * - ...
 * - Bit 8 = value 9 possible (for 9×9)
 * 
 * This representation allows:
 * - O(1) membership testing (single bit check)
 * - O(1) value removal (single bit clear)
 * - O(1) domain size query (popcount instruction)
 * - Compact memory (32 bits for domains up to size 32)
 * 
 * Example for 9×9 Sudoku:
 *   Domain {1,3,5,7,9} → bits = 0b101010101 = 0x155
 */
typedef struct {
    uint32_t bits;  ///< Bit vector of possible values
    int count;      ///< Cached count of possible values
} Domain;

// ═══════════════════════════════════════════════════════════════════
//                    NETWORK LIFECYCLE
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Create a constraint network from a Sudoku board
 * 
 * Analyzes the current board state and constructs a constraint network
 * where each empty cell gets a domain of possible values based on:
 * - Current filled cells in same row
 * - Current filled cells in same column  
 * - Current filled cells in same subgrid
 * 
 * ALLOCATION PERFORMED:
 * - Main ConstraintNetwork structure
 * - board_size² Domain structures
 * - board_size² Neighbor lists (~20 neighbors per cell)
 * 
 * Total memory: ~(board_size² × 100 bytes) for 9×9: ~8KB
 * 
 * @param board Sudoku board to analyze
 * @return Pointer to new network, or NULL on error
 * 
 * @post Each empty cell has domain based on current constraints
 * @post Each filled cell has singleton domain (one value)
 * 
 * @note Caller must call constraint_network_destroy() when done
 * 
 * Example usage:
 * @code
 * SudokuBoard *board = sudoku_board_create();
 * // ... fill some cells ...
 * 
 * ConstraintNetwork *net = constraint_network_create(board);
 * if (net == NULL) {
 *     // Handle error
 * }
 * 
 * // ... use network ...
 * 
 * constraint_network_destroy(net);
 * @endcode
 */
ConstraintNetwork* constraint_network_create(const SudokuBoard *board);

/**
 * @brief Destroy constraint network and free all memory
 * 
 * Frees all memory allocated for the network including:
 * - All domain structures
 * - All neighbor lists  
 * - The network structure itself
 * 
 * @param net Network to destroy (can be NULL)
 * 
 * @post All memory associated with network is freed
 * 
 * @note Safe to call with NULL pointer (does nothing)
 * @note After calling, pointer is dangling - caller should set to NULL
 */
void constraint_network_destroy(ConstraintNetwork *net);

// ═══════════════════════════════════════════════════════════════════
//                    DOMAIN QUERIES
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Get the domain for a specific cell
 * 
 * Returns a copy of the domain structure for the specified cell.
 * The domain indicates which values are still possible for that cell.
 * 
 * @param net Constraint network
 * @param row Row index
 * @param col Column index  
 * @return Domain structure (by value)
 * 
 * @pre net != NULL
 * @pre 0 <= row < board_size
 * @pre 0 <= col < board_size
 * 
 * @note Returns copy, not pointer (thread-safe for reads)
 * @note For filled cells, returns singleton domain
 */
Domain constraint_network_get_domain(const ConstraintNetwork *net, 
                                     int row, int col);

/**
 * @brief Check if a value is in a cell's domain
 * 
 * Fast O(1) check using bit operations.
 * 
 * @param net Constraint network
 * @param row Row index
 * @param col Column index
 * @param value Value to check (1 to board_size)
 * @return true if value is possible, false otherwise
 * 
 * Example:
 * @code
 * if (constraint_network_has_value(net, 3, 5, 7)) {
 *     printf("Value 7 is still possible at (3,5)\n");
 * }
 * @endcode
 */
bool constraint_network_has_value(const ConstraintNetwork *net,
                                  int row, int col, int value);

/**
 * @brief Get the number of possible values for a cell
 * 
 * Returns the cardinality of the domain (how many values remain).
 * Useful for MRV heuristic.
 * 
 * @param net Constraint network
 * @param row Row index
 * @param col Column index
 * @return Number of possible values (0 if domain empty, 1 if filled)
 * 
 * @note O(1) operation - result is cached in Domain structure
 */
int constraint_network_domain_size(const ConstraintNetwork *net,
                                   int row, int col);

/**
 * @brief Check if a cell's domain is empty
 * 
 * Empty domain means the cell cannot be filled - the puzzle is
 * unsolvable in the current state. This indicates a dead end.
 * 
 * @param net Constraint network
 * @param row Row index  
 * @param col Column index
 * @return true if domain is empty (count == 0)
 * 
 * @note Critical for detecting constraint violations early
 */
bool constraint_network_domain_empty(const ConstraintNetwork *net,
                                     int row, int col);

// ═══════════════════════════════════════════════════════════════════
//                    DOMAIN MODIFICATIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Remove a value from a cell's domain
 * 
 * Eliminates the specified value as a possibility for the cell.
 * This is the core operation for constraint propagation.
 * 
 * @param net Constraint network
 * @param row Row index
 * @param col Column index
 * @param value Value to remove (1 to board_size)
 * @return true if value was removed, false if already absent
 * 
 * @pre net != NULL
 * @post If true returned: domain no longer contains value
 * @post Domain count decremented if value was present
 * 
 * Example (AC-3 usage):
 * @code
 * // If cell (3,5) is assigned value 7
 * // Remove 7 from all neighbors
 * for each neighbor (r,c):
 *     constraint_network_remove_value(net, r, c, 7);
 * @endcode
 */
bool constraint_network_remove_value(ConstraintNetwork *net,
                                     int row, int col, int value);

/**
 * @brief Assign a value to a cell (set singleton domain)
 * 
 * Sets the cell's domain to contain only the specified value.
 * All other values are removed. This represents making a decision
 * about what number to place in the cell.
 * 
 * @param net Constraint network  
 * @param row Row index
 * @param col Column index
 * @param value Value to assign (1 to board_size)
 * 
 * @pre net != NULL
 * @pre value must be in current domain
 * @post Cell's domain = {value} (singleton)
 * @post Domain count = 1
 * 
 * Example:
 * @code
 * // Decide to place 7 at position (3,5)
 * constraint_network_assign_value(net, 3, 5, 7);
 * @endcode
 */
void constraint_network_assign_value(ConstraintNetwork *net,
                                     int row, int col, int value);

/**
 * @brief Restore a cell's domain to full set of possibilities
 * 
 * Resets domain to all values from 1 to board_size. Used when
 * backtracking to undo a previous assignment.
 * 
 * @param net Constraint network
 * @param row Row index  
 * @param col Column index
 * 
 * @post Domain contains all values 1..board_size
 * @post Domain count = board_size
 * 
 * @note Typically followed by re-running AC-3 to reapply constraints
 */
void constraint_network_restore_domain(ConstraintNetwork *net,
                                       int row, int col);

// ═══════════════════════════════════════════════════════════════════
//                    NETWORK QUERIES
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Get the board size (for iteration)
 * 
 * @param net Constraint network
 * @return Board dimension (e.g., 9 for 9×9)
 */
int constraint_network_get_board_size(const ConstraintNetwork *net);

/**
 * @brief Get list of neighbors for a cell
 * 
 * Returns array of positions representing all cells constrained with
 * the specified cell (same row, column, or subgrid).
 * 
 * @param net Constraint network
 * @param row Row index
 * @param col Column index
 * @param count Output parameter for number of neighbors
 * @return Pointer to array of neighbor positions (read-only)
 * 
 * @pre net != NULL
 * @post *count contains number of neighbors (~20 for 9×9)
 * @post Returned pointer valid until network destroyed
 * 
 * @note Do not modify or free returned array
 * @note For 9×9: typically 20 neighbors (8 row + 8 col + 4 subgrid)
 * 
 * Example:
 * @code
 * int count;
 * const SudokuPosition *neighbors = 
 *     constraint_network_get_neighbors(net, 4, 4, &count);
 *     
 * for (int i = 0; i < count; i++) {
 *     printf("Neighbor: (%d,%d)\n", neighbors[i].row, neighbors[i].col);
 * }
 * @endcode
 */
const SudokuPosition* constraint_network_get_neighbors(
    const ConstraintNetwork *net,
    int row, int col,
    int *count
);

// ═══════════════════════════════════════════════════════════════════
//                    DEBUGGING AND VISUALIZATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Print domain information for debugging
 * 
 * Displays the current domain for all cells in human-readable format.
 * Useful for understanding constraint propagation behavior.
 * 
 * Output format example (9×9):
 * @code
 * Constraint Network State:
 * -------------------------
 * (0,0): {1,2,3,4,5,6,7,8,9} [9]
 * (0,1): {7}                 [1] (assigned)
 * (0,2): {1,2,3,4,5,6,8,9}   [8]
 * ...
 * @endcode
 * 
 * @param net Constraint network to print
 */
void constraint_network_print(const ConstraintNetwork *net);

/**
 * @brief Count total number of possibilities across all cells
 * 
 * Sums up the domain sizes for all empty cells. Useful metric for
 * measuring progress of constraint propagation.
 * 
 * @param net Constraint network
 * @return Total possibilities (lower = more constrained)
 * 
 * Example:
 * @code
 * int before = constraint_network_total_possibilities(net);
 * ac3_propagate(net);  // Run AC-3
 * int after = constraint_network_total_possibilities(net);
 * 
 * printf("AC-3 eliminated %d possibilities\n", before - after);
 * @endcode
 */
int constraint_network_total_possibilities(const ConstraintNetwork *net);

#endif // SUDOKU_CONSTRAINT_NETWORK_H

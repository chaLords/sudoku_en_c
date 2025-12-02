/**
 * @file ac3.h
 * @brief AC-3 (Arc Consistency #3) algorithm for constraint propagation
 * @author Gonzalo Ramírez
 * @date 2025-11
 * 
 * This module implements the AC-3 algorithm, which enforces arc consistency
 * across all constraints in a Sudoku puzzle. Arc consistency means that for
 * every value in a cell's domain, there exists at least one compatible value
 * in each neighboring cell's domain.
 * 
 * ALGORITHM REFERENCE:
 * Mackworth, A. K. (1977). "Consistency in Networks of Relations"
 * Artificial Intelligence, 8(1), 99-118.
 * 
 * KEY CONCEPTS:
 * - Arc: Directed constraint between two variables
 * - Revision: Process of making an arc consistent
 * - Propagation: Cascading revisions until fixed point reached
 */

#ifndef SUDOKU_AC3_H
#define SUDOKU_AC3_H

#include "network.h"
#include <stdbool.h>

// ═══════════════════════════════════════════════════════════════════
//                    STATISTICS TRACKING
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Statistics collected during AC-3 execution
 * 
 * These metrics help analyze algorithm performance and understand
 * how much work was required to achieve arc consistency.
 */
typedef struct {
    int revisions;           ///< Number of arc revisions performed
    int values_removed;      ///< Total values eliminated from domains
    int propagations;        ///< Number of constraint propagations
    double time_ms;          ///< Execution time in milliseconds
    bool consistent;         ///< true if result is consistent, false if dead end
} AC3Statistics;

// ═══════════════════════════════════════════════════════════════════
//                    MAIN AC-3 FUNCTION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Run AC-3 algorithm to enforce arc consistency
 * 
 * Performs constraint propagation until no more values can be eliminated.
 * The algorithm maintains a queue of arcs to check and processes them
 * until the queue is empty (fixed point) or an inconsistency is detected.
 * 
 * ALGORITHM STEPS:
 * 1. Initialize queue with all arcs (cell, neighbor) pairs
 * 2. While queue not empty:
 *    a. Dequeue arc (Xi, Xj)
 *    b. If Revise(Xi, Xj) modified Xi's domain:
 *       - Enqueue all arcs (Xk, Xi) where Xk is neighbor of Xi
 *       - If Xi's domain became empty: return FAILURE
 * 3. Return SUCCESS
 * 
 * COMPLEXITY:
 * - Time: O(e × d³) where e = edges, d = max domain size
 * - Space: O(e) for the queue
 * - For 9×9: ~100,000 operations typical
 * 
 * @param net Constraint network to process (modified in-place)
 * @param stats Output parameter for statistics (can be NULL)
 * @return true if consistent, false if unsolvable state detected
 * 
 * @pre net != NULL
 * @post If true: network is arc-consistent
 * @post If false: at least one domain is empty (dead end)
 * @post Network domains may be reduced (never expanded)
 * 
 * Example usage:
 * @code
 * ConstraintNetwork *net = constraint_network_create(board);
 * AC3Statistics stats;
 * 
 * if (ac3_enforce_consistency(net, &stats)) {
 *     printf("AC-3 succeeded:\n");
 *     printf("  Revisions: %d\n", stats.revisions);
 *     printf("  Values removed: %d\n", stats.values_removed);
 *     printf("  Time: %.2f ms\n", stats.time_ms);
 * } else {
 *     printf("Dead end detected - puzzle unsolvable from this state\n");
 * }
 * @endcode
 */
bool ac3_enforce_consistency(ConstraintNetwork *net, AC3Statistics *stats);

// ═══════════════════════════════════════════════════════════════════
//                    SPECIALIZED AC-3 VARIANTS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Run AC-3 starting from a specific cell assignment
 * 
 * More efficient than full AC-3 when you've just assigned a value to
 * one cell. Only propagates constraints from that cell's neighbors.
 * 
 * OPTIMIZATION:
 * Instead of initializing queue with ALL arcs, only add arcs affected
 * by the new assignment. This reduces work significantly.
 * 
 * @param net Constraint network
 * @param row Row of newly assigned cell
 * @param col Column of newly assigned cell
 * @param value Value that was assigned
 * @param stats Output parameter for statistics
 * @return true if consistent, false if conflict detected
 * 
 * @pre Cell at (row,col) should have been just assigned value
 * 
 * Example:
 * @code
 * // Assign value 7 to cell (3,5)
 * constraint_network_assign_value(net, 3, 5, 7);
 * 
 * // Propagate from this assignment only
 * if (!ac3_propagate_from(net, 3, 5, 7, &stats)) {
 *     // Assignment leads to dead end, must backtrack
 *     constraint_network_restore_domain(net, 3, 5);
 * }
 * @endcode
 */
bool ac3_propagate_from(ConstraintNetwork *net,
                        int row, int col, int value,
                        AC3Statistics *stats);

/**
 * @brief Run AC-3 with early termination
 * 
 * Stops as soon as any domain is reduced to size 1 (cell becomes decided).
 * Useful when you only want to find obvious assignments without doing
 * complete propagation.
 * 
 * @param net Constraint network
 * @param stats Output parameter
 * @return true if found at least one singleton domain
 * 
 * @note May not achieve full arc consistency
 */
bool ac3_find_singles(ConstraintNetwork *net, AC3Statistics *stats);

// ═══════════════════════════════════════════════════════════════════
//                    LOW-LEVEL OPERATIONS (For Advanced Use)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Check and revise a single arc
 * 
 * Low-level function that implements the Revise operation from AC-3.
 * For each value v in Xi's domain, checks if there exists a compatible
 * value in Xj's domain. Removes v if no compatible value found.
 * 
 * @param net Constraint network
 * @param xi_row Row of source cell Xi
 * @param xi_col Column of source cell Xi
 * @param xj_row Row of target cell Xj
 * @param xj_col Column of target cell Xj
 * @return true if Xi's domain was modified
 * 
 * @note Most users should use ac3_enforce_consistency() instead
 * @note This is exposed for testing and specialized algorithms
 */
bool ac3_revise_arc(ConstraintNetwork *net,
                    int xi_row, int xi_col,
                    int xj_row, int xj_col);

#endif // SUDOKU_AC3_H

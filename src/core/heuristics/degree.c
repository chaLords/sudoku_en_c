/**
 * @file heuristics_complete.c
 * @brief Complete heuristics implementation for variable/value selection
 * @author Gonzalo Ramírez
 * @date 2025-11
 * 
 * UBICACIÓN: Este archivo contiene TODAS las heuristics.
 *            Copiar funciones a archivos separados según estructura:
 *            - src/core/heuristics/mrv.c
 *            - src/core/heuristics/degree.c
 *            - src/core/heuristics/density.c
 *            - src/core/heuristics/combined.c
 *            - src/core/heuristics/selector.c (main entry point)
 * 
 * HEURISTICS IMPLEMENTADAS:
 * 1. MRV (Minimum Remaining Values) - Fail-first principle
 * 2. Degree Heuristic - Tie-breaking for MRV
 * 3. Density Heuristic - Considers local constraint density
 * 4. Combined Heuristic - Weighted score combining multiple factors
 * 5. LCV (Least Constraining Value) - Value ordering
 */

#include "sudoku/algorithms/heuristics.h"
#include "sudoku/algorithms/network.h"
#include "sudoku/core/board.h"
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

// ═══════════════════════════════════════════════════════════════════
//                    HEURISTIC 2: Degree Heuristic
// ═══════════════════════════════════════════════════════════════════
// ARCHIVO: src/core/heuristics/degree.c

/**
 * @brief Count unassigned neighbors of a cell
 * 
 * Helper function for degree heuristic.
 * 
 * @param net Constraint network
 * @param row Cell row
 * @param col Cell column
 * @return Number of unassigned neighbors
 */
static int count_unassigned_neighbors(const ConstraintNetwork *net, 
                                     int row, int col) {
    int neighbor_count;
    const SudokuPosition *neighbors = 
        constraint_network_get_neighbors(net, row, col, &neighbor_count);
    
    int unassigned = 0;
    for (int i = 0; i < neighbor_count; i++) {
        int domain_size = constraint_network_domain_size(net,
                                                        neighbors[i].row,
                                                        neighbors[i].col);
        if (domain_size > 1) {  // Not assigned
            unassigned++;
        }
    }
    
    return unassigned;
}

/**
 * @brief Select cell using degree heuristic (tie-breaker for MRV)
 * 
 * When multiple cells have same minimum domain size:
 * - Choose cell with most unassigned neighbors
 * - Rationale: Constrains more other variables
 * - Reduces future search space more effectively
 * 
 * @param net Constraint network
 * @param min_domain Only consider cells with this domain size
 * @return Position of cell with highest degree
 * 
 * @complexity O(n² · k) where k=neighbors per cell
 * @note Usually called after MRV with min_domain from MRV result
 */
SudokuPosition heuristic_degree(const ConstraintNetwork *net, int min_domain) {
    assert(net != NULL);
    
    int board_size = constraint_network_get_board_size(net);
    int max_degree = -1;
    SudokuPosition best_pos = {-1, -1};
    
    for (int r = 0; r < board_size; r++) {
        for (int c = 0; c < board_size; c++) {
            int domain_size = constraint_network_domain_size(net, r, c);
            
            // Only consider cells with specified domain size
            if (domain_size != min_domain) {
                continue;
            }
            
            int degree = count_unassigned_neighbors(net, r, c);
            
            if (degree > max_degree) {
                max_degree = degree;
                best_pos.row = r;
                best_pos.col = c;
            }
        }
    }
    
    return best_pos;
}


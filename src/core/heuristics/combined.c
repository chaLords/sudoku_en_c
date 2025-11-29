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
//                    HEURISTIC 4: Combined Heuristic
// ═══════════════════════════════════════════════════════════════════
// ARCHIVO: src/core/heuristics/combined.c

/**
 * @brief Combined heuristic using weighted score
 * 
 * Combines multiple factors into single score:
 * - Domain size (primary)
 * - Degree (secondary)
 * - Density (tertiary)
 * 
 * Score = w1 * (1/domain_size) + w2 * degree + w3 * density
 * 
 * Weights tuned empirically for Sudoku.
 * 
 * @param net Constraint network
 * @return Position with highest combined score
 * 
 * @complexity O(n² · k)
 * @performance Slightly slower than pure MRV but often finds better variables
 */
SudokuPosition heuristic_combined(const ConstraintNetwork *net) {
    assert(net != NULL);
    
    // Tuned weights (adjust based on empirical performance)
    const double W_DOMAIN = 5.0;   // Domain size most important
    const double W_DEGREE = 1.0;   // Degree less important
    const double W_DENSITY = 0.5;  // Density least important
    
    int board_size = constraint_network_get_board_size(net);
    double best_score = -1e9;
    SudokuPosition best_pos = {-1, -1};
    
    for (int r = 0; r < board_size; r++) {
        for (int c = 0; c < board_size; c++) {
            int domain_size = constraint_network_domain_size(net, r, c);
            
            if (domain_size <= 1) {
                continue;
            }
            
            // Compute factors
            double domain_factor = W_DOMAIN / domain_size;
            double degree_factor = W_DEGREE * count_unassigned_neighbors(net, r, c);
            double density_factor = W_DENSITY * compute_density(net, r, c);
            
            double score = domain_factor + degree_factor + density_factor;
            
            if (score > best_score) {
                best_score = score;
                best_pos.row = r;
                best_pos.col = c;
            }
        }
    }
    
    return best_pos;
}


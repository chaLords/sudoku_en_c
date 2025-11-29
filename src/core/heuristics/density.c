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
//                    HEURISTIC 3: Density Heuristic
// ═══════════════════════════════════════════════════════════════════
// ARCHIVO: src/core/heuristics/density.c

/**
 * @brief Compute constraint density around a cell
 * 
 * Density = (number of assigned neighbors) / (total neighbors)
 * 
 * Higher density = more constrained region = better to fill first
 * 
 * @param net Constraint network
 * @param row Cell row
 * @param col Cell column
 * @return Density score [0.0 - 1.0]
 */
static double compute_density(const ConstraintNetwork *net, int row, int col) {
    int neighbor_count;
    const SudokuPosition *neighbors = 
        constraint_network_get_neighbors(net, row, col, &neighbor_count);
    
    if (neighbor_count == 0) {
        return 0.0;
    }
    
    int assigned = 0;
    for (int i = 0; i < neighbor_count; i++) {
        int domain_size = constraint_network_domain_size(net,
                                                        neighbors[i].row,
                                                        neighbors[i].col);
        if (domain_size == 1) {  // Assigned
            assigned++;
        }
    }
    
    return (double)assigned / neighbor_count;
}

/**
 * @brief Select cell in most constrained region (density heuristic)
 * 
 * Strategy:
 * - Prefer cells in regions with many assigned neighbors
 * - These regions are more constrained
 * - Easier to detect contradictions early
 * 
 * Combined with small domains for best results.
 * 
 * @param net Constraint network
 * @return Position of cell in densest region with small domain
 * 
 * @complexity O(n² · k)
 */
SudokuPosition heuristic_density(const ConstraintNetwork *net) {
    assert(net != NULL);
    
    int board_size = constraint_network_get_board_size(net);
    double best_score = -1.0;
    SudokuPosition best_pos = {-1, -1};
    
    for (int r = 0; r < board_size; r++) {
        for (int c = 0; c < board_size; c++) {
            int domain_size = constraint_network_domain_size(net, r, c);
            
            if (domain_size <= 1) {
                continue;  // Skip assigned cells
            }
            
            // Score = density / domain_size (prefer small domains in dense regions)
            double density = compute_density(net, r, c);
            double score = density / domain_size;
            
            if (score > best_score) {
                best_score = score;
                best_pos.row = r;
                best_pos.col = c;
            }
        }
    }
    
    return best_pos;
}


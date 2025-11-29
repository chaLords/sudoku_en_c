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
//                    HEURISTIC 5: LCV (Least Constraining Value)
// ═══════════════════════════════════════════════════════════════════
// ARCHIVO: src/core/heuristics/selector.c (incluir con las demás)

/**
 * @brief Count how many neighbor domains a value appears in
 * 
 * Helper for LCV heuristic.
 * 
 * @param net Constraint network
 * @param row Cell row
 * @param col Cell column
 * @param value Value to check
 * @return Number of neighbors that still have this value in domain
 */
static int count_value_in_neighbors(const ConstraintNetwork *net,
                                   int row, int col, int value) {
    int neighbor_count;
    const SudokuPosition *neighbors = 
        constraint_network_get_neighbors(net, row, col, &neighbor_count);
    
    int count = 0;
    for (int i = 0; i < neighbor_count; i++) {
        if (constraint_network_has_value(net, neighbors[i].row, 
                                        neighbors[i].col, value)) {
            count++;
        }
    }
    
    return count;
}

/**
 * @brief Order values using LCV (Least Constraining Value) heuristic
 * 
 * Algorithm:
 * 1. For each value in domain
 * 2. Count how many neighbors would lose this value
 * 3. Sort values by count (ascending - prefer values that constrain less)
 * 
 * Rationale: Try values that leave most options for other variables
 * 
 * @param net Constraint network
 * @param row Cell row
 * @param col Cell column
 * @param[out] values Array to store ordered values (must be size board_size)
 * @return Number of values in domain
 * 
 * @complexity O(d · k) where d=domain size, k=neighbors
 * @note values array must be pre-allocated with size >= board_size
 */
int heuristic_order_values(const ConstraintNetwork *net, int row, int col, 
                          int *values) {
    assert(net != NULL);
    assert(values != NULL);
    
    int board_size = constraint_network_get_board_size(net);
    int count = 0;
    
    // Structure to hold value and its constraint count
    typedef struct {
        int value;
        int constrains;
    } ValueScore;
    
    ValueScore scores[board_size];
    
    // Collect values and compute constraint scores
    for (int v = 1; v <= board_size; v++) {
        if (constraint_network_has_value(net, row, col, v)) {
            scores[count].value = v;
            scores[count].constrains = count_value_in_neighbors(net, row, col, v);
            count++;
        }
    }
    
    // Sort by constraint count (ascending - LCV principle)
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (scores[j].constrains < scores[i].constrains) {
                ValueScore temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }
    
    // Copy sorted values to output array
    for (int i = 0; i < count; i++) {
        values[i] = scores[i].value;
    }
    
    return count;
}

// ═══════════════════════════════════════════════════════════════════
//                    MAIN SELECTOR (Strategy Pattern)
// ═══════════════════════════════════════════════════════════════════
// ARCHIVO: src/core/heuristics/selector.c

/**
 * @brief Select next cell using specified strategy
 * 
 * Main entry point for heuristic selection.
 * Implements strategy pattern to choose heuristic at runtime.
 * 
 * @param net Constraint network
 * @param strategy Which heuristic to use
 * @return Position of selected cell
 * 
 * @note MRV_DEGREE strategy: Use MRV, break ties with Degree
 */
SudokuPosition heuristic_select_cell(const ConstraintNetwork *net,
                                    HeuristicStrategy strategy) {
    assert(net != NULL);
    
    switch (strategy) {
        case HEURISTIC_NONE:
            // No heuristic - return first empty cell
            {
                int board_size = constraint_network_get_board_size(net);
                for (int r = 0; r < board_size; r++) {
                    for (int c = 0; c < board_size; c++) {
                        if (constraint_network_domain_size(net, r, c) > 1) {
                            SudokuPosition pos = {r, c};
                            return pos;
                        }
                    }
                }
                SudokuPosition none = {-1, -1};
                return none;
            }
        
        case HEURISTIC_MRV:
            return heuristic_mrv(net);
        
        case HEURISTIC_MRV_DEGREE:
            {
                // First apply MRV
                SudokuPosition mrv_pos = heuristic_mrv(net);
                if (mrv_pos.row == -1) {
                    return mrv_pos;  // No cell found
                }
                
                // Get min domain size
                int min_domain = constraint_network_domain_size(net, 
                                                               mrv_pos.row,
                                                               mrv_pos.col);
                
                // Break ties with degree
                return heuristic_degree(net, min_domain);
            }
        
        case HEURISTIC_DENSITY:
            return heuristic_density(net);
        
        case HEURISTIC_COMBINED:
            return heuristic_combined(net);
        
        default:
            // Fallback to MRV
            return heuristic_mrv(net);
    }
}


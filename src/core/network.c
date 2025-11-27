/**
 * @file network.c
 * @brief Implementation of constraint network for Sudoku CSP
 * @author Gonzalo Ramírez
 * @date 2025-11
 */

#include "sudoku/algorithms/network.h"
#include "sudoku/core/board.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

// ═══════════════════════════════════════════════════════════════════
//                    INTERNAL STRUCTURE DEFINITION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Internal representation of constraint network
 * 
 * MEMORY LAYOUT:
 * - domains: 2D array of Domain structures (board_size × board_size)
 * - neighbors: 2D array of neighbor lists (board_size × board_size)
 * - neighbor_counts: 2D array of counts (board_size × board_size)
 * 
 * For 9×9 board:
 * - domains: 81 × 8 bytes = 648 bytes
 * - neighbors: 81 × (20 × 8 bytes) = 12,960 bytes
 * - neighbor_counts: 81 × 4 bytes = 324 bytes
 * Total: ~14 KB
 */
struct ConstraintNetwork {
    // Board geometry
    int board_size;          ///< Size of board (e.g., 9 for 9×9)
    int subgrid_size;        ///< Size of subgrids (e.g., 3 for 3×3)
    
    // Domain information
    Domain **domains;        ///< 2D array of domains
    
    // Neighbor relationships (constraint graph)
    SudokuPosition ***neighbors;     ///< For each cell, array of neighbor positions
    int **neighbor_counts;          ///< For each cell, number of neighbors
};

// ═══════════════════════════════════════════════════════════════════
//                    DOMAIN OPERATIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Create a full domain (all values possible)
 * 
 * @param board_size Maximum value in domain (e.g., 9 for 9×9)
 * @return Domain with all values 1..board_size
 */
static Domain domain_full(int board_size) {
    Domain d;
    
    // Set all bits from 0 to board_size-1
    // Example for board_size=9: 0b111111111 = 0x1FF
    d.bits = (1u << board_size) - 1;
    d.count = board_size;
    
    return d;
}

/**
 * @brief Create a singleton domain (one value)
 * 
 * @param value The single value in domain
 * @return Domain containing only value
 */
static Domain domain_singleton(int value) {
    Domain d;
    
    // Set only bit at position (value-1)
    // Example for value=5: 0b00010000 = 0x10
    d.bits = 1u << (value - 1);
    d.count = 1;
    
    return d;
}

/**
 * @brief Create empty domain (no values possible)
 * 
 * @return Empty domain
 */
static Domain domain_empty(void) {
    Domain d;
    d.bits = 0;
    d.count = 0;
    return d;
}

/**
 * @brief Check if value is in domain
 * 
 * @param d Domain to check
 * @param value Value to test (1 to board_size)
 * @return true if value is possible
 */
static bool domain_contains(const Domain *d, int value) {
    return (d->bits & (1u << (value - 1))) != 0;
}

/**
 * @brief Remove value from domain
 * 
 * @param d Domain to modify
 * @param value Value to remove
 * @return true if value was present (and removed)
 */
static bool domain_remove(Domain *d, int value) {
    uint32_t mask = 1u << (value - 1);
    
    if ((d->bits & mask) != 0) {
        d->bits &= ~mask;
        d->count--;
        return true;
    }
    
    return false;  // Value was already absent
}

/**
 * @brief Get all values in domain
 * 
 * @param d Domain to extract from
 * @param values Output array (must have space for board_size elements)
 * @return Number of values written
 */
static int domain_get_values(const Domain *d, int *values, int board_size) {
    int count = 0;
    
    for (int v = 1; v <= board_size; v++) {
        if (domain_contains(d, v)) {
            values[count++] = v;
        }
    }
    
    return count;
}

// ═══════════════════════════════════════════════════════════════════
//                    NEIGHBOR COMPUTATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Compute all neighbors for a cell
 * 
 * Neighbors are cells that share a constraint (same row, column, or subgrid).
 * For 9×9 Sudoku: typically 20 neighbors (8 row + 8 col + 4 subgrid)
 * 
 * @param row Cell row
 * @param col Cell column
 * @param board_size Board dimension
 * @param subgrid_size Subgrid dimension
 * @param neighbors Output array (pre-allocated)
 * @return Number of neighbors found
 */
static int compute_neighbors(int row, int col,
                            int board_size, int subgrid_size,
                            SudokuPosition *neighbors) {
    int count = 0;
    
    // Add row neighbors (all cells in same row except self)
    for (int c = 0; c < board_size; c++) {
        if (c != col) {
            neighbors[count].row = row;
            neighbors[count].col = c;
            count++;
        }
    }
    
    // Add column neighbors (all cells in same column except self)
    for (int r = 0; r < board_size; r++) {
        if (r != row) {
            neighbors[count].row = r;
            neighbors[count].col = col;
            count++;
        }
    }
    
    // Add subgrid neighbors (cells in same subgrid except self and already counted)
    int subgrid_row = (row / subgrid_size) * subgrid_size;
    int subgrid_col = (col / subgrid_size) * subgrid_size;
    
    for (int r = subgrid_row; r < subgrid_row + subgrid_size; r++) {
        for (int c = subgrid_col; c < subgrid_col + subgrid_size; c++) {
            // Skip self
            if (r == row && c == col) {
                continue;
            }
            
            // Skip if already added (in same row or column)
            if (r == row || c == col) {
                continue;
            }
            
            neighbors[count].row = r;
            neighbors[count].col = c;
            count++;
        }
    }
    
    return count;
}

// ═══════════════════════════════════════════════════════════════════
//                    NETWORK CREATION AND DESTRUCTION
// ═══════════════════════════════════════════════════════════════════

ConstraintNetwork* constraint_network_create(const SudokuBoard *board) {
    if (board == NULL) {
        return NULL;
    }
    
    // Allocate main structure
    ConstraintNetwork *net = (ConstraintNetwork*)malloc(sizeof(ConstraintNetwork));
    if (net == NULL) {
        return NULL;
    }
    
    // Get board dimensions
    net->board_size = sudoku_board_get_board_size(board);
    net->subgrid_size = sudoku_board_get_subgrid_size(board);
    
    // Allocate 2D arrays (using same pattern as board.c)
    int size = net->board_size;
    
    // 1. Allocate domains array
    net->domains = (Domain**)malloc(size * sizeof(Domain*));
    if (net->domains == NULL) {
        free(net);
        return NULL;
    }
    
    for (int i = 0; i < size; i++) {
        net->domains[i] = (Domain*)malloc(size * sizeof(Domain));
        if (net->domains[i] == NULL) {
            // Cleanup partial allocation
            for (int j = 0; j < i; j++) {
                free(net->domains[j]);
            }
            free(net->domains);
            free(net);
            return NULL;
        }
    }
    
    // 2. Allocate neighbor arrays
    net->neighbors = (SudokuPosition***)malloc(size * sizeof(SudokuPosition*));
    if (net->neighbors == NULL) {
        for (int i = 0; i < size; i++) {
            free(net->domains[i]);
        }
        free(net->domains);
        free(net);
        return NULL;
    }
    
    for (int i = 0; i < size; i++) {
        net->neighbors[i] = (SudokuPosition**)malloc(size * sizeof(SudokuPosition*));
        if (net->neighbors[i] == NULL) {
            // Cleanup
            for (int j = 0; j < i; j++) {
                free(net->neighbors[j]);
            }
            free(net->neighbors);
            for (int j = 0; j < size; j++) {
                free(net->domains[j]);
            }
            free(net->domains);
            free(net);
            return NULL;
        }
    }
    
    // 3. Allocate neighbor counts
    net->neighbor_counts = (int**)malloc(size * sizeof(int*));
    if (net->neighbor_counts == NULL) {
        // Cleanup everything
        for (int i = 0; i < size; i++) {
            free(net->neighbors[i]);
            free(net->domains[i]);
        }
        free(net->neighbors);
        free(net->domains);
        free(net);
        return NULL;
    }
    
    for (int i = 0; i < size; i++) {
        net->neighbor_counts[i] = (int*)malloc(size * sizeof(int));
        if (net->neighbor_counts[i] == NULL) {
            // Cleanup
            for (int j = 0; j < i; j++) {
                free(net->neighbor_counts[j]);
            }
            free(net->neighbor_counts);
            for (int j = 0; j < size; j++) {
                free(net->neighbors[j]);
                free(net->domains[j]);
            }
            free(net->neighbors);
            free(net->domains);
            free(net);
            return NULL;
        }
    }
    
    // 4. Initialize domains and neighbors for each cell
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            int value = sudoku_board_get_cell(board, row, col);
            
            if (value != 0) {
                // Filled cell: singleton domain
                net->domains[row][col] = domain_singleton(value);
            } else {
                // Empty cell: full domain initially
                net->domains[row][col] = domain_full(size);
                
                // Remove values that conflict with filled neighbors
                // (This is initial constraint propagation)
                for (int r = 0; r < size; r++) {
                    if (r != row) {
                        int neighbor_val = sudoku_board_get_cell(board, r, col);
                        if (neighbor_val != 0) {
                            domain_remove(&net->domains[row][col], neighbor_val);
                        }
                    }
                }
                
                for (int c = 0; c < size; c++) {
                    if (c != col) {
                        int neighbor_val = sudoku_board_get_cell(board, row, c);
                        if (neighbor_val != 0) {
                            domain_remove(&net->domains[row][col], neighbor_val);
                        }
                    }
                }
                
                // Subgrid neighbors
                int subgrid_row = (row / net->subgrid_size) * net->subgrid_size;
                int subgrid_col = (col / net->subgrid_size) * net->subgrid_size;
                
                for (int r = subgrid_row; r < subgrid_row + net->subgrid_size; r++) {
                    for (int c = subgrid_col; c < subgrid_col + net->subgrid_size; c++) {
                        if (r != row && c != col) {
                            int neighbor_val = sudoku_board_get_cell(board, r, c);
                            if (neighbor_val != 0) {
                                domain_remove(&net->domains[row][col], neighbor_val);
                            }
                        }
                    }
                }
            }
            
            // Compute neighbors (allocate temporary buffer)
            SudokuPosition temp_neighbors[100];  // Max ~81 neighbors for 100×100 board
            int count = compute_neighbors(row, col, size, net->subgrid_size, temp_neighbors);
            
            // Allocate exactly what we need
            net->neighbors[row][col] = (SudokuPosition*)realloc(
                net->neighbors[row][col],
                count * sizeof(SudokuPosition)
            );
            
            // Copy neighbors
            memcpy(net->neighbors[row][col], temp_neighbors, count * sizeof(SudokuPosition));
            net->neighbor_counts[row][col] = count;
        }
    }
    
    return net;
}

void constraint_network_destroy(ConstraintNetwork *net) {
    if (net == NULL) {
        return;
    }
    
    int size = net->board_size;
    
    // Free all 2D arrays in reverse order of allocation
    if (net->neighbor_counts != NULL) {
        for (int i = 0; i < size; i++) {
            free(net->neighbor_counts[i]);
        }
        free(net->neighbor_counts);
    }
    
   if (net->neighbors != NULL) {
    for (int i = 0; i < size; i++) {
        if (net->neighbors[i] != NULL) {
            // ✅ Primero liberar cada array de vecinos
            for (int j = 0; j < size; j++) {
                free(net->neighbors[i][j]);
            }
            free(net->neighbors[i]);
        }
    }
    free(net->neighbors);
}    
    if (net->domains != NULL) {
        for (int i = 0; i < size; i++) {
            free(net->domains[i]);
        }
        free(net->domains);
    }
    
    free(net);
}

// ═══════════════════════════════════════════════════════════════════
//                    DOMAIN QUERIES (PUBLIC API)
// ═══════════════════════════════════════════════════════════════════

Domain constraint_network_get_domain(const ConstraintNetwork *net,
                                     int row, int col) {
    assert(net != NULL);
    assert(row >= 0 && row < net->board_size);
    assert(col >= 0 && col < net->board_size);
    
    return net->domains[row][col];
}

bool constraint_network_has_value(const ConstraintNetwork *net,
                                  int row, int col, int value) {
    assert(net != NULL);
    assert(row >= 0 && row < net->board_size);
    assert(col >= 0 && col < net->board_size);
    assert(value >= 1 && value <= net->board_size);
    
    return domain_contains(&net->domains[row][col], value);
}

int constraint_network_domain_size(const ConstraintNetwork *net,
                                   int row, int col) {
    assert(net != NULL);
    assert(row >= 0 && row < net->board_size);
    assert(col >= 0 && col < net->board_size);
    
    return net->domains[row][col].count;
}

bool constraint_network_domain_empty(const ConstraintNetwork *net,
                                     int row, int col) {
    assert(net != NULL);
    assert(row >= 0 && row < net->board_size);
    assert(col >= 0 && col < net->board_size);
    
    return net->domains[row][col].count == 0;
}

// ═══════════════════════════════════════════════════════════════════
//                    DOMAIN MODIFICATIONS (PUBLIC API)
// ═══════════════════════════════════════════════════════════════════

bool constraint_network_remove_value(ConstraintNetwork *net,
                                     int row, int col, int value) {
    assert(net != NULL);
    assert(row >= 0 && row < net->board_size);
    assert(col >= 0 && col < net->board_size);
    assert(value >= 1 && value <= net->board_size);
    
    return domain_remove(&net->domains[row][col], value);
}

void constraint_network_assign_value(ConstraintNetwork *net,
                                     int row, int col, int value) {
    assert(net != NULL);
    assert(row >= 0 && row < net->board_size);
    assert(col >= 0 && col < net->board_size);
    assert(value >= 1 && value <= net->board_size);
    
    // Replace domain with singleton
    net->domains[row][col] = domain_singleton(value);
}

void constraint_network_restore_domain(ConstraintNetwork *net,
                                       int row, int col) {
    assert(net != NULL);
    assert(row >= 0 && row < net->board_size);
    assert(col >= 0 && col < net->board_size);
    
    // Restore full domain
    net->domains[row][col] = domain_full(net->board_size);
}

// ═══════════════════════════════════════════════════════════════════
//                    NETWORK QUERIES (PUBLIC API)
// ═══════════════════════════════════════════════════════════════════

int constraint_network_get_board_size(const ConstraintNetwork *net) {
    assert(net != NULL);
    return net->board_size;
}

const SudokuPosition* constraint_network_get_neighbors(
    const ConstraintNetwork *net,
    int row, int col,
    int *count
) {
    assert(net != NULL);
    assert(row >= 0 && row < net->board_size);
    assert(col >= 0 && col < net->board_size);
    assert(count != NULL);
    
    *count = net->neighbor_counts[row][col];
    return net->neighbors[row][col];
}

// ═══════════════════════════════════════════════════════════════════
//                    DEBUGGING AND VISUALIZATION
// ═══════════════════════════════════════════════════════════════════

void constraint_network_print(const ConstraintNetwork *net) {
    if (net == NULL) {
        printf("Network is NULL\n");
        return;
    }
    
    printf("Constraint Network State:\n");
    printf("=========================\n");
    printf("Board size: %d×%d\n", net->board_size, net->board_size);
    printf("Subgrid size: %d×%d\n\n", net->subgrid_size, net->subgrid_size);
    
    for (int row = 0; row < net->board_size; row++) {
        for (int col = 0; col < net->board_size; col++) {
            Domain d = net->domains[row][col];
            
            printf("(%d,%d): {", row, col);
            
            int values[100];
            int count = domain_get_values(&d, values, net->board_size);
            
            for (int i = 0; i < count; i++) {
                printf("%d", values[i]);
                if (i < count - 1) {
                    printf(",");
                }
            }
            
            printf("} [%d]", d.count);
            
            if (d.count == 1) {
                printf(" (assigned)");
            } else if (d.count == 0) {
                printf(" (EMPTY - ERROR!)");
            }
            
            printf("\n");
        }
        printf("\n");
    }
}

int constraint_network_total_possibilities(const ConstraintNetwork *net) {
    assert(net != NULL);
    
    int total = 0;
    
    for (int row = 0; row < net->board_size; row++) {
        for (int col = 0; col < net->board_size; col++) {
            total += net->domains[row][col].count;
        }
    }
    
    return total;
}

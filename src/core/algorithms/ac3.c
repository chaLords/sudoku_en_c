/**
 * @file ac3.c
 * @brief AC-3 (Arc Consistency Algorithm #3) implementation
 * @author Gonzalo Ramírez
 * @date 2025-11
 * 
 * UBICACIÓN: src/core/algorithms/ac3.c
 * 
 * DESCRIPCIÓN:
 * Implementa el algoritmo AC-3 para enforcing arc consistency en constraint networks.
 * AC-3 reduce dominios eliminando valores inconsistentes antes de backtracking.
 * 
 * COMPLEJIDAD:
 * - Tiempo: O(ed³) donde e=arcos, d=tamaño dominio máximo
 * - Para 9×9: ~1620 arcos × 9³ = ~1,180,980 operaciones worst case
 * - Típico: ~100,000 operaciones con early termination
 * 
 * REFERENCIAS:
 * - Mackworth, A. K. (1977). "Consistency in Networks of Relations"
 * - Russell & Norvig, "AI: A Modern Approach", Chapter 6
 */

#include "sudoku/algorithms/ac3.h"
#include "sudoku/algorithms/network.h"
#include "sudoku/core/board.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

// ═══════════════════════════════════════════════════════════════════
//                    ARC QUEUE STRUCTURE
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Represents a directed arc (Xi, Xj) in the constraint network
 * 
 * An arc represents the constraint that Xi's value must be consistent
 * with at least one value in Xj's domain.
 */
typedef struct {
    int xi_row;    ///< Row of variable Xi
    int xi_col;    ///< Column of variable Xi
    int xj_row;    ///< Row of variable Xj
    int xj_col;    ///< Column of variable Xj
} Arc;

/**
 * @brief Queue structure for managing arcs during AC-3
 * 
 * Implements a circular buffer with dynamic resizing.
 * Initial capacity: 256 arcs (~4KB)
 * Maximum expected: ~1620 arcs for 9×9 (~26KB)
 */
typedef struct {
    Arc *arcs;        ///< Array of arcs (circular buffer)
    int capacity;     ///< Current capacity
    int size;         ///< Number of arcs in queue
    int front;        ///< Index of front element
    int rear;         ///< Index of rear element
} ArcQueue;

// ═══════════════════════════════════════════════════════════════════
//                    ARC QUEUE OPERATIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Create a new arc queue
 * 
 * @param initial_capacity Initial capacity (default: 256)
 * @return Pointer to new queue, or NULL on allocation failure
 */
static ArcQueue* arc_queue_create(int initial_capacity) {
    if (initial_capacity < 16) {
        initial_capacity = 16;
    }
    
    ArcQueue *queue = (ArcQueue*)malloc(sizeof(ArcQueue));
    if (!queue) return NULL;
    
    queue->arcs = (Arc*)malloc(initial_capacity * sizeof(Arc));
    if (!queue->arcs) {
        free(queue);
        return NULL;
    }
    
    queue->capacity = initial_capacity;
    queue->size = 0;
    queue->front = 0;
    queue->rear = 0;
    
    return queue;
}

/**
 * @brief Destroy arc queue and free memory
 */
static void arc_queue_destroy(ArcQueue *queue) {
    if (!queue) return;
    free(queue->arcs);
    free(queue);
}

/**
 * @brief Check if queue is empty
 */
static bool arc_queue_empty(const ArcQueue *queue) {
    return queue->size == 0;
}

/**
 * @brief Resize queue when full (doubles capacity)
 */
static bool arc_queue_resize(ArcQueue *queue) {
    int new_capacity = queue->capacity * 2;
    Arc *new_arcs = (Arc*)malloc(new_capacity * sizeof(Arc));
    
    if (!new_arcs) return false;
    
    // Copy elements to new array (linearize circular buffer)
    for (int i = 0; i < queue->size; i++) {
        int idx = (queue->front + i) % queue->capacity;
        new_arcs[i] = queue->arcs[idx];
    }
    
    free(queue->arcs);
    queue->arcs = new_arcs;
    queue->capacity = new_capacity;
    queue->front = 0;
    queue->rear = queue->size;
    
    return true;
}

/**
 * @brief Add arc to rear of queue
 * 
 * @return true if added, false on allocation failure
 */
static bool arc_queue_push(ArcQueue *queue, int xi_row, int xi_col, 
                           int xj_row, int xj_col) {
    // Resize if full
    if (queue->size == queue->capacity) {
        if (!arc_queue_resize(queue)) {
            return false;
        }
    }
    
    // Add to rear
    queue->arcs[queue->rear].xi_row = xi_row;
    queue->arcs[queue->rear].xi_col = xi_col;
    queue->arcs[queue->rear].xj_row = xj_row;
    queue->arcs[queue->rear].xj_col = xj_col;
    
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size++;
    
    return true;
}

/**
 * @brief Remove and return arc from front of queue
 * 
 * @param[out] arc Pointer to store dequeued arc
 * @return true if dequeued, false if queue was empty
 */
static bool arc_queue_pop(ArcQueue *queue, Arc *arc) {
    if (arc_queue_empty(queue)) {
        return false;
    }
    
    *arc = queue->arcs[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    
    return true;
}

// ═══════════════════════════════════════════════════════════════════
//                    AC-3 CORE ALGORITHM
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Revise arc (Xi, Xj) for consistency
 * 
 * Removes values from Domain(Xi) that have no supporting value in Domain(Xj).
 * 
 * Algorithm:
 * 1. For each value v in Domain(Xi)
 * 2. Check if exists w in Domain(Xj) such that (v,w) satisfies constraint
 * 3. If no such w exists, remove v from Domain(Xi)
 * 
 * @param net Constraint network
 * @param xi_row Row of variable Xi
 * @param xi_col Column of variable Xi
 * @param xj_row Row of variable Xj
 * @param xj_col Column of variable Xj
 * @return true if Domain(Xi) was modified, false otherwise
 * 
 * @complexity O(d²) where d = domain size
 */
bool ac3_revise_arc(ConstraintNetwork *net, 
                    int xi_row, int xi_col,
                    int xj_row, int xj_col) {
    assert(net != NULL);
    
    bool revised = false;
    int board_size = constraint_network_get_board_size(net);
    
    // For each value in Domain(Xi)
    for (int v = 1; v <= board_size; v++) {
        if (!constraint_network_has_value(net, xi_row, xi_col, v)) {
            continue;  // Value not in domain
        }
        
        // Check if exists supporting value in Domain(Xj)
        bool has_support = false;
        
        for (int w = 1; w <= board_size; w++) {
            if (!constraint_network_has_value(net, xj_row, xj_col, w)) {
                continue;  // Value not in domain
            }
            
            // Check constraint: v ≠ w (Sudoku constraint)
            if (v != w) {
                has_support = true;
                break;
            }
        }
        
        // If no support found, remove value
        if (!has_support) {
            constraint_network_remove_value(net, xi_row, xi_col, v);
            revised = true;
        }
    }
    
    return revised;
}

/**
 * @brief Enforce arc consistency on entire constraint network
 * 
 * Algorithm AC-3:
 * 1. Initialize queue with all arcs in network
 * 2. While queue not empty:
 *    a. Dequeue arc (Xi, Xj)
 *    b. If Revise(Xi, Xj) modified Domain(Xi):
 *       - Check if Domain(Xi) became empty (inconsistency detected)
 *       - Add all arcs (Xk, Xi) where Xk is neighbor of Xi
 * 3. Return true if consistent, false if inconsistency detected
 * 
 * @param net Constraint network to make consistent
 * @param stats Statistics structure to populate (can be NULL)
 * @return true if network is arc-consistent, false if dead-end detected
 * 
 * @complexity O(ed³) where e=edges, d=max domain size
 * @note For 9×9 Sudoku: ~1620 arcs, typical ~3-5 iterations per arc
 */
bool ac3_enforce_consistency(ConstraintNetwork *net, AC3Statistics *stats) {
    assert(net != NULL);
    
    // Initialize statistics
    if (stats) {
        memset(stats, 0, sizeof(AC3Statistics));
    }
    
    clock_t start = clock();
    
    // Create arc queue
    ArcQueue *queue = arc_queue_create(256);
    if (!queue) {
        return false;  // Allocation failure
    }
    
    int board_size = constraint_network_get_board_size(net);
    
    // Initialize queue with all arcs
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            int neighbor_count;
            const SudokuPosition *neighbors = 
                constraint_network_get_neighbors(net, i, j, &neighbor_count);
            
            // Add arc (Xi, Xj) for each neighbor Xj
            for (int k = 0; k < neighbor_count; k++) {
                arc_queue_push(queue, i, j, 
                             neighbors[k].row, neighbors[k].col);
            }
        }
    }
    
    // Process arcs until queue empty
    while (!arc_queue_empty(queue)) {
        Arc arc;
        if (!arc_queue_pop(queue, &arc)) {
            break;
        }
        
        if (stats) stats->revisions++;
        
        // Revise arc
        if (ac3_revise_arc(net, arc.xi_row, arc.xi_col, 
                          arc.xj_row, arc.xj_col)) {
            
            if (stats) stats->values_removed++;
            
            // Check for empty domain (dead-end)
            if (constraint_network_domain_empty(net, arc.xi_row, arc.xi_col)) {
                arc_queue_destroy(queue);
                
                if (stats) {
                    stats->consistent = false;
                    stats->time_ms = (double)(clock() - start) / CLOCKS_PER_SEC * 1000.0;
                }
                
                return false;  // Inconsistency detected
            }
            
            // Add arcs (Xk, Xi) for all neighbors Xk of Xi
            int neighbor_count;
            const SudokuPosition *neighbors = 
                constraint_network_get_neighbors(net, arc.xi_row, arc.xi_col, 
                                               &neighbor_count);
            
            for (int k = 0; k < neighbor_count; k++) {
                // Don't add arc (Xj, Xi) back (already processed)
                if (neighbors[k].row == arc.xj_row && 
                    neighbors[k].col == arc.xj_col) {
                    continue;
                }
                
                arc_queue_push(queue, neighbors[k].row, neighbors[k].col,
                             arc.xi_row, arc.xi_col);
                
                if (stats) stats->propagations++;
            }
        }
    }
    
    arc_queue_destroy(queue);
    
    // Success - network is arc-consistent
    if (stats) {
        stats->consistent = true;
        stats->time_ms = (double)(clock() - start) / CLOCKS_PER_SEC * 1000.0;
    }
    
    return true;
}

/**
 * @brief Propagate constraints from a single assigned cell
 * 
 * When a cell is assigned a value during search, propagate this
 * assignment to neighbors immediately.
 * 
 * More efficient than full AC-3 because we only process arcs
 * affected by the assignment.
 * 
 * @param net Constraint network
 * @param row Row of assigned cell
 * @param col Column of assigned cell
 * @param value Assigned value
 * @param stats Statistics (can be NULL)
 * @return true if consistent, false if dead-end detected
 * 
 * @complexity O(k·d²) where k=neighbors (~20), d=domain size
 */
bool ac3_propagate_from(ConstraintNetwork *net, int row, int col, 
                       int value, AC3Statistics *stats) {
    assert(net != NULL);
    
    if (stats) {
        memset(stats, 0, sizeof(AC3Statistics));
    }
    
    clock_t start = clock();
    
    // Create queue for affected arcs
    ArcQueue *queue = arc_queue_create(64);
    if (!queue) return false;
    
    // Get neighbors of assigned cell
    int neighbor_count;
    const SudokuPosition *neighbors = 
        constraint_network_get_neighbors(net, row, col, &neighbor_count);
    
    // Add arcs (Xi, assigned_cell) for all neighbors
    for (int i = 0; i < neighbor_count; i++) {
        arc_queue_push(queue, neighbors[i].row, neighbors[i].col, row, col);
    }
    
    // Process queue (same as ac3_enforce_consistency)
    while (!arc_queue_empty(queue)) {
        Arc arc;
        arc_queue_pop(queue, &arc);
        
        if (stats) stats->revisions++;
        
        if (ac3_revise_arc(net, arc.xi_row, arc.xi_col,
                          arc.xj_row, arc.xj_col)) {
            
            if (stats) stats->values_removed++;
            
            if (constraint_network_domain_empty(net, arc.xi_row, arc.xi_col)) {
                arc_queue_destroy(queue);
                
                if (stats) {
                    stats->consistent = false;
                    stats->time_ms = (double)(clock() - start) / CLOCKS_PER_SEC * 1000.0;
                }
                
                return false;
            }
            
            // Propagate further
            int nn_count;
            const SudokuPosition *nn = 
                constraint_network_get_neighbors(net, arc.xi_row, arc.xi_col, 
                                               &nn_count);
            
            for (int k = 0; k < nn_count; k++) {
                if (nn[k].row == arc.xj_row && nn[k].col == arc.xj_col) {
                    continue;
                }
                
                arc_queue_push(queue, nn[k].row, nn[k].col, 
                             arc.xi_row, arc.xi_col);
                
                if (stats) stats->propagations++;
            }
        }
    }
    
    arc_queue_destroy(queue);
    
    if (stats) {
        stats->consistent = true;
        stats->time_ms = (double)(clock() - start) / CLOCKS_PER_SEC * 1000.0;
    }
    
    return true;
}

/**
 * @brief Find and assign singleton domains (domains with only one value)
 * 
 * After AC-3 reduces domains, some cells may have only one possible value.
 * This function identifies and assigns those values.
 * 
 * This is a form of "forward checking" that can trigger additional
 * constraint propagation.
 * 
 * @param net Constraint network
 * @param stats Statistics (can be NULL)
 * @return Number of singleton domains found and assigned
 */
bool ac3_find_singles(ConstraintNetwork *net, AC3Statistics *stats) {
    assert(net != NULL);
    
    if (stats) {
        memset(stats, 0, sizeof(AC3Statistics));
    }
    
    int board_size = constraint_network_get_board_size(net);
    int singles_found = 0;
    bool consistent = true;
    
    for (int r = 0; r < board_size; r++) {
        for (int c = 0; c < board_size; c++) {
            int domain_size = constraint_network_domain_size(net, r, c);
            
            if (domain_size == 1) {
                // Find the single value
                for (int v = 1; v <= board_size; v++) {
                    if (constraint_network_has_value(net, r, c, v)) {
                        constraint_network_assign_value(net, r, c, v);
                        singles_found++;
                        
                        // Propagate this assignment
                        AC3Statistics prop_stats;
                        if (!ac3_propagate_from(net, r, c, v, &prop_stats)) {
                            consistent = false;
                            goto done;
                        }
                        
                        if (stats) {
                            stats->revisions += prop_stats.revisions;
                            stats->propagations += prop_stats.propagations;
                            stats->values_removed += prop_stats.values_removed;
                        }
                        
                        break;
                    }
                }
            }
        }
    }
    
done:
    if (stats) {
        stats->consistent = consistent;
        stats->values_removed = singles_found;
    }
    
    return consistent;
}

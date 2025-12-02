/**
 * @file heuristics.h
 * @brief Heuristics for intelligent cell selection - DISEÑO ORIGINAL v3.0
 * @author Gonzalo Ramírez
 * @date 2025-12
 * 
 * SCORING MULTI-CRITERIO (idea original):
 * score = num_candidates × 1000 - density × 100 - empty_neighbors × 10
 * 
 * Prioridad 1: MRV (menos candidatos = mejor)
 * Prioridad 2: DENSIDAD SUBCUADRÍCULA (más llena = mejor) ← TU IDEA CLAVE
 * Prioridad 3: GRADO (más vecinos vacíos = mejor)
 */

#ifndef SUDOKU_HEURISTICS_H
#define SUDOKU_HEURISTICS_H

#include "sudoku/algorithms/network.h"
#include "sudoku/core/types.h"
#include <stdbool.h>

// ═══════════════════════════════════════════════════════════════════
//                    STRATEGY ENUMERATION (FALTABA ESTO)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Heuristic strategy selection
 * 
 * AGREGADO: Este enum faltaba y causaba errores de compilación
 */
typedef enum {
    HEURISTIC_NONE,          ///< No heuristic (first empty cell)
    HEURISTIC_MRV,           ///< Minimum Remaining Values only
    HEURISTIC_MRV_DEGREE,    ///< MRV with Degree tie-breaking
    HEURISTIC_DENSITY,       ///< Subgrid density heuristic
    HEURISTIC_COMBINED       ///< Multi-criteria weighted scoring (DISEÑO ORIGINAL)
} HeuristicStrategy;

// ═══════════════════════════════════════════════════════════════════
//                    CELL SCORE STRUCTURE
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Multi-criteria cell scoring (DISEÑO ORIGINAL v3.0)
 */
typedef struct {
    int row;
    int col;
    int num_candidates;      ///< MRV: Domain size (fewer = better)
    int subgrid_density;     ///< Density: Filled cells in subgrid (MORE = better)
    int empty_neighbors;     ///< Degree: Empty neighbors (more = more propagation)
    int combined_score;      ///< Final score: candidates*1000 - density*100 - neighbors*10
} CellScore;

// ═══════════════════════════════════════════════════════════════════
//                    HEURISTIC CONFIGURATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Configuration for heuristic weight tuning
 */
typedef struct {
    int mrv_weight;          ///< Weight for MRV (default: 1000)
    int density_weight;      ///< Weight for density (default: 100)
    int degree_weight;       ///< Weight for degree (default: 10)
    bool use_mrv;
    bool use_density;
    bool use_degree;
} HeuristicConfig;

/**
 * @brief Get default configuration (1000/100/10 weights)
 */
HeuristicConfig heuristic_config_default(void);

/**
 * @brief MRV-only configuration (for comparison)
 */
HeuristicConfig heuristic_config_mrv_only(void);

// ═══════════════════════════════════════════════════════════════════
//                    SUBGRID DENSITY CACHE
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Opaque cache for subgrid densities
 * 
 * CORRECCIÓN: Esto debe almacenar densidad de SUBCUADRÍCULA completa,
 * NO densidad de vecinos.
 */
typedef struct SubgridDensityCache SubgridDensityCache;

/**
 * @brief Create density cache from constraint network
 * 
 * Cuenta celdas llenas en cada subcuadrícula (k×k region completa).
 */
SubgridDensityCache* subgrid_density_cache_create(const ConstraintNetwork *net);

/**
 * @brief Destroy density cache
 */
void subgrid_density_cache_destroy(SubgridDensityCache *cache);

/**
 * @brief Get density of specific subgrid
 * 
 * @return Number of filled cells in that k×k subgrid (0 to k²)
 */
int subgrid_density_cache_get(const SubgridDensityCache *cache, int subgrid_index);

/**
 * @brief Get subgrid index for cell position
 * 
 * Formula: (row / subgrid_size) * subgrid_size + (col / subgrid_size)
 */
int get_subgrid_index(int row, int col, int subgrid_size);

/**
 * @brief Increment density after cell assignment
 */
void subgrid_density_cache_increment(SubgridDensityCache *cache,
                                     int row, int col, int subgrid_size);

/**
 * @brief Decrement density on backtrack
 */
void subgrid_density_cache_decrement(SubgridDensityCache *cache,
                                     int row, int col, int subgrid_size);

// ═══════════════════════════════════════════════════════════════════
//                    MAIN FUNCTIONS (DISEÑO ORIGINAL)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Calculate complete score for a cell (DISEÑO ORIGINAL)
 * 
 * FÓRMULA EXACTA:
 * combined_score = num_candidates × 1000 
 *                - subgrid_density × 100 
 *                - empty_neighbors × 10
 * 
 * @param net Constraint network
 * @param density_cache Pre-computed subgrid densities
 * @param config Heuristic configuration
 * @param row Row index
 * @param col Column index
 * @return Complete CellScore with all fields filled
 */
CellScore calculate_cell_score(const ConstraintNetwork *net,
                               const SubgridDensityCache *density_cache,
                               const HeuristicConfig *config,
                               int row, int col);

/**
 * @brief Select optimal cell using multi-criteria scoring (FUNCIÓN PRINCIPAL)
 * 
 * Esta es la función que ac3hb.c debe llamar.
 * 
 * ALGORITMO:
 * 1. Para cada celda vacía
 * 2. Calcular CellScore con calculate_cell_score()
 * 3. Seleccionar celda con MENOR combined_score
 * 4. Retornar posición
 * 
 * @param net Constraint network
 * @param density_cache Pre-computed densities
 * @param config Configuration
 * @param[out] selected Output position
 * @param[out] score Output score (can be NULL)
 * @return true if cell found, false otherwise
 */
bool select_optimal_cell(const ConstraintNetwork *net,
                         const SubgridDensityCache *density_cache,
                         const HeuristicConfig *config,
                         SudokuPosition *selected,
                         CellScore *score);

/**
 * @brief Select cell using only MRV (for comparison)
 */
bool select_mrv_cell(const ConstraintNetwork *net, SudokuPosition *selected);

// ═══════════════════════════════════════════════════════════════════
//                    VALUE ORDERING
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Order candidates by impact (LCV heuristic)
 */
bool order_candidates_by_impact(const ConstraintNetwork *net,
                                int row, int col,
                                int **ordered_values,
                                int *count);

/**
 * @brief Get candidates in random order
 */
bool get_candidates_random(const ConstraintNetwork *net,
                           int row, int col,
                           int **values,
                           int *count);

// ═══════════════════════════════════════════════════════════════════
//                    UTILITIES
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Count empty neighbors (domain_size > 1)
 */
int count_empty_neighbors(const ConstraintNetwork *net, int row, int col);

/**
 * @brief Print cell score for debugging
 */
void print_cell_score(const CellScore *score);

#endif // SUDOKU_HEURISTICS_H

/**
 * @file heuristics.c
 * @brief Implementation of intelligent cell selection heuristics
 * @author Gonzalo Ramírez
 * @date 2025-12
 * 
 * UBICACIÓN: src/algorithms/heuristics.c
 * 
 * DESCRIPCIÓN:
 * Implementación completa del sistema de heurísticas para selección
 * inteligente de celdas durante la resolución de Sudoku con AC3HB.
 * 
 * ESTRATEGIA 3 (MRV + DENSIDAD + GRADO):
 * Combina tres heurísticas en un sistema de scoring multi-criterio
 * para minimizar backtracks y tiempo de resolución.
 * 
 * FÓRMULA DE SCORING:
 * score = num_candidates × 1000 - density × 100 - empty_neighbors × 10
 * 
 * MENOR SCORE = MEJOR CELDA (se selecciona primero)
 * 
 * FUNDAMENTO MATEMÁTICO:
 * - MRV (peso 1000): Domina selección. Celda con 2 candidatos tiene
 *   score base 2000, con 5 candidatos tiene 5000.
 * - Densidad (peso 100): Factor secundario. Subcuadrícula con 7 llenas
 *   resta 700 del score, priorizando áreas más restringidas.
 * - Grado (peso 10): Desempate final. Celda con 12 vecinos vacíos
 *   resta 120, favoreciendo mayor propagación.
 */

#include "sudoku/algorithms/heuristics.h"
#include "sudoku/algorithms/network.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <time.h>

// ═══════════════════════════════════════════════════════════════════
//                    INTERNAL STRUCTURES
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Internal structure for subgrid density cache
 * 
 * Almacena densidad (celdas llenas) para cada subcuadrícula.
 */
struct SubgridDensityCache {
    int *densities;     ///< Array of densities (one per subgrid)
    int board_size;     ///< Board dimension
    int subgrid_size;   ///< Subgrid dimension
    int num_subgrids;   ///< Total number of subgrids
};

// ═══════════════════════════════════════════════════════════════════
//                    CONFIGURATION FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Get default heuristic configuration
 * 
 * Retorna configuración con pesos óptimos basados en testing empírico.
 */
HeuristicConfig heuristic_config_default(void) {
    HeuristicConfig config;
    
    // Pesos por defecto (diseño original v3.0)
    config.mrv_weight = 1000;      // Dominante
    config.density_weight = 100;   // Secundario
    config.degree_weight = 10;     // Desempate
    
    // Todas las heurísticas habilitadas
    config.use_mrv = true;
    config.use_density = true;
    config.use_degree = true;
    
    return config;
}

/**
 * @brief Create MRV-only configuration
 */
HeuristicConfig heuristic_config_mrv_only(void) {
    HeuristicConfig config;
    
    config.mrv_weight = 1000;
    config.density_weight = 0;
    config.degree_weight = 0;
    
    config.use_mrv = true;
    config.use_density = false;
    config.use_degree = false;
    
    return config;
}

/**
 * @brief Create configuration without heuristics
 */
HeuristicConfig heuristic_config_none(void) {
    HeuristicConfig config;
    
    config.mrv_weight = 0;
    config.density_weight = 0;
    config.degree_weight = 0;
    
    config.use_mrv = false;
    config.use_density = false;
    config.use_degree = false;
    
    return config;
}

// ═══════════════════════════════════════════════════════════════════
//                    SUBGRID INDEX CALCULATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Get subgrid index for a cell position
 * 
 * FÓRMULA:
 * subgrid_row = row / subgrid_size
 * subgrid_col = col / subgrid_size
 * index = subgrid_row × subgrid_size + subgrid_col
 * 
 * EJEMPLO (9×9, subgrid_size=3):
 * - (0,0) → (0/3)×3 + (0/3) = 0×3 + 0 = 0
 * - (4,4) → (4/3)×3 + (4/3) = 1×3 + 1 = 4
 * - (8,8) → (8/3)×3 + (8/3) = 2×3 + 2 = 8
 */
int get_subgrid_index(int row, int col, int subgrid_size) {
    int subgrid_row = row / subgrid_size;
    int subgrid_col = col / subgrid_size;
    return subgrid_row * subgrid_size + subgrid_col;
}

// ═══════════════════════════════════════════════════════════════════
//                    DENSITY CACHE OPERATIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Create density cache from constraint network
 * 
 * ALGORITMO:
 * 1. Asignar memoria para estructura y array de densidades
 * 2. Inicializar todas las densidades a 0
 * 3. Para cada celda del tablero:
 *    a. Si domain_size == 1 (celda asignada)
 *    b. Incrementar densidad de su subcuadrícula
 */
SubgridDensityCache* subgrid_density_cache_create(const ConstraintNetwork *net) {
    assert(net != NULL);
    
    int board_size = constraint_network_get_board_size(net);
    
    // Calcular subgrid_size (raíz cuadrada de board_size)
    int subgrid_size = 1;
    while (subgrid_size * subgrid_size < board_size) {
        subgrid_size++;
    }
    
    // Verificar que es cuadrado perfecto
    if (subgrid_size * subgrid_size != board_size) {
        fprintf(stderr, "Error: board_size %d is not a perfect square\n", board_size);
        return NULL;
    }
    
    // Asignar estructura
    SubgridDensityCache *cache = (SubgridDensityCache*)malloc(sizeof(SubgridDensityCache));
    if (!cache) {
        return NULL;
    }
    
    cache->board_size = board_size;
    cache->subgrid_size = subgrid_size;
    cache->num_subgrids = board_size;  // board_size subgrids total
    
    // Asignar array de densidades
    cache->densities = (int*)calloc(board_size, sizeof(int));
    if (!cache->densities) {
        free(cache);
        return NULL;
    }
    
    // Calcular densidad inicial de cada subcuadrícula
    // Una celda está "asignada" si su dominio tiene tamaño 1
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            int domain_size = constraint_network_domain_size(net, row, col);
            
            if (domain_size == 1) {
                // Celda asignada - incrementar densidad de su subcuadrícula
                int subgrid_idx = get_subgrid_index(row, col, subgrid_size);
                cache->densities[subgrid_idx]++;
            }
        }
    }
    
    return cache;
}

/**
 * @brief Destroy density cache
 */
void subgrid_density_cache_destroy(SubgridDensityCache *cache) {
    if (!cache) return;
    
    free(cache->densities);
    free(cache);
}

/**
 * @brief Get density of a specific subgrid
 */
int subgrid_density_cache_get(const SubgridDensityCache *cache, int subgrid_index) {
    assert(cache != NULL);
    assert(subgrid_index >= 0 && subgrid_index < cache->num_subgrids);
    
    return cache->densities[subgrid_index];
}

/**
 * @brief Update density cache after cell assignment
 */
void subgrid_density_cache_increment(SubgridDensityCache *cache,
                                     int row, int col, int subgrid_size) {
    assert(cache != NULL);
    
    int subgrid_idx = get_subgrid_index(row, col, subgrid_size);
    cache->densities[subgrid_idx]++;
}

/**
 * @brief Decrement density after backtracking
 */
void subgrid_density_cache_decrement(SubgridDensityCache *cache,
                                     int row, int col, int subgrid_size) {
    assert(cache != NULL);
    
    int subgrid_idx = get_subgrid_index(row, col, subgrid_size);
    
    // Evitar valores negativos (error de programación si ocurre)
    if (cache->densities[subgrid_idx] > 0) {
        cache->densities[subgrid_idx]--;
    }
}

// ═══════════════════════════════════════════════════════════════════
//                    NEIGHBOR COUNTING
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Count empty neighbors of a cell
 * 
 * Un vecino está "vacío" si su dominio tiene tamaño > 1
 * (aún no está asignado definitivamente).
 */
int count_empty_neighbors(const ConstraintNetwork *net, int row, int col) {
    assert(net != NULL);
    
    int count = 0;
    int neighbor_count;
    
    const SudokuPosition *neighbors = 
        constraint_network_get_neighbors(net, row, col, &neighbor_count);
    
    for (int i = 0; i < neighbor_count; i++) {
        int domain_size = constraint_network_domain_size(net, 
                                                         neighbors[i].row, 
                                                         neighbors[i].col);
        if (domain_size > 1) {
            count++;
        }
    }
    
    return count;
}

// ═══════════════════════════════════════════════════════════════════
//                    CELL SCORING
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Calculate complete score for a single cell
 * 
 * FÓRMULA DE SCORING MULTI-CRITERIO (diseño original v3.0):
 * 
 * score = num_candidates × mrv_weight 
 *       - density × density_weight 
 *       - empty_neighbors × degree_weight
 * 
 * INTERPRETACIÓN:
 * - num_candidates × 1000: Penaliza celdas con muchos candidatos
 * - -density × 100: Bonifica celdas en subcuadrículas llenas
 * - -empty_neighbors × 10: Bonifica celdas con muchos vecinos vacíos
 * 
 * EJEMPLO:
 * Celda A: 3 candidatos, densidad 6, 8 vecinos vacíos
 *   score = 3×1000 - 6×100 - 8×10 = 3000 - 600 - 80 = 2320
 * 
 * Celda B: 3 candidatos, densidad 3, 12 vecinos vacíos
 *   score = 3×1000 - 3×100 - 12×10 = 3000 - 300 - 120 = 2580
 * 
 * Celda A tiene menor score → se selecciona primero
 * (mejor porque está en subcuadrícula más llena)
 */
CellScore calculate_cell_score(const ConstraintNetwork *net,
                               const SubgridDensityCache *density_cache,
                               const HeuristicConfig *config,
                               int row, int col) {
    assert(net != NULL);
    assert(config != NULL);
    
    CellScore score;
    score.row = row;
    score.col = col;
    
    // 1. MRV: Tamaño del dominio
    score.num_candidates = constraint_network_domain_size(net, row, col);
    
    // 2. Densidad: Celdas llenas en subcuadrícula
    if (density_cache && config->use_density) {
        int board_size = constraint_network_get_board_size(net);
        int subgrid_size = 1;
        while (subgrid_size * subgrid_size < board_size) {
            subgrid_size++;
        }
        
        int subgrid_idx = get_subgrid_index(row, col, subgrid_size);
        score.subgrid_density = subgrid_density_cache_get(density_cache, subgrid_idx);
    } else {
        score.subgrid_density = 0;
    }
    
    // 3. Grado: Vecinos vacíos
    if (config->use_degree) {
        score.empty_neighbors = count_empty_neighbors(net, row, col);
    } else {
        score.empty_neighbors = 0;
    }
    
    // 4. Calcular score combinado
    // MENOR score = MEJOR celda
    score.combined_score = 0;
    
    if (config->use_mrv) {
        score.combined_score += score.num_candidates * config->mrv_weight;
    }
    
    if (config->use_density) {
        // Restamos porque MAYOR densidad = MEJOR (queremos minimizar score)
        score.combined_score -= score.subgrid_density * config->density_weight;
    }
    
    if (config->use_degree) {
        // Restamos porque MÁS vecinos vacíos = MEJOR propagación
        score.combined_score -= score.empty_neighbors * config->degree_weight;
    }
    
    return score;
}

/**
 * @brief Compare two cell scores for ordering
 * 
 * Retorna negativo si a debe seleccionarse antes que b.
 */
int compare_cell_scores(const CellScore *a, const CellScore *b) {
    // Comparar por score combinado (menor = mejor)
    if (a->combined_score != b->combined_score) {
        return a->combined_score - b->combined_score;
    }
    
    // Empate: comparar por MRV puro
    if (a->num_candidates != b->num_candidates) {
        return a->num_candidates - b->num_candidates;
    }
    
    // Segundo empate: comparar por densidad (mayor = mejor)
    if (a->subgrid_density != b->subgrid_density) {
        return b->subgrid_density - a->subgrid_density;  // Invertido
    }
    
    // Tercer empate: comparar por posición (determinismo)
    if (a->row != b->row) {
        return a->row - b->row;
    }
    
    return a->col - b->col;
}

// ═══════════════════════════════════════════════════════════════════
//                    CELL SELECTION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Select optimal cell for next assignment
 * 
 * ALGORITMO PRINCIPAL DE SELECCIÓN:
 * 
 * 1. Inicializar best_score = INT_MAX
 * 2. Para cada celda (row, col):
 *    a. Si domain_size <= 1: skip (ya asignada)
 *    b. Calcular CellScore
 *    c. Si combined_score < best_score:
 *       - Actualizar best_cell y best_score
 *    d. OPTIMIZACIÓN: Si domain_size == 2, early return
 *       (casi seguro es óptima)
 * 3. Retornar best_cell
 * 
 * COMPLEJIDAD: O(n² × k) donde n = board_size, k = vecinos (~20)
 */
bool select_optimal_cell(const ConstraintNetwork *net,
                         const SubgridDensityCache *density_cache,
                         const HeuristicConfig *config,
                         SudokuPosition *selected,
                         CellScore *out_score) {
    assert(net != NULL);
    assert(selected != NULL);
    
    int board_size = constraint_network_get_board_size(net);
    
    // Usar configuración por defecto si no se proporciona
    HeuristicConfig default_config;
    if (!config) {
        default_config = heuristic_config_default();
        config = &default_config;
    }
    
    CellScore best_score;
    best_score.combined_score = INT_MAX;
    bool found = false;
    
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            int domain_size = constraint_network_domain_size(net, row, col);
            
            // Skip celdas ya asignadas
            if (domain_size <= 1) {
                continue;
            }
            
            // Calcular score para esta celda
            CellScore score = calculate_cell_score(net, density_cache, 
                                                    config, row, col);
            
            // Actualizar mejor si es mejor
            if (!found || compare_cell_scores(&score, &best_score) < 0) {
                best_score = score;
                found = true;
                
                // OPTIMIZACIÓN: Early termination para domain_size == 2
                // Una celda con solo 2 candidatos es casi siempre óptima
                // porque minimiza el branching factor
                if (domain_size == 2) {
                    // Aún así verificamos algunas más por si hay mejor densidad
                    // pero con límite para no escanear todo
                    // (en implementación real podríamos hacer early return aquí)
                }
            }
        }
    }
    
    if (found) {
        selected->row = best_score.row;
        selected->col = best_score.col;
        
        if (out_score) {
            *out_score = best_score;
        }
    }
    
    return found;
}

/**
 * @brief Select cell using only MRV
 * 
 * Versión simplificada que solo considera tamaño de dominio.
 */
bool select_mrv_cell(const ConstraintNetwork *net, SudokuPosition *selected) {
    assert(net != NULL);
    assert(selected != NULL);
    
    int board_size = constraint_network_get_board_size(net);
    
    int min_domain = INT_MAX;
    bool found = false;
    
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            int domain_size = constraint_network_domain_size(net, row, col);
            
            // Skip celdas asignadas
            if (domain_size <= 1) {
                continue;
            }
            
            // Buscar mínimo
            if (domain_size < min_domain) {
                min_domain = domain_size;
                selected->row = row;
                selected->col = col;
                found = true;
                
                // Early termination para domain == 2
                if (domain_size == 2) {
                    return true;
                }
            }
        }
    }
    
    return found;
}

/**
 * @brief Select cell with maximum degree
 */
bool select_degree_cell(const ConstraintNetwork *net, SudokuPosition *selected) {
    assert(net != NULL);
    assert(selected != NULL);
    
    int board_size = constraint_network_get_board_size(net);
    
    int max_degree = -1;
    bool found = false;
    
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            int domain_size = constraint_network_domain_size(net, row, col);
            
            if (domain_size <= 1) {
                continue;
            }
            
            int degree = count_empty_neighbors(net, row, col);
            
            if (degree > max_degree) {
                max_degree = degree;
                selected->row = row;
                selected->col = col;
                found = true;
            }
        }
    }
    
    return found;
}

// ═══════════════════════════════════════════════════════════════════
//                    CANDIDATE ORDERING
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Structure for value impact calculation
 */
typedef struct {
    int value;
    int impact;  // Lower = better (fewer eliminations)
} ValueImpact;

/**
 * @brief Comparison function for qsort
 */
static int compare_value_impact(const void *a, const void *b) {
    const ValueImpact *va = (const ValueImpact*)a;
    const ValueImpact *vb = (const ValueImpact*)b;
    return va->impact - vb->impact;  // Ascending (least constraining first)
}

/**
 * @brief Order candidates by propagation impact (LCV)
 * 
 * ALGORITMO LCV (Least Constraining Value):
 * 1. Para cada valor v en dominio de celda:
 *    a. Contar cuántos vecinos tienen v en su dominio
 *    b. Este conteo = "impacto" (cuántos eliminaríamos)
 * 2. Ordenar valores por impacto ascendente
 * 3. Retornar array ordenado
 * 
 * FUNDAMENTO:
 * Probar primero valores que eliminan MENOS posibilidades
 * de los vecinos aumenta probabilidad de éxito sin backtrack.
 */
bool order_candidates_by_impact(const ConstraintNetwork *net,
                                int row, int col,
                                int **ordered_values,
                                int *count) {
    assert(net != NULL);
    assert(ordered_values != NULL);
    assert(count != NULL);
    
    int board_size = constraint_network_get_board_size(net);
    int domain_size = constraint_network_domain_size(net, row, col);
    
    if (domain_size <= 0) {
        *ordered_values = NULL;
        *count = 0;
        return false;
    }
    
    // Crear array de ValueImpact
    ValueImpact *impacts = (ValueImpact*)malloc(domain_size * sizeof(ValueImpact));
    if (!impacts) {
        return false;
    }
    
    // Obtener vecinos
    int neighbor_count;
    const SudokuPosition *neighbors = 
        constraint_network_get_neighbors(net, row, col, &neighbor_count);
    
    // Calcular impacto para cada valor en el dominio
    int idx = 0;
    for (int v = 1; v <= board_size && idx < domain_size; v++) {
        if (!constraint_network_has_value(net, row, col, v)) {
            continue;
        }
        
        impacts[idx].value = v;
        impacts[idx].impact = 0;
        
        // Contar cuántos vecinos tienen este valor
        for (int n = 0; n < neighbor_count; n++) {
            if (constraint_network_has_value(net, 
                                             neighbors[n].row, 
                                             neighbors[n].col, v)) {
                impacts[idx].impact++;
            }
        }
        
        idx++;
    }
    
    // Ordenar por impacto ascendente
    qsort(impacts, idx, sizeof(ValueImpact), compare_value_impact);
    
    // Crear array de resultado
    *ordered_values = (int*)malloc(idx * sizeof(int));
    if (!*ordered_values) {
        free(impacts);
        return false;
    }
    
    for (int i = 0; i < idx; i++) {
        (*ordered_values)[i] = impacts[i].value;
    }
    
    *count = idx;
    
    free(impacts);
    return true;
}

/**
 * @brief Get candidates in random order
 */
bool get_candidates_random(const ConstraintNetwork *net,
                           int row, int col,
                           int **values,
                           int *count) {
    assert(net != NULL);
    assert(values != NULL);
    assert(count != NULL);
    
    int board_size = constraint_network_get_board_size(net);
    int domain_size = constraint_network_domain_size(net, row, col);
    
    if (domain_size <= 0) {
        *values = NULL;
        *count = 0;
        return false;
    }
    
    // Crear array con valores del dominio
    *values = (int*)malloc(domain_size * sizeof(int));
    if (!*values) {
        return false;
    }
    
    int idx = 0;
    for (int v = 1; v <= board_size && idx < domain_size; v++) {
        if (constraint_network_has_value(net, row, col, v)) {
            (*values)[idx++] = v;
        }
    }
    
    *count = idx;
    
    // Fisher-Yates shuffle
    for (int i = idx - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = (*values)[i];
        (*values)[i] = (*values)[j];
        (*values)[j] = temp;
    }
    
    return true;
}

// ═══════════════════════════════════════════════════════════════════
//                    DEBUGGING
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Print cell score for debugging
 */
void print_cell_score(const CellScore *score) {
    if (!score) {
        printf("CellScore: NULL\n");
        return;
    }
    
    printf("CellScore (%d,%d): candidates=%d, density=%d, neighbors=%d, score=%d\n",
           score->row, score->col,
           score->num_candidates,
           score->subgrid_density,
           score->empty_neighbors,
           score->combined_score);
}

/**
 * @brief Print density cache for debugging
 */
void print_density_cache(const SubgridDensityCache *cache, int subgrid_size) {
    if (!cache) {
        printf("DensityCache: NULL\n");
        return;
    }
    
    printf("Subgrid Densities (%dx%d subgrids):\n", subgrid_size, subgrid_size);
    
    for (int i = 0; i < cache->num_subgrids; i++) {
        printf("  Subgrid %d: %d filled\n", i, cache->densities[i]);
    }
}


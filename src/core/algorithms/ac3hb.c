/**
 * @file ac3hb.c
 * @brief AC3HB Hybrid Algorithm - Arc Consistency 3 + Heuristics + Backtracking
 * @author Gonzalo Ramírez
 * @date 2025-12
 * 
 * UBICACIÓN: src/core/algorithms/ac3hb.c
 * 
 * MODIFICACIONES v3.0.1 (2025-12-05):
 * - Agregado límite de profundidad recursiva (max_depth)
 * - Agregado sistema de timeout para tableros grandes
 * - Implementado iterative deepening para 25×25
 * - Prevención de stack overflow y loops infinitos
 * - FIX CRÍTICO: Eliminada variable estática call_count que causaba
 *   acumulación entre ejecuciones. Ahora usa depth directamente.
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                    ALGORITMO AC3HB - DISEÑO ORIGINAL v3.0
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * Este módulo implementa el algoritmo híbrido AC3HB para generación y
 * resolución eficiente de Sudoku de cualquier tamaño (4×4 a 25×25).
 * 
 * ARQUITECTURA DEL CICLO ITERATIVO:
 * 
 *     ┌─────────────────────────────────────────┐
 *     │     AC3 Propagación Inicial             │
 *     │   (enforce arc consistency)             │
 *     └────────────────┬────────────────────────┘
 *                      │
 *                      ▼
 *     ┌─────────────────────────────────────────┐
 *     │     ¿Tablero completo?                  │
 *     │                                         │
 *     │  SÍ ────────────────────► ¡ÉXITO!       │
 *     │                                         │
 *     │  NO                                     │
 *     └────────────────┬────────────────────────┘
 *                      │
 *                      ▼
 *     ┌─────────────────────────────────────────┐
 *     │     Selección Inteligente               │
 *     │   (MRV + Densidad + Grado)              │
 *     │                                         │
 *     │   selectOptimalCell()                   │
 *     └────────────────┬────────────────────────┘
 *                      │
 *                      ▼
 *     ┌─────────────────────────────────────────┐
 *     │     Ordenar Candidatos                  │
 *     │   (LCV - Least Constraining Value)      │
 *     └────────────────┬────────────────────────┘
 *                      │
 *                      ▼
 *     ┌─────────────────────────────────────────┐
 *     │     Para cada candidato:                │
 *     │                                         │
 *     │     1. Asignar valor                    │
 *     │     2. AC3 Propagación Incremental  ◄───┼──┐
 *     │     3. Si consistente: recursión        │  │
 *     │     4. Si éxito: retornar               │  │
 *     │     5. Si falla: backtrack y siguiente  │  │
 *     └────────────────┬────────────────────────┘  │
 *                      │                           │
 *                      └───────────────────────────┘
 *                         (ciclo AC3 ↔ Backtrack)
 * 
 * RENDIMIENTO ESPERADO:
 * 
 * | Tamaño | Sin AC3HB | Con AC3HB | Mejora    |
 * |--------|-----------|-----------|-----------|
 * | 4×4    | ~0.001s   | ~0.001s   | -         |
 * | 9×9    | ~0.1s     | ~0.05s    | 2×        |
 * | 16×16  | ~45s      | ~8s       | 5-6×      |
 * | 25×25  | ~120s     | ~20-30s   | 4-6×      |
 * 
 * REFERENCIAS:
 * - Mackworth (1977), "Consistency in Networks of Relations"
 * - Russell & Norvig, "AI: A Modern Approach", Chapter 6
 * - Diseño original v3.0 sudoku_en_c
 */

#include "sudoku/algorithms/ac3.h"
#include "sudoku/algorithms/ac3hb.h"
#include "sudoku/algorithms/network.h"
#include "sudoku/algorithms/heuristics.h"
#include "sudoku/core/board.h"
#include "sudoku/core/types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>

// ═══════════════════════════════════════════════════════════════════
//                    AC3HB CONFIGURATION & TIMEOUT
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Configuración del algoritmo AC3HB con límites
 */
typedef struct {
    int max_depth;           // Límite de profundidad recursiva
    int max_time_seconds;    // Timeout en segundos
    bool use_iterative;      // Usar iterative deepening (25×25)
} AC3HBConfig;

/**
 * @brief Estructura para control de timeout
 */
typedef struct {
    time_t start_time;
    int max_seconds;
    bool timeout_triggered;
} AC3HBTimeout;

// Variable global para timeout (thread-safe en contexto single-threaded)
static AC3HBTimeout g_timeout = {0, 0, false};

/**
 * @brief Obtiene configuración óptima según tamaño de tablero
 */
static AC3HBConfig ac3hb_get_config(int board_size) {
    AC3HBConfig config;
    
    if (board_size <= 9) {
        config.max_depth = 1000;
        config.max_time_seconds = 10;
        config.use_iterative = false;
    } else if (board_size <= 16) {
        config.max_depth = 300;
        config.max_time_seconds = 15;
        config.use_iterative = false;
    } else {
        // Tableros >= 25×25: iterative deepening
        config.max_depth = 150;
        config.max_time_seconds = 60;
        config.use_iterative = true;
    }
    
    return config;
}

/**
 * @brief Verifica si se excedió el timeout
 */
static bool ac3hb_check_timeout(void) {
    if (g_timeout.max_seconds <= 0) {
        return false;
    }
    
    time_t now = time(NULL);
    double elapsed = difftime(now, g_timeout.start_time);
    
    if (elapsed >= g_timeout.max_seconds) {
        g_timeout.timeout_triggered = true;
        return true;
    }
    
    return false;
}

// ═══════════════════════════════════════════════════════════════════
//                    FORWARD DECLARATIONS
// ═══════════════════════════════════════════════════════════════════

// Función recursiva principal del ciclo AC3HB (MODIFICADA: agrega max_depth)
static bool ac3hb_solve_recursive(ConstraintNetwork *net,
                                  SudokuBoard *board,
                                  SubgridDensityCache *density_cache,
                                  const HeuristicConfig *config,
                                  AC3HBStats *stats,
                                  int depth,
                                  int max_depth);

// Función de iterative deepening para tableros grandes
static bool ac3hb_solve_iterative(ConstraintNetwork *net,
                                  SudokuBoard *board,
                                  SubgridDensityCache *density_cache,
                                  const HeuristicConfig *config,
                                  AC3HBStats *stats,
                                  const AC3HBConfig *ac3hb_config);

// ═══════════════════════════════════════════════════════════════════
//                    DOMAIN BACKUP/RESTORE
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Backup structure for domain restoration during backtracking
 * 
 * Cuando hacemos backtrack, necesitamos restaurar los dominios a su
 * estado anterior a la asignación. Esta estructura guarda el estado.
 */
typedef struct {
    int row;
    int col;
    Domain saved_domain;
} DomainBackup;

/**
 * @brief Backup all domains affected by an assignment
 * 
 * Guarda el dominio de la celda asignada y todos sus vecinos
 * para poder restaurar en caso de backtrack.
 * 
 * @param net Constraint network
 * @param row Row of assigned cell
 * @param col Column of assigned cell
 * @param[out] backups Array to store backups
 * @param[out] count Number of backups stored
 * @return true if successful
 */
static bool backup_affected_domains(const ConstraintNetwork *net,
                                    int row, int col,
                                    DomainBackup **backups,
                                    int *count) {
    int neighbor_count;
    const SudokuPosition *neighbors = 
        constraint_network_get_neighbors(net, row, col, &neighbor_count);
    
    // Necesitamos espacio para la celda + todos sus vecinos
    int total = 1 + neighbor_count;
    
    *backups = (DomainBackup*)malloc(total * sizeof(DomainBackup));
    if (!*backups) {
        return false;
    }
    
    // Backup de la celda asignada
    (*backups)[0].row = row;
    (*backups)[0].col = col;
    (*backups)[0].saved_domain = constraint_network_get_domain(net, row, col);
    
    // Backup de vecinos
    for (int i = 0; i < neighbor_count; i++) {
        (*backups)[i + 1].row = neighbors[i].row;
        (*backups)[i + 1].col = neighbors[i].col;
        (*backups)[i + 1].saved_domain = 
            constraint_network_get_domain(net, neighbors[i].row, neighbors[i].col);
    }
    
    *count = total;
    return true;
}

/**
 * @brief Restore domains from backup
 * 
 * Restaura todos los dominios guardados durante backtrack.
 */
static void restore_domains(ConstraintNetwork *net,
                           DomainBackup *backups,
                           int count) {
    for (int i = 0; i < count; i++) {
        // Restaurar dominio completo
        constraint_network_restore_domain(net, backups[i].row, backups[i].col);
        
        // Aplicar el dominio guardado (quitar valores que no estaban)
        int board_size = constraint_network_get_board_size(net);
        for (int v = 1; v <= board_size; v++) {
            bool was_present = (backups[i].saved_domain.bits & (1U << (v - 1))) != 0;
            if (!was_present) {
                constraint_network_remove_value(net, backups[i].row, backups[i].col, v);
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
//                    BOARD CONSISTENCY CHECK
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Check if all cells have been assigned (board complete)
 * 
 * Un tablero está completo cuando todas las celdas tienen
 * dominio de tamaño 1 (exactamente un valor asignado).
 */
static bool is_board_complete(const ConstraintNetwork *net) {
    int board_size = constraint_network_get_board_size(net);
    
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            if (constraint_network_domain_size(net, row, col) != 1) {
                return false;
            }
        }
    }
    
    return true;
}

/**
 * @brief Check for any empty domains (dead-end detection)
 * 
 * Si alguna celda tiene dominio vacío, el estado actual es inconsistente
 * y debemos hacer backtrack.
 */
static bool has_empty_domain(const ConstraintNetwork *net) {
    int board_size = constraint_network_get_board_size(net);
    
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            if (constraint_network_domain_empty(net, row, col)) {
                return true;
            }
        }
    }
    
    return false;
}

/**
 * @brief Extract single value from singleton domain
 * 
 * Para celdas con dominio tamaño 1, obtiene el único valor.
 */
static int get_singleton_value(const ConstraintNetwork *net, int row, int col) {
    int board_size = constraint_network_get_board_size(net);
    
    for (int v = 1; v <= board_size; v++) {
        if (constraint_network_has_value(net, row, col, v)) {
            return v;
        }
    }
    
    return 0;  // Error: no value found
}

// ═══════════════════════════════════════════════════════════════════
//                    CORE AC3HB ALGORITHM (MODIFICADO)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Recursive AC3HB solving function (MODIFICADO: con límites)
 * 
 * CAMBIOS v3.0.1:
 * - Agregado parámetro max_depth
 * - Check de profundidad al inicio
 * - Check de timeout periódico usando depth (NO variable estática)
 * - Early exit en caso de límites excedidos
 * 
 * ALGORITMO PRINCIPAL:
 * 
 * 0. VERIFICACIÓN DE LÍMITES (NUEVO):
 *    - Si depth >= max_depth → return false
 *    - Cada 100 niveles: check timeout
 * 
 * 1. PROPAGACIÓN AC3:
 *    - Aplicar AC3 para reducir dominios
 *    - Si detecta inconsistencia → return false
 * 
 * 2. VERIFICACIÓN DE COMPLETITUD:
 *    - Si todas las celdas tienen dominio singleton → ¡ÉXITO!
 *    - Copiar valores al board y return true
 * 
 * 3. DETECCIÓN DE DEAD-END:
 *    - Si existe dominio vacío → return false
 * 
 * 4. SELECCIÓN DE CELDA:
 *    - Usar heurísticas (MRV + Densidad + Grado)
 *    - Obtener celda óptima para asignar
 * 
 * 5. ORDENAR CANDIDATOS:
 *    - Usar LCV (Least Constraining Value)
 *    - Probar valores que eliminan menos posibilidades primero
 * 
 * 6. BRANCHING:
 *    - Para cada candidato:
 *      a. Backup dominios afectados
 *      b. Asignar valor
 *      c. Propagar con AC3
 *      d. Recursión (pasando depth + 1 y max_depth)
 *      e. Si éxito → return true
 *      f. Si falla → restore dominios, probar siguiente
 * 
 * 7. Si ningún candidato funcionó → return false (backtrack)
 * 
 * @param net Constraint network
 * @param board Board to fill with solution
 * @param density_cache Subgrid density cache
 * @param config Heuristic configuration
 * @param stats Statistics output
 * @param depth Current recursion depth
 * @param max_depth Maximum recursion depth (NUEVO)
 * @return true if solution found, false otherwise
 */
static bool ac3hb_solve_recursive(ConstraintNetwork *net,
                                  SudokuBoard *board,
                                  SubgridDensityCache *density_cache,
                                  const HeuristicConfig *config,
                                  AC3HBStats *stats,
                                  int depth,
                                  int max_depth) {
    // ═══════════════════════════════════════════════════════════════
    // PASO 0: Verificación de límites (NUEVO)
    // ═══════════════════════════════════════════════════════════════
    
    // Check de profundidad
    if (depth >= max_depth) {
        return false;
    }
    
    // Check de timeout (cada 100 niveles usando depth directamente)
    // FIX CRÍTICO: Eliminada variable estática call_count que causaba
    // acumulación entre múltiples ejecuciones del test
    if (depth % 100 == 0) {
        if (ac3hb_check_timeout()) {
            return false;
        }
    }
    
    // Update max depth statistic
    if (stats && depth > stats->max_depth) {
        stats->max_depth = depth;
    }
    
    // ═══════════════════════════════════════════════════════════════
    // PASO 1: Propagación AC3 (enforce arc consistency)
    // ═══════════════════════════════════════════════════════════════
    
    AC3Statistics ac3_stats;
    bool consistent = ac3_enforce_consistency(net, &ac3_stats);
    
    if (stats) {
        stats->ac3_calls++;
        stats->values_eliminated += ac3_stats.values_removed;
    }
    
    if (!consistent) {
        // AC3 detectó inconsistencia → dead-end
        return false;
    }
    
    // ═══════════════════════════════════════════════════════════════
    // PASO 2: Verificar si tablero está completo
    // ═══════════════════════════════════════════════════════════════
    
    if (is_board_complete(net)) {
        // ¡ÉXITO! Copiar solución al board
        int board_size = constraint_network_get_board_size(net);
        
        for (int row = 0; row < board_size; row++) {
            for (int col = 0; col < board_size; col++) {
                int value = get_singleton_value(net, row, col);
                sudoku_board_set_cell(board, row, col, value);
            }
        }
        
        sudoku_board_update_stats(board);
        return true;
    }
    
    // ═══════════════════════════════════════════════════════════════
    // PASO 3: Detección de dead-end (dominio vacío)
    // ═══════════════════════════════════════════════════════════════
    
    if (has_empty_domain(net)) {
        return false;
    }
    
    // ═══════════════════════════════════════════════════════════════
    // PASO 4: Selección de celda óptima
    // ═══════════════════════════════════════════════════════════════
    
    SudokuPosition selected;
    CellScore cell_score;
    
    if (!select_optimal_cell(net, density_cache, config, &selected, &cell_score)) {
        // No hay celdas vacías pero tampoco está completo → error
        return false;
    }
    
    // ═══════════════════════════════════════════════════════════════
    // PASO 5: Ordenar candidatos por impacto (LCV)
    // ═══════════════════════════════════════════════════════════════
    
    int *candidates = NULL;
    int num_candidates = 0;
    
    // Usar LCV si está habilitado, sino orden aleatorio
    if (!order_candidates_by_impact(net, selected.row, selected.col, 
                                    &candidates, &num_candidates)) {
        // Fallback a orden aleatorio
        if (!get_candidates_random(net, selected.row, selected.col,
                                   &candidates, &num_candidates)) {
            return false;
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    // PASO 6: Branching - probar cada candidato
    // ═══════════════════════════════════════════════════════════════
    
    int board_size = constraint_network_get_board_size(net);
    int subgrid_size = 1;
    while (subgrid_size * subgrid_size < board_size) {
        subgrid_size++;
    }
    
    for (int i = 0; i < num_candidates; i++) {
        int value = candidates[i];
        
        // 6a. Backup dominios afectados
        DomainBackup *backups = NULL;
        int backup_count = 0;
        
        if (!backup_affected_domains(net, selected.row, selected.col,
                                     &backups, &backup_count)) {
            free(candidates);
            return false;
        }
        
        // 6b. Asignar valor
        constraint_network_assign_value(net, selected.row, selected.col, value);
        
        if (stats) {
            stats->cells_assigned++;
        }
        
        // Actualizar density cache
        if (density_cache) {
            subgrid_density_cache_increment(density_cache, 
                                           selected.row, selected.col, 
                                           subgrid_size);
        }
        
        // 6c. Propagar con AC3 incremental
        AC3Statistics prop_stats;
        bool prop_consistent = ac3_propagate_from(net, selected.row, selected.col,
                                                  value, &prop_stats);
        
        if (stats) {
            stats->ac3_calls++;
            stats->values_eliminated += prop_stats.values_removed;
        }
        
        // 6d. Recursión si consistente (MODIFICADO: pasar max_depth)
        if (prop_consistent) {
            if (ac3hb_solve_recursive(net, board, density_cache, config, 
                                      stats, depth + 1, max_depth)) {
                // ¡ÉXITO! Limpiar y retornar
                free(backups);
                free(candidates);
                return true;
            }
        }
        
        // 6e. Backtrack - restaurar dominios
        if (stats) {
            stats->total_backtracks++;
        }
        
        restore_domains(net, backups, backup_count);
        free(backups);
        
        // Restaurar density cache
        if (density_cache) {
            subgrid_density_cache_decrement(density_cache,
                                           selected.row, selected.col,
                                           subgrid_size);
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    // PASO 7: Ningún candidato funcionó → backtrack
    // ═══════════════════════════════════════════════════════════════
    
    free(candidates);
    return false;
}

// ═══════════════════════════════════════════════════════════════════
//                    ITERATIVE DEEPENING (NUEVO)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Iterative deepening para tableros grandes
 * 
 * Estrategia para evitar stack overflow y loops infinitos:
 * 1. Intentar con depth_limit = 20, 40, 60, ..., max_depth
 * 2. Para cada límite, ejecutar solve con ese límite
 * 3. Si encuentra solución, retornar
 * 4. Si timeout, abortar
 * 
 * @return true si encuentra solución
 */
static bool ac3hb_solve_iterative(ConstraintNetwork *net,
                                  SudokuBoard *board,
                                  SubgridDensityCache *density_cache,
                                  const HeuristicConfig *config,
                                  AC3HBStats *stats,
                                  const AC3HBConfig *ac3hb_config) {
    // Intentar con profundidades incrementales
    for (int depth_limit = 20; 
         depth_limit <= ac3hb_config->max_depth; 
         depth_limit += 20) {
        
        bool success = ac3hb_solve_recursive(net, board, density_cache,
                                             config, stats, 0, depth_limit);
        
        if (success) {
            return true;
        }
        
        // Check timeout
        if (g_timeout.timeout_triggered) {
            return false;
        }
        
        // Reset stats para siguiente intento
        if (stats) {
            stats->total_backtracks = 0;
            stats->cells_assigned = 0;
        }
    }
    
    return false;
}

// ═══════════════════════════════════════════════════════════════════
//                    PUBLIC API (MODIFICADO)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Complete a partially filled board using AC3HB algorithm
 * 
 * MODIFICADO v3.0.1:
 * - Configura límites según tamaño de tablero
 * - Usa iterative deepening para tableros >= 25×25
 * - Maneja timeout apropiadamente
 * 
 * Esta es la función principal que reemplaza al backtracking simple
 * en generator.c cuando use_ac3 está habilitado.
 * 
 * FLUJO:
 * 1. Obtener configuración según board_size
 * 2. Crear ConstraintNetwork desde el board actual
 * 3. Crear density cache
 * 4. Configurar heurísticas
 * 5. Configurar timeout
 * 6. Llamar a solve (recursivo o iterativo)
 * 7. Limpiar y retornar resultado
 * 
 * @param board Board with some cells filled (modified in place)
 * @return true if board completed successfully, false otherwise
 */
bool sudoku_complete_ac3hb(SudokuBoard *board) {
    assert(board != NULL);
    
    int board_size = sudoku_board_get_board_size(board);
    AC3HBConfig ac3hb_config = ac3hb_get_config(board_size);
    
    // Crear constraint network desde el estado actual del board
    ConstraintNetwork *net = constraint_network_create(board);
    if (!net) {
        return false;
    }
    
    // Crear density cache
    SubgridDensityCache *density_cache = subgrid_density_cache_create(net);
    
    // Configuración de heurísticas (usar default)
    HeuristicConfig config = heuristic_config_default();
    
    // Estadísticas
    AC3HBStats stats = {0};
    
    // Configurar timeout
    g_timeout.start_time = time(NULL);
    g_timeout.max_seconds = ac3hb_config.max_time_seconds;
    g_timeout.timeout_triggered = false;
    
    // Resolver (elegir estrategia según configuración)
    bool success;
    if (ac3hb_config.use_iterative) {
        // Tableros grandes: iterative deepening
        success = ac3hb_solve_iterative(net, board, density_cache,
                                        &config, &stats, &ac3hb_config);
    } else {
        // Tableros normales: recursión directa
        success = ac3hb_solve_recursive(net, board, density_cache, 
                                        &config, &stats, 0, 
                                        ac3hb_config.max_depth);
    }
    
    // Limpiar
    subgrid_density_cache_destroy(density_cache);
    constraint_network_destroy(net);
    
    return success;
}

/**
 * @brief Complete board with AC3HB and return statistics
 * 
 * Versión extendida que retorna estadísticas detalladas.
 * 
 * @param board Board to complete
 * @param[out] stats Statistics output (can be NULL)
 * @return true if successful
 */
bool sudoku_complete_ac3hb_ex(SudokuBoard *board, AC3HBStats *stats) {
    assert(board != NULL);
    
    int board_size = sudoku_board_get_board_size(board);
    AC3HBConfig ac3hb_config = ac3hb_get_config(board_size);
    
    ConstraintNetwork *net = constraint_network_create(board);
    if (!net) {
        return false;
    }
    
    SubgridDensityCache *density_cache = subgrid_density_cache_create(net);
    HeuristicConfig config = heuristic_config_default();
    
    AC3HBStats local_stats = {0};
    
    g_timeout.start_time = time(NULL);
    g_timeout.max_seconds = ac3hb_config.max_time_seconds;
    g_timeout.timeout_triggered = false;
    
    bool success;
    if (ac3hb_config.use_iterative) {
        success = ac3hb_solve_iterative(net, board, density_cache,
                                        &config, 
                                        stats ? stats : &local_stats,
                                        &ac3hb_config);
    } else {
        success = ac3hb_solve_recursive(net, board, density_cache,
                                         &config, 
                                         stats ? stats : &local_stats, 
                                         0, ac3hb_config.max_depth);
    }
    
    subgrid_density_cache_destroy(density_cache);
    constraint_network_destroy(net);
    
    return success;
}

/**
 * @brief Complete board with custom heuristic configuration
 * 
 * Permite personalizar pesos de heurísticas para testing/benchmarking.
 * 
 * @param board Board to complete
 * @param config Custom heuristic configuration
 * @param[out] stats Statistics output (can be NULL)
 * @return true if successful
 */
bool sudoku_complete_ac3hb_config(SudokuBoard *board,
                                  const HeuristicConfig *config,
                                  AC3HBStats *stats) {
    assert(board != NULL);
    
    int board_size = sudoku_board_get_board_size(board);
    AC3HBConfig ac3hb_config = ac3hb_get_config(board_size);
    
    ConstraintNetwork *net = constraint_network_create(board);
    if (!net) {
        return false;
    }
    
    SubgridDensityCache *density_cache = subgrid_density_cache_create(net);
    
    // Usar configuración proporcionada o default
    HeuristicConfig local_config;
    if (!config) {
        local_config = heuristic_config_default();
        config = &local_config;
    }
    
    AC3HBStats local_stats = {0};
    
    g_timeout.start_time = time(NULL);
    g_timeout.max_seconds = ac3hb_config.max_time_seconds;
    g_timeout.timeout_triggered = false;
    
    bool success;
    if (ac3hb_config.use_iterative) {
        success = ac3hb_solve_iterative(net, board, density_cache,
                                        config,
                                        stats ? stats : &local_stats,
                                        &ac3hb_config);
    } else {
        success = ac3hb_solve_recursive(net, board, density_cache,
                                         config,
                                         stats ? stats : &local_stats,
                                         0, ac3hb_config.max_depth);
    }
    
    subgrid_density_cache_destroy(density_cache);
    constraint_network_destroy(net);
    
    return success;
}

// ═══════════════════════════════════════════════════════════════════
//                    DEBUGGING
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Print AC3HB statistics
 */
void print_ac3hb_stats(const AC3HBStats *stats) {
    if (!stats) {
        printf("AC3HBStats: NULL\n");
        return;
    }
    
    printf("═══════════════════════════════════════════════════════\n");
    printf("              AC3HB STATISTICS\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("  Total backtracks:    %d\n", stats->total_backtracks);
    printf("  AC3 calls:           %d\n", stats->ac3_calls);
    printf("  Cells assigned:      %d\n", stats->cells_assigned);
    printf("  Values eliminated:   %d\n", stats->values_eliminated);
    printf("  Max recursion depth: %d\n", stats->max_depth);
    printf("  Total time:          %.2f ms\n", stats->time_ms);
    printf("═══════════════════════════════════════════════════════\n");
}

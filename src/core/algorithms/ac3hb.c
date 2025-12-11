/**
 * @file ac3hb.c  
 * @brief AC3HB v3.1.3 - TIMEOUT FIX DEFINITIVO
 * @date 2025-12-10
 * 
 * ✅✅✅ FIX DEFINITIVO v3.1.3:
 * 
 * PROBLEMA RESUELTO:
 * - El timeout se detectaba pero NO abortaba inmediatamente
 * - ac3hb_check_timeout() retornaba true pero el código continuaba
 * - Necesitamos verificar g_timeout.timeout_triggered ANTES de operaciones costosas
 * 
 * SOLUCIÓN:
 * 1. Verificar flag ANTES de AC3 propagation (no después)
 * 2. Macro ABORT_IF_TIMEOUT para verificación consistente
 * 3. Early return inmediato cuando timeout detectado
 */

#include "sudoku/algorithms/ac3.h"
#include "sudoku/algorithms/ac3hb.h"
#include "sudoku/algorithms/network.h"
#include "sudoku/algorithms/heuristics.h"
#include "sudoku/algorithms/forced_cells.h"
#include "sudoku/core/board.h"
#include "sudoku/core/types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>

// ═══════════════════════════════════════════════════════════════════
//                    TIMEOUT SYSTEM v3.1.3
// ═══════════════════════════════════════════════════════════════════

typedef struct {
    int max_depth;
    int max_time_seconds;
    bool use_iterative;
} AC3HBConfig;

typedef struct {
    time_t start_time;
    int max_seconds;
    bool timeout_triggered;
    unsigned long operation_count;
} AC3HBTimeout;

static AC3HBTimeout g_timeout = {0, 0, false, 0};

/**
 * ✅ v3.1.3: Macro para abort inmediato
 */
#define ABORT_IF_TIMEOUT() \
    do { \
        if (g_timeout.timeout_triggered) { \
            return false; \
        } \
    } while(0)

/**
 * ✅ v3.1.3: Verificación simplificada
 */
static bool ac3hb_check_timeout(void) {
    if (g_timeout.max_seconds <= 0) {
        return false;
    }
    
    // Incrementar siempre
    g_timeout.operation_count++;
    
    // Solo verificar tiempo cada 1000 ops (costoso)
    if (g_timeout.operation_count % 1000 != 0) {
        return g_timeout.timeout_triggered;  // Retornar estado actual
    }
    
    time_t now = time(NULL);
    double elapsed = difftime(now, g_timeout.start_time);
    
    if (elapsed >= g_timeout.max_seconds) {
        if (!g_timeout.timeout_triggered) {
            g_timeout.timeout_triggered = true;
            fprintf(stderr, "\n[AC3HB] ⏱️ TIMEOUT after %.0fs (op #%lu) - ABORTING\n",
                    elapsed, g_timeout.operation_count);
        }
        return true;
    }
    
    return false;
}

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
        config.max_depth = 150;
        config.max_time_seconds = 60;  // 60s para 25×25
        config.use_iterative = true;
    }
    
    return config;
}

// ═══════════════════════════════════════════════════════════════════
//                    FORWARD DECLARATIONS
// ═══════════════════════════════════════════════════════════════════

static bool ac3hb_solve_recursive(ConstraintNetwork *net,
                                  SudokuBoard *board,
                                  SubgridDensityCache *density_cache,
                                  const HeuristicConfig *config,
                                  AC3HBStats *stats,
                                  int depth,
                                  int max_depth,
                                  ForcedCellsRegistry *forced_registry);

static bool ac3hb_solve_iterative(ConstraintNetwork *net,
                                  SudokuBoard *board,
                                  SubgridDensityCache *density_cache,
                                  const HeuristicConfig *config,
                                  AC3HBStats *stats,
                                  const AC3HBConfig *ac3hb_config,
                                  ForcedCellsRegistry *forced_registry);

// ═══════════════════════════════════════════════════════════════════
//                    DOMAIN BACKUP/RESTORE
// ═══════════════════════════════════════════════════════════════════

typedef struct {
    int row;
    int col;
    Domain saved_domain;
} DomainBackup;

static bool backup_affected_domains(const ConstraintNetwork *net,
                                    int row, int col,
                                    DomainBackup **backups,
                                    int *count) {
    int neighbor_count;
    const SudokuPosition *neighbors = 
        constraint_network_get_neighbors(net, row, col, &neighbor_count);
    
    int total = 1 + neighbor_count;
    
    *backups = (DomainBackup*)malloc(total * sizeof(DomainBackup));
    if (!*backups) {
        return false;
    }
    
    (*backups)[0].row = row;
    (*backups)[0].col = col;
    (*backups)[0].saved_domain = constraint_network_get_domain(net, row, col);
    
    for (int i = 0; i < neighbor_count; i++) {
        (*backups)[i + 1].row = neighbors[i].row;
        (*backups)[i + 1].col = neighbors[i].col;
        (*backups)[i + 1].saved_domain = 
            constraint_network_get_domain(net, neighbors[i].row, neighbors[i].col);
    }
    
    *count = total;
    return true;
}

static void restore_domains(ConstraintNetwork *net,
                           DomainBackup *backups,
                           int count) {
    for (int i = 0; i < count; i++) {
        constraint_network_restore_domain(net, backups[i].row, backups[i].col);
        
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

static int get_singleton_value(const ConstraintNetwork *net, int row, int col) {
    int board_size = constraint_network_get_board_size(net);
    
    for (int v = 1; v <= board_size; v++) {
        if (constraint_network_has_value(net, row, col, v)) {
            return v;
        }
    }
    
    return 0;
}

// ═══════════════════════════════════════════════════════════════════
//                    FORCED CELLS DETECTION
// ═══════════════════════════════════════════════════════════════════

static void register_forced_cell_if_applicable(
    const ConstraintNetwork *net,
    ForcedCellsRegistry *forced_registry,
    int row, int col) 
{
    if (!forced_registry) {
        return;
    }
    
    if (constraint_network_domain_size(net, row, col) == 1) {
        int value = get_singleton_value(net, row, col);
        if (value > 0) {
            forced_cells_register(forced_registry, row, col, value, 
                      FORCED_PROPAGATED, 0);        
        }
    }
}

static void detect_and_register_forced_cells(
    const ConstraintNetwork *net,
    ForcedCellsRegistry *forced_registry)
{
    if (!forced_registry) {
        return;
    }
    
    int board_size = constraint_network_get_board_size(net);
    
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            register_forced_cell_if_applicable(net, forced_registry, row, col);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
//                    CORE AC3HB ALGORITHM v3.1.3
// ═══════════════════════════════════════════════════════════════════

/**
 * ✅✅✅ v3.1.3: ABORT INMEDIATO cuando timeout detectado
 */
static bool ac3hb_solve_recursive(ConstraintNetwork *net,
                                  SudokuBoard *board,
                                  SubgridDensityCache *density_cache,
                                  const HeuristicConfig *config,
                                  AC3HBStats *stats,
                                  int depth,
                                  int max_depth,
                                  ForcedCellsRegistry *forced_registry) {
    
    // ✅ CRÍTICO: Verificar flag ANTES de cualquier operación
    ABORT_IF_TIMEOUT();
    
    if (depth >= max_depth) {
        return false;
    }
    
    // Check timeout (puede setear el flag)
    if (ac3hb_check_timeout()) {
        return false;
    }
    
    if (stats && depth > stats->max_depth) {
        stats->max_depth = depth;
    }
    
    // ═══════════════════════════════════════════════════════════════
    // PASO 1: Propagación AC3
    // ═══════════════════════════════════════════════════════════════
    
    // ✅ Verificar ANTES de operación costosa
    ABORT_IF_TIMEOUT();
    
    AC3Statistics ac3_stats;
    bool consistent = ac3_enforce_consistency(net, &ac3_stats);
    
    if (stats) {
        stats->ac3_calls++;
        stats->values_eliminated += ac3_stats.values_removed;
    }
    
    // ✅ Verificar DESPUÉS de operación costosa
    ABORT_IF_TIMEOUT();
    
    if (!consistent) {
        return false;
    }
    
    detect_and_register_forced_cells(net, forced_registry);
    
    // ═══════════════════════════════════════════════════════════════
    // PASO 2: Verificar si completo
    // ═══════════════════════════════════════════════════════════════
    
    if (is_board_complete(net)) {
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
    
    if (has_empty_domain(net)) {
        return false;
    }
    
    // ═══════════════════════════════════════════════════════════════
    // PASO 3: Selección de celda
    // ═══════════════════════════════════════════════════════════════
    
    ABORT_IF_TIMEOUT();
    
    SudokuPosition selected;
    CellScore cell_score;
    
    if (!select_optimal_cell(net, density_cache, config, &selected, &cell_score)) {
        return false;
    }
    
    ABORT_IF_TIMEOUT();
    
    // ═══════════════════════════════════════════════════════════════
    // PASO 4: Ordenar candidatos
    // ═══════════════════════════════════════════════════════════════
    
    int *candidates = NULL;
    int num_candidates = 0;
    
    if (!order_candidates_by_impact(net, selected.row, selected.col, 
                                    &candidates, &num_candidates)) {
        if (!get_candidates_random(net, selected.row, selected.col,
                                   &candidates, &num_candidates)) {
            return false;
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    // PASO 5: Loop de candidatos
    // ═══════════════════════════════════════════════════════════════
    
    int board_size = constraint_network_get_board_size(net);
    int subgrid_size = 1;
    while (subgrid_size * subgrid_size < board_size) {
        subgrid_size++;
    }
    
    for (int i = 0; i < num_candidates; i++) {
        // ✅ Check en cada iteración
        if (g_timeout.timeout_triggered) {
            free(candidates);
            return false;
        }
        
        int value = candidates[i];
        
        DomainBackup *backups = NULL;
        int backup_count = 0;
        
        if (!backup_affected_domains(net, selected.row, selected.col,
                                     &backups, &backup_count)) {
            free(candidates);
            return false;
        }
        
        constraint_network_assign_value(net, selected.row, selected.col, value);
        
        if (stats) {
            stats->cells_assigned++;
        }
        
        if (density_cache) {
            subgrid_density_cache_increment(density_cache, 
                                           selected.row, selected.col, 
                                           subgrid_size);
        }
        
        // ✅ Check antes de propagación
        if (g_timeout.timeout_triggered) {
            restore_domains(net, backups, backup_count);
            free(backups);
            free(candidates);
            return false;
        }
        
        AC3Statistics prop_stats;
        bool prop_consistent = ac3_propagate_from(net, selected.row, selected.col,
                                                  value, &prop_stats);
        
        if (stats) {
            stats->ac3_calls++;
            stats->values_eliminated += prop_stats.values_removed;
        }
        
        // ✅ Check después de propagación  
        if (g_timeout.timeout_triggered) {
            restore_domains(net, backups, backup_count);
            free(backups);
            free(candidates);
            return false;
        }
        
        if (prop_consistent) {
            detect_and_register_forced_cells(net, forced_registry);
        }
        
        if (prop_consistent) {
            if (ac3hb_solve_recursive(net, board, density_cache, config, 
                                      stats, depth + 1, max_depth, forced_registry)) {
                free(backups);
                free(candidates);
                return true;
            }
        }
        
        if (stats) {
            stats->total_backtracks++;
        }
        
        restore_domains(net, backups, backup_count);
        free(backups);
        
        if (density_cache) {
            subgrid_density_cache_decrement(density_cache,
                                           selected.row, selected.col,
                                           subgrid_size);
        }
    }
    
    free(candidates);
    return false;
}

// ═══════════════════════════════════════════════════════════════════
//                    ITERATIVE DEEPENING v3.1.3
// ═══════════════════════════════════════════════════════════════════

static bool ac3hb_solve_iterative(ConstraintNetwork *net,
                                  SudokuBoard *board,
                                  SubgridDensityCache *density_cache,
                                  const HeuristicConfig *config,
                                  AC3HBStats *stats,
                                  const AC3HBConfig *ac3hb_config,
                                  ForcedCellsRegistry *forced_registry) {
    
    for (int depth_limit = 20; 
         depth_limit <= ac3hb_config->max_depth; 
         depth_limit += 20) {
        
        // ✅ Check al inicio de cada iteración
        if (g_timeout.timeout_triggered) {
            return false;
        }
        
        bool success = ac3hb_solve_recursive(net, board, density_cache,
                                             config, stats, 0, depth_limit,
                                             forced_registry);
        
        if (success) {
            return true;
        }
        
        // ✅ Abort si timeout detectado
        if (g_timeout.timeout_triggered) {
            return false;
        }
        
        if (stats) {
            stats->total_backtracks = 0;
            stats->cells_assigned = 0;
        }
    }
    
    return false;
}

// ═══════════════════════════════════════════════════════════════════
//                    PUBLIC API v3.1.3
// ═══════════════════════════════════════════════════════════════════

bool sudoku_complete_ac3hb(SudokuBoard *board) {
    assert(board != NULL);
    
    int board_size = sudoku_board_get_board_size(board);
    AC3HBConfig ac3hb_config = ac3hb_get_config(board_size);

    ForcedCellsRegistry *forced_registry = 
        forced_cells_registry_create(board_size);
    
    if (!forced_registry) {
        fprintf(stderr, "Warning: Failed to create forced cells registry\n");
    }
    
    ConstraintNetwork *net = constraint_network_create(board);
    if (!net) {
        if (forced_registry) {
            forced_cells_registry_destroy(forced_registry);
        }
        return false;
    }
    
    SubgridDensityCache *density_cache = subgrid_density_cache_create(net);
    HeuristicConfig config = heuristic_config_default();
    AC3HBStats stats = {0};
    
    // ✅ Reset completo del timeout
    g_timeout.start_time = time(NULL);
    g_timeout.max_seconds = ac3hb_config.max_time_seconds;
    g_timeout.timeout_triggered = false;
    g_timeout.operation_count = 0;
    
    fprintf(stderr, "[AC3HB] Starting %dx%d (timeout: %ds)\n",
            board_size, board_size, ac3hb_config.max_time_seconds);
    
    bool success;
    if (ac3hb_config.use_iterative) {
        success = ac3hb_solve_iterative(net, board, density_cache,
                                        &config, &stats, &ac3hb_config,
                                        forced_registry);
    } else {
        success = ac3hb_solve_recursive(net, board, density_cache, 
                                        &config, &stats, 0, 
                                        ac3hb_config.max_depth,
                                        forced_registry);
    }
    
    double elapsed = difftime(time(NULL), g_timeout.start_time);
    
    if (g_timeout.timeout_triggered) {
        fprintf(stderr, "[AC3HB] ❌ Timeout (%.1fs, %lu ops)\n",
                elapsed, g_timeout.operation_count);
    } else if (success) {
        fprintf(stderr, "[AC3HB] ✅ Success (%.1fs, %lu ops)\n",
                elapsed, g_timeout.operation_count);
    }
    
    if (success) {
        sudoku_board_set_forced_cells(board, forced_registry);    
    } else {
        if (forced_registry) {
            forced_cells_registry_destroy(forced_registry);
        }
    }
    
    subgrid_density_cache_destroy(density_cache);
    constraint_network_destroy(net);
    
    return success;
}

bool sudoku_complete_ac3hb_ex(SudokuBoard *board, AC3HBStats *stats) {
    assert(board != NULL);
    
    int board_size = sudoku_board_get_board_size(board);
    AC3HBConfig ac3hb_config = ac3hb_get_config(board_size);
    
    ForcedCellsRegistry *forced_registry = 
        forced_cells_registry_create(board_size);
    
    ConstraintNetwork *net = constraint_network_create(board);
    if (!net) {
        if (forced_registry) {
            forced_cells_registry_destroy(forced_registry);
        }
        return false;
    }
    
    SubgridDensityCache *density_cache = subgrid_density_cache_create(net);
    HeuristicConfig config = heuristic_config_default();
    
    AC3HBStats local_stats = {0};
    
    g_timeout.start_time = time(NULL);
    g_timeout.max_seconds = ac3hb_config.max_time_seconds;
    g_timeout.timeout_triggered = false;
    g_timeout.operation_count = 0;
    
    bool success;
    if (ac3hb_config.use_iterative) {
        success = ac3hb_solve_iterative(net, board, density_cache,
                                        &config, 
                                        stats ? stats : &local_stats,
                                        &ac3hb_config,
                                        forced_registry);
    } else {
        success = ac3hb_solve_recursive(net, board, density_cache,
                                         &config, 
                                         stats ? stats : &local_stats, 
                                         0, ac3hb_config.max_depth,
                                         forced_registry);
    }
    
    if (success) {
        sudoku_board_set_forced_cells(board, forced_registry);
    } else {
        if (forced_registry) {
            forced_cells_registry_destroy(forced_registry);
        }
    }
    
    subgrid_density_cache_destroy(density_cache);
    constraint_network_destroy(net);
    
    return success;
}

bool sudoku_complete_ac3hb_config(SudokuBoard *board,
                                  const HeuristicConfig *config,
                                  AC3HBStats *stats) {
    assert(board != NULL);
    
    int board_size = sudoku_board_get_board_size(board);
    AC3HBConfig ac3hb_config = ac3hb_get_config(board_size);
    
    ForcedCellsRegistry *forced_registry = 
        forced_cells_registry_create(board_size);
    
    ConstraintNetwork *net = constraint_network_create(board);
    if (!net) {
        if (forced_registry) {
            forced_cells_registry_destroy(forced_registry);
        }
        return false;
    }
    
    SubgridDensityCache *density_cache = subgrid_density_cache_create(net);
    
    HeuristicConfig local_config;
    if (!config) {
        local_config = heuristic_config_default();
        config = &local_config;
    }
    
    AC3HBStats local_stats = {0};
    
    g_timeout.start_time = time(NULL);
    g_timeout.max_seconds = ac3hb_config.max_time_seconds;
    g_timeout.timeout_triggered = false;
    g_timeout.operation_count = 0;
    
    bool success;
    if (ac3hb_config.use_iterative) {
        success = ac3hb_solve_iterative(net, board, density_cache,
                                        config,
                                        stats ? stats : &local_stats,
                                        &ac3hb_config,
                                        forced_registry);
    } else {
        success = ac3hb_solve_recursive(net, board, density_cache,
                                         config,
                                         stats ? stats : &local_stats,
                                         0, ac3hb_config.max_depth,
                                         forced_registry);
    }
    
    if (success) {
        sudoku_board_set_forced_cells(board, forced_registry);
    } else {
        if (forced_registry) {
            forced_cells_registry_destroy(forced_registry);
        }
    }
    
    subgrid_density_cache_destroy(density_cache);
    constraint_network_destroy(net);
    
    return success;
}

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

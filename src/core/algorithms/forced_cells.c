/**
 * @file forced_cells.c
 * @brief Implementation of Forced Cells Registry System
 * @author Gonzalo Ramírez
 * @date 2025-12-05
 * @version 3.1.0
 */

#include "sudoku/algorithms/forced_cells.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

// ═══════════════════════════════════════════════════════════════════
//                    INTERNAL STRUCTURES
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Internal structure for registry
 * 
 * Usa hash table simple para lookup O(1) de celdas.
 * Key: (row * board_size + col)
 * Value: ForcedCell
 */
struct ForcedCellsRegistry {
    ForcedCell *cells;          /**< Array dinámico de celdas */
    int capacity;               /**< Capacidad máxima (board_size²) */
    int count;                  /**< Cantidad actual de celdas registradas */
    int board_size;             /**< Tamaño del tablero */
    bool *occupied;             /**< Bitmap para fast lookup */
};

// ═══════════════════════════════════════════════════════════════════
//                    HELPER FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Calculate linear index from (row, col)
 */
static inline int cell_index(const ForcedCellsRegistry *reg, int row, int col) {
    return row * reg->board_size + col;
}

/**
 * @brief Find cell in registry
 * @return Index in cells array, or -1 if not found
 */
static int find_cell(const ForcedCellsRegistry *reg, int row, int col) {
    if (!reg || row < 0 || col < 0 || 
        row >= reg->board_size || col >= reg->board_size) {
        return -1;
    }
    
    int idx = cell_index(reg, row, col);
    
    if (!reg->occupied[idx]) {
        return -1;
    }
    
    // Linear search en cells array (podría optimizarse con hash map real)
    for (int i = 0; i < reg->count; i++) {
        if (reg->cells[i].position.row == row &&
            reg->cells[i].position.col == col) {
            return i;
        }
    }
    
    return -1;
}

// ═══════════════════════════════════════════════════════════════════
//                    REGISTRY MANAGEMENT
// ═══════════════════════════════════════════════════════════════════

ForcedCellsRegistry* forced_cells_registry_create(int board_size) {
    if (board_size <= 0 || board_size > 100) {
        return NULL;
    }
    
    ForcedCellsRegistry *reg = malloc(sizeof(ForcedCellsRegistry));
    if (!reg) {
        return NULL;
    }
    
    int total_cells = board_size * board_size;
    
    reg->cells = calloc(total_cells, sizeof(ForcedCell));
    reg->occupied = calloc(total_cells, sizeof(bool));
    
    if (!reg->cells || !reg->occupied) {
        free(reg->cells);
        free(reg->occupied);
        free(reg);
        return NULL;
    }
    
    reg->capacity = total_cells;
    reg->count = 0;
    reg->board_size = board_size;
    
    return reg;
}

void forced_cells_registry_destroy(ForcedCellsRegistry *registry) {
    if (!registry) return;
    
    free(registry->cells);
    free(registry->occupied);
    free(registry);
}

void forced_cells_registry_clear(ForcedCellsRegistry *registry) {
    if (!registry) return;
    
    memset(registry->cells, 0, registry->capacity * sizeof(ForcedCell));
    memset(registry->occupied, 0, registry->capacity * sizeof(bool));
    registry->count = 0;
}

bool forced_cells_registry_get_stats(const ForcedCellsRegistry *registry,
                                     ForcedCellsStats *stats) {
    if (!registry || !stats) {
        return false;
    }
    
    memset(stats, 0, sizeof(ForcedCellsStats));
    stats->total_forced = registry->count;
    
    for (int i = 0; i < registry->count; i++) {
        switch (registry->cells[i].type) {
            case FORCED_NAKED_SINGLE:
                stats->naked_singles++;
                break;
            case FORCED_HIDDEN_SINGLE:
                stats->hidden_singles++;
                break;
            case FORCED_PROPAGATED:
                stats->propagated++;
                break;
            case FORCED_BACKTRACKED:
                stats->backtracked++;
                break;
        }
    }
    
    return true;
}

// ═══════════════════════════════════════════════════════════════════
//                    CELL REGISTRATION
// ═══════════════════════════════════════════════════════════════════

bool forced_cells_register(ForcedCellsRegistry *registry,
                           int row, int col, int value,
                           ForcedCellType type, int generation_step) {
    if (!registry) {
        return false;
    }
    
    if (row < 0 || col < 0 || row >= registry->board_size || 
        col >= registry->board_size) {
        return false;
    }
    
    if (value < 1 || value > registry->board_size) {
        return false;
    }
    
    // Check if already registered
    int existing_idx = find_cell(registry, row, col);
    
    if (existing_idx >= 0) {
        // Update existing entry
        registry->cells[existing_idx].value = value;
        registry->cells[existing_idx].type = type;
        registry->cells[existing_idx].generation_step = generation_step;
        registry->cells[existing_idx].difficulty_score = 
            forced_cells_calculate_difficulty_score(type, generation_step);
        return true;
    }
    
    // Check capacity
    if (registry->count >= registry->capacity) {
        return false;
    }
    
    // Add new entry
    int idx = registry->count;
    registry->cells[idx].position.row = row;
    registry->cells[idx].position.col = col;
    registry->cells[idx].value = value;
    registry->cells[idx].type = type;
    registry->cells[idx].generation_step = generation_step;
    registry->cells[idx].difficulty_score = 
        forced_cells_calculate_difficulty_score(type, generation_step);
    
    registry->occupied[cell_index(registry, row, col)] = true;
    registry->count++;
    
    return true;
}

bool forced_cells_register_backtracked(ForcedCellsRegistry *registry,
                                       int row, int col, int value,
                                       int generation_step) {
    return forced_cells_register(registry, row, col, value,
                                 FORCED_BACKTRACKED, generation_step);
}

// ═══════════════════════════════════════════════════════════════════
//                    CELL QUERY
// ═══════════════════════════════════════════════════════════════════

bool forced_cells_is_registered(const ForcedCellsRegistry *registry,
                                int row, int col) {
    return find_cell(registry, row, col) >= 0;
}

bool forced_cells_get_info(const ForcedCellsRegistry *registry,
                           int row, int col,
                           ForcedCell *cell) {
    int idx = find_cell(registry, row, col);
    
    if (idx < 0) {
        return false;
    }
    
    if (cell) {
        *cell = registry->cells[idx];
    }
    
    return true;
}

ForcedCellType forced_cells_get_type(const ForcedCellsRegistry *registry,
                                     int row, int col) {
    int idx = find_cell(registry, row, col);
    
    if (idx < 0) {
        return FORCED_BACKTRACKED;  // Default: not forced
    }
    
    return registry->cells[idx].type;
}

// ═══════════════════════════════════════════════════════════════════
//                    PROTECTION POLICY
// ═══════════════════════════════════════════════════════════════════

ForcedCellType forced_cells_protection_threshold(SudokuDifficulty difficulty) {
    switch (difficulty) {
        case DIFFICULTY_EASY:
            // EASY: Solo protege las más complejas (propagated)
            // Permite eliminar naked singles y hidden singles
            return FORCED_PROPAGATED;
            
        case DIFFICULTY_MEDIUM:
            // MEDIUM: Protege hidden singles y superiores
            // Permite eliminar solo naked singles básicos
            return FORCED_HIDDEN_SINGLE;
            
        case DIFFICULTY_HARD:
            // HARD: Protege todo excepto algunos naked singles obvios
            // Filosofía: Requiere búsqueda pero sigue siendo justo
            return FORCED_NAKED_SINGLE;
            
        case DIFFICULTY_EXPERT:
            // EXPERT: Protege TODAS las celdas forzadas
            // Solo permite eliminar backtracked (muy pocas)
            return FORCED_NAKED_SINGLE;
            
        case DIFFICULTY_UNKNOWN:
        default:
            // Default seguro: proteger todo
            return FORCED_NAKED_SINGLE;
    }
}

bool forced_cells_should_protect(const ForcedCellsRegistry *registry,
                                 int row, int col,
                                 SudokuDifficulty difficulty) {
    if (!registry) {
        return false;  // Si no hay registry, no proteger
    }
    
    int idx = find_cell(registry, row, col);
    
    if (idx < 0) {
        return false;  // Celda no registrada, no proteger
    }
    
    ForcedCellType cell_type = registry->cells[idx].type;
    ForcedCellType threshold = forced_cells_protection_threshold(difficulty);
    
    // LÓGICA DE PROTECCIÓN:
    // 
    // Proteger si el tipo de celda es >= threshold
    // 
    // Ejemplo para MEDIUM (threshold = HIDDEN_SINGLE):
    //   NAKED_SINGLE (0) < HIDDEN_SINGLE (1) → NO proteger
    //   HIDDEN_SINGLE (1) >= HIDDEN_SINGLE (1) → SÍ proteger
    //   PROPAGATED (2) >= HIDDEN_SINGLE (1) → SÍ proteger
    //   BACKTRACKED (3) >= HIDDEN_SINGLE (1) → SÍ proteger
    
    if (difficulty == DIFFICULTY_EXPERT) {
        // EXPERT: Proteger todo excepto backtracked
        return (cell_type != FORCED_BACKTRACKED);
    }
    
    return (cell_type >= threshold);
}

// ═══════════════════════════════════════════════════════════════════
//                    UTILITY FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

const char* forced_cell_type_to_string(ForcedCellType type) {
    switch (type) {
        case FORCED_NAKED_SINGLE:
            return "NAKED_SINGLE";
        case FORCED_HIDDEN_SINGLE:
            return "HIDDEN_SINGLE";
        case FORCED_PROPAGATED:
            return "PROPAGATED";
        case FORCED_BACKTRACKED:
            return "BACKTRACKED";
        default:
            return "UNKNOWN";
    }
}

int forced_cells_calculate_difficulty_score(ForcedCellType type,
                                            int generation_step) {
    int base_score = 0;
    
    switch (type) {
        case FORCED_NAKED_SINGLE:
            base_score = 1;
            break;
        case FORCED_HIDDEN_SINGLE:
            base_score = 3;
            break;
        case FORCED_PROPAGATED:
            base_score = 6;
            break;
        case FORCED_BACKTRACKED:
            base_score = 9;
            break;
    }
    
    // Ajustar según profundidad (más profundo = más difícil)
    int depth_bonus = (generation_step / 20);  // +1 cada 20 niveles
    if (depth_bonus > 3) depth_bonus = 3;      // Max +3
    
    int final_score = base_score + depth_bonus;
    if (final_score > 10) final_score = 10;
    if (final_score < 1) final_score = 1;
    
    return final_score;
}

void forced_cells_print_registry(const ForcedCellsRegistry *registry,
                                 bool verbose) {
    if (!registry) {
        printf("Registry: NULL\n");
        return;
    }
    
    printf("═══════════════════════════════════════════════════════\n");
    printf("        FORCED CELLS REGISTRY\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("  Board size: %d×%d\n", registry->board_size, registry->board_size);
    printf("  Total cells: %d / %d\n", registry->count, registry->capacity);
    printf("\n");
    
    // Statistics
    ForcedCellsStats stats;
    forced_cells_registry_get_stats(registry, &stats);
    
    printf("  📊 BY TYPE:\n");
    printf("     Naked singles:  %3d (%5.1f%%)\n", 
           stats.naked_singles,
           registry->count > 0 ? 100.0 * stats.naked_singles / registry->count : 0.0);
    printf("     Hidden singles: %3d (%5.1f%%)\n",
           stats.hidden_singles,
           registry->count > 0 ? 100.0 * stats.hidden_singles / registry->count : 0.0);
    printf("     Propagated:     %3d (%5.1f%%)\n",
           stats.propagated,
           registry->count > 0 ? 100.0 * stats.propagated / registry->count : 0.0);
    printf("     Backtracked:    %3d (%5.1f%%)\n",
           stats.backtracked,
           registry->count > 0 ? 100.0 * stats.backtracked / registry->count : 0.0);
    
    if (verbose && registry->count > 0) {
        printf("\n  📋 DETAILED LIST:\n");
        printf("     %-6s %-8s %-5s %-16s %-5s %-6s\n",
               "Index", "Position", "Value", "Type", "Step", "Score");
        printf("     %-6s %-8s %-5s %-16s %-5s %-6s\n",
               "-----", "--------", "-----", "----------------", "----", "-----");
        
        for (int i = 0; i < registry->count; i++) {
            ForcedCell *cell = &registry->cells[i];
            printf("     %-6d (%2d,%2d)   %-5d %-16s %-5d %-6d\n",
                   i,
                   cell->position.row, cell->position.col,
                   cell->value,
                   forced_cell_type_to_string(cell->type),
                   cell->generation_step,
                   cell->difficulty_score);
        }
    }
    
    printf("═══════════════════════════════════════════════════════\n");
}

/**
 * @file forced_cells.h
 * @brief Forced Cells Registry System for Intelligent Elimination
 * @author Gonzalo Ramírez
 * @date 2025-12-05
 * @version 3.1.0
 * 
 * SISTEMA DE CELDAS FORZADAS
 * ===========================
 * 
 * Este módulo implementa el registro y clasificación de "celdas forzadas"
 * durante la generación AC3HB. Una celda es "forzada" cuando AC3 la resuelve
 * automáticamente porque su dominio se reduce a un único valor posible.
 * 
 * PROPÓSITO:
 * ----------
 * Durante Phase 3 de eliminación, usar esta información para:
 * 1. PROTEGER celdas que fueron deducidas lógicamente (mejora jugabilidad)
 * 2. PRIORIZAR eliminación de celdas asignadas por backtracking
 * 3. CALIBRAR dificultad según el nivel objetivo del puzzle
 * 
 * CLASIFICACIÓN DE CELDAS FORZADAS:
 * ---------------------------------
 * - NAKED_SINGLE:    Celda con un solo candidato (más básica)
 * - HIDDEN_SINGLE:   Número con una sola posición en región
 * - PROPAGATED:      Forzada por propagación AC3 de múltiples pasos
 * - BACKTRACKED:     Asignada por backtracking (no forzada lógicamente)
 * 
 * INTEGRACIÓN CON DIFICULTAD:
 * --------------------------
 * EASY:    Protege solo PROPAGATED y COMPLEX
 * MEDIUM:  Protege HIDDEN_SINGLE y superiores
 * HARD:    Protege NAKED_SINGLE y superiores
 * EXPERT:  Protege TODAS las celdas forzadas (solo elimina BACKTRACKED)
 * 
 * USO TÍPICO:
 * ----------
 * ```c
 * // Durante generación AC3HB:
 * ForcedCellsRegistry *registry = forced_cells_registry_create(board_size);
 * 
 * // Al detectar celda forzada en AC3:
 * forced_cells_register(registry, row, col, value, FORCED_NAKED_SINGLE, depth);
 * 
 * // Durante Phase 3:
 * if (forced_cells_should_protect(registry, row, col, difficulty)) {
 *     continue;  // Saltar esta celda, no eliminar
 * }
 * 
 * // Al finalizar:
 * forced_cells_registry_destroy(registry);
 * ```
 * 
 * ARQUITECTURA:
 * ------------
 * Este sistema se integra con:
 * - AC3HB algorithm (registra durante propagación)
 * - Phase 3 elimination (usa durante selección de candidatos)
 * - Difficulty system (calibra protección según nivel)
 * - Statistics (cuenta celdas por tipo)
 */

#ifndef SUDOKU_ALGORITHMS_FORCED_CELLS_H
#define SUDOKU_ALGORITHMS_FORCED_CELLS_H

#include "sudoku/core/types.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ═══════════════════════════════════════════════════════════════════
//                    TYPE DEFINITIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Classification of how a cell was forced
 * 
 * Orden de menor a mayor dificultad para deducir:
 * NAKED_SINGLE < HIDDEN_SINGLE < PROPAGATED < BACKTRACKED
 */
typedef enum {
    FORCED_NAKED_SINGLE = 0,    /**< Celda con exactamente un candidato */
    FORCED_HIDDEN_SINGLE = 1,   /**< Número con exactamente una posición */
    FORCED_PROPAGATED = 2,      /**< Resultado de propagación AC3 multi-paso */
    FORCED_BACKTRACKED = 3      /**< Asignada por backtracking (NO forzada) */
} ForcedCellType;

/**
 * @brief Information about a single forced cell
 */
typedef struct {
    SudokuPosition position;    /**< Ubicación de la celda */
    int value;                  /**< Valor asignado (1-N) */
    ForcedCellType type;        /**< Clasificación del tipo de forzado */
    int generation_step;        /**< Profundidad de recursión cuando fue forzada */
    int difficulty_score;       /**< Score 1-10 (calculado según tipo + step) */
} ForcedCell;

/**
 * @brief Registry of all forced cells in a board
 * 
 * Estructura opaca. Usar funciones de la API para acceder.
 */
typedef struct ForcedCellsRegistry ForcedCellsRegistry;

/**
 * @brief Statistics about forced cells
 */
typedef struct {
    int total_forced;           /**< Total de celdas forzadas */
    int naked_singles;          /**< Cantidad de naked singles */
    int hidden_singles;         /**< Cantidad de hidden singles */
    int propagated;             /**< Cantidad de celdas propagadas */
    int backtracked;            /**< Cantidad de celdas por backtracking */
    int protected_by_difficulty;/**< Celdas protegidas según dificultad */
    int available_for_removal;  /**< Celdas disponibles para eliminar */
} ForcedCellsStats;

// ═══════════════════════════════════════════════════════════════════
//                    REGISTRY MANAGEMENT
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Create a new forced cells registry
 * 
 * @param board_size Tamaño del tablero (4, 9, 16, 25, etc.)
 * @return Puntero al registry o NULL si falla allocación
 * 
 * @note Debe ser liberado con forced_cells_registry_destroy()
 */
ForcedCellsRegistry* forced_cells_registry_create(int board_size);

/**
 * @brief Destroy registry and free all memory
 * 
 * @param registry Registry a destruir (puede ser NULL)
 */
void forced_cells_registry_destroy(ForcedCellsRegistry *registry);

/**
 * @brief Clear all entries from registry (mantiene allocación)
 * 
 * @param registry Registry a limpiar
 */
void forced_cells_registry_clear(ForcedCellsRegistry *registry);

/**
 * @brief Get statistics about forced cells
 * 
 * @param registry Registry a consultar
 * @param[out] stats Estructura a llenar con estadísticas
 * @return true si exitoso, false si registry es NULL
 */
bool forced_cells_registry_get_stats(const ForcedCellsRegistry *registry,
                                     ForcedCellsStats *stats);

// ═══════════════════════════════════════════════════════════════════
//                    CELL REGISTRATION
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Register a forced cell in the registry
 * 
 * @param registry Registry donde registrar
 * @param row Fila de la celda
 * @param col Columna de la celda
 * @param value Valor asignado
 * @param type Tipo de forzado (NAKED_SINGLE, HIDDEN_SINGLE, etc.)
 * @param generation_step Profundidad de recursión cuando fue forzada
 * @return true si se registró exitosamente
 * 
 * @note Si la celda ya está registrada, actualiza la información
 */
bool forced_cells_register(ForcedCellsRegistry *registry,
                           int row, int col, int value,
                           ForcedCellType type, int generation_step);

/**
 * @brief Register a cell as backtracked (not logically forced)
 * 
 * Convenience function para marcar celdas asignadas por backtracking.
 * Equivalente a: forced_cells_register(..., FORCED_BACKTRACKED, depth)
 * 
 * @param registry Registry donde registrar
 * @param row Fila de la celda
 * @param col Columna de la celda
 * @param value Valor asignado
 * @param generation_step Profundidad de recursión
 * @return true si exitoso
 */
bool forced_cells_register_backtracked(ForcedCellsRegistry *registry,
                                       int row, int col, int value,
                                       int generation_step);

// ═══════════════════════════════════════════════════════════════════
//                    CELL QUERY
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Check if a cell is registered as forced
 * 
 * @param registry Registry a consultar
 * @param row Fila de la celda
 * @param col Columna de la celda
 * @return true si la celda está en el registry
 */
bool forced_cells_is_registered(const ForcedCellsRegistry *registry,
                                int row, int col);

/**
 * @brief Get information about a forced cell
 * 
 * @param registry Registry a consultar
 * @param row Fila de la celda
 * @param col Columna de la celda
 * @param[out] cell Estructura a llenar con información (puede ser NULL)
 * @return true si la celda existe en registry
 */
bool forced_cells_get_info(const ForcedCellsRegistry *registry,
                           int row, int col,
                           ForcedCell *cell);

/**
 * @brief Get the type of forcing for a cell
 * 
 * @param registry Registry a consultar
 * @param row Fila de la celda
 * @param col Columna de la celda
 * @return Tipo de forzado, o FORCED_BACKTRACKED si no está registrada
 */
ForcedCellType forced_cells_get_type(const ForcedCellsRegistry *registry,
                                     int row, int col);

// ═══════════════════════════════════════════════════════════════════
//                    PROTECTION POLICY (INTEGRATION WITH DIFFICULTY)
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Determine if a cell should be protected from elimination
 * 
 * Implementa la política de protección según dificultad objetivo:
 * 
 * EASY (45+ clues):
 *   - Protege: PROPAGATED, BACKTRACKED
 *   - Permite eliminar: NAKED_SINGLE, HIDDEN_SINGLE
 *   - Filosofía: Puzzle debe tener muchas deducciones básicas visibles
 * 
 * MEDIUM (35-44 clues):
 *   - Protege: HIDDEN_SINGLE, PROPAGATED, BACKTRACKED
 *   - Permite eliminar: NAKED_SINGLE (más obvios)
 *   - Filosofía: Balance entre challenge y fairness
 * 
 * HARD (25-34 clues):
 *   - Protege: NAKED_SINGLE, HIDDEN_SINGLE, PROPAGATED, BACKTRACKED
 *   - Permite eliminar: Solo algunos NAKED_SINGLE muy obvios
 *   - Filosofía: Requiere búsqueda cuidadosa pero sigue siendo lógico
 * 
 * EXPERT (<25 clues):
 *   - Protege: TODAS las celdas forzadas
 *   - Permite eliminar: Solo BACKTRACKED (muy pocas)
 *   - Filosofía: Máximo desafío manteniendo jugabilidad lógica
 * 
 * @param registry Registry con información de celdas forzadas
 * @param row Fila de la celda a evaluar
 * @param col Columna de la celda a evaluar
 * @param difficulty Nivel de dificultad objetivo del puzzle
 * @return true si la celda debe ser protegida (NO eliminar)
 */
bool forced_cells_should_protect(const ForcedCellsRegistry *registry,
                                 int row, int col,
                                 SudokuDifficulty difficulty);

/**
 * @brief Get protection threshold for a difficulty level
 * 
 * Retorna el tipo de celda más básico que debe protegerse para
 * el nivel de dificultad dado.
 * 
 * @param difficulty Nivel de dificultad
 * @return Tipo mínimo a proteger
 * 
 * Ejemplos:
 *   EASY   → FORCED_PROPAGATED (protege solo las complejas)
 *   MEDIUM → FORCED_HIDDEN_SINGLE
 *   HARD   → FORCED_NAKED_SINGLE
 *   EXPERT → FORCED_NAKED_SINGLE (protege todas)
 */
ForcedCellType forced_cells_protection_threshold(SudokuDifficulty difficulty);

// ═══════════════════════════════════════════════════════════════════
//                    UTILITY FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Convert ForcedCellType to string
 * 
 * @param type Tipo a convertir
 * @return String representation (no debe ser liberado)
 */
const char* forced_cell_type_to_string(ForcedCellType type);

/**
 * @brief Calculate difficulty score for a cell
 * 
 * Score 1-10 donde:
 * - 1-2: Naked singles muy obvios (depth bajo)
 * - 3-4: Naked singles profundos o hidden singles básicos
 * - 5-6: Hidden singles profundos
 * - 7-8: Celdas propagadas (AC3 multi-step)
 * - 9-10: Backtracked (requieren guess)
 * 
 * @param type Tipo de forzado
 * @param generation_step Profundidad cuando fue forzada
 * @return Score 1-10
 */
int forced_cells_calculate_difficulty_score(ForcedCellType type,
                                            int generation_step);

/**
 * @brief Print registry contents (debugging)
 * 
 * @param registry Registry a imprimir
 * @param verbose Si true, muestra información detallada de cada celda
 */
void forced_cells_print_registry(const ForcedCellsRegistry *registry,
                                 bool verbose);

#ifdef __cplusplus
}
#endif

#endif // SUDOKU_ALGORITHMS_FORCED_CELLS_H

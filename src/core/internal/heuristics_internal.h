/**
 * @file heuristics_internal.h
 * @brief Internal structures for heuristics implementation
 * @author Gonzalo Ramírez
 * @date 2025-12
 * 
 * ESTE ARCHIVO DEFINE LAS ESTRUCTURAS INTERNAS (OPACAS)
 */

#ifndef SUDOKU_HEURISTICS_INTERNAL_H
#define SUDOKU_HEURISTICS_INTERNAL_H

// ═══════════════════════════════════════════════════════════════════
//                    INTERNAL STRUCTURES
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Internal structure for SubgridDensityCache
 * 
 * CORRECCIÓN: Almacena densidad de SUBCUADRÍCULA (no vecinos)
 * 
 * Para 9×9: densities[0..8] contiene el número de celdas llenas
 * en cada subcuadrícula 3×3 (0 a 9 posibles).
 */
struct SubgridDensityCache {
    int *densities;      ///< Array[board_size] de densidades por subcuadrícula
    int board_size;      ///< Tamaño del tablero (9 para 9×9)
    int subgrid_size;    ///< Tamaño de subcuadrícula (3 para 9×9)
};

#endif // SUDOKU_HEURISTICS_INTERNAL_H

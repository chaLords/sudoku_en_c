// src/core/internal/config_internal.h
#ifndef SUDOKU_CONFIG_INTERNAL_H
#define SUDOKU_CONFIG_INTERNAL_H

/**
 * Nivel de verbosidad global que controla la cantidad de información
 * de progreso y debugging que se muestra durante la ejecución.
 * 
 * Niveles:
 *   0 = MINIMAL: Solo título, tablero final y dificultad
 *   1 = COMPACT: Resúmenes de cada fase (default)
 *   2 = DETAILED: Información paso a paso de debugging
 * 
 * Esta variable es definida en el CLI principal y accesible
 * en toda la biblioteca mediante declaración extern.
 * 
 * Nota arquitectural: En futuras versiones esta variable debería
 * eliminarse en favor de pasar verbosity como parámetro en la
 * estructura SudokuConfig, mejorando la testabilidad y eliminando
 * estado global.
 */
extern int VERBOSITY_LEVEL;

#endif // SUDOKU_CONFIG_INTERNAL_H

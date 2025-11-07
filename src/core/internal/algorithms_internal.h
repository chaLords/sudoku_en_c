// src/core/internal/algorithms_internal.h

#ifndef SUDOKU_ALGORITHMS_INTERNAL_H
#define SUDOKU_ALGORITHMS_INTERNAL_H

#include <stdbool.h>
#include "sudoku/core/board.h"

/**
 * @brief Completa el tablero usando backtracking recursivo
 * 
 * @param board Puntero al tablero a completar
 * @return true si se completó exitosamente, false si no hay solución
 */
bool sudoku_complete_backtracking(SudokuBoard *board);

/**
 * @brief Genera una permutación aleatoria de números consecutivos
 * 
 * Llena el array con números consecutivos desde 'start' y luego
 * los mezcla usando Fisher-Yates.
 * 
 * @param array Array a llenar y mezclar (modificado in-place)
 * @param size Tamaño del array
 * @param start Valor inicial (ej: 1 para [1-9], 0 para [0-80])
 */
void sudoku_generate_permutation(int *array, int size, int start);

#endif // SUDOKU_ALGORITHMS_INTERNAL_H

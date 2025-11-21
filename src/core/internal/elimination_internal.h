#ifndef SUDOKU_ELIMINATION_INTERNAL_H
#define SUDOKU_ELIMINATION_INTERNAL_H

#include "sudoku/core/types.h"

// Declaraciones de las tres fases
int phase1Elimination(SudokuBoard *board, const int *index, int count);
int phase2Elimination(SudokuBoard *board, const int *index, int count);
int phase3Elimination(SudokuBoard *board, int target);

/**
 * @brief Phase 3 elimination with automatic target calculation
 * 
 * This wrapper function automatically calculates an appropriate removal
 * target based on the board's dimensions, eliminating the need for the
 * caller to manage size-dependent constants.
 * 
 * The target is calculated as a percentage of total cells, with the
 * percentage adjusted based on board size to maintain consistent difficulty:
 * - Small boards (≤9×9): 31% of cells
 * - Medium boards (≤16×16): 27% of cells
 * - Large boards (>16×16): 23% of cells
 * 
 * This is the recommended way to invoke Phase 3 elimination.
 * 
 * @param board Board to perform Phase 3 elimination on
 * @return Number of cells successfully removed
 * 
 * @note Internally calls calculate_phase3_target() and phase3Elimination()
 */
int phase3EliminationAuto(SudokuBoard *board);

// Funciones auxiliares privadas
bool hasAlternative(SudokuBoard *board, const SudokuPosition *pos, int num);

// Contador de soluciones usado por phase3
int countSolutionsExact(SudokuBoard *board, int limit);

#endif

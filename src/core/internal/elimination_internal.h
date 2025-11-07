// src/core/internal/elimination_internal.h
#ifndef SUDOKU_ELIMINATION_INTERNAL_H
#define SUDOKU_ELIMINATION_INTERNAL_H

#include "sudoku/core/board.h"
#include "sudoku/core/types.h"

// Declaraciones de las tres fases
int phase1Elimination(SudokuBoard *board, const int *index, int count);
int phase2Elimination(SudokuBoard *board, const int *index, int count);
int phase3Elimination(SudokuBoard *board, int target);

// Funciones auxiliares privadas
bool hasAlternative(SudokuBoard *board, const SudokuPosition *pos, int num);

// Contador de soluciones usado por phase3
int countSolutionsExact(SudokuBoard *board, int limit);

#endif

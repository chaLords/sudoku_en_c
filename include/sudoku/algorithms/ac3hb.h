/**
 * @file ac3hb.h
 * @brief AC3HB algorithm public API
 * @author Gonzalo Ramírez
 * @date 2025-12
 */

#ifndef SUDOKU_AC3HB_H
#define SUDOKU_AC3HB_H

#include "sudoku/core/types.h"
#include "sudoku/core/board.h"
#include <stdbool.h>

// ═══════════════════════════════════════════════════════════════════
//                    AC3HB STATISTICS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Statistics from AC3HB execution
 */
typedef struct {
    int total_backtracks;      ///< Total backtrack operations
    int ac3_calls;             ///< Number of AC3 propagations
    int cells_assigned;        ///< Total cell assignments
    int max_depth;             ///< Maximum recursion depth
    int values_eliminated;     ///< Values eliminated by AC3
    double time_ms;            ///< Total execution time
} AC3HBStats;

// ═══════════════════════════════════════════════════════════════════
//                    MAIN AC3HB FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Complete a partially filled board using AC3HB
 * 
 * Main entry point for AC3HB algorithm. This function should be
 * called from generator.c when AC3HB strategy is selected.
 * 
 * @param board Board to complete (modified in place)
 * @return true if board completed successfully
 */
bool sudoku_complete_ac3hb(SudokuBoard *board);

/**
 * @brief Complete board with AC3HB and return statistics
 * 
 * Extended version that returns detailed statistics.
 * 
 * @param board Board to complete
 * @param[out] stats Statistics output (can be NULL)
 * @return true if successful
 */
bool sudoku_complete_ac3hb_ex(SudokuBoard *board, AC3HBStats *stats);

/**
 * @brief Print AC3HB statistics for debugging
 * 
 * @param stats Statistics to print
 */
void print_ac3hb_stats(const AC3HBStats *stats);

#endif // SUDOKU_AC3HB_H

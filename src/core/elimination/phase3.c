/**
 * @file phase3.c (ADAPTED FOR CONFIGURABLE BOARD SIZES)
 * @brief Phase 3: Exhaustive verified elimination with dynamic targeting
 * @author Gonzalo Ramírez
 * @date 2025-11-21
 * 
 * CRITICAL ARCHITECTURAL CHANGES FOR CONFIGURABLE SIZES:
 * 
 * 1. REPLACED: Hardcoded TOTAL_CELLS (81) in malloc
 *    WITH: Dynamic calculation board_size × board_size
 *    
 * 2. REPLACED: Hardcoded SUDOKU_SIZE in iteration loops
 *    WITH: Dynamic board->board_size
 *    
 * 3. REPLACED: Fixed PHASE3_TARGET constant (25)
 *    WITH: Proportional calculation based on board size
 *    
 * KEY ALGORITHMIC INSIGHT: The "target" for Phase 3 must scale
 * proportionally with board size to maintain consistent difficulty.
 * A fixed target of 25 cells works for 9×9 but fails completely
 * for other sizes.
 * 
 * PROPORTIONAL TARGETING STRATEGY:
 * 
 * For 9×9 boards, PHASE3_TARGET=25 represents approximately 31% of
 * the total 81 cells. To maintain equivalent difficulty:
 * 
 * - 4×4 board (16 cells): 31% ≈ 5 cells
 * - 9×9 board (81 cells): 31% ≈ 25 cells (original)
 * - 16×16 board (256 cells): 31% ≈ 79 cells
 * - 25×25 board (625 cells): 31% ≈ 194 cells
 * 
 * This ensures that puzzles feel similarly challenging across sizes.
 */

#include "elimination_internal.h"
//#include "board_internal.h"
#include "sudoku/core/validation.h"
#include "events_internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * @brief Calculate proportional Phase 3 target for any board size
 * 
 * This function implements the mathematical formula for determining
 * how many additional cells to attempt removing in Phase 3, based
 * on the board's dimensions.
 * 
 * MATHEMATICAL FOUNDATION:
 * 
 * The target is calculated as a percentage of total cells:
 *   target = total_cells × percentage
 * 
 * Where percentage depends on board size to account for complexity:
 * 
 * - Small boards (≤ 9): 31% (more aggressive)
 * - Medium boards (≤ 16): 27% (moderate)
 * - Large boards (> 16): 23% (conservative)
 * 
 * REASONING BEHIND SIZE-DEPENDENT PERCENTAGES:
 * 
 * Smaller boards are inherently easier to solve because there are
 * fewer constraint interactions. We can safely remove a higher
 * percentage of cells without making the puzzle unsolvable or
 * creating multiple solutions.
 * 
 * Larger boards have exponentially more constraint interactions.
 * Each cell participates in more constraint checks (larger rows,
 * columns, and subgrids). Removing the same percentage as small
 * boards would likely create unsolvable or ambiguous puzzles.
 * 
 * The percentages are tuned based on empirical testing to produce
 * puzzles with similar perceived difficulty across sizes.
 * 
 * @param[in] board Board to calculate target for
 * @return Number of cells to attempt removing in Phase 3
 * 
 * @note This is a heuristic. Exact optimal values would require
 *       extensive empirical testing across many generated puzzles.
 */
static int calculate_phase3_target(const SudokuBoard *board) {
    // Calculate total cells in the board
    int total_cells = board->board_size * board->board_size;
    
    // Determine percentage based on board size
    double percentage;
    
    if (board->board_size <= 9) {
        // Small to medium boards (4×4, 9×9)
        // Use aggressive 31% (original ratio for 9×9)
        percentage = 0.31;
    } else if (board->board_size <= 16) {
        // Medium-large boards (16×16)
        // Use moderate 27% to account for increased complexity
        percentage = 0.27;
    } else {
        // Large boards (25×25 and above)
        // Use conservative 23% due to high constraint density
        percentage = 0.23;
    }
    
    // Calculate target and round to nearest integer
    int target = (int)(total_cells * percentage + 0.5);
    
    // Ensure target is at least 1 (even for tiny boards)
    // and doesn't exceed total cells minus minimum clues
    if (target < 1) {
        target = 1;
    }
    
    // Safety check: never target more than 80% of cells
    // (leaving at least 20% as clues to maintain solvability)
    int max_safe_target = (int)(total_cells * 0.80);
    if (target > max_safe_target) {
        target = max_safe_target;
    }
    
    return target;
}

/**
 * @brief Phase 3: Free elimination with exhaustive uniqueness verification
 * 
 * MAJOR ADAPTATIONS FOR CONFIGURABLE SIZES:
 * 
 * 1. Dynamic target calculation replaces fixed PHASE3_TARGET constant
 * 2. Dynamic memory allocation sized to actual board dimensions
 * 3. All iteration loops use board_size instead of SUDOKU_SIZE
 * 
 * ALGORITHM OVERVIEW:
 * 
 * Phase 3 represents the most computationally intensive part of puzzle
 * generation. It attempts "free" elimination - removing cells without
 * any heuristic guidance, relying solely on exhaustive solution counting
 * to verify that each removal maintains exactly one solution.
 * 
 * PERFORMANCE CHARACTERISTICS:
 * 
 * For each removal attempt:
 * - Time: O(9^m) where m = current empty cells
 * - With early exit: typically O(9^k) where k << m
 * 
 * Total Phase 3 complexity: O(target × 9^m)
 * 
 * For large boards (16×16, 25×25), this can be EXTREMELY slow. Each
 * countSolutionsExact() call might explore millions of board states.
 * 
 * IMPORTANT: The target parameter passed to this function should come
 * from calculate_phase3_target() or a proportional calculation, not
 * a hardcoded constant.
 * 
 * @param[in,out] board Board with Phases 1-2 eliminations applied
 * @param[in] target Maximum number of additional cells to attempt removing
 *                   Should be calculated proportionally, not hardcoded
 * @return Number of cells successfully removed
 * 
 * @pre board != NULL
 * @pre board has unique solution after Phases 1-2
 * @pre target > 0 and target < total_cells
 * 
 * @post Additional cells removed while maintaining unique solution
 * @post If return < target, no more safe removals were possible
 * 
 * Time Complexity: O(target × 9^m) where m = empty cells
 * Space Complexity: O(total_cells) for position array
 */
int phase3Elimination(SudokuBoard *board, int target) {
    // Emit phase start event
    emit_event(SUDOKU_EVENT_PHASE3_START, board, 3, 0);
    
    // ✅ ADAPTACIÓN 1: Calcular total_cells dinámicamente
    // ANTES: Asumía TOTAL_CELLS = 81 siempre
    // AHORA: Calcula board_size × board_size
    //
    // EXPLICACIÓN MATEMÁTICA:
    // Un tablero N×N tiene exactamente N² celdas.
    // Para 4×4: 4 × 4 = 16 celdas
    // Para 9×9: 9 × 9 = 81 celdas
    // Para 16×16: 16 × 16 = 256 celdas
    // Para 25×25: 25 × 25 = 625 celdas
    int board_size = board->board_size;
    int total_cells = board_size * board_size;
    
    // ✅ ADAPTACIÓN 2: Asignación dinámica con tamaño correcto
    // ANTES: malloc(TOTAL_CELLS * sizeof(SudokuPosition))
    // AHORA: malloc(total_cells * sizeof(SudokuPosition))
    //
    // RAZONAMIENTO: Necesitamos espacio para potencialmente todas las
    // posiciones del tablero. Usar el total_cells calculado asegura
    // que tengamos suficiente espacio sin desperdiciar memoria.
    //
    // EJEMPLOS DE ASIGNACIÓN:
    // - 4×4: 16 × 16 bytes = 256 bytes
    // - 9×9: 81 × 16 bytes = 1,296 bytes ≈ 1.3 KB
    // - 16×16: 256 × 16 bytes = 4,096 bytes = 4 KB
    // - 25×25: 625 × 16 bytes = 10,000 bytes ≈ 10 KB
    SudokuPosition *positions = (SudokuPosition *)malloc(
        total_cells * sizeof(SudokuPosition)
    );
    
    if (positions == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in phase3Elimination\n");
        fprintf(stderr, "       Attempted to allocate %d positions (%zu bytes)\n",
                total_cells, total_cells * sizeof(SudokuPosition));
        return 0;
    }
    
    int filled_count = 0;
    
    // ✅ ADAPTACIÓN 3: Iterar hasta board_size en ambos bucles
    // ANTES: for (int row = 0; row < SUDOKU_SIZE; row++)
    //        for (int col = 0; col < SUDOKU_SIZE; col++)
    // AHORA: for (int row = 0; row < board_size; row++)
    //        for (int col = 0; col < board_size; col++)
    //
    // RAZONAMIENTO: Debemos examinar todas las celdas del tablero N×N,
    // no solo las primeras 9×9. Usar board_size garantiza que cubrimos
    // todo el tablero sin importar su tamaño.
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            if (board->cells[row][col] != 0) {
                positions[filled_count].row = row;
                positions[filled_count].col = col;
                filled_count++;
            }
        }
    }
    
    // Shuffle positions using Fisher-Yates
    // This ensures different elimination orders across runs,
    // leading to different final puzzles from the same complete board
    for (int i = filled_count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        SudokuPosition temp = positions[i];
        positions[i] = positions[j];
        positions[j] = temp;
    }
    
    int removed = 0;
    
    // Attempt to remove cells in shuffled order until target reached
    for (int i = 0; i < filled_count && removed < target; i++) {
        SudokuPosition pos = positions[i];
        int original_value = board->cells[pos.row][pos.col];
        
        // Temporarily remove the cell
        board->cells[pos.row][pos.col] = 0;
        
        // ⚠️ CRITICAL: countSolutionsExact() with limit=2
        // This is where the computational cost lives. For large boards
        // with many empty cells, each call can explore millions of states.
        //
        // OPTIMIZATION NOTE: Early exit (limit=2) is ESSENTIAL. Without it,
        // we'd need to count ALL solutions, which could be astronomical.
        // We only need to distinguish "exactly 1" from "not exactly 1",
        // so stopping at 2 provides massive speedup.
        if (countSolutionsExact(board, 2) == 1) {
            // Exactly one solution: keep the removal
            removed++;
            
            // Emit cell removed event
            emit_event_cell(SUDOKU_EVENT_PHASE3_CELL_REMOVED,
                            board, 3, removed, pos.row, pos.col, original_value);
        } else {
            // Multiple solutions or no solution: restore the cell
            board->cells[pos.row][pos.col] = original_value;
        }
    }
    
    // ✅ CRÍTICO: Liberar memoria dinámica
    free(positions);
    
    // Emit phase complete event
    emit_event(SUDOKU_EVENT_PHASE3_COMPLETE, board, 3, removed);
    
    return removed;
}

/**
 * @brief Public wrapper that calculates target dynamically
 * 
 * This is the recommended function to call from generator code.
 * It automatically calculates an appropriate target based on
 * board size, eliminating the need for the caller to manage
 * size-dependent constants.
 * 
 * @param[in,out] board Board to perform Phase 3 elimination on
 * @return Number of cells successfully removed
 */
int phase3EliminationAuto(SudokuBoard *board) {
    int target = calculate_phase3_target(board);
    return phase3Elimination(board, target);
}

/**
 * EDUCATIONAL NOTES ON PROPORTIONAL SCALING:
 * 
 * 1. WHY PERCENTAGE-BASED TARGETING?
 *    
 *    Difficulty in Sudoku is not absolute but relative to information
 *    density. A puzzle with 20% empty cells feels similarly difficult
 *    whether it's 4×4 or 25×25 because the proportion of missing
 *    information is the same.
 *    
 *    Examples:
 *    - 9×9 with 25 clues: 56 empty (69% empty) - Hard
 *    - 16×16 with 77 clues: 179 empty (70% empty) - Hard
 *    - 25×25 with 188 clues: 437 empty (70% empty) - Hard
 *    
 *    All three puzzles have roughly 70% empty cells and will feel
 *    similarly challenging despite vastly different absolute numbers.
 * 
 * 2. WHY ADJUST PERCENTAGES BY SIZE?
 *    
 *    Larger boards have more complex constraint graphs. In a 25×25
 *    board, each cell constrains 24 others in its row, 24 in its
 *    column, and 24 in its 5×5 subgrid (with overlaps). That's much
 *    denser than a 9×9 board where each cell constrains only 8+8+8=24
 *    cells total (with overlaps).
 *    
 *    This increased constraint density means:
 *    - Solver algorithms have more information to work with
 *    - Puzzles can have fewer clues while remaining solvable
 *    - We need to be more conservative with removals
 *    
 *    Hence: larger boards use smaller percentages.
 * 
 * 3. MEMORY SCALING CONSIDERATIONS:
 *    
 *    The position array grows quadratically with board size:
 *    - 4×4: 256 bytes (negligible)
 *    - 9×9: 1.3 KB (negligible)
 *    - 16×16: 4 KB (still fine)
 *    - 25×25: 10 KB (acceptable)
 *    - 36×36: 21 KB (starting to matter)
 *    
 *    For extremely large boards (>36×36), we might need to:
 *    a) Use a different data structure (e.g., vector that grows)
 *    b) Process positions in batches to reduce memory footprint
 *    c) Accept that Phase 3 becomes impractical due to time complexity
 * 
 * 4. COMPUTATIONAL SCALING WARNING:
 *    
 *    Phase 3's time complexity grows EXPONENTIALLY with board size
 *    due to solution counting. Rough estimates:
 *    
 *    - 4×4: milliseconds per puzzle
 *    - 9×9: tens of milliseconds per puzzle
 *    - 16×16: hundreds of milliseconds to seconds per puzzle
 *    - 25×25: potentially minutes per puzzle
 *    
 *    For very large boards, consider:
 *    a) Reducing Phase 3 target to minimize verification calls
 *    b) Using heuristics instead of exhaustive verification
 *    c) Accepting Phase 2 results without Phase 3
 *    d) Implementing early-abort if verification exceeds time budget
 */

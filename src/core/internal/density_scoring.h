/**
 * @file density_scoring.h
 * @brief Density-based scoring system for intelligent cell elimination
 * 
 * This module implements the scoring algorithms that make elimination "intelligent".
 * The core concept is DENSITY: how full or empty each subgrid is. By analyzing
 * density patterns, we can make strategic decisions about which cells to eliminate
 * while maintaining solution uniqueness.
 * 
 * CONCEPTUAL FOUNDATION:
 * 
 * Imagine you're playing Jenga (the block-stacking game). Which blocks are safer
 * to remove: ones from the top (well-supported, high density of blocks below) or
 * ones from the bottom (risky, low support)?
 * 
 * Similarly, in Sudoku:
 * - High-density subgrids (many filled cells): Safer to eliminate from
 *   → More constraints = easier to maintain unique solution
 * - Low-density subgrids (few filled cells): Risky to eliminate from
 *   → Fewer constraints = easier to create multiple solutions
 * 
 * This module provides tools to:
 * 1. Calculate density for each subgrid
 * 2. Score subgrids for elimination priority
 * 3. Score individual cells within subgrids
 * 4. Sort by scores to process in optimal order
 * 
 * MATHEMATICAL BASIS:
 * 
 * Density is defined as:
 *   density = (filled_cells / total_cells) × 100
 * 
 * For a 9×9 subgrid with 6 filled cells:
 *   density = (6 / 9) × 100 = 66.67%
 * 
 * We combine density with other heuristics (like "number of alternatives")
 * to create comprehensive scores that guide elimination decisions.
 * 
 * @author Gonzalo Ramírez (@chaLords)
 * @date 2025-01-XX
 * @version 3.0.0
 */

#ifndef SUDOKU_DENSITY_SCORING_H
#define SUDOKU_DENSITY_SCORING_H

#include "sudoku/core/types.h"
#include "sudoku/core/board.h"
#include "elimination_config.h"

/**
 * @brief Score structure for a subgrid (3×3 region)
 * 
 * This structure captures all relevant information about a subgrid that helps
 * us decide whether it's a good candidate for elimination.
 * 
 * DESIGN RATIONALE:
 * Instead of passing multiple separate variables around, we bundle related
 * information into a structure. This is a fundamental principle in software
 * engineering called "data cohesion" - data that's used together should live together.
 * 
 * USAGE PATTERN:
 * ```c
 * // Calculate scores for all subgrids
 * int count;
 * SubGridScore *scores = sudoku_score_subgrids(board, config, &count);
 * 
 * // Sort by score (highest first)
 * qsort(scores, count, sizeof(SubGridScore), compare_subgrid_scores_desc);
 * 
 * // Process in order of score
 * for (int i = 0; i < count; i++) {
 *     printf("Subgrid %d: density=%.1f%%, candidates=%d\n",
 *            scores[i].subgrid_idx, scores[i].density, scores[i].candidates);
 * }
 * 
 * // Always free when done
 * free(scores);
 * ```
 */
typedef struct {
    /**
     * @brief Index of the subgrid (0 to N-1)
     * 
     * For a 9×9 board with 9 subgrids, this ranges from 0 to 8.
     * The index corresponds to row-major order:
     * 
     * ```
     * ┌───┬───┬───┐
     * │ 0 │ 1 │ 2 │
     * ├───┼───┼───┤
     * │ 3 │ 4 │ 5 │
     * ├───┼───┼───┤
     * │ 6 │ 7 │ 8 │
     * └───┴───┴───┘
     * ```
     */
    int subgrid_idx;
    
    /**
     * @brief Density percentage (0.0 to 100.0)
     * 
     * CALCULATION:
     * density = (filled_cells / total_cells) × 100
     * 
     * INTERPRETATION:
     * - 100%: Completely full (all cells have numbers)
     * - 75-99%: High density (mostly full, safe to eliminate from)
     * - 50-74%: Medium density (balanced)
     * - 25-49%: Low density (mostly empty, risky)
     * - 0-24%: Very low density (almost empty, very risky)
     * 
     * WHY IT MATTERS:
     * Higher density means more constraints, which makes it easier to maintain
     * a unique solution when eliminating cells. Think of it like having more
     * "support beams" in a building.
     */
    float density;
    
    /**
     * @brief Number of candidate cells (cells that can potentially be eliminated)
     * 
     * A cell is a "candidate" if it currently has a number AND eliminating it
     * wouldn't immediately create multiple solutions (passes preliminary checks).
     * 
     * CALCULATION:
     * For each filled cell in the subgrid:
     *   - Check if the number has alternatives elsewhere
     *   - If NO alternatives: it's a candidate
     * 
     * INTERPRETATION:
     * - High candidates (6-9): Many options for elimination
     * - Medium candidates (3-5): Some options
     * - Low candidates (1-2): Few options
     * - Zero candidates: Cannot safely eliminate from this subgrid
     * 
     * WHY IT MATTERS:
     * More candidates means more flexibility in elimination strategy. If a
     * subgrid has zero candidates, we skip it entirely.
     */
    int candidates;
    
} SubGridScore;

/**
 * @brief Score structure for an individual cell
 * 
 * While SubGridScore evaluates entire 3×3 regions, CellScore evaluates
 * individual cells. This finer granularity is used in Phase 3 where we
 * need to make decisions at the cell level rather than subgrid level.
 * 
 * USAGE PATTERN:
 * ```c
 * // Calculate scores for all filled cells
 * int count;
 * CellScore *scores = sudoku_score_cells(board, config, &count);
 * 
 * // Sort by priority
 * qsort(scores, count, sizeof(CellScore), compare_cell_scores_desc);
 * 
 * // Attempt elimination in priority order
 * for (int i = 0; i < count && removed < target; i++) {
 *     if (try_eliminate_cell(board, &scores[i].pos)) {
 *         removed++;
 *     }
 * }
 * 
 * free(scores);
 * ```
 */
typedef struct {
    /**
     * @brief Position of the cell (row, column)
     * 
     * This is the absolute position on the board, not relative to any subgrid.
     * For a 9×9 board, row and col each range from 0 to 8.
     */
    SudokuPosition pos;
    
    /**
     * @brief Density of the subgrid containing this cell (0.0 to 100.0)
     * 
     * We inherit the density from the parent subgrid. This allows us to
     * prioritize cells in denser subgrids, similar to how Phase 2 works.
     * 
     * RATIONALE:
     * A cell in a high-density subgrid is safer to eliminate than one in
     * a low-density subgrid, all else being equal.
     */
    float density;
    
    /**
     * @brief Number of alternative positions for this cell's number (0 to 26)
     * 
     * This counts how many other empty cells could potentially hold the same
     * number (considering row, column, and subgrid constraints).
     * 
     * CALCULATION:
     * - Check row: how many empty cells could hold this number?
     * - Check column: how many empty cells could hold this number?
     * - Check subgrid: how many empty cells could hold this number?
     * - Sum all unique positions (max 26 for 9×9: 8 row + 8 col + 8 subgrid, minus overlaps)
     * 
     * INTERPRETATION:
     * - 0 alternatives: This is the ONLY place this number can go (critical cell)
     * - 1-3 alternatives: Few options (relatively safe to keep)
     * - 4-8 alternatives: Many options (candidate for elimination)
     * - 9+ alternatives: Very flexible (good candidate)
     * 
     * WHY IT MATTERS:
     * Cells with MORE alternatives are better candidates for elimination because
     * the solver still has other places to put that number. Cells with ZERO
     * alternatives are critical and should not be eliminated.
     */
    int alternatives;
    
    /**
     * @brief The actual number in this cell (1 to N)
     * 
     * Stored for convenience so we don't need to look it up separately.
     * This is redundant data (it's already in board->cells[pos.row][pos.col])
     * but copying it here makes the code cleaner and slightly faster.
     * 
     * TRADE-OFF:
     * We use a bit more memory (4 bytes per cell) but avoid repeated lookups.
     * For a 9×9 board with 81 cells, this is only 324 bytes of overhead,
     * which is negligible on modern systems.
     */
    int value;
    
} CellScore;

/**
 * @brief Calculates the density of a specific subgrid
 * 
 * This is a fundamental building block used by all other scoring functions.
 * It answers the question: "What percentage of this subgrid is filled?"
 * 
 * ALGORITHM:
 * 1. Create a SubGrid object for the specified index
 * 2. Iterate through all cells in the subgrid
 * 3. Count how many are filled (non-zero)
 * 4. Calculate: (filled / total) × 100
 * 
 * @param board The current board state
 * @param subgrid_idx Index of the subgrid to analyze (0 to N-1)
 * @return Density percentage (0.0 to 100.0)
 * 
 * @example
 * ```c
 * // Calculate density of center subgrid in 9×9 board
 * float density = sudoku_calculate_subgrid_density(board, 4);
 * printf("Center subgrid is %.1f%% full\n", density);
 * 
 * // Output might be: "Center subgrid is 66.7% full"
 * ```
 * 
 * @note This function is pure (no side effects) and can be called repeatedly
 *       without affecting board state.
 */
float sudoku_calculate_subgrid_density(const SudokuBoard *board, int subgrid_idx);

/**
 * @brief Calculates scores for all subgrids, preparing them for prioritization
 * 
 * This is the main function for Phase 2 scoring. It evaluates every subgrid
 * and returns an array of scores that can be sorted and processed in optimal order.
 * 
 * ALGORITHM:
 * 1. Allocate array for N subgrid scores (where N = board_size)
 * 2. For each subgrid:
 *    a. Calculate density using sudoku_calculate_subgrid_density()
 *    b. Count candidate cells (cells without alternatives)
 *    c. Store both in SubGridScore structure
 * 3. Return the array
 * 
 * MEMORY MANAGEMENT:
 * This function allocates memory dynamically using malloc(). The caller is
 * responsible for freeing this memory when done:
 * 
 * ```c
 * int count;
 * SubGridScore *scores = sudoku_score_subgrids(board, config, &count);
 * // ... use scores ...
 * free(scores);  // CRITICAL: Must free to avoid memory leak
 * ```
 * 
 * @param board The current board state
 * @param config Elimination configuration (unused currently, for future extensibility)
 * @param count Output parameter: will be set to the number of scores returned
 * @return Dynamically allocated array of SubGridScore structures
 * @retval NULL if memory allocation fails
 * 
 * @example
 * ```c
 * int num_subgrids;
 * SubGridScore *scores = sudoku_score_subgrids(board, &config, &num_subgrids);
 * 
 * if (scores == NULL) {
 *     fprintf(stderr, "ERROR: Failed to allocate memory for scoring\n");
 *     return false;
 * }
 * 
 * // Process scores...
 * for (int i = 0; i < num_subgrids; i++) {
 *     printf("Subgrid %d: %.1f%% density, %d candidates\n",
 *            scores[i].subgrid_idx, scores[i].density, scores[i].candidates);
 * }
 * 
 * free(scores);
 * ```
 * 
 * @warning The returned array MUST be freed by the caller to avoid memory leaks.
 *          Use free(scores) when done.
 */
SubGridScore* sudoku_score_subgrids(const SudokuBoard *board,
                                    const SudokuEliminationConfig *config,
                                    int *count);

/**
 * @brief Calculates scores for all filled cells, preparing them for Phase 3
 * 
 * This is similar to sudoku_score_subgrids() but operates at the individual
 * cell level rather than subgrid level. It's used in Phase 3 where we need
 * fine-grained control over which specific cells to eliminate.
 * 
 * ALGORITHM:
 * 1. Allocate array for N×N cell scores (worst case: all cells filled)
 * 2. For each cell in the board:
 *    a. Skip if empty
 *    b. Determine which subgrid contains this cell
 *    c. Calculate subgrid density
 *    d. Count alternatives for this cell's number
 *    e. Store all information in CellScore structure
 * 3. Return the array
 * 
 * @param board The current board state
 * @param config Elimination configuration (affects scoring behavior)
 * @param count Output parameter: will be set to the number of cells scored
 * @return Dynamically allocated array of CellScore structures
 * @retval NULL if memory allocation fails
 * 
 * @example
 * ```c
 * int num_cells;
 * CellScore *cell_scores = sudoku_score_cells(board, &config, &num_cells);
 * 
 * if (cell_scores == NULL) {
 *     fprintf(stderr, "ERROR: Failed to allocate memory\n");
 *     return false;
 * }
 * 
 * printf("Analyzed %d filled cells\n", num_cells);
 * 
 * // Find best candidate
 * CellScore *best = &cell_scores[0];
 * for (int i = 1; i < num_cells; i++) {
 *     if (cell_scores[i].density > best->density) {
 *         best = &cell_scores[i];
 *     }
 * }
 * 
 * printf("Best candidate: (%d,%d) with %.1f%% density\n",
 *        best->pos.row, best->pos.col, best->density);
 * 
 * free(cell_scores);
 * ```
 * 
 * @warning The returned array MUST be freed by the caller.
 */
CellScore* sudoku_score_cells(const SudokuBoard *board,
                              const SudokuEliminationConfig *config,
                              int *count);

/**
 * @brief Comparison function for sorting subgrids in descending order (highest score first)
 * 
 * This is a callback function designed to be used with qsort() from the
 * standard library. It defines how to compare two SubGridScore structures.
 * 
 * SORTING CRITERIA (in order of priority):
 * 1. Density (primary): Higher density = higher priority
 * 2. Candidates (tiebreaker): More candidates = higher priority
 * 
 * QSORT INTERFACE:
 * qsort() expects a comparison function with this exact signature:
 *   int compare(const void *a, const void *b)
 * 
 * The function must return:
 *   - Negative value if a should come BEFORE b
 *   - Zero if a and b are equal
 *   - Positive value if a should come AFTER b
 * 
 * @param a Pointer to first SubGridScore (cast from void*)
 * @param b Pointer to second SubGridScore (cast from void*)
 * @return Comparison result: <0 if a>b, 0 if a==b, >0 if a<b
 * 
 * @example
 * ```c
 * SubGridScore scores[9];
 * // ... fill scores ...
 * 
 * // Sort in descending order (highest density first)
 * qsort(scores, 9, sizeof(SubGridScore), sudoku_compare_subgrid_scores_desc);
 * 
 * // Now scores[0] is the highest-scoring subgrid
 * ```
 * 
 * @note This function is specifically for DESCENDING order. For ascending order,
 *       swap the return values or use sudoku_compare_subgrid_scores_asc().
 */
int sudoku_compare_subgrid_scores_desc(const void *a, const void *b);

/**
 * @brief Comparison function for sorting cells in descending order
 * 
 * Similar to sudoku_compare_subgrid_scores_desc() but for CellScore structures.
 * 
 * SORTING CRITERIA:
 * 1. Density (primary): Higher density = higher priority
 * 2. Alternatives (tiebreaker): MORE alternatives = higher priority
 *    (counterintuitive but correct: we WANT to eliminate flexible cells)
 * 
 * WHY MORE ALTERNATIVES IS BETTER:
 * If a cell has many alternatives, eliminating it still leaves the solver
 * with options. If a cell has zero alternatives, it's CRITICAL and must stay.
 * 
 * @param a Pointer to first CellScore
 * @param b Pointer to second CellScore
 * @return Comparison result for descending sort
 * 
 * @example
 * ```c
 * CellScore cell_scores[81];
 * // ... fill scores ...
 * 
 * qsort(cell_scores, 81, sizeof(CellScore), sudoku_compare_cell_scores_desc);
 * 
 * // Now cell_scores[0] is the best candidate for elimination
 * ```
 */
int sudoku_compare_cell_scores_desc(const void *a, const void *b);

#endif /* SUDOKU_DENSITY_SCORING_H */

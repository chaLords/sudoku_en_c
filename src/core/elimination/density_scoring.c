/**
 * @file density_scoring.c
 * @brief Implementation of density-based scoring algorithms
 */

#include "density_scoring.h"
#include "sudoku/core/validation.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * ══════════════════════════════════════════════════════════════════
 *                   HELPER FUNCTION DECLARATIONS
 * ══════════════════════════════════════════════════════════════════
 * 
 * These internal helper functions are not exposed in the header file
 * because they're implementation details. This is a key principle in
 * software engineering called "information hiding" or "encapsulation".
 * 
 * BENEFITS OF HELPER FUNCTIONS:
 * - Break complex logic into manageable pieces
 * - Enable code reuse
 * - Make testing easier
 * - Improve readability
 */

/**
 * @brief Counts how many alternative positions exist for a number
 * 
 * This is a critical helper function that determines whether a cell is
 * a good candidate for elimination. The more alternatives a number has,
 * the safer it is to eliminate that cell.
 * 
 * @param board The board to analyze
 * @param pos Position of the cell
 * @param num The number to check for alternatives
 * @return Number of alternative positions (0 to 26 for 9×9)
 */
static int count_alternatives(const SudokuBoard *board, 
                             const SudokuPosition *pos, 
                             int num);

/*
 * ══════════════════════════════════════════════════════════════════
 *                   PUBLIC FUNCTION IMPLEMENTATIONS
 * ══════════════════════════════════════════════════════════════════
 */

float sudoku_calculate_subgrid_density(const SudokuBoard *board, int subgrid_idx) {
    /*
     * STEP 1: Create a SubGrid object
     * 
     * The SubGrid structure knows how to map from a linear index (0-8 for 9×9)
     * to the actual position on the board. This abstraction hides the messy
     * arithmetic from us.
     */
    SudokuSubGrid sg = sudoku_subgrid_create(subgrid_idx, board->subgrid_size);
    
    /*
     * STEP 2: Get the size of the subgrid
     * 
     * For a 9×9 board: board_size = 9 (each subgrid has 9 cells)
     * For a 4×4 board: board_size = 4 (each subgrid has 4 cells)
     * For a 16×16 board: board_size = 16 (each subgrid has 16 cells)
     * 
     * This dynamic sizing is what makes the code work for any board size.
     */
    int board_size = sudoku_board_get_board_size(board);
    
    /*
     * STEP 3: Count filled cells
     * 
     * We iterate through every cell in the subgrid and count how many
     * have numbers (are non-zero).
     */
    int filled = 0;
    
    for (int i = 0; i < board_size; i++) {
        // Get the absolute position of this cell
        SudokuPosition pos = sudoku_subgrid_get_position(&sg, i);
        
        // Is there a number in this cell?
        if (board->cells[pos.row][pos.col] != 0) {
            filled++;
        }
    }
    
    /*
     * STEP 4: Calculate density percentage
     * 
     * The formula is: (filled / total) × 100
     * 
     * We cast filled to float to ensure floating-point division.
     * Without the cast, integer division would truncate (5/9 = 0 instead of 0.555).
     * 
     * EXAMPLE:
     * - filled = 6 cells
     * - board_size = 9 cells
     * - density = (6.0 / 9) × 100 = 66.67%
     */
    return ((float)filled / board_size) * 100.0f;
}

SubGridScore* sudoku_score_subgrids(const SudokuBoard *board,
                                    const SudokuEliminationConfig *config,
                                    int *count) {
    /*
     * STEP 1: Determine how many subgrids exist
     * 
     * For a 9×9 board: 9 subgrids (3×3 grid of subgrids)
     * For a 4×4 board: 4 subgrids (2×2 grid of subgrids)
     * 
     * The number of subgrids always equals board_size.
     */
    int board_size = sudoku_board_get_board_size(board);
    int num_subgrids = board_size;
    
    /*
     * STEP 2: Allocate memory for the scores array
     * 
     * MEMORY MANAGEMENT LESSON:
     * 
     * We use malloc() to allocate memory on the HEAP because:
     * - The array size is only known at runtime
     * - The caller needs the array to persist after this function returns
     * - Stack allocation would be freed when the function exits
     * 
     * The caller is responsible for calling free() when done.
     */
    SubGridScore *scores = malloc(num_subgrids * sizeof(SubGridScore));
    
    /*
     * DEFENSIVE PROGRAMMING: Check for allocation failure
     * 
     * malloc() returns NULL if it cannot allocate memory. This is rare
     * on modern systems but can happen if:
     * - The system is out of memory
     * - We requested an unreasonably large amount
     * - There's memory corruption elsewhere
     * 
     * Always check malloc() return values in production code.
     */
    if (scores == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate memory for subgrid scores\n");
        *count = 0;
        return NULL;
    }
    
    /*
     * STEP 3: Calculate score for each subgrid
     * 
     * This is the heart of the function. For each subgrid, we:
     * 1. Calculate its density
     * 2. Count how many candidate cells it has
     * 3. Store both pieces of information
     */
    for (int idx = 0; idx < num_subgrids; idx++) {
        // Store the subgrid index
        scores[idx].subgrid_idx = idx;
        
        // Calculate density using our dedicated function
        scores[idx].density = sudoku_calculate_subgrid_density(board, idx);
        
        /*
         * Count candidate cells in this subgrid
         * 
         * A cell is a "candidate" if:
         * - It currently has a number (is filled)
         * - That number has NO alternatives elsewhere
         * 
         * WHY NO ALTERNATIVES:
         * If a number has alternatives, eliminating it might not affect
         * solvability (the solver can put the number elsewhere). If it
         * has NO alternatives, it's the only place that number can go,
         * making it potentially safe to eliminate.
         */
        int candidates = 0;
        SudokuSubGrid sg = sudoku_subgrid_create(idx, board->subgrid_size);
        
        // Check each cell in this subgrid
        for (int i = 0; i < board_size; i++) {
            SudokuPosition pos = sudoku_subgrid_get_position(&sg, i);
            int num = board->cells[pos.row][pos.col];
            
            // Skip empty cells
            if (num == 0) continue;
            
            /*
             * Check if this number has alternatives
             * 
             * We use count_alternatives() helper function which checks
             * if the number can be placed in any other empty cell within
             * the same row, column, or subgrid.
             */
            int alts = count_alternatives(board, &pos, num);
            
            // If zero alternatives, it's a candidate
            if (alts == 0) {
                candidates++;
            }
        }
        
        scores[idx].candidates = candidates;
    }
    
    /*
     * STEP 4: Return results
     * 
     * We use an output parameter (count) to return the array size.
     * This is a common C pattern for returning both data and metadata.
     */
    *count = num_subgrids;
    return scores;
}

CellScore* sudoku_score_cells(const SudokuBoard *board,
                              const SudokuEliminationConfig *config,
                              int *count) {
    /*
     * STEP 1: Calculate maximum possible cells
     * 
     * In the worst case, ALL cells are filled, so we need space for N² scores.
     */
    int board_size = sudoku_board_get_board_size(board);
    int max_cells = board_size * board_size;
    
    /*
     * STEP 2: Allocate memory for cell scores
     * 
     * We allocate for max_cells even though we might use fewer.
     * This is a trade-off:
     * - PRO: Simple allocation, no reallocation needed
     * - CON: Might waste some memory temporarily
     * 
     * For a 9×9 board: 81 × sizeof(CellScore) = 81 × 20 = 1,620 bytes
     * This is small enough that the waste is negligible.
     */
    CellScore *cell_scores = malloc(max_cells * sizeof(CellScore));
    
    if (cell_scores == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate memory for cell scores\n");
        *count = 0;
        return NULL;
    }
    
    /*
     * STEP 3: Iterate through ALL cells and score filled ones
     * 
     * We use a count variable to track how many cells we actually scored.
     * This will be less than max_cells because some cells are empty.
     */
    int scored = 0;
    
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            int num = board->cells[row][col];
            
            // Skip empty cells
            if (num == 0) continue;
            
            /*
             * STEP 3a: Store position and value
             */
            cell_scores[scored].pos.row = row;
            cell_scores[scored].pos.col = col;
            cell_scores[scored].value = num;
            
            /*
             * STEP 3b: Determine which subgrid contains this cell
             * 
             * FORMULA:
             * subgrid_idx = (row / subgrid_size) × (board_size / subgrid_size) + 
             *               (col / subgrid_size)
             * 
             * For a 9×9 board (subgrid_size=3):
             * - Cell (4,5) is in subgrid: (4/3)×3 + (5/3) = 1×3 + 1 = 4
             * 
             * EXPLANATION:
             * - (row / subgrid_size) gives us the subgrid row (0, 1, or 2)
             * - (col / subgrid_size) gives us the subgrid column (0, 1, or 2)
             * - We convert 2D subgrid coordinates to 1D index
             */
            int subgrid_row = row / board->subgrid_size;
            int subgrid_col = col / board->subgrid_size;
            int subgrid_idx = subgrid_row * (board_size / board->subgrid_size) + subgrid_col;
            
            /*
             * STEP 3c: Get density from parent subgrid
             * 
             * Instead of recalculating for every cell, we could optimize this
             * by calculating all subgrid densities once and caching them.
             * However, the current approach is clearer and the performance
             * difference is negligible (densities are cheap to calculate).
             */
            cell_scores[scored].density = sudoku_calculate_subgrid_density(board, subgrid_idx);
            
            /*
             * STEP 3d: Count alternatives
             * 
             * This is the most expensive operation in scoring because it
             * requires checking up to 27 positions (9 row + 9 col + 9 subgrid).
             */
            SudokuPosition pos = {row, col};
            cell_scores[scored].alternatives = count_alternatives(board, &pos, num);
            
            scored++;
        }
    }
    
    /*
     * STEP 4: Return results
     * 
     * We return the full array but set *count to the actual number of
     * cells scored. The caller should only use indices [0, count).
     */
    *count = scored;
    return cell_scores;
}

int sudoku_compare_subgrid_scores_desc(const void *a, const void *b) {
    /*
     * STEP 1: Cast void pointers to specific types
     * 
     * qsort() passes void* because it's generic, but we know these are
     * actually SubGridScore pointers. We cast them to access the fields.
     * 
     * CONST CORRECTNESS:
     * The parameters are const void* because qsort() doesn't modify the
     * elements, only compares them. We preserve this const-ness.
     */
    const SubGridScore *sa = (const SubGridScore *)a;
    const SubGridScore *sb = (const SubGridScore *)b;
    
    /*
     * STEP 2: Compare by density (primary criterion)
     * 
     * DESCENDING ORDER LOGIC:
     * - If sa->density > sb->density, return NEGATIVE (a comes before b)
     * - If sa->density < sb->density, return POSITIVE (b comes before a)
     * - If equal, fall through to tiebreaker
     * 
     * This seems backwards from natural ordering, but it's correct for
     * descending sort. Think of it as "negative difference" means "a wins".
     */
    if (sa->density > sb->density) return -1;
    if (sa->density < sb->density) return 1;
    
    /*
     * STEP 3: Tiebreaker - compare by candidates
     * 
     * If densities are equal, we prefer the subgrid with MORE candidates.
     * This gives us more flexibility in elimination.
     * 
     * SHORTCUT:
     * Instead of if/else, we can directly return the difference:
     * - If sb->candidates > sa->candidates: difference is positive (b wins)
     * - If sb->candidates < sa->candidates: difference is negative (a wins)
     * - If equal: difference is zero (tie)
     * 
     * Note the order: sb - sa (not sa - sb) because we want MORE candidates first.
     */
    return sb->candidates - sa->candidates;
}

int sudoku_compare_cell_scores_desc(const void *a, const void *b) {
    const CellScore *ca = (const CellScore *)a;
    const CellScore *cb = (const CellScore *)b;
    
    /*
     * PRIMARY: Compare by density (higher first)
     */
    if (ca->density > cb->density) return -1;
    if (ca->density < cb->density) return 1;
    
    /*
     * TIEBREAKER: Compare by alternatives (MORE is better)
     * 
     * COUNTERINTUITIVE BUT CORRECT:
     * We want cells with MORE alternatives to come first because they're
     * better candidates for elimination. If a cell has many alternatives,
     * removing it still gives the solver options.
     * 
     * EXAMPLE:
     * Cell A: 8 alternatives (very flexible, good to remove)
     * Cell B: 0 alternatives (critical, must keep)
     * 
     * We want A to come before B, so we compare cb - ca (larger alternatives first).
     */
    return cb->alternatives - ca->alternatives;
}

/*
 * ══════════════════════════════════════════════════════════════════
 *                   HELPER FUNCTION IMPLEMENTATIONS
 * ══════════════════════════════════════════════════════════════════
 */

static int count_alternatives(const SudokuBoard *board,
                             const SudokuPosition *pos,
                             int num) {
    /*
     * CRITICAL: Temporarily remove the number
     * 
     * We need to check if the number can go elsewhere, so we temporarily
     * set this cell to empty. This allows isSafePosition() to consider
     * this cell as a potential placement location.
     * 
     * IMPORTANT: We MUST restore the value before returning.
     */
    int temp = board->cells[pos->row][pos->col];
    board->cells[pos->row][pos->col] = 0;
    
    int alternatives = 0;
    int board_size = sudoku_board_get_board_size(board);
    
    /*
     * CHECK 1: Search in the same row
     * 
     * Look for empty cells in this row where the number could legally go.
     */
    for (int c = 0; c < board_size; c++) {
        // Skip the original position
        if (c == pos->col) continue;
        
        // Skip filled cells
        if (board->cells[pos->row][c] != 0) continue;
        
        // Can the number go here?
        SudokuPosition test = {pos->row, c};
        if (sudoku_is_safe_position(board, &test, num)) {
            alternatives++;
        }
    }
    
    /*
     * CHECK 2: Search in the same column
     */
    for (int r = 0; r < board_size; r++) {
        if (r == pos->row) continue;
        if (board->cells[r][pos->col] != 0) continue;
        
        SudokuPosition test = {r, pos->col};
        if (sudoku_is_safe_position(board, &test, num)) {
            alternatives++;
        }
    }
    
    /*
     * CHECK 3: Search in the same subgrid
     * 
     * This is more complex because we need to iterate through the subgrid,
     * which requires calculating the base position and iterating through
     * the local cells.
     */
    int subgrid_size = board->subgrid_size;
    int row_start = (pos->row / subgrid_size) * subgrid_size;
    int col_start = (pos->col / subgrid_size) * subgrid_size;
    
    for (int i = 0; i < subgrid_size; i++) {
        for (int j = 0; j < subgrid_size; j++) {
            int r = row_start + i;
            int c = col_start + j;
            
            // Skip original position
            if (r == pos->row && c == pos->col) continue;
            
            // Skip filled cells
            if (board->cells[r][c] != 0) continue;
            
            /*
             * IMPORTANT CHECK: Avoid double-counting
             * 
             * This cell might have already been counted in the row or column
             * checks. We need to skip it if it's in the same row or column.
             * 
             * WHY THIS MATTERS:
             * Without this check, a cell that's in the same row AND subgrid
             * would be counted twice, inflating the alternatives count.
             */
            if (r == pos->row || c == pos->col) continue;
            
            SudokuPosition test = {r, c};
            if (sudoku_is_safe_position(board, &test, num)) {
                alternatives++;
            }
        }
    }
    
    /*
     * CRITICAL: Restore the original value
     * 
     * We modified board state temporarily. Now we MUST restore it.
     * Forgetting this would corrupt the board and cause subtle bugs.
     */
    board->cells[pos->row][pos->col] = temp;
    
    return alternatives;
}

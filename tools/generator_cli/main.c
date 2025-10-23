#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

/*
 * Sudoku Generator - Hybrid Fisher-Yates + Backtracking
 * Version 2.2.1 - Structure-Based Refactoring
 * 
 * Copyright 2025 Gonzalo Ramírez (@chaLords)
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * VERBOSITY SYSTEM FOR SUDOKU GENERATOR
 * 
 * DISPLAY MODES:
 * 0 = MINIMAL    - Only title and sudoku with difficulty
 * 1 = COMPACT    - Main phases summarized + sudoku + statistics (default)
 * 2 = DETAILED   - All complete information (full debugging output)
 * 
 * USAGE:
 * Change the VERBOSITY_LEVEL value as needed, or pass as command line argument:
 * - For clean presentations: use 0
 * - For development/testing: use 1
 * - For complete debugging: use 2
 * 
 * Examples:
 *   ./sudoku 0    - Minimal output
 *   ./sudoku 1    - Compact output (default)
 *   ./sudoku 2    - Detailed debugging output
 */

#define SIZE 9
#define SUBGRID_SIZE 3
#define TOTAL_CELLS (SIZE * SIZE)
#define PHASE3_TARGET 25

// ═══════════════════════════════════════════════════════════════════
//                    DATA STRUCTURES
// ═══════════════════════════════════════════════════════════════════

/**
 * Structure: Position
 * Represents a position (row, column) on the board
 * Purpose: Avoids passing multiple row/col parameters separately
 */
typedef struct {
    int row;
    int col;
} Position;

/**
 * Structure: SudokuBoard
 * Encapsulates the 9x9 board and its metadata
 * Advantage: All operations work on this single structure
 */
typedef struct {
    int cells[SIZE][SIZE];  // The board itself
    int clues;              // Number of clues (filled cells)
    int empty;              // Number of empty cells
} SudokuBoard;

/**
 * Structure: SubGrid
 * Defines a 3x3 region of the board
 * Includes: index, base position, and access method
 */
typedef struct {
    int index;          // 0-8, subgrid index
    Position base;      // Top-left corner
} SubGrid;

/**
 * Structure: GenerationStats
 * Stores statistics from the generation process
 */
typedef struct {
    int phase1_removed;
    int phase2_removed;
    int phase2_rounds;
    int phase3_removed;
    int total_attempts;
} GenerationStats;

// ═══════════════════════════════════════════════════════════════════
//                    GLOBAL CONSTANTS
// ═══════════════════════════════════════════════════════════════════

static const int DIAGONAL_INDICES[3] = {0, 4, 8};
static const int ALL_INDICES[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

int VERBOSITY_LEVEL = 1;
// ═══════════════════════════════════════════════════════════════════
//                    FORWARD DECLARATIONS
// ═══════════════════════════════════════════════════════════════════

// Initialization functions
void initBoard(SudokuBoard *board);
void updateBoardStats(SudokuBoard *board);
SubGrid createSubGrid(int index);
Position getPositionInSubGrid(const SubGrid *sg, int cell_index);

// Fisher-Yates algorithm
void fisherYatesShuffle(int *array, int size, int start_value);

// Validation functions
bool isSafePosition(const SudokuBoard *board, const Position *pos, int num);
bool findEmptyCell(const SudokuBoard *board, Position *pos);
int countSolutionsExact(SudokuBoard *board, int limit);

// Sudoku generation
void fillSubGrid(SudokuBoard *board, const SubGrid *sg);
void fillDiagonal(SudokuBoard *board);
bool completeSudoku(SudokuBoard *board);

// Cell elimination phases
bool hasAlternative(SudokuBoard *board, const Position *pos, int num);
int phase1Elimination(SudokuBoard *board, const int *indices, int count);
int phase2Elimination(SudokuBoard *board, const int *indices, int count);
int phase3Elimination(SudokuBoard *board, int target);

// Main generation function
bool generateSudoku(SudokuBoard *board, GenerationStats *stats);

// Utilities and visualization
const char* evaluateDifficulty(const SudokuBoard *board);
void printBoard(const SudokuBoard *board);
bool validateBoard(const SudokuBoard *board);

// ═══════════════════════════════════════════════════════════════════
//                    INITIALIZATION FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * Initializes an empty board
 * Pointer usage: Modifies the board directly without copying
 */
void initBoard(SudokuBoard *board) {
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            board->cells[i][j] = 0;
        }
    }
    board->clues = 0;
    board->empty = TOTAL_CELLS;
}

/**
 * Updates the clue and empty cell counters
 */
void updateBoardStats(SudokuBoard *board) {
    int count = 0;
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            if(board->cells[i][j] != 0) count++;
        }
    }
    board->clues = count;
    board->empty = TOTAL_CELLS - count;
}

/**
 * Creates a SubGrid from an index (0-8)
 * Automatically calculates the base position
 */
SubGrid createSubGrid(int index) {
    SubGrid sg;
    sg.index = index;
    sg.base.row = (index / 3) * 3;
    sg.base.col = (index % 3) * 3;
    return sg;
}

/**
 * Gets a Position within a SubGrid
 * @param sg: The subgrid
 * @param cell_index: index 0-8 within the subgrid
 */
Position getPositionInSubGrid(const SubGrid *sg, int cell_index) {
    Position pos;
    pos.row = sg->base.row + (cell_index / 3);
    pos.col = sg->base.col + (cell_index % 3);
    return pos;
}

// ═══════════════════════════════════════════════════════════════════
//                    FISHER-YATES ALGORITHM
// ═══════════════════════════════════════════════════════════════════

/**
 * Shuffles an array using Fisher-Yates algorithm
 * Parameters:
 * - array: pointer to the array to shuffle (modifies in-place)
 * - size: array size
 * - start_value: initial value to fill (usually 1)
 */
void fisherYatesShuffle(int *array, int size, int start_value) {
    // Fill consecutively
    for(int i = 0; i < size; i++) {
        array[i] = start_value + i;
    }
    
    // Shuffle (Fisher-Yates backward)
    for(int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        // Swap using XOR arithmetic (educational alternative)
        if(i != j) {
            array[i] ^= array[j];
            array[j] ^= array[i];
            array[i] ^= array[j];
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
//                    VALIDATION FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * Checks if it's safe to place a number at a position
 * const usage: The board is not modified in this function
 */
bool isSafePosition(const SudokuBoard *board, const Position *pos, int num) {
    // Check row
    for(int x = 0; x < SIZE; x++) {
        if(board->cells[pos->row][x] == num) return false;
    }
    
    // Check column
    for(int x = 0; x < SIZE; x++) {
        if(board->cells[x][pos->col] == num) return false;
    }
    
    // Check 3x3 subgrid
    int rowStart = pos->row - (pos->row % SUBGRID_SIZE);
    int colStart = pos->col - (pos->col % SUBGRID_SIZE);
    
    for(int i = 0; i < SUBGRID_SIZE; i++) {
        for(int j = 0; j < SUBGRID_SIZE; j++) {
            if(board->cells[rowStart + i][colStart + j] == num) {
                return false;
            }
        }
    }
    
    return true;
}

/**
 * Finds the first empty cell
 * @param board: board to inspect (const because it's not modified)
 * @param pos: pointer where to store the found position
 * @return: true if empty cell found, false if complete
 */
bool findEmptyCell(const SudokuBoard *board, Position *pos) {
    for(pos->row = 0; pos->row < SIZE; pos->row++) {
        for(pos->col = 0; pos->col < SIZE; pos->col++) {
            if(board->cells[pos->row][pos->col] == 0) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Counts solutions using exhaustive backtracking
 * Stops searching after finding 'limit' solutions
 */
int countSolutionsExact(SudokuBoard *board, int limit) {
    Position pos;
    
    if(!findEmptyCell(board, &pos)) {
        return 1; // Complete solution found
    }
    
    int totalSolutions = 0;
    
    for(int num = 1; num <= SIZE; num++) {
        if(isSafePosition(board, &pos, num)) {
            board->cells[pos.row][pos.col] = num;
            
            totalSolutions += countSolutionsExact(board, limit);
            
            if(totalSolutions >= limit) {
                board->cells[pos.row][pos.col] = 0;
                return totalSolutions;
            }
            
            board->cells[pos.row][pos.col] = 0;
        }
    }
    
    return totalSolutions;
}

// ═══════════════════════════════════════════════════════════════════
//                    SUDOKU GENERATION
// ═══════════════════════════════════════════════════════════════════

/**
 * Fills a specific subgrid with Fisher-Yates
 */
void fillSubGrid(SudokuBoard *board, const SubGrid *sg) {
    int numbers[SIZE];
    fisherYatesShuffle(numbers, SIZE, 1);
    
    if(VERBOSITY_LEVEL == 2) {
        printf("   SubGrid %d (base: %d,%d): ", 
               sg->index, sg->base.row, sg->base.col);
    }
    
    for(int i = 0; i < SIZE; i++) {
        Position pos = getPositionInSubGrid(sg, i);
        board->cells[pos.row][pos.col] = numbers[i];
        
        if(VERBOSITY_LEVEL == 2) {
            printf("%d ", numbers[i]);
        }
    }
    
    if(VERBOSITY_LEVEL == 2) {
        printf("\n");
    }
}

/**
 * Fills the diagonal subgrids (0, 4, 8)
 * These are independent and don't have conflicts with each other
 */
void fillDiagonal(SudokuBoard *board) {
    if(VERBOSITY_LEVEL == 2) {
        printf("🎲 Filling diagonal with Fisher-Yates...\n");
    } else if(VERBOSITY_LEVEL == 1) {
        printf("🎲 Diagonal + Backtracking...");
        fflush(stdout);
    }
    
    for(int i = 0; i < 3; i++) {
        SubGrid sg = createSubGrid(DIAGONAL_INDICES[i]);
        fillSubGrid(board, &sg);
    }
    
    if(VERBOSITY_LEVEL == 2) {
        printf("✅ Diagonal successfully filled!\n\n");
    }
}

/**
 * Completes the board using recursive backtracking
 */
bool completeSudoku(SudokuBoard *board) {
    Position pos;
    
    if(!findEmptyCell(board, &pos)) {
        return true; // Complete board
    }
    
    // Array of numbers to try (1-9) shuffled
    int numbers[SIZE];
    fisherYatesShuffle(numbers, SIZE, 1);
    
    for(int i = 0; i < SIZE; i++) {
        int num = numbers[i];
        
        if(isSafePosition(board, &pos, num)) {
            board->cells[pos.row][pos.col] = num;
            
            if(completeSudoku(board)) {
                return true;
            }
            
            board->cells[pos.row][pos.col] = 0; // Backtrack
        }
    }
    
    return false;
}

// ═══════════════════════════════════════════════════════════════════
//                    CELL ELIMINATION (3 PHASES)
// ═══════════════════════════════════════════════════════════════════

/**
 * Checks if a number has alternatives in its row/column/subgrid
 */
bool hasAlternative(SudokuBoard *board, const Position *pos, int num) {
    int temp = board->cells[pos->row][pos->col];
    board->cells[pos->row][pos->col] = 0;
    
    int alternatives = 0;
    
    // Search in row
    for(int x = 0; x < SIZE && alternatives == 0; x++) {
        if(x != pos->col && board->cells[pos->row][x] == 0) {
            Position testPos = {pos->row, x};
            if(isSafePosition(board, &testPos, num)) {
                alternatives++;
            }
        }
    }
    
    // Search in column
    for(int x = 0; x < SIZE && alternatives == 0; x++) {
        if(x != pos->row && board->cells[x][pos->col] == 0) {
            Position testPos = {x, pos->col};
            if(isSafePosition(board, &testPos, num)) {
                alternatives++;
            }
        }
    }
    
    // Search in subgrid
    if(alternatives == 0) {
        int rowStart = pos->row - (pos->row % SUBGRID_SIZE);
        int colStart = pos->col - (pos->col % SUBGRID_SIZE);
        
        for(int i = 0; i < SUBGRID_SIZE && alternatives == 0; i++) {
            for(int j = 0; j < SUBGRID_SIZE && alternatives == 0; j++) {
                int r = rowStart + i;
                int c = colStart + j;
                
                if((r != pos->row || c != pos->col) && board->cells[r][c] == 0) {
                    Position testPos = {r, c};
                    if(isSafePosition(board, &testPos, num)) {
                        alternatives++;
                    }
                }
            }
        }
    }
    
    board->cells[pos->row][pos->col] = temp;
    return alternatives > 0;
}

/**
 * PHASE 1: Removes one random number from each subgrid
 */
int phase1Elimination(SudokuBoard *board, const int *indices, int count) {
    if(VERBOSITY_LEVEL == 2) {
        printf("🎲 PHASE 1: Selecting numbers per subgrid with Fisher-Yates...\n");
    } else if(VERBOSITY_LEVEL == 1) {
        printf("🎲 Phase 1: Fisher-Yates selection...");
        fflush(stdout);
    }
    
    int numbers[SIZE];
    fisherYatesShuffle(numbers, SIZE, 1);
    int removed = 0;
    
    for(int idx = 0; idx < count; idx++) {
        SubGrid sg = createSubGrid(indices[idx]);
        int initial_row = sg.base.row;
        int initial_column = sg.base.col;
        
        if(VERBOSITY_LEVEL == 2) {
            printf("   Subgrid %d (base: %d,%d): ", sg.index, initial_row, initial_column);
        }

        int targetValue = numbers[idx];
        
        // Find and remove the target value
        for(int i = 0; i < SIZE; i++) {
            Position pos = getPositionInSubGrid(&sg, i);
            
            if(board->cells[pos.row][pos.col] == targetValue) {
                board->cells[pos.row][pos.col] = 0;
                removed++;
                
                if(VERBOSITY_LEVEL == 2) {
                    printf("%d ", targetValue);
                }
                break;
            }
        }
        
        if(VERBOSITY_LEVEL == 2) {
            printf("\n");
        }
    }
    
    if(VERBOSITY_LEVEL >= 1) {
        printf("✅ Phase 1 completed!\n");
    }
    
    if(VERBOSITY_LEVEL >= 1) {
        printf("📊 PHASE 1 TOTAL: Removed %d cells\n\n", removed);
    }
    
    return removed;
}

/**
 * PHASE 2: Removes numbers without alternatives
 */
int phase2Elimination(SudokuBoard *board, const int *indices, int count) {
    if(VERBOSITY_LEVEL == 2) {
        printf("🎲 PHASE 2: Selecting numbers without alternatives...\n");
    }

    int removed = 0;
    
    for(int idx = 0; idx < count; idx++) {
        SubGrid sg = createSubGrid(indices[idx]);
        int initial_row = sg.base.row;
        int initial_column = sg.base.col;
        
        if(VERBOSITY_LEVEL == 2) {
            printf("   Subgrid %d (base: %d,%d): ", sg.index, initial_row, initial_column);
        }
        
        for(int i = 0; i < SIZE; i++) {
            Position pos = getPositionInSubGrid(&sg, i);
            
            if(board->cells[pos.row][pos.col] != 0) {
                int num = board->cells[pos.row][pos.col];
                
                if(!hasAlternative(board, &pos, num)) {
                    board->cells[pos.row][pos.col] = 0;

                    if(VERBOSITY_LEVEL == 2) {
                        printf("%d ", num);
                    }

                    removed++;
                    break; // Only one per subgrid per round
                }
            }
        }
        
        if(VERBOSITY_LEVEL == 2) {
            printf("\n");
        }
    }
    
    if(VERBOSITY_LEVEL == 2) {
        printf("✅ Phase 2 completed! Removed: %d\n\n", removed);
    }

    return removed;
}

/**
 * PHASE 3: Free elimination with unique solution verification
 * Uses dynamic memory for the positions array
 */
int phase3Elimination(SudokuBoard *board, int target) {
    if(VERBOSITY_LEVEL == 2) {
        printf("🎲 PHASE 3: Free elimination with unique solution verification...\n");
    } else if(VERBOSITY_LEVEL == 1) {
        printf("🎲 Phase 3: Free elimination...");
        fflush(stdout);
    }
    
    // Dynamic memory for positions
    Position *positions = (Position *)malloc(TOTAL_CELLS * sizeof(Position));
    if(positions == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 0;
    }
    
    int count = 0;
    
    // Collect positions with numbers
    for(int r = 0; r < SIZE; r++) {
        for(int c = 0; c < SIZE; c++) {
            if(board->cells[r][c] != 0) {
                positions[count].row = r;
                positions[count].col = c;
                count++;
            }
        }
    }
    
    // Shuffle positions
    for(int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Position temp = positions[i];
        positions[i] = positions[j];
        positions[j] = temp;
    }
    
    int removed = 0;
    
    // Try to remove in random order
    for(int i = 0; i < count && removed < target; i++) {
        Position *pos = &positions[i];
        int row = positions[i].row;
        int col = positions[i].col;
        int temp = board->cells[pos->row][pos->col];
        
        board->cells[pos->row][pos->col] = 0;
        
        if(countSolutionsExact(board, 2) == 1) {
            removed++;
            
            if(VERBOSITY_LEVEL == 2) {
                printf("   Removed %d at memAddr: %p (%d,%d) - Total: %d\n", 
                       temp, (void *)pos, row, col, removed);
            }
        } else {
            board->cells[pos->row][pos->col] = temp;
        }
    }
    
    free(positions); // Free memory
    
    if(VERBOSITY_LEVEL >= 1) {
        printf("✅ Phase 3 completed! Removed: %d\n", removed);
    }
    
    return removed;
}

// ═══════════════════════════════════════════════════════════════════
//                    MAIN GENERATION FUNCTION
// ═══════════════════════════════════════════════════════════════════

/**
 * Generates a playable Sudoku using the hybrid method
 */
bool generateSudoku(SudokuBoard *board, GenerationStats *stats) {
    initBoard(board);
    memset(stats, 0, sizeof(GenerationStats));
    
    // STEP 1: Fill diagonal
    fillDiagonal(board);
    
    // STEP 2: Complete with backtracking
    if(VERBOSITY_LEVEL == 2) {
        printf("🔄 Backtracking in progress...\n");
    }
    
    if(!completeSudoku(board)) {
        return false;
    }
    
    if(VERBOSITY_LEVEL >= 1) {
        printf("✅ Completed!\n");
    }
    
    // STEP 3: Phase 1
    stats->phase1_removed = phase1Elimination(board, ALL_INDICES, 9);
    
    // STEP 4: Phase 2 (loop)
    if(VERBOSITY_LEVEL == 1) {
        printf("🎲 Phase 2: Removal rounds...");
        fflush(stdout);
    }
    
    int round = 1;
    int removed;

    do {
        if(VERBOSITY_LEVEL == 2) {
            printf("--- ROUND %d ---\n", round);
        }

        removed = phase2Elimination(board, ALL_INDICES, 9);
        stats->phase2_removed += removed;
        if(removed > 0) stats->phase2_rounds++;
        round++;

    } while(removed > 0);
    
    if(VERBOSITY_LEVEL == 2) {
        printf("🛑 Cannot remove more numbers in PHASE 2\n\n");
    }
    
    if(VERBOSITY_LEVEL >= 1) {
        printf("\n📊 PHASE 2 TOTAL: %d rounds, removed %d cells\n\n", 
               stats->phase2_rounds, stats->phase2_removed);
    }
    
    // STEP 5: Phase 3
    stats->phase3_removed = phase3Elimination(board, PHASE3_TARGET);
    
    updateBoardStats(board);
    return true;
}
 
// ═══════════════════════════════════════════════════════════════════
//                    UTILITIES AND VISUALIZATION
// ═══════════════════════════════════════════════════════════════════

/**
 * Evaluates difficulty based on number of clues
 */
const char* evaluateDifficulty(const SudokuBoard *board) {
    if(board->clues >= 45) return "EASY";
    else if(board->clues >= 35) return "MEDIUM";
    else if(board->clues >= 25) return "HARD";
    else return "EXPERT";
}

/**
 * Prints the board with visual formatting
 */
void printBoard(const SudokuBoard *board) {
    printf("┌───────┬───────┬───────┐\n");
    for(int i = 0; i < SIZE; i++) {
        printf("│");
        for(int j = 0; j < SIZE; j++) {
            if(board->cells[i][j] == 0) {
                printf(" .");
            } else {
                printf(" %d", board->cells[i][j]);
            }
            if((j + 1) % 3 == 0) printf(" │");
        }
        printf("\n");
        if((i + 1) % 3 == 0 && i < 8) {
            printf("├───────┼───────┼───────┤\n");
        }
    }
    printf("└───────┴───────┴───────┘\n");
    
    if(VERBOSITY_LEVEL >= 1) {
        printf("📊 Empty: %d | Clues: %d\n", board->empty, board->clues);
    }
}

/**
 * Validates that the board has no conflicts
 */
bool validateBoard(const SudokuBoard *board) {
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            if(board->cells[i][j] == 0) continue;
            
            Position pos = {i, j};
            int num = board->cells[i][j];
            
            // Create a temporary copy for validation
            SudokuBoard temp = *board;
            temp.cells[i][j] = 0;
            
            if(!isSafePosition(&temp, &pos, num)) {
                return false;
            }
        }
    }
    return true;
}

// ═══════════════════════════════════════════════════════════════════
//                           MAIN
// ═══════════════════════════════════════════════════════════════════

int main(int argc, char *argv[]) {
    #ifdef _WIN32
        system("chcp 65001 > nul");  // UTF-8 on Windows
    #endif
    
    srand(time(NULL));
    
    // Parse arguments
    if(argc > 1) {
        int level = atoi(argv[1]);
        if(level >= 0 && level <= 2) {
            VERBOSITY_LEVEL = level;
        } else {
            printf("❌ Invalid verbosity level: %s\n", argv[1]);
            printf("\nUsage: %s [level]\n", argv[0]);
            printf("  level: 0 (minimal), 1 (compact - default), 2 (detailed)\n\n");
            printf("Examples:\n");
            printf("  %s       - Run with default mode (compact)\n", argv[0]);
            printf("  %s 0     - Run in minimal mode\n", argv[0]);
            printf("  %s 1     - Run in compact mode\n", argv[0]);
            printf("  %s 2     - Run in detailed mode\n", argv[0]);
            printf("  time %s 0 - Run in minimal mode with timing\n", argv[0]);
            return 1;
        }
    }
    
    // Allocate memory for the board (educational: demonstrates malloc)
    SudokuBoard *board = (SudokuBoard *)malloc(sizeof(SudokuBoard));
    if(board == NULL) {
        fprintf(stderr, "❌ Error: Could not allocate memory for the board\n");
        return 1;
    }
    
    GenerationStats stats;
    
    // Display title based on verbosity level
    if(VERBOSITY_LEVEL == 0 || VERBOSITY_LEVEL == 1) {
        printf("═══════════════════════════════════════════════════════════════\n");
        printf("  SUDOKU GENERATOR v2.2.1 – STRUCTURE-BASED REFACTORING\n");
        printf("═══════════════════════════════════════════════════════════════\n\n");
    } else if(VERBOSITY_LEVEL == 2) {
        printf("═══════════════════════════════════════════════════════════════\n");
        printf("    SUDOKU GENERATOR v2.2.1 – STRUCTURE-BASED REFACTORING\n");
        printf("           Fisher-Yates + Backtracking + 3 Phases\n");
        printf("═══════════════════════════════════════════════════════════════\n\n");
    }

    bool success = false;
    
    // Try up to 5 attempts to generate a valid Sudoku
    for(int attempt = 1; attempt <= 5; attempt++) {
        if(VERBOSITY_LEVEL >= 1) {
            printf("🚀 ATTEMPT #%d:\n", attempt);
        }
        
        if(generateSudoku(board, &stats)) {
            if(VERBOSITY_LEVEL >= 1) {
                printf("✅ SUCCESS! Sudoku generated\n\n");
            }
            
            printBoard(board);
            printf("\n");
            
            if(validateBoard(board)) {
                if(VERBOSITY_LEVEL >= 1) {
                    printf("🎉 VERIFIED! The puzzle is valid\n");
                }
            }
            
            if(VERBOSITY_LEVEL <= 2) {
                printf("\n📊 Difficulty level: %s\n", evaluateDifficulty(board));
            }
            
            success = true;
            break;
        } else {
            // Only in DETAILED mode show each failure
            if(VERBOSITY_LEVEL == 2) {
                printf("❌ Failed (very rare with this method)\n\n");
            }
        }
    }
    
    // Free memory
    free(board);
    
    if(!success) {
        fprintf(stderr, "\n❌ ERROR: Could not generate a valid Sudoku after multiple attempts!\n");
        return 1;
    }
    
    return 0;
}

/*
 * ═══════════════════════════════════════════════════════════════════
 *                    EDUCATIONAL CONCEPTS APPLIED
 * ═══════════════════════════════════════════════════════════════════
 * 
 * 1. STRUCTURES (struct):
 *    - SudokuBoard: Encapsulates board + metadata
 *    - Position: Abstracts coordinates (row, col)
 *    - SubGrid: Represents 3x3 regions
 *    - GenerationStats: Groups statistics
 * 
 * 2. TYPEDEF:
 *    - Simplifies declarations
 *    - Improves code readability
 *    - Makes type names more semantic
 * 
 * 3. POINTERS:
 *    - Pass by reference (avoids copies)
 *    - Modify structures in-place
 *    - Efficient memory access
 *    - Demonstrates proper pointer usage with const
 * 
 * 4. CONST:
 *    - Indicates read-only parameters
 *    - Prevents accidental modifications
 *    - Improves code semantics and safety
 *    - Enables compiler optimizations
 * 
 * 5. DYNAMIC MEMORY (malloc/free):
 *    - Flexible allocation at runtime
 *    - Important to free memory (prevent leaks)
 *    - Error handling for allocation failures
 *    - Educational demonstration of heap usage
 * 
 * 6. MODULARITY:
 *    - Small, specialized functions
 *    - Code reusability
 *    - Easier to maintain and debug
 *    - Single Responsibility Principle
 * 
 * 7. ALGORITHM EFFICIENCY:
 *    - Fisher-Yates: O(n) shuffling
 *    - Backtracking: O(9^m) where m = empty cells
 *    - Early exit optimizations in validation
 *    - Pruning strategies in solution counting
 * 
 * ═══════════════════════════════════════════════════════════════════
 */

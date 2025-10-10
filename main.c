#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

/*
 * Sudoku Generator - Hybrid Fisher-Yates + Backtracking
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
 * 1 = COMPACT    - Main phases summarized + sudoku + statistics
 * 2 = DETAILED   - All complete information (full output)
 * 
 * INSTRUCTIONS:
 * Change the VERBOSITY_LEVEL value as needed:
 * - For clean presentations: use 0
 * - For development/testing: use 1
 * - For complete debugging: use 2
 */

#define SIZE 9
#define PHASE3_TARGET 25  // Constant for phase 3
int VERBOSITY_LEVEL = 1;  // <-- CHANGE THIS: 0, 1, or 2

// ═══════════════════════════════════════════════════════════════════
//                         GLOBAL CONSTANTS
// ═══════════════════════════════════════════════════════════════════

/**
 * All 9 subgrid indices in a standard Sudoku
 * Used for cell elimination phases
 */
static const int ALL_SUBGRIDS[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

/**
 * Diagonal subgrids (0, 4, 8) that don't share rows/columns
 * Used for independent Fisher-Yates filling
 */
static const int DIAGONAL_SUBGRIDS[3] = {0, 4, 8};

// ═══════════════════════════════════════════════════════════════════
//                    FORWARD DECLARATIONS
// ═══════════════════════════════════════════════════════════════════

// Basic utilities
void fisherYatesShuffle(int *array, int size, int num_in);

// Verification functions
bool isSafePosition(int sudoku[SIZE][SIZE], int row, int col, int num);
bool findEmptyCell(int sudoku[SIZE][SIZE], int *row, int *col);
bool hasMultipleSolutions(int sudoku[SIZE][SIZE]);
int countSolutionsExact(int sudoku[SIZE][SIZE], int limit);

// Sudoku generation
void fillDiagonal(int sudoku[SIZE][SIZE]);
bool completeSudoku(int sudoku[SIZE][SIZE]);

// Cell elimination phases
int firstRandomElimination(int sudoku[SIZE][SIZE], const int *subgrid);
bool hasAlternativeInRowCol(int sudoku[SIZE][SIZE], int row, int col, int num);
int secondNoAlternativeElimination(int sudoku[SIZE][SIZE], const int *subgrid);
int thirdFreeElimination(int sudoku[SIZE][SIZE], int objetivo);

// Main generation and utilities
bool generateHybridSudoku(int sudoku[SIZE][SIZE]);
void printSudoku(int sudoku[SIZE][SIZE]);
bool validateSudoku(int sudoku[SIZE][SIZE]);
const char* evaluarDificultad(int sudoku[SIZE][SIZE]);

// ═══════════════════════════════════════════════════════════════════
//                    1. BASIC UTILITIES
// ═══════════════════════════════════════════════════════════════════

/**
 * Generates a random permutation using Fisher-Yates
 * @param array: Array to fill
 * @param size: Array size
 * @param num_in: Starting number (usually 1)
 */
void fisherYatesShuffle(int *array, int size, int num_in) {
    // Fill array consecutively
    for(int i = 0; i < size; i++) {
        array[i] = num_in + i;
    }
    
    // Shuffle (Fisher-Yates)
    for(int i = size-1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// ═══════════════════════════════════════════════════════════════════
//                    2. VERIFICATION FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * Checks if it's safe to place a number at a position
 * @return true if valid, false if there's a conflict
 */
bool isSafePosition(int sudoku[SIZE][SIZE], int row, int col, int num) {
    // Check row
    for(int x = 0; x < SIZE; x++) {
        if(sudoku[row][x] == num) return false;
    }
    
    // Check column
    for(int x = 0; x < SIZE; x++) {
        if(sudoku[x][col] == num) return false;
    }
    
    // Check 3x3 subgrid
    int rowStart = row - row % 3;
    int colStart = col - col % 3;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(sudoku[i + rowStart][j + colStart] == num) return false;
        }
    }
    
    return true;
}

/**
 * Finds the next empty cell (with value 0)
 * @param row: Pointer to store found row
 * @param col: Pointer to store found column
 * @return true if empty cell found, false if complete
 */
bool findEmptyCell(int sudoku[SIZE][SIZE], int *row, int *col) {
    for(*row = 0; *row < SIZE; (*row)++) {
        for(*col = 0; *col < SIZE; (*col)++) {
            if(sudoku[*row][*col] == 0) return true;
        }
    }
    return false;
}

/**
 * Searches for cells with multiple candidates
 * This heuristic checks if any empty cell has more than one valid number,
 * indicating potential multiple solutions
 */
bool hasMultipleSolutions(int sudoku[SIZE][SIZE]) {
    /// Search for cells with multiple candidates
    for(int r = 0; r < SIZE; r++) {
        for(int c = 0; c < SIZE; c++) {
            if(sudoku[r][c] == 0) {
                int candidates = 0;
                for(int num = 1; num <= 9; num++) {
                    if(isSafePosition(sudoku, r, c, num)) {
                        candidates++;
                        if(candidates > 1) return true;  // Early exit
                    }
                }
            }
        }
    }
    return false;
}

/**
 * Counts solutions using exhaustive backtracking
 * @param sudoku: The board to analyze
 * @param limit: Stop counting after finding this many solutions (usually 2)
 * @return Number of solutions found (capped at limit)
 */
int countSolutionsExact(int sudoku[SIZE][SIZE], int limit) {
    int row, col;
    
    // Base case: if no empty cells, we found a complete solution
    if(!findEmptyCell(sudoku, &row, &col)) {
        return 1;
    }
    
    int totalSolutions = 0;
    
    // Try each number from 1 to 9
    for(int num = 1; num <= 9; num++) {
        if(isSafePosition(sudoku, row, col, num)) {
            sudoku[row][col] = num;
            
            // Recursively count solutions
            totalSolutions += countSolutionsExact(sudoku, limit);
            
            // Early exit optimization: if we already found multiple solutions, stop
            if(totalSolutions >= limit) {
                sudoku[row][col] = 0;
                return totalSolutions;
            }
            
            sudoku[row][col] = 0;  // Backtrack
        }
    }
    
    return totalSolutions;
}

// ═══════════════════════════════════════════════════════════════════
//                    3. SUDOKU GENERATION
// ═══════════════════════════════════════════════════════════════════

/**
 * Fills the main diagonal subgrids (0, 4, 8)
 * These don't interfere with each other, allowing independent filling
 */
void fillDiagonal(int sudoku[SIZE][SIZE]) {
    if(VERBOSITY_LEVEL == 2) {
        printf("🎲 Filling main diagonal with Fisher-Yates...\n");
    } else if(VERBOSITY_LEVEL == 1) {
        printf("🎲 Diagonal + Backtracking...");
        fflush(stdout);
    }
    
    int random[SIZE];
    
    for(int idx = 0; idx < 3; idx++) {
        int grid = DIAGONAL_SUBGRIDS[idx];
        fisherYatesShuffle(random, SIZE, 1);
        
        int initial_row = (grid/3) * 3;
        int initial_column = (grid%3) * 3;
        
        if(VERBOSITY_LEVEL == 2) {
            printf("   Subgrid %d (base: %d,%d): ", grid, initial_row, initial_column);
        }
        
        for(int i = 0; i < SIZE; i++) {
            int row = initial_row + (i/3);
            int col = initial_column + (i%3);
            sudoku[row][col] = random[i];
            
            if(VERBOSITY_LEVEL == 2) {
                printf("%d ", random[i]);
            }
        }
        
        if(VERBOSITY_LEVEL == 2) {
            printf("\n");
        }
    }
    
    if(VERBOSITY_LEVEL == 2) {
        printf("✅ Diagonal completed!\n\n");
    }
}

/**
 * Completes the sudoku using recursive backtracking
 * @return true if successfully completed, false if no solution
 */
bool completeSudoku(int sudoku[SIZE][SIZE]) {
    int row, col;
    
    if(!findEmptyCell(sudoku, &row, &col)) {
        return true;
    }
    
    int numbers[9] = {1,2,3,4,5,6,7,8,9};
    
    // Shuffle
    for(int i = 8; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = numbers[i];
        numbers[i] = numbers[j];
        numbers[j] = temp;
    }
    
    for(int i = 0; i < 9; i++) {
        int num = numbers[i];
        
        if(isSafePosition(sudoku, row, col, num)) {
            sudoku[row][col] = num;
            
            if(completeSudoku(sudoku)) {
                return true;
            }
            
            sudoku[row][col] = 0;
        }
    }
    
    return false;
}

// ═══════════════════════════════════════════════════════════════════
//                    4. CELL ELIMINATION (3 PHASES)
// ═══════════════════════════════════════════════════════════════════

/**
 * PHASE 1: Removes one random number from each subgrid
 * Uses Fisher-Yates to choose which number (1-9) to remove from each
 * @return Number of removed cells (always 9)
 */
int firstRandomElimination(int sudoku[SIZE][SIZE], const int *subgrid) {
    if(VERBOSITY_LEVEL == 2) {
        printf("🎲 PHASE 1: Selecting numbers per subgrid with Fisher-Yates...\n");
    } else if(VERBOSITY_LEVEL == 1) {
        printf("🎲 Phase 1: Fisher-Yates selection...");
        fflush(stdout);
    }
    
    int random[SIZE];
    fisherYatesShuffle(random, SIZE, 1);
    int removed_count = 0;
    
    for(int idx = 0; idx < 9; idx++) {
        int grid = subgrid[idx];
        int initial_row = (grid/3) * 3;
        int initial_column = (grid%3) * 3;
        
        if(VERBOSITY_LEVEL == 2) {
            printf("   Subgrid %d (base: %d,%d): ", grid, initial_row, initial_column);
        }
        
        int valueToRemove = random[idx];
        
        for(int i = 0; i < SIZE; i++) {
            int row = initial_row + (i/3);
            int col = initial_column + (i%3);
            
            if(sudoku[row][col] == valueToRemove) {
                sudoku[row][col] = 0;
                removed_count++;
                
                if(VERBOSITY_LEVEL == 2) {
                    printf("%d ", valueToRemove);
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
    
    if(VERBOSITY_LEVEL == 1) {
        printf("📊 PHASE 1 TOTAL: Removed %d cells\n", removed_count);
    }
    
    return removed_count;
}

/**
 * Checks if a number has alternatives in its row, column, or subgrid
 * @return true if there ARE alternatives (should not remove), false if NO alternatives (can remove)
 */
bool hasAlternativeInRowCol(int sudoku[SIZE][SIZE], int row, int col, int num) {
    int temp = sudoku[row][col];
    sudoku[row][col] = 0;
    
    int possibleInRow = 0;
    int possibleInCol = 0;
    int possibleInSubgrid = 0;
    
    // Check another position in the ROW
    for(int x = 0; x < SIZE; x++) {
        if(x != col && sudoku[row][x] == 0) {
            if(isSafePosition(sudoku, row, x, num)) {
                possibleInRow++;
            }
        }
    }
    
    // Check another position in the COLUMN
    for(int x = 0; x < SIZE; x++) {
        if(x != row && sudoku[x][col] == 0) {
            if(isSafePosition(sudoku, x, col, num)) {
                possibleInCol++;
            }
        }
    }
    
    // Check another position in the 3x3 SUBGRID
    int rowStart = row - row % 3;
    int colStart = col - col % 3;
    
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            int f = rowStart + i;
            int c = colStart + j;
            
            // Skip the original position
            if(f == row && c == col) continue;
            
            if(sudoku[f][c] == 0 && isSafePosition(sudoku, f, c, num)) {
                possibleInSubgrid++;
            }
        }
    }
    
    sudoku[row][col] = temp; // Restore
    
    return (possibleInRow > 0) || (possibleInCol > 0) || (possibleInSubgrid > 0);
}

/**
 * PHASE 2: Removes numbers that have NO alternatives in their row/column/subgrid
 * Executes in a loop until no more can be removed
 * @return Number of removed values in this round
 */
int secondNoAlternativeElimination(int sudoku[SIZE][SIZE], const int *subgrid) {
    if(VERBOSITY_LEVEL == 2) {
        printf("🎲 PHASE 2: Selecting numbers without alternatives...\n");
    }
    
    int excluded_values = 0;
    
    for(int idx = 0; idx < 9; idx++) {
        int grid = subgrid[idx];
        int initial_row = (grid/3) * 3;
        int initial_column = (grid%3) * 3;
        
        if(VERBOSITY_LEVEL == 2) {
            printf("   Subgrid %d (base: %d,%d): ", grid, initial_row, initial_column);
        }
        
        for(int i = 0; i < SIZE; i++) {
            int row = initial_row + (i/3);
            int col = initial_column + (i%3);
            
            if(sudoku[row][col] != 0) {
                int currentNumber = sudoku[row][col];
                
                if(!hasAlternativeInRowCol(sudoku, row, col, currentNumber)) {
                    sudoku[row][col] = 0;
                    
                    if(VERBOSITY_LEVEL == 2) {
                        printf("%d ", currentNumber);
                    }
                    
                    excluded_values++;
                    break;
                }
            }
        }
        
        if(VERBOSITY_LEVEL == 2) {
            printf("\n");
        }
    }
    
    if(VERBOSITY_LEVEL == 2) {
        printf("✅ Phase 2 completed! Removed: %d\n\n", excluded_values);
    }
    
    return excluded_values;
}

/**
 * PHASE 3: Free elimination until reaching target
 * Verifies that unique solution is maintained using countSolutions()
 * @param objetivo: Additional number of cells to empty
 * @return Number of removed values
 */
int thirdFreeElimination(int sudoku[SIZE][SIZE], int objetivo) {
    if(VERBOSITY_LEVEL == 2) {
        printf("🎲 PHASE 3: Free elimination with unique solution verification...\n");
    } else if(VERBOSITY_LEVEL == 1) {
        printf("🎲 Phase 3: Free elimination...");
        fflush(stdout);
    }
    
    // Structure to store positions
    typedef struct {
        int row;
        int col;
    } Position;
    
    Position positions[81];
    int count = 0;
    
    // Collect all positions with numbers
    for(int f = 0; f < SIZE; f++) {
        for(int c = 0; c < SIZE; c++) {
            if(sudoku[f][c] != 0) {
                positions[count].row = f;
                positions[count].col = c;
                count++;
            }
        }
    }
    
    // Shuffle positions (Fisher-Yates)
    for(int i = count-1; i > 0; i--) {
        int j = rand() % (i + 1);
        Position temp = positions[i];
        positions[i] = positions[j];
        positions[j] = temp;
    }
    
    // Try to remove in random order
    int excluded_values = 0;
    for(int i = 0; i < count && excluded_values < objetivo; i++) {
        int row = positions[i].row;
        int col = positions[i].col;
        
        int temp = sudoku[row][col];
        sudoku[row][col] = 0; // Temporarily remove
        
        // EXACT Verification: count actual solutions
        int numSolutions = countSolutionsExact(sudoku, 2);
        
        if(numSolutions == 1) {
            // Safe to remove: exactly one solution exists
            excluded_values++;
            
            if(VERBOSITY_LEVEL == 2) {
                printf("   Removed %d at (%d,%d) - Total: %d\n", 
                       temp, row, col, excluded_values);
            }
        } else {
            sudoku[row][col] = temp; // Restore if multiple solutions
        }
    }
    
    if(VERBOSITY_LEVEL >= 1) {
        printf("✅ Phase 3 completed! Removed: %d\n", excluded_values);
    }
    
    return excluded_values;
}

// ═══════════════════════════════════════════════════════════════════
//                    5. MAIN FUNCTION
// ═══════════════════════════════════════════════════════════════════

/**
 * Generates a playable sudoku using hybrid method + cell elimination
 * @return true if successfully generated, false on error
 */
bool generateHybridSudoku(int sudoku[SIZE][SIZE]) {
    // Initialize all cells with 0
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            sudoku[i][j] = 0;
        }
    }
    
    // STEP 1: Fill main diagonal with Fisher-Yates
    fillDiagonal(sudoku);
    
    // STEP 2: Complete with backtracking
    if(VERBOSITY_LEVEL == 2) {
        printf("🔄 Completing with backtracking...\n");
    }
    
    bool success = completeSudoku(sudoku);
    
    if(VERBOSITY_LEVEL >= 1 && success) {
        printf("✅ Completed!\n");
    }
    
    if(success) {
        // STEP 3: PHASE 1 - Remove 1 per subgrid
        firstRandomElimination(sudoku, ALL_SUBGRIDS);
        
        // STEP 4: PHASE 2 - Loop of elimination without alternatives
        if(VERBOSITY_LEVEL == 1) {
            printf("🎲 Phase 2: Removal rounds...");
            fflush(stdout);
        }
        
        int round = 1;
        int excluded_values;
        int phase2_total = 0;
        
        do {
            if(VERBOSITY_LEVEL == 2) {
                printf("--- ROUND %d ---\n", round);
            }
            
            excluded_values = secondNoAlternativeElimination(sudoku, ALL_SUBGRIDS);
            phase2_total += excluded_values;
            round++;
        } while(excluded_values > 0);
        
        if(VERBOSITY_LEVEL == 2) {
            printf("🛑 Cannot remove more numbers in PHASE 2\n\n");
        }
        
        if(VERBOSITY_LEVEL == 1) {
            printf("\n📊 PHASE 2 TOTAL: Removed %d cells across %d rounds\n", 
                   phase2_total, round - 1);
        } else if(VERBOSITY_LEVEL == 2) {
            printf("📊 PHASE 2 TOTAL: Removed %d cells across %d rounds\n\n", 
                   phase2_total, round - 1);
        }
        
        // STEP 5: PHASE 3 - Free elimination until target
        thirdFreeElimination(sudoku, PHASE3_TARGET);
    }
    
    return success;
}

// ═══════════════════════════════════════════════════════════════════
//                    6. AUXILIARY FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

/**
 * Evaluate difficulty based on number of clues
 */
const char* evaluarDificultad(int sudoku[SIZE][SIZE]) {
    int pistas = 0;
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            if(sudoku[i][j] != 0) pistas++;
        }
    }
    
    if(pistas >= 45) return "EASY";
    else if(pistas >= 35) return "MEDIUM";
    else if(pistas >= 25) return "HARD";
    else return "EXPERT";
}

/**
 * Prints the sudoku in visual format with borders
 * Shows asterisks (*) for empty cells
 */
void printSudoku(int sudoku[SIZE][SIZE]) {
    int asterisks = 0;
    
    printf("┌───────┬───────┬───────┐\n");
    for(int i = 0; i < SIZE; i++) {
        printf("│");
        for(int j = 0; j < SIZE; j++) {
            if(sudoku[i][j] == 0) {
                printf(" *");
                asterisks++;
            } else {
                printf(" %d", sudoku[i][j]);
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
        printf("📊 Empty cells: %d | Filled cells: %d\n", asterisks, 81 - asterisks);
    }
}

/**
 * Verifies that the sudoku is valid (no conflicts)
 * @return true if valid, false if there are errors
 */
bool validateSudoku(int sudoku[SIZE][SIZE]) {
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            if(sudoku[i][j] == 0) continue; // Skip empty cells
            
            int num = sudoku[i][j];
            sudoku[i][j] = 0; // Temporary
            
            if(!isSafePosition(sudoku, i, j, num)) {
                sudoku[i][j] = num; // Restore
                return false;
            }
            
            sudoku[i][j] = num; // Restore
        }
    }
    return true;
}

// ═══════════════════════════════════════════════════════════════════
//                           7. MAIN
// ═══════════════════════════════════════════════════════════════════

int main(int argc, char *argv[]) {
    // Configure encoding based on operating system
    #ifdef _WIN32
        system("chcp 65001 > nul");  // UTF-8 on Windows
    #endif
    
    // Initialize random seed
    srand(time(NULL));
    
    // ========================================================================
    // PARSE COMMAND LINE ARGUMENTS
    // ========================================================================
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
    
    int sudoku[SIZE][SIZE];
    bool generatedSudoku = false;
    
    // ========================================================================
    // TITLES ACCORDING TO MODE
    // ========================================================================
    if(VERBOSITY_LEVEL == 0) {
        printf("═══════════════════════════════════════════════════════════════\n");
        printf("        SUDOKU GENERATOR v2.2.0 - HYBRID METHOD\n");
        printf("═══════════════════════════════════════════════════════════════\n\n");
    } else if(VERBOSITY_LEVEL == 1) {
        printf("═══════════════════════════════════════════════════════════════\n");
        printf("        SUDOKU GENERATOR v2.2.0 - HYBRID METHOD\n");
        printf("═══════════════════════════════════════════════════════════════\n\n");
    } else if(VERBOSITY_LEVEL == 2) {
        printf("═══════════════════════════════════════════════════════════════\n");
        printf("        SUDOKU GENERATOR v2.2.0 - HYBRID METHOD\n");
        printf("           Fisher-Yates + Backtracking + 3 Phases\n");
        printf("═══════════════════════════════════════════════════════════════\n\n");
    }
    
    // ========================================================================
    // SUDOKU GENERATION WITH CONTROLLED VERBOSITY
    // ========================================================================
    for(int attempt = 1; attempt <= 5; attempt++) {
        // Show attempt according to mode
        if(VERBOSITY_LEVEL == 2) {
            printf("🚀 ATTEMPT #%d:\n", attempt);
        } else if(VERBOSITY_LEVEL == 1) {
            printf("🚀 ATTEMPT #%d:\n", attempt);
        }
        
        if(generateHybridSudoku(sudoku)) {
            // Success message according to mode
            if(VERBOSITY_LEVEL == 2) {
                printf("✅ SUCCESS! Sudoku generated\n\n");
            } else if(VERBOSITY_LEVEL == 1) {
                printf("✅ SUCCESS! Sudoku generated\n\n");
            }
            
            // All modes show the sudoku
            printSudoku(sudoku);
            printf("\n");
            
            // Validation with controlled verbosity
            if(validateSudoku(sudoku)) {
                if(VERBOSITY_LEVEL == 2) {
                    printf("🎉 VERIFIED! The puzzle is valid\n");
                } else if(VERBOSITY_LEVEL == 1) {
                    printf("🎉 VERIFIED! The puzzle is valid\n");
                }
            } else {
                if(VERBOSITY_LEVEL >= 1) {
                    printf("❌ Verification error\n");
                }
            }
            
            // Show difficulty (in mode 0 and 1)
            if(VERBOSITY_LEVEL <= 1) {
                printf("\n📊 Difficulty level: %s\n", evaluarDificultad(sudoku));
            }
            
            generatedSudoku = true;
            break;
            
        } else {
            // Only in DETAILED mode show each failure
            if(VERBOSITY_LEVEL == 2) {
                printf("❌ Failed (very rare with this method)\n\n");
            }
        }
    }
    
    // ========================================================================
    // ERROR HANDLING IF SUDOKU WAS NOT GENERATED
    // ========================================================================
    if(!generatedSudoku) {
        printf("\n❌ ERROR: Could not generate a valid Sudoku after multiple attempts!\n");
        return 1;
    }
    
    return 0;
}

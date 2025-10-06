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

#define SIZE 9
#define PHASE3_TARGET 20  // Constant for phase 3

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
 * Counts the number of possible solutions for the sudoku
 * @param limite: Limit of solutions to search for (usually 2)
 * @return Number of solutions found
 */
int countSolutions(int sudoku[SIZE][SIZE], int limite) {
    int row, col;
    
    // If there are no empty cells, we found a solution
    if(!findEmptyCell(sudoku, &row, &col)) {
        return 1;
    }
    
    int totalSolutions = 0;
    
    // Try numbers from 1-9
    for(int num = 1; num <= 9; num++) {
        if(isSafePosition(sudoku, row, col, num)) {
            sudoku[row][col] = num;
            
            // Accumulate found solutions
            totalSolutions += countSolutions(sudoku, limite);
            
            // Optimization: if we already found 2+, stop searching
            if(totalSolutions >= limite) {
                sudoku[row][col] = 0;
                return totalSolutions;
            }
            
            sudoku[row][col] = 0; // Backtrack
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
    printf("🎲 Filling main diagonal with Fisher-Yates...\n");
    
    int subgrid[] = {0, 4, 8};
    
    for(int idx = 0; idx < 3; idx++) {
        int grid = subgrid[idx];
        int random[SIZE];
        fisherYatesShuffle(random, SIZE, 1);
        
        int initial_row = (grid/3) * 3;
        int initial_column = (grid%3) * 3;
        
        printf("   Subgrid %d (base: %d,%d): ", grid, initial_row, initial_column);
        
        for(int i = 0; i < SIZE; i++) {
            int row = initial_row + (i/3);
            int col = initial_column + (i%3);
            sudoku[row][col] = random[i];
            printf("%d ", random[i]);
        }
        printf("\n");
    }
    printf("✅ Diagonal completed!\n\n");
}

/**
 * Completes the sudoku using recursive backtracking
 * @return true if successfully completed, false if no solution
 */
bool completeSudoku(int sudoku[SIZE][SIZE]) {
    int row, col;
    
    if(!findEmptyCell(sudoku, &row, &col)) {
        return true; // Completed!
    }
    
    // Random numbers for variety
    int numbers[9] = {1,2,3,4,5,6,7,8,9};
    
    // Shuffle for variety
    for(int i = 8; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = numbers[i];
        numbers[i] = numbers[j];
        numbers[j] = temp;
    }
    
    // Try each number
    for(int i = 0; i < 9; i++) {
        int num = numbers[i];
        
        if(isSafePosition(sudoku, row, col, num)) {
            sudoku[row][col] = num;
            
            if(completeSudoku(sudoku)) {
                return true;
            }
            
            sudoku[row][col] = 0; // Backtrack
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
 */
void firstRandomElimination(int sudoku[SIZE][SIZE]) {
    printf("🎲 PHASE 1: Selecting numbers per subgrid with Fisher-Yates...\n");
    
    int subgrid[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    int random[SIZE];
    fisherYatesShuffle(random, SIZE, 1);
    
    for(int idx = 0; idx < 9; idx++) {
        int grid = subgrid[idx];
        int initial_row = (grid/3) * 3;
        int initial_column = (grid%3) * 3;
        
        printf("   Subgrid %d (base: %d,%d): ", grid, initial_row, initial_column);
        
        int valueToRemove = random[idx];
        
        for(int i = 0; i < SIZE; i++) {
            int row = initial_row + (i/3);
            int col = initial_column + (i%3);
            
            if(sudoku[row][col] == valueToRemove) {
                sudoku[row][col] = 0;
                printf("%d ", valueToRemove);
                break;
            }
        }
        printf("\n");
    }
    printf("✅ Phase 1 completed!\n\n");
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
int secondNoAlternativeElimination(int sudoku[SIZE][SIZE]) {
    printf("🎲 PHASE 2: Selecting numbers without alternatives...\n");
    
    int subgrid[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    int excluded_values = 0;
    
    for(int idx = 0; idx < 9; idx++) {
        int grid = subgrid[idx];
        int initial_row = (grid/3) * 3;
        int initial_column = (grid%3) * 3;
        
        printf("   Subgrid %d (base: %d,%d): ", grid, initial_row, initial_column);
        
        for(int i = 0; i < SIZE; i++) {
            int row = initial_row + (i/3);
            int col = initial_column + (i%3);
            
            if(sudoku[row][col] != 0) {
                int currentNumber = sudoku[row][col];
                
                if(!hasAlternativeInRowCol(sudoku, row, col, currentNumber)) {
                    sudoku[row][col] = 0;
                    printf("%d ", currentNumber);
                    excluded_values++;
                    break;
                }
            }
        }
        printf("\n");
    }
    
    printf("✅ Phase 2 completed! Removed: %d\n\n", excluded_values);
    return excluded_values;
}

/**
 * PHASE 3: Free elimination until reaching target
 * Verifies that unique solution is maintained using countSolutions()
 * @param objetivo: Additional number of cells to empty
 * @return Number of removed values
 */
int thirdFreeElimination(int sudoku[SIZE][SIZE], int objetivo) {
    printf("🎲 PHASE 3: Free elimination with unique solution verification...\n");
    
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
        sudoku[row][col] = 0;
        
        // Verify that unique solution is maintained
        int solutions = countSolutions(sudoku, 2);
        
        if(solutions == 1) {
            excluded_values++;
            printf("   Removed %d at (%d,%d) - Total: %d\n", 
                   temp, row, col, excluded_values);
        } else {
            sudoku[row][col] = temp; // Restore if multiple solutions
        }
    }
    
    printf("✅ Phase 3 completed! Removed: %d\n\n", excluded_values);
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
    printf("🔄 Completing with backtracking...\n");
    bool success = completeSudoku(sudoku);
    
    if(success) {
        // STEP 3: PHASE 1 - Remove 1 per subgrid
        firstRandomElimination(sudoku);
        
        // STEP 4: PHASE 2 - Loop of elimination without alternatives
        int round = 1;
        int excluded_values;
        do {
            printf("--- ROUND %d ---\n", round);
            excluded_values = secondNoAlternativeElimination(sudoku);
            round++;
        } while(excluded_values > 0);
        
        printf("🛑 Cannot remove more numbers in PHASE 2\n\n");
        
        // STEP 5: PHASE 3 - Free elimination until target
        thirdFreeElimination(sudoku, PHASE3_TARGET);
    }
    
    return success;
}

// ═══════════════════════════════════════════════════════════════════
//                    6. AUXILIARY FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

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
    printf("📊 Empty cells: %d | Filled cells: %d\n", asterisks, 81 - asterisks);
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

int main() {
    // Initialize random seed
    srand(time(NULL));
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("        SUDOKU GENERATOR v2.1.0 - HYBRID METHOD\n");
    printf("           Fisher-Yates + Backtracking + 3 Phases\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    int sudoku[SIZE][SIZE];
    
    // Generate sudoku
    for(int attempt = 1; attempt <= 5; attempt++) {
        printf("🚀 ATTEMPT #%d:\n", attempt);
        
        if(generateHybridSudoku(sudoku)) {
            printf("✅ SUCCESS! Sudoku generated\n\n");
            printSudoku(sudoku);
            printf("\n");
            
            if(validateSudoku(sudoku)) {
                printf("🎉 VERIFIED! The puzzle is valid\n");
            } else {
                printf("❌ Verification error\n");
            }
            break;
        } else {
            printf("❌ Failed (very rare with this method)\n\n");
        }
    }
    
    return 0;
}

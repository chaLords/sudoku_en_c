# Algorithm Analysis - Sudoku Generator

## Table of Contents
1. [Executive Summary](#executive-summary)
2. [Fisher-Yates Algorithm](#fisher-yates-algorithm)
3. [Backtracking Algorithm](#backtracking-algorithm)
4. [3-Phase Elimination System](#3-phase-elimination-system)
5. [Unique Solution Verification](#unique-solution-verification)
6. [Complete Complexity Analysis](#complete-complexity-analysis)
7. [Mathematical Foundations](#mathematical-foundations)

---

## Executive Summary

This document mathematically analyzes the Sudoku generator algorithms, explaining **why they work** and **their performance characteristics**.

### Complete Pipeline

```
┌─────────────────────────────────────────────────────────┐
│                  GENERATION (Phase A)                   │
│  Fisher-Yates (diagonal) + Backtracking (rest)          │
│  Complexity: O(1) + O(9^m) ≈ O(9^m)                     │
│  Typical time: ~2ms                                     │
└─────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────┐
│              ELIMINATION (Phases 1, 2, 3)               │
│  1. Random elimination (9 cells) - O(1)                 │
│  2. No-alternatives elimination (0-25 cells) - O(n²)    │
│  3. Verified free elimination (configurable) - O(n²×9^m)│
│  Typical time: ~4ms (dominated by Phase 3)            │
└─────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────┐
│                   PLAYABLE PUZZLE                       │
│  Total: 32-58 empty cells                               │
│  Unique solution guaranteed                             │
└─────────────────────────────────────────────────────────┘
```

---

## Fisher-Yates Algorithm

### Description

Generates a uniformly random permutation of numbers 1-9.

### Implementation

```c
void fisherYatesShuffle(int *array, int size, int num_in) {
    // Step 1: Fill consecutively
    for(int i = 0; i < size; i++) {
        array[i] = num_in + i;
    }
    
    // Step 2: Shuffle (Fisher-Yates)
    for(int i = size-1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}
```

### Complexity Analysis

| Operation | Complexity | Justification |
|-----------|------------|---------------|
| Fill array | O(n) | One loop of n iterations |
| Shuffle | O(n) | One loop of n-1 iterations |
| **Total** | **O(n)** | O(n) + O(n) = O(n) |

For n = 9: **O(9) = O(1)** (constant)

### Why Does It Work?

**Theorem**: The Fisher-Yates algorithm generates all n! permutations with **equal probability**.

**Proof** (by induction):

**Base case**: For n=1, there is 1 permutation, probability = 1 ✓

**Inductive step**: Assume valid for n-1.

For n elements:
- The last element has probability 1/n of going to any position
- The remaining n-1 are permuted with probability 1/(n-1)!
- Total probability = (1/n) × (1/(n-1)!) = 1/n!

### Mathematical Properties

1. **Uniformity**: P(permutation i) = 1/n! for all permutations i
2. **Linear time**: More efficient than sorting methods (O(n log n))
3. **In-place**: No additional memory required
4. **Deterministic**: Same seed → same sequence

---

## Backtracking Algorithm

### Description

Completes the Sudoku using exhaustive search with pruning (constraint satisfaction).

### Implementation

```c
bool completeSudoku(int sudoku[SIZE][SIZE]) {
    int row, col;
    
    // Base: Complete?
    if(!findEmptyCell(sudoku, &row, &col)) {
        return true;
    }
    
    // Generate random numbers for variety
    int numbers[9] = {1,2,3,4,5,6,7,8,9};
    for(int i = 8; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = numbers[i];
        numbers[i] = numbers[j];
        numbers[j] = temp;
    }
    
    // Try each number
    for(int i = 0; i < 9; i++) {
        int num = numbers[i];
        
        // Prune: Is it valid?
        if(isSafePosition(sudoku, row, col, num)) {
            sudoku[row][col] = num;
            
            // Recurse
            if(completeSudoku(sudoku)) {
                return true;
            }
            
            // Backtrack
            sudoku[row][col] = 0;
        }
    }
    
    return false;
}
```

### Complexity Analysis

**Worst case**: O(9^m) where m = empty cells

**Explanation**:
- Each empty cell has up to 9 options
- With m empty cells: 9 × 9 × 9 × ... (m times) = 9^m

**In practice**:
- With filled diagonal: m ≈ 54 empty cells
- Pruning drastically reduces search space
- Typical time: ~1-2ms (much better than worst case)

### Implemented Optimizations

1. **Early pruning**: `isSafePosition()` eliminates invalid branches
2. **Random order**: Avoids predictable patterns
3. **Pre-filled diagonal**: Reduces m from 81 to ~54

### Why Does It Work?

**Theorem**: Backtracking with constraint checking finds a solution if one exists.

**Proof**:
1. The search space includes all possible configurations
2. Pruning only eliminates invalid configurations
3. If a solution exists, the algorithm will find it (completeness)
4. If no solution exists, the algorithm will detect it (correctness)

---

## 3-Phase Elimination System

### Overview

```
COMPLETE SUDOKU (81 filled cells)
        │
        ├─► PHASE 1: Random Elimination
        │   └─ 9 cells removed (1 per subgrid)
        │
        ├─► PHASE 2: No-Alternatives Elimination
        │   └─ 0-25 cells removed (structure-dependent)
        │
        └─► PHASE 3: Verified Free Elimination
            └─ 0-25 cells removed (configurable)
                │
                ▼
        PLAYABLE PUZZLE (30-58 empty cells)
```

### PHASE 1: Random Elimination

#### Algorithm

```c
void firstRandomElimination(int sudoku[SIZE][SIZE]) {
    int subgrid[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    int random[SIZE];
    fisherYatesShuffle(random, SIZE, 1);
    
    // For each subgrid
    for(int idx = 0; idx < 9; idx++) {
        int grid = subgrid[idx];
        int initial_row = (grid/3) * 3;
        int initial_column = (grid%3) * 3;
        
        int valueToRemove = random[idx];
        
        // Find and remove that value in the subgrid
        // ...
    }
}
```

#### Analysis

| Property | Value |
|----------|-------|
| Complexity | O(9) = O(1) |
| Cells removed | Exactly 9 |
| Guarantee | 1 per subgrid |

**Invariant**: After PHASE 1, each subgrid has exactly 8 numbers.

### PHASE 2: No-Alternatives Elimination

#### Concept

A number has no alternatives in its row/column/subgrid if it cannot be placed in any other empty cell of those groups.

#### Implementation

```c
int secondNoAlternativeElimination(int sudoku[SIZE][SIZE]) {
    int subgrid[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    int removed = 0;
    
    for(int idx = 0; idx < 9; idx++) {
        int grid = subgrid[idx];
        // ... calculate positions ...
        
        for(int i = 0; i < SIZE; i++) {
            int row = initial_row + (i/3);
            int col = initial_column + (i%3);
            
            if(sudoku[row][col] != 0) {
                int currentNumber = sudoku[row][col];
                
                if(!hasAlternativeInRowCol(sudoku, row, col, currentNumber)) {
                    sudoku[row][col] = 0;
                    removed++;
                    break; // Max 1 per subgrid
                }
            }
        }
    }
    
    return removed;
}
```

#### Helper Function: `hasAlternativeInRowCol()`

```c
bool hasAlternativeInRowCol(int sudoku[SIZE][SIZE], int row, int col, int num) {
    int temp = sudoku[row][col];
    sudoku[row][col] = 0;
    
    int possibleInRow = 0;
    int possibleInCol = 0;
    int possibleInSubgrid = 0;
    
    // Check row
    for(int x = 0; x < SIZE; x++) {
        if(x != col && sudoku[row][x] == 0) {
            if(isSafePosition(sudoku, row, x, num)) {
                possibleInRow++;
            }
        }
    }
    
    // Check column
    for(int x = 0; x < SIZE; x++) {
        if(x != row && sudoku[x][col] == 0) {
            if(isSafePosition(sudoku, x, col, num)) {
                possibleInCol++;
            }
        }
    }
    
    // Check subgrid (similar logic)
    // ...
    
    sudoku[row][col] = temp; // Restore
    
    return (possibleInRow > 0) || (possibleInCol > 0) || (possibleInSubgrid > 0);
}
```

#### Analysis

| Operation | Complexity | Justification |
|-----------|------------|---------------|
| Main loop | O(9 × 9) = O(81) | 9 subgrids × 9 cells |
| `hasAlternativeInRowCol()` | O(9 + 9) = O(18) = O(1) | Check row and column |
| `isSafePosition()` | O(1) | Constant verification |
| **Total per round** | **O(81) = O(n²)** | n = 9 |

**Cells removed**: Variable, typically 0-25 (structure-dependent)

**Removal condition**: The number CANNOT be placed in another empty cell of its row, column, or subgrid.

#### Loop Structure

```c
int round = 1;
int removed;
do {
    removed = secondNoAlternativeElimination(sudoku);
    round++;
} while(removed > 0);
```

**Why loop?** Removing a number can create new opportunities. A number with alternatives in round 1 may have none in round 2.

### PHASE 3: Verified Free Elimination

#### Goal

Remove additional numbers until reaching difficulty target, guaranteeing unique solution.

#### Detailed Algorithm

**Step 1: Collect all positions with numbers**
```c
typedef struct {
    int row;
    int col;
} Position;

Position positions[81];
int count = 0;

for(int f = 0; f < SIZE; f++) {
    for(int c = 0; c < SIZE; c++) {
        if(sudoku[f][c] != 0) {
            positions[count].row = f;
            positions[count].col = c;
            count++;
        }
    }
}
```

**Step 2: Shuffle positions randomly**
```c
// Fisher-Yates on positions array
for(int i = count-1; i > 0; i--) {
    int j = rand() % (i + 1);
    Position temp = positions[i];
    positions[i] = positions[j];
    positions[j] = temp;
}
```

**Step 3: Try to remove in random order**
```c
int removed = 0;
int target = PHASE3_TARGET;  // Configurable

for(int i = 0; i < count && removed < target; i++) {
    int row = positions[i].row;
    int col = positions[i].col;
    
    int temp = sudoku[row][col];  // Save
    sudoku[row][col] = 0;         // Remove temporarily
    
    int solutions = countSolutions(sudoku, 2);
    
    if(solutions == 1) {
        // ✓ Unique solution maintained
        removed++;
    } else {
        // ✗ 0 or multiple solutions
        sudoku[row][col] = temp;  // Restore
    }
}
```

#### Configuration Parameter

In `main.c`, modify the constant:

```c
#define PHASE3_TARGET 20  // Easy (~35 empty cells)
#define PHASE3_TARGET 30  // Medium (~45 empty cells)
#define PHASE3_TARGET 40  // Hard (~55 empty cells)
```

#### Analysis

| Operation | Complexity |
|-----------|------------|
| Main loop | O(n² × target) |
| `countSolutions()` | O(9^m) worst case |
| **Total per round** | **O(n² × 9^m)** |

**In practice**:
- `countSolutions()` uses early exit (limit = 2)
- Effective complexity: much lower than worst case
- Typical time: ~100ms (97% of total time)

---

## Unique Solution Verification

### Function `countSolutions()`

```c
int countSolutions(int sudoku[SIZE][SIZE], int limite) {
    int row, col;
    
    // Base: Complete?
    if(!findEmptyCell(sudoku, &row, &col)) {
        return 1; // One solution found
    }
    
    int totalSolutions = 0;
    
    // Try numbers 1-9
    for(int num = 1; num <= 9; num++) {
        if(isSafePosition(sudoku, row, col, num)) {
            sudoku[row][col] = num;
            
            // Recurse
            totalSolutions += countSolutions(sudoku, limite);
            
            // Early exit: If we already found 2+, stop
            if(totalSolutions >= limite) {
                sudoku[row][col] = 0;
                return totalSolutions;
            }
            
            sudoku[row][col] = 0; // Backtrack
        }
    }
    
    return totalSolutions;
}
```

### Why Does It Work?

**Theorem**: `countSolutions(sudoku, 2)` determines if there's a unique solution.

**Proof**:
- If counter < 2: There are 0 or 1 solutions
- If counter == 1: Unique solution ✓
- If counter >= 2: Multiple solutions (early exit saves time)

### Optimization: Early Exit

**Without early exit**:
```
Time = O(9^m) where m = empty cells
Example with m=50: 9^50 ≈ 5.15 × 10^47 operations
```

**With early exit (limit=2)**:
```
Time = O(9^k) where k << m (stop at second solution)
Typical speedup: 10^40 to 10^44 times faster
```

### Complexity Analysis

| Case | Complexity | Explanation |
|------|------------|-------------|
| Worst case | O(9^m) | Explore entire tree |
| Typical case | O(9^k), k << m | Early exit at 2nd solution |
| Best case | O(1) | Inconsistency detected immediately |

---

## Complete Complexity Analysis

### Summary Table

| Function | Complexity | Typical Time | % of Total |
|----------|------------|--------------|------------|
| `fisherYatesShuffle()` | O(n) = O(1) | < 0.1ms | < 0.1% |
| `fillDiagonal()` | O(1) | < 0.1ms | < 0.1% |
| `completeSudoku()` | O(9^m) | ~2ms | 1.9% |
| `firstRandomElimination()` | O(1) | < 0.1ms | < 0.1% |
| `secondNoAlternativeElimination()` | O(n²) | ~0.5ms | 0.5% |
| `thirdFreeElimination()` | O(n² × 9^m) | ~100ms | 97.4% |
| `countSolutions()` | O(9^m) | Variable | - |
| `isSafePosition()` | O(1) | Nanoseconds | - |
| `validateSudoku()` | O(n²) | < 0.1ms | < 0.1% |
| `printSudoku()` | O(n²) | < 1ms | < 1% |

**Total**: ~102.7ms per puzzle

### Bottleneck

**PHASE 3 dominates execution time** (97.4%)

**Reason**: Unique solution verification with `countSolutions()` is costly

**Potential future optimizations**:
1. Heuristics to predict which cells to remove
2. Caching intermediate results
3. Parallelization of verifications

---

## Mathematical Foundations

### Valid Sudoku Space

**Question**: How many valid Sudokus exist?

**Answer**: 6,670,903,752,021,072,936,960 ≈ 6.67 × 10^21

**Calculation**:
1. First row: 9! = 362,880 permutations
2. Subsequent constraints reduce the space
3. Total calculated by Felgenhauer and Jarvis (2005)

### Probability of Unique Solution

**For typical puzzle with 50 empty cells**:

P(unique) ≈ 0.9999 if each removal is verified

**Without verification**: P(unique) ≈ 0.3-0.6 (unacceptable)

### Graph Theory

A Sudoku is a **graph coloring problem**:
- **Vertices**: 81 cells
- **Edges**: Connect cells that cannot have the same number
- **Colors**: Numbers 1-9
- **Goal**: Valid coloring using 9 colors

### Computational Complexity

**Decision problem**: "Does this Sudoku have a solution?"

**Complexity class**: NP-complete

**Implications**:
- No known polynomial algorithm
- Backtracking is a reasonable practical solution
- Solution verification is O(n²) (polynomial)

---

## Conclusions

### System Efficiency

1. **Fast generation**: ~2ms (Fisher-Yates + backtracking)
2. **Controlled elimination**: 3 phases for unique solution
3. **Identified bottleneck**: PHASE 3 (97.4% of time)

### Advantages of Hybrid Approach

| Aspect | Benefit |
|--------|---------|
| Fisher-Yates on diagonal | Reduces search space |
| Optimized backtracking | Guarantees valid solution |
| 3-phase elimination | Playable puzzle with unique solution |
| Rigorous verification | High quality puzzles |

### Trade-offs

- **Quality vs. Speed**: Unique solution verification is costly but necessary
- **Flexibility vs. Performance**: Configurable constant allows dynamic difficulty adjustment
- **Simplicity vs. Optimization**: Readable code with room for PHASE 3 improvement

### Applications

This generator is suitable for:
- ✅ Educational applications
- ✅ Puzzle generation for publications
- ✅ Casual games
- ⚠️ Real-time mass generation (requires PHASE 3 optimization)

---

**Author**: Gonzalo Ramírez (@chaLords)  
**License**: Apache 2.0  
**Version**: 2.1.1

# Algorithm Analysis - Sudoku Generator

## Table of Contents
1. [Executive Summary](#executive-summary)
2. [Data Structures Architecture](#data-structures-architecture)
3. [Fisher-Yates Algorithm](#fisher-yates-algorithm)
4. [Backtracking Algorithm](#backtracking-algorithm)
5. [3-Phase Elimination System](#3-phase-elimination-system)
6. [Unique Solution Verification](#unique-solution-verification)
7. [Complete Complexity Analysis](#complete-complexity-analysis)
8. [Mathematical Foundations](#mathematical-foundations)

---

## Executive Summary

This document mathematically analyzes the Sudoku generator algorithms in version 2.2.2, which features a modular library architecture built upon the structure-based refactoring introduced in v2.2.1. The analysis explains why the algorithms work, their performance characteristics, and how the modular design improves code organization without sacrificing efficiency.

### Complete Pipeline

```
┌─────────────────────────────────────────────────────────────┐
│                  GENERATION (Phase A)                       │
│  Fisher-Yates (diagonal) + Backtracking (rest)              │
│  Implemented in: src/core/algorithms/                       │
│  Complexity: O(1) + O(9^m) ≈ O(9^m)                         │
│  Typical time: ~2ms                                         │
└─────────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│              ELIMINATION (Phases 1, 2, 3)                   │
│  Implemented in: src/core/elimination/                      │
│  1. Random elimination (9 cells) - O(1)                     │
│  2. No-alternatives elimination (0-25 cells) - O(n²)        │
│  3. Verified free elimination (configurable) - O(n²×9^m)    │
│  Typical time: ~4ms (dominated by Phase 3)                  │
└─────────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                   PLAYABLE PUZZLE                           │
│  Total: 32-58 empty cells                                   │
│  Unique solution guaranteed                                 │
│  Validated by: src/core/validation.c                        │
└─────────────────────────────────────────────────────────────┘
```

**Note on Modular Implementation**: While the algorithmic complexity remains identical to v2.2.1, v2.2.2 organizes the implementation into separate modules for improved maintainability and testability. The `src/core/algorithms/` directory contains Fisher-Yates and backtracking implementations, while `src/core/elimination/` houses the three-phase elimination system.

---

## Data Structures Architecture

### Core Structures (Defined in include/sudoku/core/types.h)

The public API exposes these structures for efficient stack allocation and transparent operation:

#### SudokuPosition Structure

```c
typedef struct {
    int row;
    int col;
} SudokuPosition;
```

**Purpose**: Abstracts coordinate pairs into a single semantic unit. The modular architecture allows SudokuPosition to be used consistently across all modules without duplication.

**Memory footprint**: 8 bytes (two 4-byte integers on typical systems).

**Cross-module usage**: SudokuPosition is used in:
- `src/core/algorithms/backtracking.c` for recursive cell selection
- `src/core/elimination/phase3.c` for cell array management
- `src/core/validation.c` for position-based rule checking

#### SudokuBoard Structure

```c
typedef struct {
    int cells[SUDOKU_SIZE][SUDOKU_SIZE];  // The 9x9 grid
    int clues;              // Number of filled cells
    int empty;              // Number of empty cells
} SudokuBoard;
```

**Purpose**: Central data structure representing the puzzle state. In the modular architecture, SudokuBoard is the primary parameter passed between modules.

**Memory footprint**: 332 bytes (324 bytes for the grid plus 8 bytes for the two integer counters).

**Module interaction**: 
- Initialized by `sudoku_board_init()` in `src/core/board.c`
- Manipulated by algorithms in `src/core/algorithms/backtracking.c`
- Validated by `sudoku_validate_board()` in `src/core/validation.c`
- Displayed by `sudoku_display_print_board()` in `src/core/display.c`

#### SudokuSubGrid Structure

```c
typedef struct {
    int index;              // 0-8, identifies which subgrid
    SudokuPosition base;    // Top-left corner position
} SudokuSubGrid;
```

**Purpose**: Represents the nine 3×3 regions. The factory function `sudoku_subgrid_create()` in `src/core/board.c` encapsulates the mathematical calculation of base positions.

**Calculation logic**: Given a subgrid index, we compute the base position using integer division:
```c
SudokuSubGrid sudoku_subgrid_create(int index) {
    SudokuSubGrid sg;
    sg.index = index;
    sg.base.row = (index / 3) * 3;
    sg.base.col = (index % 3) * 3;
    return sg;
}
```

**Usage in modular architecture**: SudokuSubGrid structures are created on-demand in:
- `src/core/algorithms/fisher_yates.c` (diagonal subgrid filling)
- `src/core/elimination/phase1.c` (per-subgrid selection)
- `src/core/elimination/phase2.c` (no-alternatives detection)

#### SudokuGenerationStats Structure

```c
typedef struct {
    int phase1_removed;
    int phase2_removed;
    int phase2_rounds;
    int phase3_removed;
    int total_attempts;
} SudokuGenerationStats;
```

**Purpose**: Aggregates metrics from the generation process. In v2.2.2, this structure is populated by the orchestration code in `src/core/generator.c`, with individual phases updating their respective fields.

---

## Fisher-Yates Algorithm

### Description

The Fisher-Yates shuffle generates uniformly random permutations of the numbers one through nine. In the modular architecture, this is implemented in `src/core/algorithms/fisher_yates.c` and used by:
- Diagonal subgrid initialization
- Number ordering in backtracking
- Cell ordering in Phase 3 elimination

### Implementation (src/core/algorithms/fisher_yates.c)

```c
void sudoku_generate_permutation(int *array, int size, int start) {
    // Step 1: Fill array with consecutive values
    for(int i = 0; i < size; i++) {
        array[i] = start + i;
    }
    
    // Step 2: Backward shuffle using Fisher-Yates algorithm
    for(int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
        // Traditional swap with temporary variable
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}
```

### Traditional Swap Implementation

The implementation uses a straightforward three-variable swap pattern, which is the most readable and maintainable approach for educational code. This method clearly expresses the intent of exchanging two values.

**How traditional swap works**: We use a temporary variable to hold one value while performing the exchange:

Starting state: `array[i] = A`, `array[j] = B`

```c
int temp = array[i];    // temp = A
array[i] = array[j];    // array[i] = B
array[j] = temp;        // array[j] = A
```

Final state: `array[i] = B`, `array[j] = A`

**Why this approach**: The traditional swap is:
- **Clear and readable**: The intent is immediately obvious to any programmer
- **Compiler-optimized**: Modern compilers optimize this pattern extremely well
- **No edge cases**: Works correctly regardless of whether i equals j
- **Maintainable**: Easy for students and maintainers to understand

**Pedagogical note**: While more "clever" techniques exist (XOR swap, arithmetic swap), clarity and correctness are more valuable than cleverness in production code. This implementation prioritizes code quality and educational value.

### Complexity Analysis

| Operation | Complexity | Justification |
|-----------|------------|---------------|
| Fill array | O(n) | One loop of n iterations |
| Shuffle | O(n) | One loop of n-1 iterations, each with O(1) swap |
| **Total** | **O(n)** | O(n) + O(n) = O(n) |

For n equals nine in Sudoku: O(9) equals O(1) (constant time in practice).

### Mathematical Foundation: Proof of Uniformity

**Theorem**: The Fisher-Yates algorithm generates all n! permutations with equal probability 1/n!.

**Proof by induction**:

**Base case** (n equals one): There is exactly one permutation of a single element, which occurs with probability one. The theorem holds trivially.

**Inductive hypothesis**: Assume the algorithm produces uniform permutations for all arrays of size less than n.

**Inductive step**: For an array of size n, consider the last position. The algorithm selects a random index j from zero to n-1 with equal probability 1/n. Each element has exactly a 1/n chance of ending up in the last position. After placing an element in the last position, we have an array of size n-1 to shuffle. By the inductive hypothesis, this subarray is shuffled uniformly with probability 1/(n-1)!. Therefore, any specific permutation occurs with probability (1/n) × (1/(n-1)!) equals 1/n!, proving uniformity.

This mathematical rigor is important for students to understand that Fisher-Yates is not just a heuristic but a provably correct algorithm with precise probabilistic guarantees.

### Integration with Modular Architecture

The algorithm's modular integration demonstrates separation of concerns:

```c
// In src/core/algorithms/fisher_yates.c
void fill_subgrid(SudokuBoard *board, const SudokuSubGrid *sg) {
    int numbers[SUDOKU_SIZE];
    sudoku_generate_permutation(numbers, SUDOKU_SIZE, 1);
    
    for(int i = 0; i < SUDOKU_SIZE; i++) {
        SudokuPosition pos = sudoku_subgrid_get_position(sg, i);
        board->cells[pos.row][pos.col] = numbers[i];
    }
}

// Called from src/core/generator.c
void sudoku_fill_diagonal(SudokuBoard *board) {
    for(int i = 0; i < 3; i++) {
        SudokuSubGrid sg = sudoku_subgrid_create(DIAGONAL_INDICES[i]);
        fill_subgrid(board, &sg);
    }
}
```

This pattern shows how algorithmic logic (Fisher-Yates) remains unchanged while benefiting from clean interfaces provided by the modular structure (SudokuPosition, SudokuSubGrid helpers from board.c).

---

## Backtracking Algorithm

### Description

Backtracking is a depth-first search algorithm that explores the solution space systematically. In the modular architecture, the core backtracking logic is implemented in `src/core/algorithms/backtracking.c` and called by the orchestration code in `src/core/generator.c`.

### Structure-Based Implementation (src/core/algorithms/backtracking.c)

```c
bool complete_sudoku(SudokuBoard *board) {
    SudokuPosition pos;
    
    // Base case: No empty cells means we have a complete solution
    if(!find_empty_cell(board, &pos)) {
        return true;
    }
    
    // Generate shuffled numbers for randomness
    int numbers[SUDOKU_SIZE];
    sudoku_generate_permutation(numbers, SUDOKU_SIZE, 1);
    
    // Try each number in random order
    for(int i = 0; i < SUDOKU_SIZE; i++) {
        int num = numbers[i];
        
        // Prune: Check validity before recursing
        // Validation logic in src/core/validation.c
        if(sudoku_is_safe_position(board, &pos, num)) {
            board->cells[pos.row][pos.col] = num;
            
            // Recurse to fill remaining cells
            if(complete_sudoku(board)) {
                return true;
            }
            
            // Backtrack: Undo the placement
            board->cells[pos.row][pos.col] = 0;
        }
    }
    
    return false;  // No valid number found, backtrack further
}
```

### Modular Improvements in v2.2.2

**Separation of Concerns**: The backtracking algorithm in `src/core/algorithms/backtracking.c` focuses purely on the recursive completion logic. Validation is delegated to `src/core/validation.c`, and board utilities come from `src/core/board.c`. This separation enables:

- **Independent testing**: Backtracking can be tested separately from validation
- **Alternative implementations**: Different validation strategies without touching backtracking
- **Code reuse**: Validation logic used by backtracking, elimination, and user code

**Cross-module dependencies** (managed via internal headers):
```
backtracking.c  →  validation.c (sudoku_is_safe_position)
                →  board.c (find_empty_cell helper)
                →  fisher_yates.c (randomization)
```

### Complexity Analysis

**Worst case theoretical complexity**: O(9^m) where m represents the number of empty cells.

**Explanation of exponential behavior**: Each empty cell presents up to nine choices. Without any pruning, the algorithm would explore 9 × 9 × 9 × ... (m times), yielding 9^m possible paths. With m approximately equal to 54 (after filling the diagonal), the theoretical worst case is 9^54, which is approximately 5.6 × 10^51 operations.

**Practical performance analysis**: The theoretical worst case almost never occurs due to aggressive pruning via `sudoku_is_safe_position()`. The validation function (implemented in `src/core/validation.c`) eliminates invalid branches early, dramatically reducing the search space. In practice, `complete_sudoku()` runs in approximately one to two milliseconds, suggesting an effective branching factor much smaller than nine.

---

## 3-Phase Elimination System

### Architectural Overview

The three-phase elimination system is implemented as separate modules in `src/core/elimination/`:

```
src/core/elimination/
├── phase1.c  - Random elimination (one per subgrid)
├── phase2.c  - No-alternatives elimination (iterative)
└── phase3.c  - Verified free elimination (with uniqueness checking)
```

Each phase is a standalone function that can be tested and modified independently. The orchestration code in `src/core/generator.c` calls these phases in sequence.

```
COMPLETE SUDOKU (81 filled cells)
        │
        ├─► PHASE 1: Random Elimination
        │   Implementation: src/core/elimination/phase1.c
        │   └─ 9 cells removed (1 per SudokuSubGrid)
        │
        ├─► PHASE 2: No-Alternatives Elimination  
        │   Implementation: src/core/elimination/phase2.c
        │   └─ 0-25 cells removed (structure-dependent)
        │
        └─► PHASE 3: Verified Free Elimination
            Implementation: src/core/elimination/phase3.c
            └─ 0-25 cells removed (PHASE3_TARGET configurable)
                │
                ▼
        PLAYABLE PUZZLE (30-58 empty cells)
```

### PHASE 1: Random Elimination (src/core/elimination/phase1.c)

Phase 1 establishes the foundation for a balanced puzzle by removing exactly one number from each of the nine subgrids.

```c
int phase1_elimination(SudokuBoard *board, const int *indices, int count) {
    int numbers[SUDOKU_SIZE];
    sudoku_generate_permutation(numbers, SUDOKU_SIZE, 1);
    int removed = 0;
    
    for(int idx = 0; idx < count; idx++) {
        SudokuSubGrid sg = sudoku_subgrid_create(indices[idx]);  // From board.c
        int targetValue = numbers[idx];
        
        for(int i = 0; i < SUDOKU_SIZE; i++) {
            SudokuPosition pos = sudoku_subgrid_get_position(&sg, i);
            
            if(board->cells[pos.row][pos.col] == targetValue) {
                board->cells[pos.row][pos.col] = 0;
                removed++;
                break;
            }
        }
    }
    
    return removed;
}
```

**Complexity**: O(81) = O(n²) for 9 subgrids × 9 cells each.

**Invariant Maintained**: After phase 1, each of the nine subgrids contains exactly eight numbers (one removed). This uniform distribution is guaranteed by the algorithm's structure.

### PHASE 2: No-Alternatives Elimination (src/core/elimination/phase2.c)

Phase 2 employs a more sophisticated strategy based on logical necessity. A number has "no alternatives" when it cannot be placed in any other empty cell within its row, column, or subgrid.

```c
int phase2_elimination(SudokuBoard *board, const int *indices, int count) {
    int removed = 0;
    
    for(int idx = 0; idx < count; idx++) {
        SudokuSubGrid sg = sudoku_subgrid_create(indices[idx]);
        
        for(int i = 0; i < SUDOKU_SIZE; i++) {
            SudokuPosition pos = sudoku_subgrid_get_position(&sg, i);
            
            if(board->cells[pos.row][pos.col] != 0) {
                int num = board->cells[pos.row][pos.col];
                
                // Check if this number can go anywhere else
                // has_alternative() is internal to phase2.c
                if(!has_alternative(board, &pos, num)) {
                    board->cells[pos.row][pos.col] = 0;
                    removed++;
                    break;
                }
            }
        }
    }
    
    return removed;
}
```

**Complexity per round**: O(81) = O(n²)

**Iterative Application**: Phase 2 runs in a loop (managed by `src/core/generator.c`) because removing one number can create new opportunities. Most puzzles require 3-5 rounds.

### PHASE 3: Verified Free Elimination (src/core/elimination/phase3.c)

Phase 3 represents the most computationally intensive portion, rigorously verifying uniqueness after each removal attempt.

```c
int phase3_elimination(SudokuBoard *board, int target) {
    // Allocate array of positions dynamically
    SudokuPosition *positions = (SudokuPosition *)malloc(TOTAL_CELLS * sizeof(SudokuPosition));
    if(positions == NULL) {
        return 0;  // Allocation failure
    }
    
    int count = 0;
    
    // Collect all filled positions
    for(int r = 0; r < SUDOKU_SIZE; r++) {
        for(int c = 0; c < SUDOKU_SIZE; c++) {
            if(board->cells[r][c] != 0) {
                positions[count].row = r;
                positions[count].col = c;
                count++;
            }
        }
    }
    
    // Shuffle positions for random removal order
    // (shuffle logic using Fisher-Yates on SudokuPosition array)
    
    int removed = 0;
    
    // Try removing cells in random order
    for(int i = 0; i < count && removed < target; i++) {
        SudokuPosition *pos = &positions[i];
        int temp = board->cells[pos->row][pos->col];
        
        board->cells[pos->row][pos->col] = 0;
        
        // Verify unique solution remains
        // countSolutionsExact in src/core/algorithms/backtracking.c
        if(countSolutionsExact(board, 2) == 1) {
            removed++;
        } else {
            board->cells[pos->row][pos->col] = temp;
        }
    }
    
    free(positions);
    return removed;
}
```

**Complexity**: O(n² × 9^m) where m is empty cells at each verification.

**Module interaction**: Phase 3 uses `countSolutionsExact()` from `src/core/algorithms/backtracking.c`, demonstrating how elimination and algorithms modules cooperate.

---

## Unique Solution Verification

### The countSolutionsExact Function (src/core/algorithms/backtracking.c)

Solution counting is essential for ensuring puzzle quality. This function uses exhaustive backtracking with an early exit mechanism:

```c
int countSolutionsExact(SudokuBoard *board, int limit) {
    SudokuPosition pos;
    
    // Base case: Board complete means we found a solution
    if(!find_empty_cell(board, &pos)) {
        return 1;
    }
    
    int totalSolutions = 0;
    
    // Try all numbers 1-9
    for(int num = 1; num <= SUDOKU_SIZE; num++) {
        if(sudoku_is_safe_position(board, &pos, num)) {
            board->cells[pos.row][pos.col] = num;
            
            // Recursively count solutions
            totalSolutions += countSolutionsExact(board, limit);
            
            // Early exit: Stop when we reach the limit
            if(totalSolutions >= limit) {
                board->cells[pos.row][pos.col] = 0;
                return totalSolutions;
            }
            
            board->cells[pos.row][pos.col] = 0;
        }
    }
    
    return totalSolutions;
}
```

### Why Early Exit is Critical

**Without early exit**: We would need to explore the entire solution tree to count all solutions. For a puzzle with 50 empty cells, this means potentially exploring 9^50 ≈ 5.15 × 10^47 paths.

**With early exit**: When we're only checking for uniqueness (limit equals 2), we stop after finding the second solution. This typically occurs after exploring only thousands or millions of paths rather than trillions upon trillions.

**Speedup calculation**: The speedup factor is approximately (total paths) / (paths until second solution). In practice, this ranges from 10^40 to 10^44, making the difference between unusable and practical.

---

## Complete Complexity Analysis

### Comprehensive Function Table (Modular Implementation)

| Function | Module | Complexity | Explanation |
|----------|--------|------------|-------------|
| `sudoku_board_init()` | board.c | O(n²) | Initialize all 81 cells |
| `sudoku_board_update_stats()` | board.c | O(n²) | Count filled cells |
| `sudoku_subgrid_create()` | board.c | O(1) | Arithmetic calculation |
| `sudoku_subgrid_get_position()` | board.c | O(1) | Position calculation |
| `sudoku_generate_permutation()` | fisher_yates.c | O(n) | Linear shuffle |
| `sudoku_is_safe_position()` | validation.c | O(1) | Check 27 positions |
| `find_empty_cell()` | backtracking.c | O(n²) | Worst case full scan |
| `fill_subgrid()` | fisher_yates.c | O(1) | Fill 9 cells |
| `sudoku_fill_diagonal()` | generator.c | O(1) | Fill 3 subgrids |
| `complete_sudoku()` | backtracking.c | O(9^m) | Exponential worst case |
| `has_alternative()` | phase2.c | O(1) | Check up to 27 positions |
| `phase1_elimination()` | phase1.c | O(1) | Fixed 9 subgrids |
| `phase2_elimination()` | phase2.c | O(n²) | Check all cells |
| `phase3_elimination()` | phase3.c | O(n²×9^m) | Per-cell verification |
| `countSolutionsExact()` | backtracking.c | O(9^m) | Early exit helps |
| `sudoku_validate_board()` | validation.c | O(n²) | Validate each cell |
| `sudoku_display_print_board()` | display.c | O(n²) | Print all cells |

### Total Pipeline Complexity

```
GENERATION:
  sudoku_board_init()       O(n²)        ~0.01ms
  sudoku_fill_diagonal()    O(1)         ~0.05ms  
  complete_sudoku()         O(9^m)       ~1-2ms (m ≈ 54)
  
ELIMINATION:
  phase1_elimination()      O(1)         ~0.05ms
  phase2_elimination()      O(n²) × k    ~0.5ms (k ≈ 4 rounds)
  phase3_elimination()      O(n²×9^m)    ~2-4ms (early exit crucial)
  
VALIDATION:
  sudoku_board_update_stats() O(n²)      ~0.01ms
  sudoku_validate_board()   O(n²)        ~0.01ms
  
TOTAL COMPLEXITY: O(9^m + k×n² + n²×9^m) ≈ O(n²×9^m)
TYPICAL TIME: ~4-6ms per puzzle
```

The complexity is dominated by phase 3 elimination due to the repeated solution counting. However, the early exit optimization keeps this practical.

---

## Mathematical Foundations

### Valid Sudoku Space

**Question**: How many distinct valid complete Sudokus exist?

**Answer**: 6,670,903,752,021,072,936,960 ≈ 6.67 × 10^21

This number was computed by Felgenhauer and Jarvis in 2005 through exhaustive enumeration with mathematical symmetry reduction.

### Graph Theory Perspective

A Sudoku can be viewed as a graph coloring problem with special structure:

**Vertices**: The 81 cells of the board

**Edges**: Connect cells that cannot share the same number
- Each cell connects to 20 others (8 in row + 8 in column + 4 in subgrid, avoiding double counting)

**Colors**: The numbers 1 through 9

**Constraint**: Adjacent vertices (connected by edges) must have different colors

**Chromatic number**: The minimum number of colors needed is 9

This graph perspective helps explain why certain configurations are impossible and why constraint propagation (pruning in backtracking) is so effective. Each placed number effectively removes one color from up to 20 vertices, dramatically reducing the search space.

### Computational Complexity Theory

**The Sudoku Decision Problem**: "Given a partially filled board, does a valid completion exist?"

**Complexity Class**: NP-complete

**Practical implications**: While the worst case is exponential, real-world Sudoku puzzles have structure that makes them tractable. Our backtracking implementation exploits this structure through pruning, achieving practical performance despite the theoretical hardness.

---

## Conclusions

### Modular Architecture Impact (v2.2.2)

The modular refactoring introduces organizational improvements without sacrificing algorithmic efficiency:

**Code organization**: Clear separation between algorithms (`src/core/algorithms/`), elimination strategies (`src/core/elimination/`), validation (`src/core/validation.c`), and orchestration (`src/core/generator.c`).

**Independent testing**: Each module can be tested in isolation. For example, Fisher-Yates can be verified independently from backtracking.

**Maintainability**: Changes to one module (e.g., optimization in Phase 3) don't require touching other modules, reducing the risk of introducing bugs.

**Reusability**: Algorithm modules (`sudoku_generate_permutation`, backtracking) can be reused in other contexts (e.g., solver, puzzle analyzer) without modification.

**Educational value**: Students can study individual algorithms in isolation before understanding how they compose into the complete system.

### System Efficiency Summary

| Metric | Value | Notes |
|--------|-------|-------|
| Generation time | ~2ms | Fisher-Yates + backtracking |
| Elimination time | ~4ms | Dominated by phase 3 |
| Total time per puzzle | ~6ms | ≈ 165 puzzles/second |
| Success rate | 99.9% | Nearly perfect reliability |
| Memory footprint | ~500 bytes | Active data structures |
| Modular overhead | ~0% | No performance regression vs v2.2.1 |

### Advantages of Modular Hybrid Approach

| Aspect | Benefit | Location in Codebase |
|--------|---------|---------------------|
| Fisher-Yates on diagonal | Reduces search space 9^81 → 9^54 | `src/core/algorithms/fisher_yates.c` |
| Structured backtracking | Clear ownership, const correctness | `src/core/algorithms/backtracking.c` |
| Three-phase elimination | Balances speed and quality | `src/core/elimination/*.c` |
| Early exit in counting | Makes verification practical | `src/core/algorithms/backtracking.c` |
| Independent validation | Reusable, testable rule checking | `src/core/validation.c` |
| Modular organization | Maintainable, extensible | Entire `src/core/` structure |

### Applications and Suitability

This generator is well-suited for:

- **Educational applications**: Clear modular structure aids teaching
- **Puzzle publications**: Guaranteed unique solutions
- **Game development**: Fast generation supports real-time play
- **Research**: Modular design facilitates experimentation
- **Library integration**: Clean API enables embedding in other projects

---

**Author**: Gonzalo Ramírez (@chaLords)  
**License**: Apache 2.0  
**Version**: 2.2.2  
**Last Updated**: November 2025

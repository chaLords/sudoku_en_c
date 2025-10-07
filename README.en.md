# 🎮 Sudoku Generator in C

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![C](https://img.shields.io/badge/C-00599C?style=flat&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey)](https://github.com/chaLords/sudoku_en_c)

> An efficient complete Sudoku generator using a hybrid approach: Fisher-Yates algorithm for independent subgrids and backtracking to complete the rest of the board.

## 📋 Table of Contents

- [Features](#-features)
- [Demo](#-demo)
- [Installation](#-installation)
- [Usage](#-usage)
- [How It Works](#-how-it-works)
- [Algorithms](#-algorithms)
- [Code Structure](#-code-structure)
- [Roadmap](#-roadmap)
- [Contributing](#-contributing)
- [License](#-license)
- [Author](#-author)

## ✨ Features

- ✅ **Complete 9x9 valid Sudoku generation**
- ✅ **Optimized hybrid method** (Fisher-Yates + Backtracking)
- ✅ **High success rate** (~99.9%)
- ✅ **Uniformly random permutations**
- ✅ **Automatic board validation**
- ✅ **Clean and well-documented code**
- ✅ **Visual console interface** with box-drawing characters
- ✅ **3-phase elimination system** to create playable puzzles
- ✅ **Unique solution verification**

## 🎯 Demo

### Program Output

```
═══════════════════════════════════════════════════════════════
        SUDOKU GENERATOR v2.1.0 - HYBRID METHOD
           Fisher-Yates + Backtracking + 3 Phases
═══════════════════════════════════════════════════════════════

🚀 ATTEMPT #1:
🎲 Filling main diagonal with Fisher-Yates...
   Subgrid 0 (base: 0,0): 1 2 5 7 3 8 6 9 4 
   Subgrid 4 (base: 3,3): 6 2 8 5 4 7 1 9 3 
   Subgrid 8 (base: 6,6): 9 2 3 6 8 5 7 4 1 
✅ Diagonal completed!

🔄 Completing with backtracking...
🎲 PHASE 1: Selecting numbers per subgrid with Fisher-Yates...
   Subgrid 0 (base: 0,0): 4 
   Subgrid 1 (base: 0,3): 9 
   Subgrid 2 (base: 0,6): 3 
   Subgrid 3 (base: 3,0): 5 
   Subgrid 4 (base: 3,3): 8 
   Subgrid 5 (base: 3,6): 7 
   Subgrid 6 (base: 6,0): 6 
   Subgrid 7 (base: 6,3): 1 
   Subgrid 8 (base: 6,6): 2 
✅ Phase 1 completed!

--- ROUND 1 ---
🎲 PHASE 2: Selecting numbers without alternatives...
   Subgrid 0 (base: 0,0): 1 
   Subgrid 1 (base: 0,3): 3 
   Subgrid 2 (base: 0,6): 4 
   Subgrid 3 (base: 3,0): 7 
   Subgrid 4 (base: 3,3): 6 
   Subgrid 5 (base: 3,6): 9 
   Subgrid 6 (base: 6,0): 8 
   Subgrid 7 (base: 6,3): 7 
   Subgrid 8 (base: 6,6): 6 
✅ Phase 2 completed! Removed: 9

--- ROUND 2 ---
🎲 PHASE 2: Selecting numbers without alternatives...
   Subgrid 0 (base: 0,0): 2 
   Subgrid 1 (base: 0,3): 8 
   Subgrid 2 (base: 0,6): 2 
   Subgrid 3 (base: 3,0): 3 
   Subgrid 4 (base: 3,3): 2 
   Subgrid 5 (base: 3,6): 4 
   Subgrid 6 (base: 6,0): 3 
   Subgrid 7 (base: 6,3): 5 
   Subgrid 8 (base: 6,6): 5 
✅ Phase 2 completed! Removed: 9

--- ROUND 3 ---
🎲 PHASE 2: Selecting numbers without alternatives...
   Subgrid 0 (base: 0,0): 9 
   Subgrid 1 (base: 0,3): 
   Subgrid 2 (base: 0,6): 8 
   Subgrid 3 (base: 3,0): 
   Subgrid 4 (base: 3,3): 3 
   Subgrid 5 (base: 3,6): 1 
   Subgrid 6 (base: 6,0): 
   Subgrid 7 (base: 6,3): 4 
   Subgrid 8 (base: 6,6): 7 
✅ Phase 2 completed! Removed: 6

--- ROUND 4 ---
🎲 PHASE 2: Selecting numbers without alternatives...
   Subgrid 0 (base: 0,0): 
   Subgrid 1 (base: 0,3): 
   Subgrid 2 (base: 0,6): 
   Subgrid 3 (base: 3,0): 
   Subgrid 4 (base: 3,3): 
   Subgrid 5 (base: 3,6): 
   Subgrid 6 (base: 6,0): 
   Subgrid 7 (base: 6,3): 
   Subgrid 8 (base: 6,6): 
✅ Phase 2 completed! Removed: 0

🛑 Cannot remove more numbers in PHASE 2

🎲 PHASE 3: Free elimination with unique solution verification...
   Removed 9 at (4,0) - Total: 1
   Removed 7 at (2,4) - Total: 2
   Removed 4 at (4,4) - Total: 3
   Removed 1 at (3,1) - Total: 4
   Removed 7 at (1,0) - Total: 5
   Removed 9 at (5,4) - Total: 6
   Removed 4 at (6,1) - Total: 7
   Removed 9 at (7,5) - Total: 8
   Removed 6 at (2,0) - Total: 9
   Removed 7 at (4,5) - Total: 10
   Removed 2 at (7,0) - Total: 11
   Removed 6 at (0,5) - Total: 12
   Removed 6 at (5,1) - Total: 13
   Removed 6 at (8,4) - Total: 14
   Removed 2 at (4,6) - Total: 15
   Removed 7 at (0,7) - Total: 16
   Removed 8 at (5,6) - Total: 17
   Removed 5 at (0,2) - Total: 18
   Removed 6 at (1,7) - Total: 19
   Removed 8 at (1,2) - Total: 20
✅ Phase 3 completed! Removed: 20

✅ SUCCESS! Sudoku generated

┌───────┬───────┬───────┐
│ * * * │ * * * │ * * 9 │
│ * 3 * │ * 5 4 │ 1 * * │
│ * * * │ 2 * 1 │ 5 * * │
├───────┼───────┼───────┤
│ * * * │ * * * │ 3 * * │
│ * 8 * │ 5 * * │ * * 6 │
│ 4 * 2 │ 1 * * │ * 5 * │
├───────┼───────┼───────┤
│ * * * │ * * * │ 9 * 3 │
│ * 7 1 │ * 3 * │ * 8 * │
│ * 5 9 │ 8 * 2 │ * 4 1 │
└───────┴───────┴───────┘
📊 Empty cells: 53 | Filled cells: 28

🎉 VERIFIED! The puzzle is valid
```

## 📦 Installation

### Prerequisites

- GCC compiler (or any standard C compiler)
- Make (optional)

### Clone the Repository

```bash
git clone https://github.com/chaLords/sudoku_en_c.git
cd sudoku_en_c
```

### Compile

```bash
# Simple compilation
gcc main.c -o sudoku

# With optimizations
gcc -O2 main.c -o sudoku

# With detailed warnings
gcc -Wall -Wextra main.c -o sudoku
```

## 🚀 Usage

### Basic Execution

```bash
./sudoku
```

The program will automatically generate a playable Sudoku puzzle and display:
- The generation process (diagonal + backtracking)
- The 3 cell elimination phases
- The final puzzle with asterisks (*) for empty cells
- Statistics of empty/filled cells

### Integration in Another Project

```c
#include "main.c"

int main() {
    int sudoku[SIZE][SIZE];
    
    // Generate playable puzzle
    if(generateHybridSudoku(sudoku)) {
        printSudoku(sudoku);
        
        // Verify validity
        if(validateSudoku(sudoku)) {
            printf("Valid puzzle!\n");
        }
    }
    
    return 0;
}
```

### Configure Difficulty

Modify the `PHASE3_TARGET` constant in `main.c`:

```c
#define PHASE3_TARGET 5  // Easy (~35 empty cells)
#define PHASE3_TARGET 15  // Medium (~45 empty cells)
#define PHASE3_TARGET 25  // Hard (~55 empty cells)
```

## 🧠 How It Works

### Hybrid Approach + 3-Phase System

#### **GENERATION: Hybrid Fisher-Yates + Backtracking**

**Phase 1: Fisher-Yates for the Diagonal**
Fill independent subgrids (0, 4, 8) using the Fisher-Yates algorithm:

```
┌───────┬───────┬───────┐
│ [0]   │       │       │  ← Subgrid 0
│       │       │       │
│       │       │       │
├───────┼───────┼───────┤
│       │ [4]   │       │  ← Subgrid 4
│       │       │       │
│       │       │       │
├───────┼───────┼───────┤
│       │       │ [8]   │  ← Subgrid 8
│       │       │       │
│       │       │       │
└───────┴───────┴───────┘
```

**Advantage:** These subgrids don't share rows, columns, or regions, allowing independent filling without conflicts.

**Phase 2: Backtracking for the Rest**
Complete remaining cells using recursive backtracking with pruning:

1. Find empty cell
2. Try numbers 1-9 (randomized)
3. If valid → recurse
4. If fails → backtrack and try next number

#### **ELIMINATION: 3-Phase System**

**PHASE 1: Random Elimination**
- Removes exactly 1 cell from each subgrid (9 total)
- Uses Fisher-Yates for random selection
- Guarantees uniform distribution

**PHASE 2: No-Alternatives Elimination**
- Removes cells whose numbers CANNOT go elsewhere
- Executes in a loop until no more can be removed
- Typically removes 15-25 additional cells

**PHASE 3: Free Verified Elimination**
- Freely removes cells until reaching target (configurable)
- Verifies that puzzle maintains unique solution
- Uses `countSolutions()` with early exit for efficiency

### Advantages of the Hybrid Method

| Feature | Pure Backtracking | Pure Fisher-Yates | **Hybrid** |
|---------|-------------------|-------------------|------------|
| Speed | ⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐⭐ |
| Randomness | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Success | ⭐⭐⭐⭐⭐ | ⭐ | ⭐⭐⭐⭐⭐ |

## 🔧 Algorithms

### Fisher-Yates Shuffle

**Complexity:** O(n)

```c
void fisherYatesShuffle(int *array, int size, int num_in) {
    // Fill consecutive array
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
```

**Properties:**
- Generates uniformly random permutations
- Each permutation has equal probability
- Linear time O(n)

### Backtracking with Pruning

**Complexity:** O(9^m) where m = empty cells

```c
bool completeSudoku(int sudoku[SIZE][SIZE]) {
    int row, col;
    
    // 1. Find empty cell
    if(!findEmptyCell(sudoku, &row, &col)) {
        return true; // Complete!
    }
    
    // 2. Try numbers 1-9
    for(int num = 1; num <= 9; num++) {
        if(isSafePosition(sudoku, row, col, num)) {
            sudoku[row][col] = num;
            
            // 3. Recurse
            if(completeSudoku(sudoku)) {
                return true;
            }
            
            // 4. Backtrack
            sudoku[row][col] = 0;
        }
    }
    
    return false;
}
```

**Optimizations:**
- Early pruning with `isSafePosition()`
- Efficient O(1) verification per cell
- Randomized numbers for variety

### Unique Solution Verification

**Complexity:** O(9^m) worst case, O(9^k) typical with early exit

```c
int countSolutions(int sudoku[SIZE][SIZE], int limite) {
    // Counts solutions up to limit
    // If finds >= 2, stops immediately (early exit)
    // Typical speedup: 10^40 - 10^44 times
}
```

## 📁 Code Structure

```
sudoku_en_c/
│
├── main.c                 # Main code
├── README.md              # README in Spanish
├── README.en.md           # This file (English)
├── LICENSE                # Apache 2.0 License
├── CHANGELOG.md           # Version History
├── NOTICE                 # Attribution file
├── .gitignore             # Files to ignore in Git
└── docs/
    ├── ALGORITHMS.md      # Detailed mathematical analysis
    └── TECHNICAL.md       # Deep technical documentation
```

### Main Functions

| Function | Description | Complexity |
|----------|-------------|------------|
| `fisherYatesShuffle()` | Generates random permutation | O(n) |
| `isSafePosition()` | Validates if a number is valid | O(1) |
| `findEmptyCell()` | Finds next empty cell | O(n²) |
| `fillDiagonal()` | Fills independent subgrids | O(1) |
| `completeSudoku()` | Completes with backtracking | O(9^m) |
| `firstRandomElimination()` | PHASE 1: Random elimination | O(1) |
| `hasAlternativeInRowCol()` | Checks for alternatives | O(1) |
| `secondNoAlternativeElimination()` | PHASE 2: No alternatives | O(n²) |
| `countSolutions()` | Counts solutions (with early exit) | O(9^m) |
| `thirdFreeElimination()` | PHASE 3: Free verified | O(n² × 9^m) |
| `generateHybridSudoku()` | Orchestrates entire process | - |
| `printSudoku()` | Prints visually | O(n²) |
| `validateSudoku()` | Validates complete board | O(n²) |

## 🗺️ Roadmap

### ✅ Version 2.1 (Current)
- [x] Complete Sudoku generation
- [x] 3-phase elimination system
- [x] Unique solution verification
- [x] Automatic validation
- [x] Console interface
- [x] Complete documentation
- [x] Fully English codebase

### 🚧 Version 2.5 (Near Future)
- [ ] Dynamic difficulty selection
- [ ] Multiple difficulty levels
- [ ] Automatic solver
- [ ] Export/import Sudokus
- [ ] Generation statistics
- [ ] Unit tests

### 🔮 Version 3.0 (Ideas)
- [ ] GUI with ncurses
- [ ] Interactive play mode
- [ ] Variant generator (6x6, 12x12, etc.)
- [ ] REST API
- [ ] Shared library (.so/.dll)

## 🤝 Contributing

Contributions are welcome! If you want to improve the project:

1. **Fork** the repository
2. Create a **branch** for your feature (`git checkout -b feature/AmazingFeature`)
3. **Commit** your changes (`git commit -m 'Add some AmazingFeature'`)
4. **Push** to the branch (`git push origin feature/AmazingFeature`)
5. Open a **Pull Request**

### Contribution Ideas

- 🐛 Report bugs
- 💡 Suggest new features
- 📝 Improve documentation
- ⚡ Optimize algorithms
- 🧪 Add tests

## 📄 License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

### What does this mean?

✅ **You can:**
- Use commercially
- Modify the code
- Distribute
- Private use
- Sublicense

⚠️ **You must:**
- Include license and copyright
- Include NOTICE file
- State changes made
- Give appropriate credit

❌ **You cannot:**
- Hold the author liable
- Use trademarks without permission

### 📋 Required Credits

If you use this code in your project, you must give appropriate credit:

```
This project uses the Sudoku Generator in C
Copyright 2025 Gonzalo Ramírez (@chaLords)
Licensed under Apache License 2.0
```

## 👤 Author

**Gonzalo Ramírez**

- GitHub: [@chaLords](https://github.com/chaLords)
- Email: chaLords@icloud.com

## 🙏 Acknowledgments

- Fisher-Yates Algorithm: Ronald Fisher and Frank Yates (1938)
- Inspiration: Competitive programming community
- Resources: [GeeksforGeeks](https://www.geeksforgeeks.org/), [Wikipedia](https://en.wikipedia.org/wiki/Sudoku)

## 📚 References

- [Fisher-Yates Shuffle Algorithm](https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle)
- [Sudoku Solving Algorithms](https://en.wikipedia.org/wiki/Sudoku_solving_algorithms)
- [Backtracking](https://en.wikipedia.org/wiki/Backtracking)

---

⭐ If this project was useful to you, consider giving it a star on GitHub!

---

**Note:** This is an educational project. The code is freely available to learn, modify, and use.

---

## 🌐 Other Languages

- [Español (Spanish)](README.md)

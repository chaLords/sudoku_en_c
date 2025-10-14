# 🎮 Sudoku Generator in C

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![C](https://img.shields.io/badge/C-00599C?style=flat&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey)](https://github.com/chaLords/sudoku_en_c)
[![Version](https://img.shields.io/badge/version-2.2.1-brightgreen.svg)](https://github.com/chaLords/sudoku_en_c/releases)

> An efficient complete Sudoku generator using a hybrid approach: Fisher-Yates algorithm for independent subgrids and backtracking to complete the rest of the board. Now featuring refactored architecture based on data structures.
## 📋 Table of Contents

- [Features](#-features)
- [Demo](#-demo)
- [Installation](#-installation)
- [Verbosity Modes](#-verbosity-modes)
- [Usage](#-usage)
- [Cross-Platform Compatibility](#-cross-platform-compatibility)
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
- ✅ **Configurable verbosity modes** (0/1/2)
- ✅ **Compatible with Linux, macOS, and Windows** (automatic UTF-8 configuration)
- ✅ **Refactored architecture with data structures** (v2.2.1)
- ✅ **Efficient memory management** with pointers and malloc/free
- ✅ **Modular and maintainable code** following SOLID principles

## 🎯 Demo

### Program Output

```
═══════════════════════════════════════════════════════════════
    SUDOKU GENERATOR v2.2.1 – STRUCTURE-BASED REFACTORING
           Fisher-Yates + Backtracking + 3 Phases
═══════════════════════════════════════════════════════════════

🚀 ATTEMPT #1:
🎲 Filling diagonal with Fisher-Yates...
   SubGrid 0 (base: 0,0): 5 1 9 8 2 4 6 7 3 
   SubGrid 4 (base: 3,3): 7 1 4 9 3 6 5 2 8 
   SubGrid 8 (base: 6,6): 7 3 2 9 5 4 8 1 6 
✅ Diagonal successfully filled!

🔄 Backtracking in progress...
✅ Completed!
🎲 PHASE 1: Selecting numbers per subgrid with Fisher-Yates...
   Subgrid 0 (base: 0,0): 9 
   Subgrid 1 (base: 0,3): 2 
   Subgrid 2 (base: 0,6): 5 
   Subgrid 3 (base: 3,0): 8 
   Subgrid 4 (base: 3,3): 3 
   Subgrid 5 (base: 3,6): 1 
   Subgrid 6 (base: 6,0): 6 
   Subgrid 7 (base: 6,3): 4 
   Subgrid 8 (base: 6,6): 7 
✅ Phase 1 completed!
📊 PHASE 1 TOTAL: Removed 9 cells

--- ROUND 1 ---
🎲 PHASE 2: Selecting numbers without alternatives...
   Subgrid 0 (base: 0,0): 5 
   Subgrid 1 (base: 0,3): 8 
   Subgrid 2 (base: 0,6): 6 
   Subgrid 3 (base: 3,0): 3 
   Subgrid 4 (base: 3,3): 7 
   Subgrid 5 (base: 3,6): 6 
   Subgrid 6 (base: 6,0): 5 
   Subgrid 7 (base: 6,3): 1 
   Subgrid 8 (base: 6,6): 3 
✅ Phase 2 completed! Removed: 9

--- ROUND 2 ---
🎲 PHASE 2: Selecting numbers without alternatives...
   Subgrid 0 (base: 0,0): 1 
   Subgrid 1 (base: 0,3): 7 
   Subgrid 2 (base: 0,6): 3 
   Subgrid 3 (base: 3,0): 2 
   Subgrid 4 (base: 3,3): 9 
   Subgrid 5 (base: 3,6): 2 
   Subgrid 6 (base: 6,0): 4 
   Subgrid 7 (base: 6,3): 8 
   Subgrid 8 (base: 6,6): 9 
✅ Phase 2 completed! Removed: 9

--- ROUND 3 ---
🎲 PHASE 2: Selecting numbers without alternatives...
   Subgrid 0 (base: 0,0): 4 
   Subgrid 1 (base: 0,3): 
   Subgrid 2 (base: 0,6): 4 
   Subgrid 3 (base: 3,0): 
   Subgrid 4 (base: 3,3): 5 
   Subgrid 5 (base: 3,6): 4 
   Subgrid 6 (base: 6,0): 7 
   Subgrid 7 (base: 6,3): 
   Subgrid 8 (base: 6,6): 
✅ Phase 2 completed! Removed: 5

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


📊 PHASE 2 TOTAL: 3 rounds, removed 23 cells

🎲 PHASE 3: Free elimination with unique solution verification...
   Removed 8 at memAddr: 0x5ab31d02b410 (6,2) - Total: 1
   Removed 1 at memAddr: 0x5ab31d02b418 (5,0) - Total: 2
   Removed 6 at memAddr: 0x5ab31d02b420 (4,5) - Total: 3
   Removed 5 at memAddr: 0x5ab31d02b428 (7,7) - Total: 4
   Removed 3 at memAddr: 0x5ab31d02b430 (7,3) - Total: 5
   Removed 3 at memAddr: 0x5ab31d02b438 (8,1) - Total: 6
   Removed 6 at memAddr: 0x5ab31d02b440 (2,0) - Total: 7
   Removed 3 at memAddr: 0x5ab31d02b448 (1,5) - Total: 8
   Removed 9 at memAddr: 0x5ab31d02b450 (2,5) - Total: 9
   Removed 1 at memAddr: 0x5ab31d02b458 (7,2) - Total: 10
   Removed 9 at memAddr: 0x5ab31d02b460 (5,1) - Total: 11
   Removed 6 at memAddr: 0x5ab31d02b468 (6,4) - Total: 12
   Removed 2 at memAddr: 0x5ab31d02b470 (8,3) - Total: 13
   Removed 1 at memAddr: 0x5ab31d02b480 (1,6) - Total: 14
   Removed 7 at memAddr: 0x5ab31d02b488 (0,7) - Total: 15
   Removed 7 at memAddr: 0x5ab31d02b490 (2,1) - Total: 16
   Removed 9 at memAddr: 0x5ab31d02b498 (1,7) - Total: 17
   Removed 9 at memAddr: 0x5ab31d02b4b8 (8,4) - Total: 18
   Removed 5 at memAddr: 0x5ab31d02b4c8 (3,6) - Total: 19
   Removed 2 at memAddr: 0x5ab31d02b4d0 (2,7) - Total: 20
   Removed 5 at memAddr: 0x5ab31d02b4d8 (2,4) - Total: 21
   Removed 7 at memAddr: 0x5ab31d02b4f0 (5,8) - Total: 22
   Removed 8 at memAddr: 0x5ab31d02b4f8 (4,7) - Total: 23
   Removed 8 at memAddr: 0x5ab31d02b508 (5,5) - Total: 24
   Removed 8 at memAddr: 0x5ab31d02b538 (1,0) - Total: 25
✅ Phase 3 completed! Removed: 25
✅ SUCCESS! Sudoku generated

┌───────┬───────┬───────┐
│ . . . │ . 4 . │ . . . │
│ . 2 . │ 6 . . │ . . . │
│ . . 3 │ 1 . . │ . . 8 │
├───────┼───────┼───────┤
│ . . . │ . 1 4 │ . . 9 │
│ 7 4 5 │ . . . │ . . . │
│ . . 6 │ . 2 . │ 3 . . │
├───────┼───────┼───────┤
│ 9 . . │ . . . │ . . 2 │
│ 2 . . │ . . 7 │ . . 4 │
│ . . . │ . . 5 │ 8 1 6 │
└───────┴───────┴───────┘
📊 Empty: 57 | Clues: 24

🎉 VERIFIED! The puzzle is valid

📊 Difficulty level: EXPERT

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

#### 🐧 **Linux / macOS**
```bash
# Simple compilation
gcc main.c -o sudoku

# With optimizations (recommended)
gcc -O2 main.c -o sudoku -std=c11

# With detailed warnings
gcc -Wall -Wextra main.c -o sudoku -std=c11
```

**Note**: Using `-std=c11` is recommended to leverage modern language features.

#### 🪟 **Windows (VS Code / MinGW / MSYS2)**
```bash
# Compilation with UTF-8 support and C11
gcc -g main.c -o sudoku.exe -fexec-charset=UTF-8 -std=c11

# Or simply (program auto-configures itself)
gcc main.c -o sudoku.exe -std=c11
```
**Note:** The program automatically detects Windows and configures UTF-8 at runtime. No additional configuration needed.

## 🎛️ Verbosity Modes

### Version 2.2.0: Configurable Output Control

The generator supports three verbosity levels controllable via command-line arguments:

#### Mode 0: Minimal
**Purpose:** Clean output for presentations or benchmarks

```bash
./sudoku 0
```

**Shows:**
- Program title
- Final Sudoku board
- Difficulty level

**Execution time:** ~0.22s

**Sample output:**
```
═══════════════════════════════════════════════════════════════
  SUDOKU GENERATOR v2.2.1 – STRUCTURE-BASED REFACTORING
═══════════════════════════════════════════════════════════════

┌───────┬───────┬───────┐
│ . . . │ . 2 1 │ . . . │
│ 5 . . │ 4 . . │ 7 . . │
│ . 7 . │ . 8 . │ . . . │
├───────┼───────┼───────┤
│ . 3 . │ . . . │ . . 6 │
│ . . . │ 5 . 9 │ 2 . 3 │
│ . . 4 │ . . 7 │ . . . │
├───────┼───────┼───────┤
│ . . . │ . . . │ . . 8 │
│ 6 5 9 │ . . 2 │ . . . │
│ . . . │ . . 4 │ 9 1 . │
└───────┴───────┴───────┘


📊 Difficulty level: EXPERT
```

---

#### Mode 1: Compact (Default)
**Purpose:** Normal usage with summarized information

```bash
./sudoku 1
# or simply:
./sudoku
```

**Shows:**
- Generation phase summaries
- Elimination statistics
- Final board
- Empty/filled cell count
- Difficulty level

**Execution time:** ~0.56s

**Sample output:**
```
═══════════════════════════════════════════════════════════════
  SUDOKU GENERATOR v2.2.1 – STRUCTURE-BASED REFACTORING
═══════════════════════════════════════════════════════════════

🚀 ATTEMPT #1:
🎲 Diagonal + Backtracking...✅ Completed!
🎲 Phase 1: Fisher-Yates selection...✅ Phase 1 completed!
📊 PHASE 1 TOTAL: Removed 9 cells

🎲 Phase 2: Removal rounds...
📊 PHASE 2 TOTAL: 4 rounds, removed 23 cells

🎲 Phase 3: Free elimination...✅ Phase 3 completed! Removed: 25
✅ SUCCESS! Sudoku generated

┌───────┬───────┬───────┐
│ . . . │ . . . │ . . . │
│ . . . │ 5 . 1 │ 9 . . │
│ 7 . 9 │ . 3 . │ . 2 . │
├───────┼───────┼───────┤
│ . 4 . │ . . . │ . . 5 │
│ . 8 . │ 6 . 3 │ . . . │
│ . . 2 │ . . . │ . . . │
├───────┼───────┼───────┤
│ 1 . . │ . . 5 │ . 3 . │
│ . 6 . │ . . . │ . . . │
│ 4 5 . │ 1 2 7 │ 6 9 . │
└───────┴───────┴───────┘
📊 Empty: 57 | Clues: 24

🎉 VERIFIED! The puzzle is valid

📊 Difficulty level: EXPERT

```

---

#### Mode 2: Detailed
**Purpose:** Complete debugging and detailed algorithm analysis

```bash
./sudoku 2
```

**Shows:**
- All generation steps
- Fisher-Yates generated numbers
- Each PHASE 2 round
- Each removed cell in PHASE 3
- Complete debugging information

**Execution time:** ~0.08s (variable)

**Sample output (partial):**
```
═══════════════════════════════════════════════════════════════
    SUDOKU GENERATOR v2.2.1 – STRUCTURE-BASED REFACTORING
           Fisher-Yates + Backtracking + 3 Phases
═══════════════════════════════════════════════════════════════

🚀 ATTEMPT #1:
🎲 Filling main diagonal with Fisher-Yates...
   Subgrid 0 (base: 0,0): 5 3 7 6 2 1 9 8 4 
   Subgrid 4 (base: 3,3): 8 1 6 4 5 7 9 2 3 
   Subgrid 8 (base: 6,6): 2 7 9 3 5 1 8 4 6 
✅ Diagonal completed!

🔄 Completing with backtracking...
🎲 PHASE 1: Selecting numbers per subgrid with Fisher-Yates...
   Subgrid 0 (base: 0,0): 4 
   Subgrid 1 (base: 0,3): 9 
   [...]

--- ROUND 1 ---
🎲 PHASE 2: Selecting numbers without alternatives...
   Subgrid 0 (base: 0,0): 1 
   [...]

🎲 PHASE 3: Free elimination with unique solution verification...
   Removed 8 at (6,0) - Total: 1
   Removed 6 at (8,6) - Total: 2
   [...]
✅ Phase 3 completed! Removed: 23

✅ SUCCESS! Sudoku generated
[Board]
```

---

### Mode Comparison

| Feature | Mode 0 | Mode 1 | Mode 2 |
|---------|--------|--------|--------|
| **Phases shown** | No | Summary | Complete |
| **Statistics** | Minimal | Totals | Detailed |
| **PHASE 2 rounds** | No | Total | Each one |
| **PHASE 3 cells** | No | Total | One by one |
| **Best for** | Presentations | Daily use | Debugging |
| **Output lines** | ~15 | ~25 | ~80+ |

---

### Usage with Time

Measure generator performance:

```bash
# Minimal mode for clean benchmarks
time ./sudoku 0

# Compact mode to see statistics
time ./sudoku 1

# Detailed mode for deep analysis
time ./sudoku 2
```

---

### Changing Default Mode

To change the default mode (currently mode 1), edit `main.c`:

```c
int VERBOSITY_LEVEL = 1;  // Change to 0, 1, or 2
```

Then recompile:
```bash
gcc -O2 main.c -o sudoku -std=c11
```

## 🚀 Usage

### Basic Execution

#### 🐧 Linux / macOS
```bash
./sudoku
```

#### 🪟 Windows
```bash
sudoku.exe
# or simply:
./sudoku.exe
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
    // Allocate memory for board
    SudokuBoard *board = (SudokuBoard *)malloc(sizeof(SudokuBoard));
    if(board == NULL) {
        fprintf(stderr, "Error: Could not allocate memory\n");
        return 1;
    }
    
    GenerationStats stats;
    
    // Generate playable puzzle
    if(generateSudoku(board, &stats)) {
        printBoard(board);
        
        // Verify validity
        if(validateBoard(board)) {
            printf("✅ Valid puzzle!\n");
            printf("📊 Difficulty: %s\n", evaluateDifficulty(board));
        }
    }
    
    // Free memory
    free(board);
    return 0;
}
```

**Note**: This example demonstrates the use of new data structures and dynamic memory management introduced in v2.2.1

### Configure Difficulty

Modify the `PHASE3_TARGET` constant in `main.c`:

```c
#define PHASE3_TARGET 5   // Easy (~35 empty cells)
#define PHASE3_TARGET 15  // Medium (~45 empty cells)
#define PHASE3_TARGET 25  // Hard (~55 empty cells)
```

## 🌍 Cross-Platform Compatibility

### ✅ Automatic UTF-8 Configuration

The program automatically detects the operating system and configures the correct encoding:

```c
int main() {
    // Configure encoding based on operating system
    #ifdef _WIN32
        system("chcp 65001 > nul");  // UTF-8 on Windows
    #endif
    
    // ... rest of code
}
```

### 📝 What Does This Mean?

- **🐧 Linux/macOS:** The code compiles without Windows-specific lines. No overhead, no changes.
- **🪟 Windows:** The code automatically configures UTF-8 to properly display special characters (═, │, ┌, etc.).

### 🔧 Troubleshooting on Windows

#### Problem: Corrupted characters (�������)

**Cause:** Windows terminal using incorrect encoding.

**Automatic Solution:** The program auto-configures on execution. You don't need to do anything.

**Manual Solution (if still fails):**

1. **In VS Code Terminal:**
   - Open `settings.json` (Ctrl+Shift+P → "Preferences: Open Settings (JSON)")
   - Add:
   ```json
   {
       "terminal.integrated.defaultProfile.windows": "Command Prompt",
       "terminal.integrated.profiles.windows": {
           "Command Prompt": {
               "path": "C:\\Windows\\System32\\cmd.exe",
               "args": ["/K", "chcp 65001"]
           }
       }
   }
   ```

2. **In PowerShell:**
   ```powershell
   chcp 65001
   ./sudoku.exe
   ```

3. **In CMD:**
   ```cmd
   chcp 65001
   sudoku.exe
   ```

### 📋 .gitattributes File

The project includes a `.gitattributes` file to maintain line ending consistency:

```
*.c text eol=lf
*.h text eol=lf
*.md text eol=lf
```

This ensures the code works correctly when cloned on any operating system.
## 🏗️ Code Architecture (v2.2.1)

### Structure-Based Refactoring

Version 2.2.1 introduces a complete code refactoring using modern software engineering principles:

#### Data Structures

**Position**: Abstracts coordinates (row, column)
```c
typedef struct {
    int row;
    int col;
} Position;
```

**SudokuBoard**: Encapsulates the board and its metadata
```c
typedef struct {
    int cells[SIZE][SIZE];
    int clues;
    int empty;
} SudokuBoard;
```

**SubGrid**: Represents 3x3 regions of the board
```c
typedef struct {
    int index;
    Position base;
} SubGrid;
```

**GenerationStats**: Groups generation statistics
```c
typedef struct {
    int phase1_removed;
    int phase2_removed;
    int phase2_rounds;
    int phase3_removed;
} GenerationStats;
```

#### Refactoring Advantages

**Improved modularity**: Each function has a clear, single responsibility.

**Memory efficiency**: Using pointers reduces unnecessary copies. Instead of passing 324 bytes by value, we pass only 8 bytes (memory address).

**Extensibility**: Adding new board features (like timestamps or configurations) only requires modifying structures, not function signatures.

**Maintainability**: Clear separation of concerns makes code easier to understand, test, and modify.

**Type safety**: Using `const` prevents accidental modifications and enables compiler optimizations.

#### Technical Improvements

- **Forward declarations**: Professional code organization with forward declarations
- **Typedef**: Simplifies declarations and improves readability
- **Const correctness**: Read-only parameters explicitly marked
- **Dynamic memory**: Educational use of malloc/free in phase 3
- **Doxygen documentation**: Professional comments on all functions

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
- Uses `countSolutionsExact()` with early exit for efficiency

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
vvoid fisherYatesShuffle(int *array, int size, int start_value) {
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
```

**Properties:**
- Generates uniformly random permutations
- Each permutation has equal probability
- Linear time O(n)

### Backtracking with Pruning

**Complexity:** O(9^m) where m = empty cells

```c
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
```

**Optimizations:**
- Early pruning with `isSafePosition()`
- Efficient O(1) verification per cell
- Randomized numbers for variety

### Unique Solution Verification

**Complexity:** O(9^m) worst case, O(9^k) typical with early exit

```c
int countSolutionsExact(SudokuBoard *board, int limit) {
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
├── CHANGELOG.md           # Version history
├── NOTICE                 # Attribution file
├── .gitignore             # Files to ignore in Git
├── .gitattributes         # Line ending normalization
└── docs/
    ├── ALGORITHMS.md      # Detailed mathematical analysis
    └── TECHNICAL.md       # Deep technical documentation
```

### Main Functions

| Function | Description | Complexity |
|----------|-------------|------------|
| `fisherYatesShuffle()` | Generates random permutation | O(n) |
| `isSafePosition()` | Validates if number is valid | O(1) |
| `findEmptyCell()` | Finds next empty cell | O(n²) |
| `fillDiagonal()` | Fills independent subgrids | O(1) |
| `fillSubGrid()` | Fills specific subgrid | O(1) |
| `completeSudoku()` | Completes with backtracking | O(9^m) |
| `hasAlternative()` | Checks for alternatives | O(1) |
| `phase1Elimination()` | PHASE 1: Random elimination | O(1) |
| `phase2Elimination()` | PHASE 2: No-alternatives elimination | O(n²) |
| `phase3Elimination()` | PHASE 3: Free verified elimination | O(n² × 9^m) |
| `countSolutionsExact()` | Counts solutions with early exit | O(9^m) |
| `generateSudoku()` | Orchestrates entire process | - |
| `printBoard()` | Prints board visually | O(n²) |
| `validateBoard()` | Validates complete board | O(n²) |
| `evaluateDifficulty()` | Evaluates difficulty by clues | O(1) |
| `initBoard()` | Initializes empty board | O(n²) |
| `updateBoardStats()` | Updates statistics | O(n²) |
| `createSubGrid()` | Creates SubGrid structure | O(1) |
| `getPositionInSubGrid()` | Calculates position in subgrid | O(1) |

## 🗺️ Roadmap

### ✅ Version 2.2.1 (Current - January 2025)
- [x] Complete Sudoku generation
- [x] 3-phase elimination system
- [x] Unique solution verification
- [x] Automatic validation
- [x] Console interface
- [x] Complete documentation
- [x] Fully English codebase
- [x] Cross-platform compatibility (Linux/macOS/Windows)
- [x] Automatic UTF-8 configuration
- [x] Configurable verbosity modes (0/1/2)
- [x] Command-line argument parsing
- [x] **Refactoring with data structures**
- [x] **Professional forward declarations**
- [x] **Memory management with malloc/free**
- [x] **Modular and extensible architecture**

### 🚧 Version 2.3 (Next - Q1 2025)
- [ ] Unit tests with testing framework
- [ ] Flexible configuration (Config struct)
- [ ] Puzzle export/import (save/load)
- [ ] Detailed generation statistics

### 🔮 Version 3.0 (Future - Q2 2025)
- [ ] Multi-size support (4x4, 16x16, 25x25)
- [ ] Enhanced CLI with two-digit system (`./sudoku 00`)
- [ ] Dynamic memory for variable boards
- [ ] Integrated difficulty selector
- [ ] Automatic solver
- [ ] GUI with ncurses
- [ ] Interactive play mode

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
- 🌍 Improve cross-platform compatibility

## 📄 License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

### What Does This Mean?

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

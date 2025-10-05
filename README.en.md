# 🎮 Sudoku Generator in C

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://www.apache.org/licenses/LICENSE-2.0)
[![C](https://img.shields.io/badge/C-00599C?style=flat&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey)](https://github.com/chaLords/sudoku_en_c)
[![Version](https://img.shields.io/badge/version-2.0.0-green)](https://github.com/chaLords/sudoku_en_c/releases)

> An efficient Sudoku puzzle generator using a hybrid approach: Fisher-Yates algorithm for independent subgrids, backtracking to complete the board, and a 3-phase system to create playable puzzles with guaranteed unique solutions.

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
- ✅ **Playable puzzle generation with 3-phase elimination**
- ✅ **Unique solution verification** using solution counter
- ✅ **Optimized hybrid method** (Fisher-Yates + Backtracking)
- ✅ **High success rate** (~99.9%)
- ✅ **Uniformly random permutations**
- ✅ **Automatic board validation**
- ✅ **Multiple difficulty levels** (configurable)
- ✅ **Clean and well-documented code**
- ✅ **Visual console interface** with box-drawing characters

## 🎯 Demo

### Program Output (v2.0)

```
═══════════════════════════════════════════════════════════════
              RECOMMENDED HYBRID METHOD
          Your Fisher-Yates + Backtracking
═══════════════════════════════════════════════════════════════

🚀 ATTEMPT #1:
🎲 Filling main diagonal with Fisher-Yates...
   Subgrid 0 (base: 0,0): 3 6 4 1 5 2 8 9 7 
   Subgrid 4 (base: 3,3): 3 7 4 9 6 1 5 2 8 
   Subgrid 8 (base: 6,6): 2 4 5 7 9 8 1 6 3 
✅ Diagonal completed!

🔄 Completing with backtracking...

🎲 PHASE 1: Choosing numbers per subgrid with Fisher-Yates...
Random array generated: 5 8 3 1 7 9 6 2 4
   Subgrid 0 (base: 0,0): 5
   Subgrid 1 (base: 0,3): 8
   Subgrid 2 (base: 0,6): 3
   Subgrid 3 (base: 3,0): 1
   Subgrid 4 (base: 3,3): 7
   Subgrid 5 (base: 3,6): 9
   Subgrid 6 (base: 6,0): 6
   Subgrid 7 (base: 6,3): 2
   Subgrid 8 (base: 6,6): 4
✅ Phase 1 completed!

--- ROUND 1 ---
🎲 PHASE 2: Choosing numbers per subgrid with Fisher-Yates...
   Subgrid_2 0 (base: 0,0): 
   Subgrid_2 1 (base: 0,3): 
   Subgrid_2 2 (base: 0,6): 
   Subgrid_2 3 (base: 3,0): 
   Subgrid_2 4 (base: 3,3): 
   Subgrid_2 5 (base: 3,6): 
   Subgrid_2 6 (base: 6,0): 
   Subgrid_2 7 (base: 6,3): 
   Subgrid_2 8 (base: 6,6): 
✅ Phase 2 completed! Removed: 0

🛑 No more numbers can be removed in PHASE 2

🎯 PHASE 3: Free elimination with unique solution verification...
Removed 3 at (1,1) - Total: 1
Removed 9 at (4,0) - Total: 2
Removed 9 at (8,3) - Total: 3
Removed 4 at (2,7) - Total: 4
Removed 7 at (8,5) - Total: 5
Removed 3 at (4,6) - Total: 6
Removed 8 at (5,5) - Total: 7
Removed 2 at (8,2) - Total: 8
Removed 1 at (5,2) - Total: 9
Removed 7 at (5,8) - Total: 10
Removed 9 at (5,8) - Total: 11
Removed 1 at (1,8) - Total: 12
Removed 6 at (0,3) - Total: 13
Removed 5 at (0,5) - Total: 14
Removed 8 at (7,1) - Total: 15
Removed 4 at (5,4) - Total: 16
Removed 8 at (8,7) - Total: 17
Removed 6 at (5,6) - Total: 18
Removed 3 at (7,8) - Total: 19
Removed 4 at (8,6) - Total: 20
PHASE 3 completed! Removed: 20

✅ SUCCESS! Sudoku generated

┌───────┬───────┬───────┐
│ * * * │ * * * │ * * 2 │
│ * * 7 │ * 9 4 │ * 6 * │
│ 8 9 * │ * 1 * │ 7 * 5 │
├───────┼───────┼───────┤
│ * 7 * │ 2 * * │ * * * │
│ * 5 * │ * 6 1 │ * 2 8 │
│ 3 * * │ 8 * * │ * * * │
├───────┼───────┼───────┤
│ * 6 * │ * * 5 │ * 1 9 │
│ 4 * * │ * * * │ 5 * * │
│ * 1 * │ * 3 * │ * * * │
└───────┴───────┴───────┘
Empty cells: 54 | Filled cells: 27

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
    if(generarSudokuHibrido(sudoku)) {
        imprimirSudoku(sudoku);
        
        // Verify validity
        if(verificarSudoku(sudoku)) {
            printf("Valid puzzle!\n");
        }
    }
    
    return 0;
}
```

### Configure Difficulty

Modify the `objetivo_adicional` variable in the `generarSudokuHibrido()` function inside `main.c`:

```c
// In the generarSudokuHibrido() function
int objetivo_adicional = 10;  // Easy (~40-45 empty cells)
int objetivo_adicional = 20;  // Medium (~50-55 empty cells)
int objetivo_adicional = 30;  // Hard (~60-65 empty cells)
```

**Note:** The final number of empty cells depends on:
- PHASE 1: Always removes 9 cells (1 per subgrid)
- PHASE 2: Removes variable amount based on structure (0-25 cells typically)
- PHASE 3: Removes up to specified target while maintaining unique solution

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

#### **ELIMINATION: 3-Phase System for Playable Puzzles**

**PHASE 1: Random Elimination per Subgrid**
- Removes 1 random number from each subgrid (9 cells)
- Uses Fisher-Yates to choose which number (1-9) to remove
- **Guarantees uniform distribution**

**PHASE 2: Elimination Without Alternatives (Loop)**
- Searches for numbers with NO alternatives in their row/column/subgrid
- If found, removes it and repeats the process
- Continues until no more can be removed
- **Typically removes 0-25 additional cells**

**PHASE 3: Free Elimination with Verification**
- Attempts to remove additional cells in sequential order
- Verifies **unique solution** using `contarSoluciones()`
- Configurable via `objetivo_adicional` variable
- **Allows difficulty adjustment**

### Advantages of Complete Method

| Feature | Pure Backtracking | Simple Elimination | **Hybrid 3 Phases** |
|---------|-------------------|-------------------|---------------------|
| Generation Speed | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Randomness | ⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Unique Solution | ❌ | ❌ | ✅ |
| Difficulty Control | ❌ | ⭐⭐ | ⭐⭐⭐⭐⭐ |
| Playable Puzzles | ❌ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |

## 🔧 Algorithms

### Fisher-Yates Shuffle

**Complexity:** O(n)

```c
void num_orden_fisher_yates(int *array, int size, int num_in) {
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
bool completarSudoku(int sudoku[SIZE][SIZE]) {
    // 1. Find empty cell
    if(!encontrarCeldaVacia(sudoku, &fila, &col)) {
        return true; // Complete!
    }
    
    // 2. Try numbers 1-9
    for(int num = 1; num <= 9; num++) {
        if(esSafePosicion(sudoku, fila, col, num)) {
            sudoku[fila][col] = num;
            
            // 3. Recurse
            if(completarSudoku(sudoku)) {
                return true;
            }
            
            // 4. Backtrack
            sudoku[fila][col] = 0;
        }
    }
    
    return false;
}
```

**Optimizations:**
- Early pruning with `esSafePosicion()`
- Efficient O(1) verification per cell
- Randomized numbers for variety

### Solution Counter

**Complexity:** O(9^m) with configurable limit

```c
int contarSoluciones(int sudoku[SIZE][SIZE], int limite) {
    // Find empty cell
    if(!encontrarCeldaVacia(sudoku, &fila, &col)) {
        return 1; // Solution found
    }
    
    int total = 0;
    for(int num = 1; num <= 9; num++) {
        if(esSafePosicion(sudoku, fila, col, num)) {
            sudoku[fila][col] = num;
            total += contarSoluciones(sudoku, limite);
            
            // Early exit if multiple found
            if(total >= limite) {
                sudoku[fila][col] = 0;
                return total;
            }
            
            sudoku[fila][col] = 0;
        }
    }
    return total;
}
```

**Usage:** Guarantees the puzzle has **exactly one solution**

## 📁 Code Structure

```
sudoku_en_c/
│
├── main.c                 # Main source code
├── README.md              # Documentation in Spanish
├── README.en.md           # This file (English)
├── LICENSE                # Apache License 2.0
├── NOTICE                 # Attribution requirements
├── CHANGELOG.md           # Version history and changes
├── .gitignore             # Git ignored files
└── docs/
    ├── TECHNICAL.md       # Technical documentation: architecture and performance
    └── ALGORITHMS.md      # Mathematical analysis: theorems and complexity
```

### Project Files

| File | Description |
|------|-------------|
| `main.c` | Complete source code with all generator functions |
| `README.md` / `README.en.md` | Main project documentation (Spanish/English) |
| `LICENSE` | Apache 2.0 License with terms of use |
| `NOTICE` | Attribution file required by Apache 2.0 |
| `CHANGELOG.md` | Chronological record of all changes between versions |
| `docs/TECHNICAL.md` | Detailed technical documentation: architecture, benchmarks, design decisions |
| `docs/ALGORITHMS.md` | Deep mathematical analysis: theorems, proofs, complexity |

### Main Functions

| Function | Description | Complexity |
|----------|-------------|------------|
| `num_orden_fisher_yates()` | Generates random permutation | O(n) |
| `esSafePosicion()` | Validates if a number is valid | O(1) |
| `llenarDiagonal()` | Fills independent subgrids | O(1) |
| `completarSudoku()` | Completes with backtracking | O(9^m) |
| `primeraEleccionAleatoria()` | PHASE 1: Random elimination | O(1) |
| `tieneAlternativaEnFilaCol()` | Checks number alternatives | O(1) |
| `segundaEleccionSinAlternativas()` | PHASE 2: No-alternatives elimination | O(n²) |
| `contarSoluciones()` | Counts possible solutions | O(9^m) |
| `terceraEleccionLibre()` | PHASE 3: Verified elimination | O(n² × 9^m) |
| `verificarSudoku()` | Validates complete board | O(n²) |
## 🗺️ Roadmap

### ✅ Version 1.0
- [x] Complete Sudoku generation
- [x] Automatic validation
- [x] Console interface
- [x] Basic documentation

### ✅ Version 2.0 (Current)
- [x] Playable puzzle generation
- [x] 3-phase elimination system
- [x] Unique solution verification
- [x] Configurable difficulty control
- [x] Complete technical documentation

### 🚧 Version 2.1 (Next)
- [ ] Interactive menu for difficulty selection
- [ ] Export puzzles to .txt file
- [ ] Batch mode (generate N puzzles)
- [ ] Unit tests

### 🔮 Version 3.0 (Future)
- [ ] Automatic solver with visualization
- [ ] GUI with ncurses
- [ ] Interactive play mode
- [ ] Variant generator (6x6, 12x12, Samurai, etc.)
- [ ] REST API
- [ ] Shared library (.so/.dll)
- [ ] Advanced generation statistics

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
- 🌐 Translate to other languages
- 🎨 Improve visual interface

## 📄 License

This project is licensed under the **Apache License 2.0** - see the [LICENSE](LICENSE) file for details.

### 🎯 Main Requirement: YOU MUST Give Credit

**If you use, modify, or distribute this code, YOU MUST:**

1. **✅ Credit the original author:**
   ```
   Based on Sudoku Generator by Gonzalo Ramírez (@chaLords)
   https://github.com/chaLords/sudoku_en_c
   ```

2. **✅ Include the NOTICE file** with attributions

3. **✅ Keep the copyright:**
   ```
   Copyright 2025 Gonzalo Ramírez
   Licensed under the Apache License, Version 2.0
   ```

4. **✅ State your modifications** if you change the code

### What can you do?

✅ **Allowed:**
- ✅ Commercial use (sell products based on this)
- ✅ Modify the code however you want
- ✅ Distribute copies
- ✅ Private use
- ✅ Sublicense your derivative work
- ✅ Patent protection

⚠️ **Required (or you violate the license):**
- ⚠️ **Give credit to the original author** (Gonzalo Ramírez)
- ⚠️ **Include a copy of the Apache 2.0 license**
- ⚠️ **Include the NOTICE file** with attributions
- ⚠️ **Document the changes** you make
- ⚠️ **Keep copyright notices**

❌ **Prohibited:**
- ❌ Hold the author liable for issues
- ❌ Use the name "Gonzalo Ramírez" or "@chaLords" for promotion without permission
- ❌ Use trademarks without authorization

### 🆚 Comparison with other licenses

| Feature | Apache 2.0 | MIT | GPL 3.0 |
|---------|------------|-----|---------|
| **Requires crediting author** | ✅ **YES** | ✅ YES | ✅ YES |
| Commercial use | ✅ | ✅ | ✅ |
| Modification | ✅ | ✅ | ✅ |
| Distribution | ✅ | ✅ | ✅ |
| Patent protection | ✅ **Explicit** | ❌ No | ✅ Yes |
| Requires documenting changes | ✅ **YES** | ❌ No | ✅ Yes |
| Requires open source | ❌ No | ❌ No | ✅ **YES** |

**Why Apache 2.0?**
- ✅ Better legal protection than MIT
- ✅ **Guarantees you get credit**
- ✅ Explicit patent protection
- ✅ Allows commercial use without restrictions
- ✅ Doesn't force open source (like GPL)

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
- [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0)

---

⭐ If this project was useful to you, consider giving it a star on GitHub!

---

**Note:** This is an educational project under Apache License 2.0. The code is freely available to learn, modify, and use, including commercial use under the conditions specified in the license.

---

## 🌐 Other Languages

- [Español (Spanish)](README.md)

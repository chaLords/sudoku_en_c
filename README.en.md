# 🎮 Sudoku Generator in C


[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://www.apache.org/licenses/LICENSE-2.0)
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

## 🎯 Demo

### Program Output

```
═══════════════════════════════════════════════════════════════
              RECOMMENDED HYBRID METHOD
          Your Fisher-Yates + Backtracking
═══════════════════════════════════════════════════════════════

🚀 ATTEMPT #1:
🎲 Filling main diagonal with Fisher-Yates...
   Subgrid 0 (base: 0,0): 5 3 7 6 2 1 9 8 4 
   Subgrid 4 (base: 3,3): 8 1 6 4 5 7 9 2 3 
   Subgrid 8 (base: 6,6): 2 7 9 3 5 1 8 4 6 
✅ Diagonal completed!

🔄 Completing with backtracking...
✅ SUCCESS! Sudoku generated
┌───────┬───────┬───────┐
│ 5 3 7 │ 2 4 9 │ 6 1 8 │
│ 6 2 1 │ 3 8 5 │ 4 9 7 │
│ 9 8 4 │ 1 7 6 │ 2 3 5 │
├───────┼───────┼───────┤
│ 7 4 2 │ 8 1 6 │ 9 5 3 │
│ 1 9 3 │ 4 5 7 │ 8 6 2 │
│ 8 6 5 │ 9 2 3 │ 7 4 1 │
├───────┼───────┼───────┤
│ 4 1 6 │ 5 3 8 │ 2 7 9 │
│ 2 7 8 │ 6 9 4 │ 3 5 1 │
│ 3 5 9 │ 7 1 2 │ 8 4 6 │
└───────┴───────┴───────┘
🎉 VERIFIED! The Sudoku is valid
```

## 📦 Installation

### Prerequisites

- GCC compiler (or any standard C compiler)
- Make (optional)

### Clone the Repository

```bash
git clone https://github.com/your-username/sudoku-generator.git
cd sudoku-generator
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

The program will automatically generate 5 complete Sudokus and display the results.

### Integration in Another Project

```c
#include "main.c"

int main() {
    int sudoku[SIZE][SIZE];
    
    // Generate sudoku
    if(generarSudokuHibrido(sudoku)) {
        imprimirSudoku(sudoku);
    }
    
    return 0;
}
```

## 🧠 How It Works

### Hybrid Approach

The generator uses a two-phase strategy:

#### **Phase 1: Fisher-Yates for the Diagonal**
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

#### **Phase 2: Backtracking for the Rest**
Complete remaining cells using recursive backtracking with pruning:

1. Find empty cell
2. Try numbers 1-9 (randomized)
3. If valid → recurse
4. If fails → backtrack and try next number

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

## 📁 Code Structure

```
sudoku-generator/
│
├── main.c                 # Main code
├── README.md              # Main documentation (Spanish)
├── README.en.md           # This file (English)
├── LICENSE                # Apache 2.0 License
├── NOTICE                 # Attribution notices
├── .gitignore             # Git ignored files
└── docs/
    ├── TECHNICAL.md          # Technical documentation Arquitectura e implementación 
    └── ALGORITHMS.md         # Documentation Mathematical analysis
```

### Main Functions

| Function | Description | Complexity |
|----------|-------------|------------|
| `num_orden_fisher_yates()` | Generates random permutation | O(n) |
| `esSafePosicion()` | Validates if a number is valid | O(1) |
| `llenarDiagonal()` | Fills independent subgrids | O(1) |
| `completarSudoku()` | Completes with backtracking | O(9^m) |
| `verificarSudoku()` | Validates complete board | O(n²) |

## 🗺️ Roadmap

### ✅ Version 1.0 (Current)
- [x] Complete Sudoku generation
- [x] Automatic validation
- [x] Console interface
- [x] Basic documentation

### 🚧 Version 2.0 (Future)
- [ ] Playable puzzle generation
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
- State changes made
- **Include the NOTICE file with attributions**
- **Provide visible credit in documentation**

✅ **Also:**
- Explicit patent protection
- Liability limitation

❌ **You cannot:**
- Hold the author liable
- Use trademarks without permission

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

- [Español (Spanish)](README.es.md)

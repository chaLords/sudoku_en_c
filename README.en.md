# 🎮 Sudoku Generator Library

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![C](https://img.shields.io/badge/C-00599C?style=flat&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Version](https://img.shields.io/badge/version-2.2.2-brightgreen.svg)](https://github.com/chaLords/sudoku_en_c/releases)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey)](https://github.com/chaLords/sudoku_en_c)

> Professional C library for generating Sudoku puzzles with guaranteed unique solutions. Modular architecture, clean API, and optimized hybrid Fisher-Yates + Backtracking algorithm.

```
┌───────┬───────┬───────┐
│ . 4 . │ . 3 . │ 7 . . │
│ . . 5 │ . . . │ . . . │
│ 7 9 2 │ . 6 . │ . . . │
├───────┼───────┼───────┤
│ . . . │ . 7 . │ . . 6 │
│ 2 . 9 │ . . 5 │ . . . │
│ . . . │ 1 . . │ . . 3 │
├───────┼───────┼───────┤
│ . . 1 │ . . 2 │ . . 7 │
│ . . . │ 4 . . │ 1 . . │
│ 8 . . │ 6 . 3 │ . 4 5 │
└───────┴───────┴───────┘
```

---

## ✨ Key Features

- **🎯 High Success Rate**: ~99.9% on first attempt
- **✓ Guaranteed Unique Solution**: Exhaustive verification with early-exit
- **⚡ Fast**: Typical generation <10ms
- **🎲 Uniformly Random**: Fisher-Yates for uniform distribution
- **🎚️ Difficulty Levels**: Easy, Medium, Hard, Expert
- **📦 Clean API**: Well-documented public interface
- **🔧 Modular**: Architecture with public-private separation
- **🌍 Cross-platform**: Linux, macOS, Windows
- **📖 Educational**: Documented code for learning

---

## 🚀 Quick Start

```bash
# Clone repository
git clone https://github.com/chaLords/sudoku_en_c.git
cd sudoku_en_c

# Build
make

# Run
./bin/sudoku_generator
```

### Use as Library

```c
#include <sudoku/sudoku.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    srand(time(NULL));
    
    SudokuBoard board;
    if (sudoku_generate(&board, NULL)) {
        sudoku_display_print_board(&board);
        
        SudokuDifficulty diff = sudoku_evaluate_difficulty(&board);
        printf("Difficulty: %s\n", sudoku_difficulty_to_string(diff));
    }
    
    return 0;
}
```

```bash
gcc my_program.c -I include -L lib -lsudoku_core -o my_program
./my_program
```

---

## 📦 Installation

### Prerequisites

- GCC/Clang (C11 or later)
- Make or CMake
- Git

### Option 1: Make (Recommended)

```bash
# Build library and CLI
make

# Library only
make lib

# CLI only
make cli

# Install system-wide (optional)
sudo make install

# Run tests
make test
```

This generates:
- `lib/libsudoku_core.a` - Static library
- `bin/sudoku_generator` - CLI executable
- Headers in `include/sudoku/`

### Option 2: CMake

```bash
mkdir build && cd build
cmake ..
make
sudo make install  # Optional
```

### Option 3: Manual Compilation

```bash
# Compile library
gcc -c -I include src/core/*.c src/core/*/*.c
ar rcs libsudoku.a *.o

# Compile CLI
gcc -I include tools/generator_cli/main.c -L. -lsudoku -o sudoku
```

---

## 📖 Basic Usage

### CLI - Verbosity Modes

```bash
# Mode 0: Minimal (result only)
./sudoku_generator 0

# Mode 1: Compact (process summary) - Default
./sudoku_generator 1
./sudoku_generator

# Mode 2: Detailed (complete debugging)
./sudoku_generator 2
```

### API - Main Functions

#### Generate Puzzle

```c
SudokuBoard board;
SudokuGenerationStats stats;

if (sudoku_generate(&board, &stats)) {
    // Generation successful
    printf("Clues: %d, Empty: %d\n", board.clues, board.empty);
}
```

#### Validate Puzzle

```c
if (sudoku_validate_board(&board)) {
    printf("✓ Valid puzzle\n");
}
```

#### Verify Unique Solution

```c
int solutions = countSolutionsExact(&board, 2);
if (solutions == 1) {
    printf("✓ Unique solution\n");
}
```

#### Configure Verbosity

```c
sudoku_set_verbosity(1);  // 0=minimal, 1=compact, 2=detailed
```

📖 **Complete documentation**: See [docs/API_REFERENCE.md](docs/API_REFERENCE.md)

---

## 🏗️ Project Structure

```
sudoku_en_c/
├── include/sudoku/       # 🔓 Public API
│   └── core/             # Public headers (types, generator, validation, board, display)
├── src/core/             # 🔒 Implementation
│   ├── algorithms/       # Backtracking, Fisher-Yates
│   ├── elimination/      # 3-phase system
│   └── internal/         # Private headers
├── tests/unit/           # ✅ Tests by module
├── tools/                # 🛠️ CLI and utilities
└── docs/                 # 📚 Technical documentation
```

**Modular architecture** with stable public API and flexible private implementation.

📖 **Detailed analysis**: See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)

---

## 🧠 Algorithm

### Generation (Hybrid)

1. **Fisher-Yates Diagonal**: Fill independent subgrids (0, 4, 8) with random permutations
2. **Backtracking**: Complete remaining 54 cells with randomized backtracking
3. **3-Phase Elimination**:
   - **Phase 1**: Random selection (1 per subgrid)
   - **Phase 2**: No-alternatives elimination (iterative)
   - **Phase 3**: Free elimination with uniqueness verification

### Technical Characteristics

- **Complexity**: O(9^m) where m = empty cells
- **Optimizations**: Early-exit, intelligent pruning, randomization
- **Verification**: Exhaustive counting with configurable limit
- **Distribution**: Uniformly random thanks to Fisher-Yates

📖 **Mathematical analysis**: See [docs/ALGORITHMS.md](docs/ALGORITHMS.md)

---

## 🗺️ Roadmap

### ✅ v2.2.2 (Current - November 2025)
- Complete modularization
- Stable public API (`include/sudoku/`)
- Architecture prepared for v3.0

### 🚧 v2.3 (Q1 2025)
- Complete unit tests
- Flexible configuration system
- Puzzle export/import

### 🔮 v3.0 (Q2 2025)
- Multi-size support (4x4, 16x16, 25x25)
- Opaque pointers
- Dynamic memory
- Integrated solver
- Interactive mode

---

## 🤝 Contributing

Contributions are welcome! To contribute:

1. Fork the repository
2. Create a branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

📖 **Complete guide**: See [CONTRIBUTING.md](CONTRIBUTING.md)

---

## 📄 License

This project is licensed under Apache License 2.0 - see [LICENSE](LICENSE) for details.

**Attribution Requirements**:
```
This project uses Sudoku Generator Library
Copyright 2025 Gonzalo Ramírez (@chaLords)
Licensed under Apache License 2.0
```

---

## 👤 Author

**Gonzalo Ramírez**
- GitHub: [@chaLords](https://github.com/chaLords)
- Email: chaLords@icloud.com

---

## 📚 Documentation

- **[API Reference](docs/API_REFERENCE.md)** - Complete reference of all public functions
- **[Architecture](docs/ARCHITECTURE.md)** - Architectural design and implementation decisions
- **[Algorithms](docs/ALGORITHMS.md)** - Mathematical analysis and algorithms
- **[Usage Guide](docs/USAGE.md)** - Advanced examples and use cases
- **[Contributing](CONTRIBUTING.md)** - Contributor's guide
- **[Changelog](CHANGELOG.md)** - Version history

---

## 🙏 Acknowledgments

- **Fisher-Yates Algorithm**: Ronald Fisher and Frank Yates (1938)
- **Inspiration**: Competitive programming community
- **References**: [GeeksforGeeks](https://www.geeksforgeeks.org/), [Wikipedia](https://en.wikipedia.org/wiki/Sudoku)

---

⭐ **If this project was useful to you, consider giving it a star on GitHub!**

---

## 🌐 Other Languages

- [Español](README.md)

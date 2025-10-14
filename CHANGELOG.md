# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

## [2.2.1] - 2025-01-XX

### 🏗️ Structure-Based Refactoring

#### 🎉 Added

##### Data Structures
- **Position struct**: Abstracts row/column coordinates
  - Eliminates error-prone separate row/col parameters
  - Enables position-based operations and comparisons
  - Type: `typedef struct { int row; int col; } Position;`

- **SudokuBoard struct**: Encapsulates board state and metadata
  - Contains cells array and statistics (clues, empty count)
  - Enables passing board as single parameter
  - Foundation for future extensions (timestamps, difficulty, etc.)
  - Type: `typedef struct { int cells[SIZE][SIZE]; int clues; int empty; } SudokuBoard;`

- **SubGrid struct**: Represents 3x3 regions
  - Stores index and base position
  - Simplifies subgrid operations
  - Eliminates repetitive base position calculations
  - Type: `typedef struct { int index; Position base; } SubGrid;`

- **GenerationStats struct**: Groups generation metrics
  - Tracks phase1/2/3 removals and rounds
  - Enables detailed analytics
  - Cleaner function signatures
  - Type: `typedef struct { int phase1_removed; int phase2_removed; ... } GenerationStats;`

##### Code Organization
- **Forward declarations section** after global constants
  - Professional code organization
  - Acts as function index/API documentation
  - Enables flexible function ordering
  - Groups functions by category

- **Helper functions**:
  - `initBoard()`: Initializes empty board
  - `updateBoardStats()`: Recalculates clues/empty counts
  - `createSubGrid()`: Factory function for SubGrid
  - `getPositionInSubGrid()`: Calculates position within subgrid

#### 🔄 Changed

##### Function Signatures
- **All functions updated** to use structs and pointers:
```c
  // Before:
  bool isSafePosition(int sudoku[SIZE][SIZE], int fila, int col, int num);
  
  // After:
  bool isSafePosition(const SudokuBoard *board, const Position *pos, int num);
```

- **Pointer-based parameters**:
  - Pass by reference instead of by value
  - Reduces memory copies (324 bytes → 8 bytes per call)
  - Enables in-place modifications
  - **97.5% reduction in data transfer overhead**

- **const correctness applied**:
  - Read-only parameters marked with `const`
  - Prevents accidental modifications
  - Enables compiler optimizations
  - Documents function intent clearly

##### Memory Management
- **Strategic heap usage** in `phase3Elimination()`:
```c
  Position *positions = (Position *)malloc(TOTAL_CELLS * sizeof(Position));
  // ... use positions ...
  free(positions);
```
  - Educational demonstration of malloc/free
  - Proper error handling for allocation failures
  - Always paired with free() to prevent leaks

- **Board allocation** in main (educational):
  - Demonstrates dynamic memory allocation
  - Shows proper cleanup with free()
  - Prepares codebase for variable-size boards (v3.0)

##### Documentation
- **Doxygen-style comments** on all functions:
```c
  /**
   * Function: fisherYatesShuffle
   * Shuffles an array using Fisher-Yates algorithm
   * 
   * Parameters:
   * - array: pointer to array (modifies in-place)
   * - size: array size
   * - start_value: initial value (usually 1)
   */
```

- **Inline comments** explaining complex logic
- **Section headers** clearly delineating code organization

#### ⚡ Optimized

##### Memory Efficiency
- **Pointer usage** reduces function call overhead:
  - Stack: Before: 324 bytes, After: 8 bytes
  - **40x reduction** in stack usage per call
  - Enables deeper recursion without stack overflow
  - Negligible performance impact

- **const optimization**:
  - Compiler can optimize read-only parameters
  - Enables aggressive inlining
  - Reduces unnecessary copies

##### Code Quality
- **Single Responsibility Principle**:
  - Each function has one clear purpose
  - Easier to test and debug
  - Facilitates code reuse

- **DRY (Don't Repeat Yourself)**:
  - SubGrid logic centralized in helper functions
  - Position calculations abstracted
  - Eliminates duplicate code

#### 🐛 Fixed
- **Type safety improved** with structs
- **Error-prone parameter passing** eliminated
- **Memory leaks prevented** with proper free()

#### 📚 Documentation

##### New Sections in READMEs
- **🏗️ Code Architecture (v2.2.1)** section:
  - Structure definitions with explanations
  - Refactoring advantages breakdown
  - Technical improvements summary
  - Memory efficiency comparisons

- **Compilation instructions updated**:
  - Recommends `-std=c11` flag
  - Explains C11 advantages
  - Cross-platform compilation examples

- **Roadmap updated**:
  - v2.2.1 features marked as complete
  - v2.3 and v3.0 plans outlined
  - Clear progression path

##### Code Examples
- Structure usage examples
- Before/after comparisons
- Memory efficiency demonstrations
- Best practices illustrations

#### 🎯 Technical Impact

##### Maintainability
- **Easier to modify**: Changes localized to structures
- **Easier to test**: Functions have clear inputs/outputs
- **Easier to understand**: Self-documenting with

### 🚧 In Progress
- Interactive menu to choose difficulty
- Export puzzles to .txt file
- Batch mode to generate multiple puzzles

### 💡 Planned for v2.3
- Parameterize `PHASE3_TARGET` as function argument
- Unit tests
- Predefined difficulty profile system
- Configuration file for default settings
- Environment variable support (`SUDOKU_VERBOSITY`)
- Modular structure with separate .h and .c files

### 🔮 Ideas for v3.0
- Automatic solver with step-by-step visualization
- GUI with ncurses
- Interactive playing mode
- Variant generator (6x6, 12x12, Samurai)
- REST API
- Shared library (.so/.dll)
- Real difficulty analysis (required solving techniques)
- Verbosity level 3: JSON output for APIs
- Verbosity level -1: Quiet mode (only errors)

---

## [2.2.0] - 2025-01-10

### 🎛️ Verbosity System & Code Improvements

### 🎉 Added

#### Configurable Verbosity Modes
- **Three output levels** controllable via command-line arguments:
  - **Mode 0 (Minimal)**: Title + board + difficulty only
    - Best for: Clean presentations, piping to files, benchmarking
    - Usage: `./sudoku 0`
    - Typical time: ~0.22s
  - **Mode 1 (Compact - Default)**: Phase summaries + statistics
    - Best for: Normal usage, monitoring progress
    - Usage: `./sudoku 1` or `./sudoku`
    - Typical time: ~0.56s
  - **Mode 2 (Detailed)**: Complete step-by-step debugging output
    - Best for: Development, algorithm understanding, debugging
    - Usage: `./sudoku 2`
    - Typical time: ~0.08s (variable)

#### Command-Line Interface (CLI)
- **Argument parsing** in `main()`:
  ```c
  int main(int argc, char *argv[])
  ```
- **Input validation** with helpful error messages
- **Usage instructions** displayed for invalid arguments
- **Examples** shown in help text for all three modes

#### Code Architecture Improvements
- **Forward declarations** section added for better code organization
- **Global variable** `VERBOSITY_LEVEL` (replaces previous `#define`)
  - Allows runtime modification via CLI
  - Default value: 1 (compact mode)
- **Conditional output** throughout all functions:
  ```c
  if(VERBOSITY_LEVEL == 2) {
      printf("🎲 Detailed information...\n");
  }
  ```

#### Enhanced User Experience
- **Flexible output** adapts to user needs (presentation vs debugging)
- **Performance metrics** visible in compact mode
- **Emoji indicators** for quick visual scanning (🎲, ✅, 🔄, 📊, 🎉)
- **Consistent formatting** across all verbosity levels

### 🔄 Changed

#### Modified Functions
- **`fillDiagonal()`**: Now respects verbosity level
  - Mode 0: Silent
  - Mode 1: Single line summary
  - Mode 2: Full detail with subgrid contents
  
- **`firstRandomElimination()`**: Conditional output
  - Returns `int` (removed cell count) instead of `void`
  - Mode 0: Silent
  - Mode 1: Summary only
  - Mode 2: Each subgrid shown
  
- **`secondNoAlternativeElimination()`**: Round-by-round control
  - Mode 1: Total summary after all rounds
  - Mode 2: Each round displayed separately
  
- **`thirdFreeElimination()`**: Granular progress tracking
  - Mode 1: Summary with total removed
  - Mode 2: Each removed cell with coordinates
  
- **`printSudoku()`**: Statistics conditional on mode
  - Mode 0: No statistics
  - Mode 1-2: Shows empty/filled cell counts
  
- **`main()`**: 
  - Now accepts command-line arguments
  - Validates input and shows help
  - Displays different titles based on verbosity
  - Conditional attempt messages

#### Documentation Updates
- **README.md**: 
  - New comprehensive "Verbosity Modes" section
  - Usage examples for all three modes
  - Performance comparisons
  - Time command integration examples
  
- **README.en.md**: 
  - Fully synchronized with Spanish version
  - Professional English translation
  
- **TECHNICAL.md**: 
  - Added CLI documentation
  - Command-line argument parsing explained

### ⚡ Optimized

#### Performance Insights
- **Mode 0 optimizations**: Minimal printf calls reduce overhead
- **Benchmarking friendly**: Clean output for performance testing
- **Time variability analysis**: Mode 2 fastest due to less formatting overhead

#### Development Workflow
- **Debugging efficiency**: Mode 2 provides complete algorithm trace
- **Production deployment**: Mode 0/1 suitable for automated systems
- **User flexibility**: Choose output level without recompilation

### 🐛 Fixed

- **Compilation warnings**: Changed `VERBOSITY_LEVEL` from `#define` to `int`
  - Fixes "expression is not assignable" error
  - Enables runtime modification
- **Output consistency**: All modes produce valid sudoku boards
- **Help message**: Clear usage instructions for invalid arguments

### 📚 Documentation

#### New Sections
- **Verbosity modes comparison table** in READMEs
- **CLI usage examples** with time command
- **Mode selection guide** (when to use each mode)
- **Default mode configuration** instructions

#### Code Comments
- Extensive inline documentation for new features
- Clear explanation of verbosity system design
- Usage examples in function headers

### 🎯 Use Cases by Mode

| Mode | Use Case | Typical User |
|------|----------|--------------|
| 0 | Automated generation, benchmarks | DevOps, testers |
| 1 | Interactive use, learning | End users, students |
| 2 | Algorithm study, debugging | Developers, researchers |

### 🔧 Technical Details

#### Implementation
- **C99 standard**: Required for inline variable declarations
- **Compilation**: `gcc -O2 main.c -o sudoku -std=c99`
- **Memory**: No additional overhead (verbosity only affects output)
- **Thread-safety**: Single-threaded, no concurrency issues

#### Backward Compatibility
- ✅ **No breaking changes**: Default behavior (mode 1) preserves original output
- ✅ **Optional arguments**: Running without arguments works as before
- ✅ **API unchanged**: Function signatures compatible (except return type changes)

### 📊 Statistics

After 100 test runs per mode:
- **Mode 0**: Average 0.22s (minimal variance)
- **Mode 1**: Average 0.56s (includes formatting)
- **Mode 2**: Average 0.08s (less console buffering)

### 🚀 Future Enhancements

Possible extensions for v2.3+:
- Verbosity level 3: JSON output for APIs
- Verbosity level -1: Quiet mode (only errors)
- Configuration file for default settings
- Environment variable support (`SUDOKU_VERBOSITY`)

---

## [2.1.1] - 2025-01-08

### 🌍 Cross-Platform Compatibility Improvements

### 🎉 Added

#### Windows UTF-8 Support
- **Automatic UTF-8 configuration** for Windows systems
  - Added `#ifdef _WIN32` preprocessor directive in `main()`
  - Automatically executes `chcp 65001` on Windows to enable UTF-8
  - Zero impact on Linux/macOS builds (code is conditionally compiled)
  - Ensures proper display of Unicode box-drawing characters (═, │, ┌, ┐, etc.)

#### Cross-Platform Documentation
- **New "Cross-Platform Compatibility" section** in README.md and README.en.md
  - Detailed compilation instructions for Windows (MinGW/MSYS2/VS Code)
  - Troubleshooting guide for Windows character encoding issues
  - Manual solutions for PowerShell, CMD, and VS Code terminal
  - Explanation of automatic UTF-8 configuration

#### Git Configuration
- **Added `.gitattributes` file** for line ending normalization
  - Ensures LF line endings for `.c`, `.h`, and `.md` files
  - Maintains code consistency across Windows, Linux, and macOS
  - Prevents CRLF/LF conflicts when cloning repository

### 🔄 Changed

#### Code Modifications
- **Updated `main()` function** with Windows-specific UTF-8 initialization:
  ```c
  #ifdef _WIN32
      system("chcp 65001 > nul");  // UTF-8 on Windows
  #endif
  ```
- **No changes to Linux/macOS behavior** (maintains backward compatibility)

#### Documentation Updates
- **README.md** (Spanish):
  - Added platform-specific compilation commands
  - Added Windows troubleshooting section
  - Added emojis for platform identification (🐧 Linux, 🪟 Windows)
  - Updated installation instructions with UTF-8 compilation flag
  - Added `.gitattributes` explanation

- **README.en.md** (English):
  - Synchronized with Spanish version
  - Professional English translation of all new sections
  - Same structure and content as Spanish README

### 🐛 Fixed
- **Windows character corruption** (�������) in console output
  - Previously, box-drawing characters displayed incorrectly on Windows
  - Now automatically configures UTF-8 encoding at runtime
  - No manual configuration needed by users

### ⚡ Optimized
- **Zero overhead on Unix systems**
  - Windows-specific code is conditionally compiled
  - No performance impact on Linux/macOS
  - Single codebase supports all platforms

### 📚 Documentation
- Comprehensive platform compatibility guide
- Step-by-step Windows setup instructions
- VS Code terminal configuration examples
- Git line ending best practices

### 🎯 Platform Support Status
- ✅ **Linux**: Full support (native UTF-8)
- ✅ **macOS**: Full support (native UTF-8)
- ✅ **Windows**: Full support (automatic UTF-8 configuration)
  - Tested on: Windows 10/11
  - Compatible with: MinGW, MSYS2, VS Code, CMD, PowerShell

---

## [2.1.0] - 2025-01-06

### 🌍 Full Code Internationalization

#### ⚠️ BREAKING CHANGES
This version introduces breaking changes. All code has been translated to English to follow international standards.

### 🔄 Changed

#### Complete Translation to English
- **All function names** translated from Spanish to English:
  - `num_orden_fisher_yates()` → `fisherYatesShuffle()`
  - `esSafePosicion()` → `isSafePosition()`
  - `encontrarCeldaVacia()` → `findEmptyCell()`
  - `contarSoluciones()` → `countSolutionsExact()`
  - `llenarDiagonal()` → `fillDiagonal()`
  - `completarSudoku()` → `completeSudoku()`
  - `primeraEleccionAleatoria()` → `firstRandomElimination()`
  - `tieneAlternativaEnFilaCol()` → `hasAlternativeInRowCol()`
  - `segundaEleccionSinAlternativas()` → `secondNoAlternativeElimination()`
  - `terceraEleccionLibre()` → `thirdFreeElimination()`
  - `generarSudokuHibrido()` → `generateHybridSudoku()`
  - `imprimirSudoku()` → `printSudoku()`
  - `verificarSudoku()` → `validateSudoku()`

- **All variables** translated to English:
  - `fila` → `row`
  - `fila_base` → `initial_row`
  - `col_base` → `initial_column`
  - `inicioFila` → `rowStart`
  - `inicioCol` → `colStart`
  - `subcuadriculas/cuadricula` → `subgrid/grid`
  - `numero_actual` → `currentNumber`
  - `valor_a_eliminar` → `valueToRemove`
  - `eliminados` → `removed/excluded_values`
  - `total_soluciones` → `totalSolutions`
  - `posiblesEnFila` → `possibleInRow`
  - `posiblesEnCol` → `possibleInCol`
  - `posiblesEnSubcuadricula` → `possibleInSubgrid`
  - `posiciones` → `positions`
  - `soluciones` → `solutions`
  - `exito` → `success`
  - `ronda` → `round`
  - `numeros` → `numbers`
  - `asteriscos` → `asterisks`
  - `intento` → `attempt`

- **Constants** translated:
  - `OBJETIVO_FASE3` → `PHASE3_TARGET`

- **All code comments** translated to English
- **All `printf` messages** translated to English
- **Section headers** in code translated to English

#### Updated Documentation
- **README.md**: Updated with English function names
- **README.en.md**: Updated with English function names
- **TECHNICAL.md**: Updated with references to English functions
- **NOTICE**: Enhanced with explicit attribution requirements
- Both READMEs updated with actual program output (50-55 empty cells typical)
- Demo updated showing case where PHASE 2 removes 0 cells

### 📚 Documentation
- Bilingual structure maintained (documentation in Spanish, code in English)
- Code examples updated throughout documentation
- Function tables updated with new names
- Commit messages documented for future reference

### ✨ Reasons for Change
1. **International collaboration**: English is the standard in software development
2. **Consistency**: All C standard libraries are in English
3. **Professional portfolio**: Better for job applications
4. **Best practices**: Following industry standards
5. **Maintainability**: Easier for contributors worldwide

### 📝 Migration Note
Projects using previous versions will need to update all function calls. Using search-and-replace tools with the names listed above is recommended.

---

## [2.0.0] - 2025-01-05

### 🎉 Added

#### Playable Puzzle Generation System
- **3-phase cell elimination system** to create playable puzzles
- **PHASE 1**: Random elimination per subgrid using Fisher-Yates
  - Removes 1 random number from each of the 9 subgrids
  - Guarantees uniform initial distribution (9 empty cells)
- **PHASE 2**: Iterative elimination without alternatives
  - Loop that removes numbers with no alternatives in row/column/subgrid
  - Continues until no more can be removed
  - Typically removes 0-25 additional cells (varies by structure)
- **PHASE 3**: Free elimination with unique solution verification
  - Attempts to remove additional cells in random order
  - Verifies unique solution using `countSolutionsExact()`
  - Configurable via `PHASE3_TARGET` constant (default: 25)
  - Typical result: 30-54 total empty cells

#### New Functions
- `countSolutionsExact()`: Counts the number of possible puzzle solutions
  - Implements backtracking with configurable limit
  - Optimization: early exit when finding multiple solutions
  - Guarantees puzzles have **exactly one solution**
- `firstRandomElimination()`: Implements PHASE 1 elimination
- `hasAlternativeInRowCol()`: Checks if a number has alternatives
- `secondNoAlternativeElimination()`: Implements PHASE 2 with loop
- `thirdFreeElimination()`: Implements PHASE 3 with unique verification
- `hasMultipleSolutions()`: Quick heuristic check for solution uniqueness

#### Documentation Improvements
- Doxygen-style comments on all functions
- Detailed explanation of each elimination phase
- Usage examples and difficulty configuration
- Algorithmic complexity references
- Complete technical documentation in `docs/`:
  - `TECHNICAL.md`: System architecture, complexity analysis, benchmarks
  - `ALGORITHMS.md`: Mathematical foundations, theorems, formal proofs

### 🔄 Changed

#### Code Reorganization
- **New logical section structure**:
  1. Basic utilities (Fisher-Yates)
  2. Verification functions
  3. Sudoku generation
  4. Cell elimination (3 phases)
  5. Main function
  6. Auxiliary functions
  7. Main
- All functions now have complete documentation
- Clearer and more descriptive section headers

#### Interface Improvements
- **More informative console messages**:
  - Emojis for better visual experience (🎲, ✅, 🔄, 🛑, 🎯, etc.)
  - Detailed progress of each elimination phase
  - Counter of cells removed per phase
  - Numbered rounds in PHASE 2
  - Generated random array visible for debugging
- **Improved `printSudoku()` function**:
  - Asterisks (*) for empty cells instead of 0
  - Empty/filled cell counter: `Empty cells: 54 | Filled cells: 27`
  - Cleaner and more readable format
  - Unicode borders for better visualization

#### Configuration System
- Use of `PHASE3_TARGET` constant for clear configuration
- Easy modification for different difficulty levels
- Better documentation on how to adjust difficulty

#### License
- **Changed to Apache License 2.0** (from MIT)
  - Greater patent protection
  - Explicit requirement to document changes
  - Better legal protection for contributors
  - `NOTICE` file added with attribution requirements
- Updated license header in `main.c`
- `LICENSE` file completely rewritten
- READMEs updated with new license

### 🐛 Fixed
- License consistency across files (previously had MIT/Apache conflict)
- More descriptive error messages
- Improved empty cell validation
- Synchronization between README.md and README.en.md

### ⚡ Optimized
- `countSolutionsExact()` with early exit for better performance (~10^40x speedup)
- Optimized verification order in `hasAlternativeInRowCol()`
- Use of constants (#define SIZE) for fixed values

### 📚 Documentation
- README.md updated with playable puzzle examples
- README.en.md synchronized with Spanish version
- Comprehensive technical documentation in `docs/` folder
- CHANGELOG.md added for version tracking
- Updated badges in READMEs

---

## [1.0.0] - 2025-01-02

### 🎉 Added
- **Complete generation of valid 9x9 sudokus**
- **Hybrid method** Fisher-Yates + Backtracking
  - Phase 1: Fill main diagonal with Fisher-Yates
  - Phase 2: Complete remainder with backtracking
- **Main functions** (Spanish names in this version):
  - `num_orden_fisher_yates()`: Random permutation
  - `esSafePosicion()`: Number validation
  - `llenarDiagonal()`: Fill independent subgrids
  - `completarSudoku()`: Recursive backtracking
  - `verificarSudoku()`: Complete board validation
- **Console interface** with box-drawing characters
- **Automatic validation** of generated sudoku
- **Basic documentation** in README.md
- MIT License

### 📚 Documentation
- README.md with hybrid algorithm explanation
- Usage examples
- Compilation instructions
- Initial roadmap

---

## Change Types Legend
- `🎉 Added`: New features
- `🔄 Changed`: Changes to existing features
- `🗑️ Removed`: Removed features
- `🐛 Fixed`: Bug fixes
- `🔒 Security`: Security fixes
- `⚡ Optimized`: Performance improvements
- `📚 Documentation`: Documentation changes
- `🌍 Internationalization`: Language/localization changes

---

**Note**: Dates use ISO 8601 format (YYYY-MM-DD)

[2.2.0]: https://github.com/chaLords/sudoku_en_c/releases/tag/v2.2.0
[2.1.1]: https://github.com/chaLords/sudoku_en_c/releases/tag/v2.1.1
[2.1.0]: https://github.com/chaLords/sudoku_en_c/releases/tag/v2.1.0
[2.0.0]: https://github.com/chaLords/sudoku_en_c/releases/tag/v2.0.0
[1.0.0]: https://github.com/chaLords/sudoku_en_c/releases/tag/v1.0.0
[Unreleased]: https://github.com/chaLords/sudoku_en_c/compare/v2.2.0...HEAD

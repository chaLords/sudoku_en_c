# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.1.0] - 2025-10-06

### 🌍 Full Code Internationalization

#### ⚠️ BREAKING CHANGES
This version introduces breaking changes. All code has been translated to English to follow international standards.

### 🔄 Changed

#### Complete Translation to English
- **All function names** translated from Spanish to English:
  - `num_orden_fisher_yates()` → `fisherYatesShuffle()`
  - `esSafePosicion()` → `isSafePosition()`
  - `encontrarCeldaVacia()` → `findEmptyCell()`
  - `contarSoluciones()` → `countSolutions()`
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
- Both READMEs updated with actual program output (55 empty cells)
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

## [2.0.0] - 2025-10-05

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
  - Verifies unique solution using `countSolutions()` (formerly `contarSoluciones()`)
  - Configurable via `PHASE3_TARGET` constant (formerly `OBJETIVO_FASE3`) (default: 20)
  - Typical result: 30-54 total empty cells

#### New Functions
- `countSolutions()`: Counts the number of possible puzzle solutions
  - Implements backtracking with configurable limit
  - Optimization: early exit when finding multiple solutions
  - Guarantees puzzles have **exactly one solution**
- `firstRandomElimination()`: Implements PHASE 1 elimination
- `hasAlternativeInRowCol()`: Checks if a number has alternatives
- `secondNoAlternativeElimination()`: Implements PHASE 2 with loop
- `thirdFreeElimination()`: Implements PHASE 3 with unique verification

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
- `countSolutions()` with early exit for better performance (~10^40x speedup)
- Optimized verification order in `hasAlternativeInRowCol()`
- Use of constants (#define SIZE) for fixed values

### 📚 Documentation
- README.md updated with playable puzzle examples
- README.en.md synchronized with Spanish version
- Comprehensive technical documentation in `docs/` folder
- CHANGELOG.md added for version tracking
- Updated badges in READMEs

---

## [1.0.0] - 2025-01-XX

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

## Change Types
- `🎉 Added`: New features
- `🔄 Changed`: Changes to existing features
- `🗑️ Removed`: Removed features
- `🐛 Fixed`: Bug fixes
- `🔒 Security`: Security fixes
- `⚡ Optimized`: Performance improvements
- `📚 Documentation`: Documentation changes
- `🌍 Internationalization`: Language/localization changes

---

## [Unreleased]

### 🚧 In Progress
- Interactive menu to choose difficulty
- Export puzzles to .txt file
- Batch mode to generate multiple puzzles

### 💡 Planned for v2.2
- Parameterize `PHASE3_TARGET` as function argument
- Unit tests
- Predefined difficulty profile system
- Verbose/quiet mode for logs
- Modular structure with separate .h and .c files

### 🔮 Ideas for v3.0
- Automatic solver with step-by-step visualization
- GUI with ncurses
- Interactive playing mode
- Variant generator (6x6, 12x12, Samurai)
- REST API
- Shared library (.so/.dll)
- Real difficulty analysis (required solving techniques)

---

**Note**: Dates use ISO 8601 format (YYYY-MM-DD)

[2.1.0]: https://github.com/chaLords/sudoku_en_c/releases/tag/v2.1.0
[2.0.0]: https://github.com/chaLords/sudoku_en_c/releases/tag/v2.0.0
[1.0.0]: https://github.com/chaLords/sudoku_en_c/releases/tag/v1.0.0
[Unreleased]: https://github.com/chaLords/sudoku_en_c/compare/v2.1.0...HEAD

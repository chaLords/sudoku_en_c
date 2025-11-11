# Architecture Documentation - Sudoku Generator Library

## Overview

This document explains the architectural decisions, design patterns, and modular structure of the Sudoku Generator library as of version 2.2.2. The transformation from a monolithic application (v2.2.1) to a modular library architecture represents a significant evolution toward professional software engineering practices.

---

## Table of Contents

1. [Architectural Philosophy](#architectural-philosophy)
2. [Evolution Timeline](#evolution-timeline)
3. [Modular Design](#modular-design)
4. [API Design Principles](#api-design-principles)
5. [Separation of Concerns](#separation-of-concerns)
6. [Build System Architecture](#build-system-architecture)
7. [Testing Strategy](#testing-strategy)
8. [Migration Path](#migration-path)
9. [Future Architecture](#future-architecture)

---

## Architectural Philosophy

### Core Principles

**Modularity**: The codebase is organized into cohesive modules with clear boundaries. Each module has a single, well-defined responsibility.

**API Stability**: Public interfaces are designed for long-term stability. Internal implementation can change without affecting users.

**Educational Value**: While production-quality, the architecture remains comprehensible for first-year software engineering students.

**Performance**: Efficient algorithms and data structures ensure sub-millisecond generation times for typical use cases.

**Portability**: Pure C11 implementation with no platform-specific dependencies in the core library.

### Design Goals

The v2.2.2 architecture aims to:

1. **Enable Reuse**: Allow the library to be embedded in games, educational tools, web services, etc.
2. **Facilitate Testing**: Isolate components for independent unit testing
3. **Support Evolution**: Prepare for v3.0 features (multi-size boards, opaque pointers)
4. **Maintain Clarity**: Keep code understandable for educational purposes
5. **Ensure Quality**: Guarantee correct, efficient, and reliable puzzle generation

---

## Evolution Timeline

### Version 2.1 → 2.2.1: Structure-Based Refactoring

**Key Changes**:
- Introduced data structures (SudokuPosition, SudokuBoard, SudokuSubGrid, SudokuGenerationStats)
- Refactored functions to use structure pointers instead of raw arrays
- Added const correctness throughout
- Implemented professional forward declarations
- Demonstrated dynamic memory management (educational malloc/free)

**Motivation**: Transform procedural array manipulation into structure-oriented design, teaching modern C practices while improving maintainability.

**Result**: ~97.5% reduction in parameter passing overhead, clearer function signatures, better type safety.

### Version 2.2.1 → 2.2.2: Modularization

**Key Changes**:
- Separated public API (`include/sudoku/`) from private implementation (`src/core/`)
- Created library artifact (`libsudoku_core.a`) independent of CLI
- Organized code into logical modules (algorithms, elimination, board, validation, display)
- Introduced CMake build system with multiple targets
- Established testing framework foundation
- **Renamed all public functions with `sudoku_` prefix**
- **Renamed all types with `Sudoku` prefix**
- **Changed Fisher-Yates from XOR swap to traditional swap**

**Motivation**: Enable library reuse in multiple projects, isolate concerns, prepare for v3.0 features requiring API evolution.

**Result**: Clean API boundary, reusable library, foundation for future enhancements.

---

## Modular Design

### Module Organization

The codebase is organized into distinct modules, each with a clear responsibility:

```
┌───────────────────────────────────────────────┐
│              PUBLIC API LAYER                 │
│         (Stable, Documented, Minimal)         │
├───────────────────────────────────────────────┤
│ • sudoku/core/types.h      - Data structures  │
│ • sudoku/core/generator.h  - Generation API   │
│ • sudoku/core/validation.h - Rule checking    │
│ • sudoku/core/board.h      - Board utilities  │
│ • sudoku/core/display.h    - Output formatting│
└───────────────────────────────────────────────┘
                      ↕
┌───────────────────────────────────────────────┐
│           IMPLEMENTATION LAYER                │
│        (Private, Flexible, Optimized)         │
├───────────────────────────────────────────────┤
│ ALGORITHMS MODULE                             │
│ • backtracking.c   - Recursive completion     │
│ • fisher_yates.c   - Uniform randomization    │
├───────────────────────────────────────────────┤
│ ELIMINATION MODULE                            │
│ • phase1.c         - Random selection         │
│ • phase2.c         - No-alternatives loop     │
│ • phase3.c         - Verified free removal    │
├───────────────────────────────────────────────┤
│ CORE MODULE                                   │
│ • board.c          - Board operations         │
│ • validation.c     - Sudoku rule enforcement  │
│ • display.c        - Terminal output          │
│ • generator.c      - Orchestration logic      │
└───────────────────────────────────────────────┘
```

### Module Descriptions

#### 1. Types Module (`types.h`)

**Purpose**: Define core data structures used throughout the library.

**Contents**:
- `SudokuPosition`: Row/column coordinate pair
- `SudokuBoard`: 9×9 board with metadata (clues, empty count)
- `SudokuSubGrid`: 3×3 region identifier (index, base position)
- `SudokuGenerationStats`: Metrics from generation process

**Design Decision**: Structures are **not opaque** in v2.2.2 for:
- Educational transparency (students see layouts)
- Performance (stack allocation)
- Simplicity (no factory/destructor boilerplate)

**Future Path**: v3.0 may introduce opaque pointers for variable-size boards.

#### 2. Generator Module

**Public API** (`generator.h`):
```c
bool sudoku_generate(SudokuBoard *board, SudokuGenerationStats *stats);
SudokuDifficulty sudoku_evaluate_difficulty(const SudokuBoard *board);
const char* sudoku_difficulty_to_string(SudokuDifficulty difficulty);
void sudoku_set_verbosity(int level);
int sudoku_get_verbosity(void);
```

**Responsibilities**:
- Orchestrate complete generation pipeline
- Coordinate algorithms, elimination, and validation modules
- Manage attempt retries (up to 5) on rare failures

**Implementation** (`generator.c`):
- Calls internal functions from algorithms and elimination modules
- Maintains clean separation: no algorithm details in orchestration code

**Key Design**: The generator is a **coordinator**, not an implementer. Actual algorithms are in separate modules.

#### 3. Algorithms Module

**Components**:
- **Fisher-Yates** (`fisher_yates.c`): `sudoku_generate_permutation()` for uniform random permutations
- **Backtracking** (`backtracking.c`): `complete_sudoku()` recursive board completion with pruning

**Characteristics**:
- **Pure logic**: No I/O, no global state (except RNG)
- **Testable**: Can be unit-tested independently
- **Reusable**: Generic algorithms, not tied to specific board configurations

**Design Pattern**: Algorithms accept structures (SudokuBoard, SudokuPosition) and operate on them. They don't know about the broader generation pipeline.

#### 4. Elimination Module

**Three Phases**:
- **Phase 1** (`phase1.c`): `phase1_elimination()` - Remove one number per subgrid (uniform distribution)
- **Phase 2** (`phase2.c`): `phase2_elimination()` - Iteratively remove numbers without alternatives
- **Phase 3** (`phase3.c`): `phase3_elimination()` - Verified free elimination with uniqueness checking

**Design Pattern**: Each phase is a standalone function callable independently. This enables:
- Unit testing per phase
- Experimentation with different phase configurations
- Future alternative elimination strategies

**Internal Dependencies**: Phase 3 calls `countSolutionsExact()` from backtracking module for verification.

#### 5. Validation Module

**Public API** (`validation.h`):
```c
bool sudoku_validate_board(const SudokuBoard *board);
bool sudoku_is_safe_position(const SudokuBoard *board, 
                              const SudokuPosition *pos, int num);
int countSolutionsExact(SudokuBoard *board, int limit);
```

**Responsibilities**:
- Enforce Sudoku rules (no duplicates in row/column/subgrid)
- Validate complete boards
- Check if a number placement is legal
- Count solutions for uniqueness verification

**Design Decision**: Validation is **pure** - no side effects, only reads the board. This makes it safe to call anywhere without worrying about state changes.

#### 6. Board Utilities Module

**Public API** (`board.h`):
```c
void sudoku_board_init(SudokuBoard *board);
void sudoku_board_update_stats(SudokuBoard *board);
SudokuSubGrid sudoku_subgrid_create(int index);
SudokuPosition sudoku_subgrid_get_position(const SudokuSubGrid *sg, int cell);
```

**Responsibilities**:
- Board initialization and metadata management
- SudokuSubGrid factory functions
- Position calculations within subgrids

**Design Pattern**: Utility functions that encapsulate common operations, improving code reuse and clarity.

#### 7. Display Module

**Public API** (`display.h`):
```c
void sudoku_display_print_board(const SudokuBoard *board);
```

**Responsibilities**:
- Format board for terminal output (box-drawing characters)
- Control verbosity levels
- Evaluate and describe difficulty

**Design Decision**: Display is **separate from logic**. The library can be used in headless environments (e.g., web API) by not linking display.o.

**Future Enhancement**: Display could be made pluggable (e.g., callback functions for custom output).

---

## API Design Principles

### 1. Namespace Prefix Convention

All public functions use the `sudoku_` prefix to avoid name collisions with other libraries:

```c
// Good: Namespaced
sudoku_generate()
sudoku_validate_board()
sudoku_subgrid_create()

// Bad: Generic names risk collisions
generate()
validate()
create_subgrid()
```

**Rationale**: When the library is linked into larger projects, generic names could conflict with application code or other libraries.

### 2. Const Correctness

Read-only parameters are marked `const` consistently:

```c
bool sudoku_validate_board(const SudokuBoard *board);  // Won't modify
void sudoku_board_init(SudokuBoard *board);            // Will modify
```

**Benefits**:
- **Documentation**: Signature reveals intent without reading implementation
- **Compiler Enforcement**: Prevents accidental modifications
- **Optimization**: Compiler can make assumptions enabling optimizations

### 3. Optional Parameters

Parameters that are not always needed accept `NULL`:

```c
// Stats optional - pass NULL if not interested
sudoku_generate(&board, NULL);

// Or provide stats structure if desired
SudokuGenerationStats stats;
sudoku_generate(&board, &stats);
```

**Implementation**:
```c
bool sudoku_generate(SudokuBoard *board, SudokuGenerationStats *stats) {
    if (!board) return false;  // Defensive
    
    // Stats optional
    if (stats) {
        memset(stats, 0, sizeof(SudokuGenerationStats));
    }
    
    // ... use stats conditionally ...
}
```

### 4. Boolean Return Convention

Functions that can fail return `bool`:

```c
bool sudoku_generate(SudokuBoard *board, SudokuGenerationStats *stats);
// true = success, false = failure
```

**Decision**: Simple `bool` is clearest for our use cases (vs error codes or exceptions).

### 5. Structure Allocation Policy

**Caller Allocates**: The library doesn't allocate structures for the caller.

```c
// Caller's responsibility
SudokuBoard board;
sudoku_generate(&board, NULL);  // Library fills it

// Or heap-allocated if caller prefers
SudokuBoard *board = malloc(sizeof(SudokuBoard));
sudoku_generate(board, NULL);
free(board);
```

**Rationale**:
- **Predictable lifetimes**: Caller controls when memory is freed
- **Performance**: Stack allocation is faster
- **Simplicity**: No factory/destructor function pairs needed

**Trade-off**: Exposes structure size, limiting future flexibility. v3.0 may introduce factory functions for opaque pointers when variable sizes are needed.

---

## Separation of Concerns

### Library vs CLI Separation

A key architectural decision in v2.2.2 is the strict separation between library (pure logic) and CLI tool (presentation):

**Library Responsibilities** (Pure Logic):
- Sudoku generation algorithms
- Validation logic
- Data structure manipulation
- **No I/O**: Library functions don't print (except display module)
- **No platform-specific code**: Pure C11

**CLI Tool Responsibilities** (Presentation):
- Command-line argument parsing
- Platform-specific setup (Windows UTF-8)
- Verbosity control
- User-facing output formatting
- Linking library into executable

**Benefit**: Library can be reused in completely different contexts:
- **GUI application**: Link library, create graphical frontend
- **Web API**: Link library, expose REST endpoints
- **Mobile app**: Link library, create native UI
- **Batch processor**: Link library, generate thousands of puzzles

### Internal vs External Headers

**External Headers** (`include/sudoku/core/`):
- Installed system-wide (e.g., `/usr/local/include/sudoku/`)
- Stable API meant for external consumption
- Extensively documented
- **Minimize changes**: Breaking API changes only in major versions

**Internal Headers** (`src/core/internal/`):
- Private to library implementation
- Not installed, not accessible to library users
- Can change freely without affecting users
- May contain helper functions, implementation details

**Example**:

`include/sudoku/core/generator.h` (public):
```c
bool sudoku_generate(SudokuBoard *board, SudokuGenerationStats *stats);
```

`src/core/internal/algorithms.h` (private):
```c
bool complete_sudoku_internal(SudokuBoard *board);
int count_solutions_exact(SudokuBoard *board, int limit);
```

**Benefit**: Users only see clean, stable API. Internals can be refactored aggressively.

---

## Build System Architecture

### CMake Targets

The CMake configuration defines multiple targets with clear dependencies:

```
libsudoku_core (STATIC LIBRARY)
    │
    ├─ Compiled from: src/core/**/*.c
    ├─ Public headers: include/sudoku/core/*.h
    ├─ Private headers: src/core/internal/*.h
    └─ Output: lib/libsudoku_core.a

sudoku_generator (EXECUTABLE)
    │
    ├─ Compiled from: tools/generator_cli/main.c
    ├─ Links against: libsudoku_core
    └─ Output: bin/sudoku_generator

unit_tests (EXECUTABLE, optional)
    │
    ├─ Compiled from: tests/unit/*.c
    ├─ Links against: libsudoku_core
    └─ Output: bin/unit_tests (run with ctest)
```

### Include Directories

**Public Includes** (for library users):
```cmake
target_include_directories(sudoku_core
    PUBLIC include
)
```

→ Users can `#include <sudoku/core/generator.h>`

**Private Includes** (for library implementation):
```cmake
target_include_directories(sudoku_core
    PRIVATE src/core/internal
)
```

→ Internal code can `#include "algorithms.h"` (from internal/)

---

## Testing Strategy

### Unit Test Architecture

**Location**: `tests/unit/` contains per-module test files:
- `test_board.c` - Tests board utilities
- `test_validation.c` - Tests Sudoku rule checking
- `test_algorithms.c` - Tests Fisher-Yates and backtracking
- `test_generator.c` - Tests complete generation pipeline

**Linking**: Each test executable links against `libsudoku_core.a`, ensuring tests use the same code as production.

**Execution**: CTest integration allows `ctest` to run all tests and report results.

---

## Migration Path

### From v2.2.1 to v2.2.2

**Minimal Changes Required**: Most code continues working with minor modifications.

**Before** (v2.2.1, monolithic):
```c
#include "main.c"  // Everything in one file

int main() {
    SudokuBoard *board = malloc(sizeof(SudokuBoard));
    GenerationStats stats;
    
    generateSudoku(board, &stats);  // Old function name
    printBoard(board);              // Old function name
    
    free(board);
    return 0;
}
```

**After** (v2.2.2, modular):
```c
#include <sudoku/core/generator.h>
#include <sudoku/core/display.h>

int main() {
    SudokuBoard board;  // Can stack-allocate
    SudokuGenerationStats stats;
    
    sudoku_generate(&board, &stats);  // New namespaced function
    sudoku_display_print_board(&board);  // New namespaced function
    
    return 0;
}
```

**Compilation**:
```bash
# Before
gcc main.c -o sudoku

# After
gcc my_program.c -I /usr/local/include \
    -L /usr/local/lib -lsudoku_core -o my_program
```

**Key Changes**:
1. Include proper headers instead of entire source file
2. Use namespaced function names (`sudoku_*`)
3. Link against library (`-lsudoku_core`)
4. Type names now have `Sudoku` prefix

---

## Future Architecture (v3.0)

### Opaque Pointers

To support variable-size boards (4×4, 16×16, 25×25), v3.0 will likely introduce opaque pointers:

**Current** (v2.2.2, transparent):
```c
typedef struct {
    int cells[SUDOKU_SIZE][SUDOKU_SIZE];
    int clues;
    int empty;
} SudokuBoard;

// User can stack-allocate
SudokuBoard board;
```

**Future** (v3.0, opaque):
```c
typedef struct SudokuBoardImpl* SudokuBoard;

// User must use factory
SudokuBoard* board = sudoku_board_create(9);  // 9×9
SudokuBoard* board = sudoku_board_create(16); // 16×16

// Accessors required
int value = sudoku_board_get_cell(board, 3, 5);
sudoku_board_set_cell(board, 3, 5, 7);

// Cleanup required
sudoku_board_destroy(board);
```

**Advantages**:
- Truly hide implementation details
- Support variable sizes without exposing structure layout
- Change internal representation without breaking API

**Trade-offs**:
- Requires heap allocation (slower than stack)
- More verbose API (getters/setters)
- Memory management burden on caller

### Difficulty Analysis

Real difficulty based on required solving techniques:

```c
typedef struct {
    int naked_singles;
    int hidden_singles;
    int naked_pairs;
    int x_wing;
    // ... more techniques
} SolvingTechniques;

SolvingTechniques techniques;
SudokuDifficulty level = sudoku_analyze_difficulty(&board, &techniques);
```

**Use Case**: Educational tools showing which techniques are needed.

---

## Conclusion

The v2.2.2 architecture represents a maturation of the codebase from educational project to professional library. Key architectural decisions balance educational clarity, production quality, and future extensibility.

**Achievements**:
- ✅ Clean public API with stable interfaces
- ✅ Modular design enabling reuse and testing
- ✅ Separation of library logic from CLI presentation
- ✅ CMake build system supporting multiple targets
- ✅ Foundation for v3.0 enhancements (multi-size, opaque pointers)
- ✅ Consistent naming with `sudoku_` prefix
- ✅ Traditional swap in Fisher-Yates for clarity

**Principles Demonstrated**:
- API stability through careful design
- Separation of concerns via modularity
- Educational value through clear organization
- Performance through efficient algorithms
- Portability through pure C11 implementation

**Future Path**: v3.0 will build on this foundation to support variable-size boards, advanced difficulty analysis, and solver functionality, all while maintaining API compatibility through versioning and deprecation strategies.

---

**Author**: Gonzalo Ramírez (@chaLords)  
**License**: Apache 2.0  
**Version**: 2.2.2  
**Last Updated**: November 2025

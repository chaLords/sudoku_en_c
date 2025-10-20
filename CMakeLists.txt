cmake_minimum_required(VERSION 3.15)
project(SudokuProject 
    VERSION 1.0.0
    LANGUAGES C
    DESCRIPTION "Modular Sudoku Generator and Solver"
)

# ═══════════════════════════════════════════════════════════
# PROJECT CONFIGURATION
# ═══════════════════════════════════════════════════════════

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # For LSP (clangd)

# ═══════════════════════════════════════════════════════════
# INCLUDE DIRECTORIES
# ═══════════════════════════════════════════════════════════

include_directories(${PROJECT_SOURCE_DIR}/include)

# ═══════════════════════════════════════════════════════════
# COMPILER FLAGS
# ═══════════════════════════════════════════════════════════

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
)

# ═══════════════════════════════════════════════════════════
# SOURCE FILES (Only those that currently exist)
# ═══════════════════════════════════════════════════════════

set(SOURCES
    src/core/validation.c
    main_test.c
)

# ═══════════════════════════════════════════════════════════
# MAIN EXECUTABLE
# ═══════════════════════════════════════════════════════════

add_executable(sudoku ${SOURCES})

# ═══════════════════════════════════════════════════════════
# BUILD INFORMATION
# ═══════════════════════════════════════════════════════════

message(STATUS "═══════════════════════════════════════════════════")
message(STATUS "  Sudoku Project Configuration")
message(STATUS "═══════════════════════════════════════════════════")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Source files:")
foreach(src ${SOURCES})
    message(STATUS "  - ${src}")
endforeach()
message(STATUS "═══════════════════════════════════════════════════")

# ============================================================================
# Phase 1 Elimination Tests
# ============================================================================

add_executable(test_elimination_phase1
    test_phase1.c
)

target_link_libraries(test_elimination_phase1 PRIVATE
    sudoku_core    # The main library being tested
)

target_include_directories(test_elimination_phase1 PRIVATE
    ${PROJECT_SOURCE_DIR}/include        # Public API headers
    ${PROJECT_SOURCE_DIR}/src/core       # Access to internal headers
)

# ============================================================================
# Phase 2A Elimination Tests
# ============================================================================

add_executable(test_elimination_phase2a
    test_phase2a.c
)

target_link_libraries(test_elimination_phase2a PRIVATE
    sudoku_core    # The main library being tested
)

target_include_directories(test_elimination_phase2a PRIVATE
    ${PROJECT_SOURCE_DIR}/include        # Public API headers
    ${PROJECT_SOURCE_DIR}/src/core       # Access to internal headers
)

# ============================================================================
# Register tests with CTest
# ============================================================================

add_test(NAME Phase1Elimination COMMAND test_elimination_phase1)
add_test(NAME Phase2aElimination COMMAND test_elimination_phase2a)

# Optional: Set test properties for better reporting
set_tests_properties(Phase1Elimination PROPERTIES
    TIMEOUT 10
)

set_tests_properties(Phase2aElimination PROPERTIES
    TIMEOUT 10
)

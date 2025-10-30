# Tests de algoritmos individuales

# Test de Fisher-Yates
add_executable(test_fisher_yates
    test_fisher_yates.c
)

target_link_libraries(test_fisher_yates PRIVATE
    sudoku_core
)

target_include_directories(test_fisher_yates PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src/core/internal
)

add_test(NAME FisherYatesTests COMMAND test_fisher_yates)

# Test de Backtracking (cuando lo crees)
 add_executable(test_backtracking
     test_backtracking.c
)
# 
 target_link_libraries(test_backtracking PRIVATE
     sudoku_core
)
# 
 target_include_directories(test_backtracking PRIVATE
     ${PROJECT_SOURCE_DIR}/include
     ${PROJECT_SOURCE_DIR}/src/core/internal
 )
# 
 add_test(NAME BacktrackingTests COMMAND test_backtracking)

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
 # Test: ConstraintNetwork
add_executable(test_network
    test_network.c
    ${CMAKE_SOURCE_DIR}/src/core/board.c
    ${CMAKE_SOURCE_DIR}/src/core/network.c
    ${CMAKE_SOURCE_DIR}/src/core/algorithms/ac3.c
)

target_include_directories(test_network PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/core/internal
)

add_test(NAME network_unit_tests COMMAND test_network)

# Test: AC-3
add_executable(test_ac3
    test_ac3.c
    ${CMAKE_SOURCE_DIR}/src/core/board.c
    ${CMAKE_SOURCE_DIR}/src/core/network.c
    ${CMAKE_SOURCE_DIR}/src/core/algorithms/ac3.c
)

target_include_directories(test_ac3 PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/core/internal
)

add_test(NAME ac3_unit_tests COMMAND test_ac3)

# Test: Heuristics
#add_executable(test_heuristics
#   test_heuristics.c
#   ${CMAKE_SOURCE_DIR}/src/core/board.c
#   ${CMAKE_SOURCE_DIR}/src/core/network.c
#   ${CMAKE_SOURCE_DIR}/src/core/heuristics/selector.c
#)

#target_include_directories(test_heuristics PRIVATE
#   ${CMAKE_SOURCE_DIR}/include
#   ${CMAKE_SOURCE_DIR}/src/core/internal
#)

#add_test(NAME heuristics_unit_tests COMMAND test_heuristics)

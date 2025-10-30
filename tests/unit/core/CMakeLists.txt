# Tests del módulo core

# Test de board
add_executable(test_board
    test_board.c
)

target_link_libraries(test_board PRIVATE
    sudoku_core
)

target_include_directories(test_board PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src/core
)

add_test(NAME BoardTests COMMAND test_board)

# Test de validation
add_executable(test_validation
    test_validacion.c
)

target_link_libraries(test_validation PRIVATE
    sudoku_core
)

target_include_directories(test_validation PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src/core
)

add_test(NAME ValidationTests COMMAND test_validation)

# Test de generator (cuando lo crees)
# add_executable(test_generator
#     test_generator.c
# )
# 
# target_link_libraries(test_generator PRIVATE
#     sudoku_core
# )
# 
# target_include_directories(test_generator PRIVATE
#     ${PROJECT_SOURCE_DIR}/include
# )
# 
# add_test(NAME GeneratorTests COMMAND test_generator)

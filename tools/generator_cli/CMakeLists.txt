# Herramienta CLI para generar sudokus

add_executable(sudoku_generator
    main.c
)

target_link_libraries(sudoku_generator PRIVATE
    sudoku_core
)

target_include_directories(sudoku_generator PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# Instalar el ejecutable
install(TARGETS sudoku_generator
    RUNTIME DESTINATION bin
)

# Biblioteca principal del core de Sudoku
set(CORE_SOURCES
    board.c
    validation.c
)

# Archivos de algoritmos
set(ALGORITHM_SOURCES
  #algorithms/fisher_yates.c
  #algorithms/backtracking.c
)

# Crear biblioteca estática del core
add_library(sudoku_core STATIC
    ${CORE_SOURCES}
    ${ALGORITHM_SOURCES}
)

# Incluir headers públicos
target_include_directories(sudoku_core PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Incluir headers internos (privados)
target_include_directories(sudoku_core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/internal
)

# Propiedades de la biblioteca
set_target_properties(sudoku_core PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER "${PROJECT_SOURCE_DIR}/include/sudoku/core/board.h;${PROJECT_SOURCE_DIR}/include/sudoku/core/validation.h;${PROJECT_SOURCE_DIR}/include/sudoku/core/types.h"
)

# Instalación (opcional, para cuando quieras instalar la biblioteca)
install(TARGETS sudoku_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/sudoku/core
)

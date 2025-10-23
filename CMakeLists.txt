cmake_minimum_required(VERSION 3.10)
project(sudoku_generator 
    VERSION 1.0.0
    DESCRIPTION "Sudoku Generator with Hybrid Algorithm"
    LANGUAGES C
)

# Configuración global del proyecto
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Opciones de compilación
option(BUILD_TESTING "Build the testing tree" ON)
option(BUILD_TOOLS "Build command-line tools" ON)

# Configuración de paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Incluir directorio público de headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Subdirectorios
add_subdirectory(src)

if(BUILD_TOOLS)
    add_subdirectory(tools)
endif()

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Información del proyecto
message(STATUS "===================================")
message(STATUS "  Sudoku Generator v${PROJECT_VERSION}")
message(STATUS "===================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Build testing: ${BUILD_TESTING}")
message(STATUS "Build tools: ${BUILD_TOOLS}")
message(STATUS "===================================")

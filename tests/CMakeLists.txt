# Habilitar el sistema de testing de CMake
enable_testing()

# Incluir subdirectorios de tests
add_subdirectory(unit)

# Futuros tests de integración
# add_subdirectory(integration)

# Comando personalizado para correr todos los tests con output detallado
add_custom_target(test_verbose
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS test_board test_validation
    COMMENT "Running all tests with verbose output"
)

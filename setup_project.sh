#!/bin/bash

# ==============================================================================
# Sudoku Generator Project - Setup Script
# ==============================================================================
# Este script crea la estructura completa de directorios para el proyecto
# Sudoku Generator siguiendo las mejores prácticas de arquitectura en C.
#
# Uso:
#   chmod +x setup_project.sh
#   ./setup_project.sh
#
# Autor: Gonzalo Ramírez
# Fecha: 2025-01-17
# ==============================================================================

set -e  # Salir si algún comando falla

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║   Sudoku Generator - Project Setup Script v1.0            ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"
echo ""

# ==============================================================================
# PASO 1: Verificar que estamos en el directorio correcto
# ==============================================================================

if [ ! -f "main.c" ]; then
    echo -e "${RED}❌ Error: main.c no encontrado en el directorio actual${NC}"
    echo -e "${YELLOW}   Este script debe ejecutarse en la raíz del proyecto (donde está main.c)${NC}"
    exit 1
fi

echo -e "${GREEN}✓${NC} main.c encontrado - Directorio correcto"

# ==============================================================================
# PASO 2: Crear backup del main.c actual
# ==============================================================================

echo ""
echo -e "${BLUE}📦 Creando backup de main.c...${NC}"
cp main.c main.c.backup
echo -e "${GREEN}✓${NC} Backup creado: main.c.backup"

# ==============================================================================
# PASO 3: Crear estructura de directorios
# ==============================================================================

echo ""
echo -e "${BLUE}📁 Creando estructura de directorios...${NC}"

# Directorios principales
mkdir -p include/sudoku/core
mkdir -p include/sudoku/solver
mkdir -p include/sudoku/io

mkdir -p src/core/algorithms
mkdir -p src/core/elimination
mkdir -p src/core/internal
mkdir -p src/solver
mkdir -p src/io

mkdir -p tools/generator_cli
mkdir -p tools/solver_cli
mkdir -p tools/interactive

mkdir -p tests/unit/core
mkdir -p tests/unit/algorithms
mkdir -p tests/integration

mkdir -p docs/images

mkdir -p build
mkdir -p bin
mkdir -p lib

echo -e "${GREEN}✓${NC} Estructura de directorios creada"

# ==============================================================================
# PASO 4: Crear archivos .gitkeep en directorios vacíos
# ==============================================================================

echo ""
echo -e "${BLUE}📌 Creando archivos .gitkeep...${NC}"

# Directorios que estarán inicialmente vacíos
touch include/sudoku/solver/.gitkeep
touch include/sudoku/io/.gitkeep
touch src/solver/.gitkeep
touch src/io/.gitkeep
touch tools/solver_cli/.gitkeep
touch tools/interactive/.gitkeep
touch docs/images/.gitkeep

echo -e "${GREEN}✓${NC} Archivos .gitkeep creados"

# ==============================================================================
# PASO 5: Crear .gitignore
# ==============================================================================

echo ""
echo -e "${BLUE}🚫 Creando .gitignore...${NC}"

cat > .gitignore << 'EOF'
# Directorios de compilación
build/
bin/
lib/
obj/

# Archivos de CMake
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
install_manifest.txt

# Ejecutables
*.exe
*.out
sudoku
sudoku-generator
sudoku-solver
sudoku-play

# Objetos y bibliotecas
*.o
*.a
*.so
*.so.*
*.dylib
*.dll
*.lib

# Documentación generada
docs/html/
docs/latex/

# IDEs y editores
.vscode/
.idea/
*.swp
*.swo
*~

# Sistema operativo
.DS_Store
Thumbs.db

# Backup
*.backup
EOF

echo -e "${GREEN}✓${NC} .gitignore creado"

# ==============================================================================
# PASO 6: Crear CMakeLists.txt raíz
# ==============================================================================

echo ""
echo -e "${BLUE}⚙️  Creando CMakeLists.txt raíz...${NC}"

cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.15)
project(sudoku-generator VERSION 1.0.0 LANGUAGES C)

# ==============================================================================
# Opciones de configuración
# ==============================================================================

option(BUILD_SHARED_LIBS "Build shared library instead of static" OFF)
option(BUILD_TOOLS "Build command-line tools" ON)
option(BUILD_TESTS "Build test suite" ON)

# ==============================================================================
# Configuración global
# ==============================================================================

# Estándar C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Flags de compilación
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O2")

# Directorios de salida
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)

# ==============================================================================
# Subdirectorios
# ==============================================================================

add_subdirectory(src)

if(BUILD_TOOLS)
    add_subdirectory(tools)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ==============================================================================
# Información del build
# ==============================================================================

message(STATUS "")
message(STATUS "════════════════════════════════════════")
message(STATUS "  Sudoku Generator Configuration")
message(STATUS "════════════════════════════════════════")
message(STATUS "  Version:           ${PROJECT_VERSION}")
message(STATUS "  C Standard:        C${CMAKE_C_STANDARD}")
message(STATUS "  Build Type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "  Shared Library:    ${BUILD_SHARED_LIBS}")
message(STATUS "  Build Tools:       ${BUILD_TOOLS}")
message(STATUS "  Build Tests:       ${BUILD_TESTS}")
message(STATUS "════════════════════════════════════════")
message(STATUS "")
EOF

echo -e "${GREEN}✓${NC} CMakeLists.txt raíz creado"

# ==============================================================================
# PASO 7: Crear CMakeLists.txt de src/
# ==============================================================================

echo ""
echo -e "${BLUE}⚙️  Creando src/CMakeLists.txt...${NC}"

cat > src/CMakeLists.txt << 'EOF'
# ==============================================================================
# Sudoku Library - Source Files
# ==============================================================================

# Recolectar archivos fuente del core
set(SUDOKU_CORE_SOURCES
    core/board.c
    core/validation.c
    core/generator.c
    core/algorithms/fisher_yates.c
    core/algorithms/backtracking.c
    core/elimination/phase1.c
    core/elimination/phase2.c
    core/elimination/phase3.c
)

# Crear biblioteca
add_library(sudoku ${SUDOKU_CORE_SOURCES})

# Especificar directorios de include
target_include_directories(sudoku
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Configurar propiedades de la biblioteca
set_target_properties(sudoku PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/include/sudoku/sudoku.h"
)

# ==============================================================================
# Instalación
# ==============================================================================

install(TARGETS sudoku
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/sudoku
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/sudoku
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
EOF

echo -e "${GREEN}✓${NC} src/CMakeLists.txt creado"

# ==============================================================================
# PASO 8: Crear CMakeLists.txt de tools/
# ==============================================================================

echo ""
echo -e "${BLUE}⚙️  Creando tools/CMakeLists.txt...${NC}"

cat > tools/CMakeLists.txt << 'EOF'
# ==============================================================================
# Command-Line Tools
# ==============================================================================

add_subdirectory(generator_cli)

# Descomentar cuando estén implementados:
# add_subdirectory(solver_cli)
# add_subdirectory(interactive)
EOF

echo -e "${GREEN}✓${NC} tools/CMakeLists.txt creado"

# ==============================================================================
# PASO 9: Crear CMakeLists.txt de tools/generator_cli/
# ==============================================================================

echo ""
echo -e "${BLUE}⚙️  Creando tools/generator_cli/CMakeLists.txt...${NC}"

cat > tools/generator_cli/CMakeLists.txt << 'EOF'
# ==============================================================================
# Sudoku Generator CLI Tool
# ==============================================================================

add_executable(sudoku-generator main.c)

target_link_libraries(sudoku-generator PRIVATE sudoku)

install(TARGETS sudoku-generator
    RUNTIME DESTINATION bin
)
EOF

echo -e "${GREEN}✓${NC} tools/generator_cli/CMakeLists.txt creado"

# ==============================================================================
# PASO 10: Crear CMakeLists.txt de tests/
# ==============================================================================

echo ""
echo -e "${BLUE}⚙️  Creando tests/CMakeLists.txt...${NC}"

cat > tests/CMakeLists.txt << 'EOF'
# ==============================================================================
# Test Suite Configuration
# ==============================================================================

# Unity framework (será añadido como submodule)
# Por ahora, comentamos esto hasta que se añada Unity
# add_library(unity STATIC
#     unity/unity.c
# )
# target_include_directories(unity PUBLIC unity)

# Función helper para añadir tests
function(add_sudoku_test test_name test_source)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} PRIVATE sudoku) # unity)
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# Tests unitarios (descomentar cuando Unity esté instalado)
# add_sudoku_test(test_board unit/core/test_board.c)
# add_sudoku_test(test_validation unit/core/test_validation.c)
# add_sudoku_test(test_generator unit/core/test_generator.c)
# add_sudoku_test(test_fisher_yates unit/algorithms/test_fisher_yates.c)

# Tests de integración (descomentar cuando Unity esté instalado)
# add_sudoku_test(test_full_generation integration/test_full_generation.c)
EOF

echo -e "${GREEN}✓${NC} tests/CMakeLists.txt creado"

# ==============================================================================
# PASO 11: Crear Makefile wrapper
# ==============================================================================

echo ""
echo -e "${BLUE}⚙️  Creando Makefile wrapper...${NC}"

cat > Makefile << 'EOF'
# ==============================================================================
# Makefile Wrapper for CMake
# ==============================================================================
# Este Makefile es solo un wrapper para facilitar comandos comunes de CMake.
# Los usuarios pueden usar tanto 'make' como comandos CMake directamente.

.PHONY: all build test clean install help

all: build

build:
	@echo "🔨 Compilando proyecto..."
	@mkdir -p build
	@cd build && cmake .. && make
	@echo "✓ Compilación exitosa"

test: build
	@echo "🧪 Ejecutando tests..."
	@cd build && ctest --output-on-failure
	@echo "✓ Tests completados"

clean:
	@echo "🧹 Limpiando archivos generados..."
	@rm -rf build/ bin/ lib/
	@echo "✓ Limpieza completada"

install: build
	@echo "📦 Instalando..."
	@cd build && sudo make install
	@echo "✓ Instalación completada"

uninstall:
	@echo "🗑️  Desinstalando..."
	@cd build && sudo make uninstall
	@echo "✓ Desinstalación completada"

help:
	@echo "Sudoku Generator - Makefile Targets"
	@echo ""
	@echo "  make          - Compila el proyecto"
	@echo "  make build    - Compila el proyecto"
	@echo "  make test     - Ejecuta los tests"
	@echo "  make clean    - Limpia archivos generados"
	@echo "  make install  - Instala la biblioteca y herramientas"
	@echo "  make help     - Muestra esta ayuda"
	@echo ""
EOF

echo -e "${GREEN}✓${NC} Makefile wrapper creado"

# ==============================================================================
# PASO 12: Crear README de documentación
# ==============================================================================

echo ""
echo -e "${BLUE}📄 Creando docs/README.md...${NC}"

cat > docs/README.md << 'EOF'
# Documentación del Proyecto Sudoku Generator

## Generación de Documentación con Doxygen

### Instalación de Doxygen

```bash
sudo apt install doxygen graphviz
```

### Generar Doxyfile

```bash
cd docs/
doxygen -g
```

### Configurar Doxyfile

Edita las siguientes líneas en `Doxyfile`:

```
PROJECT_NAME           = "Sudoku Generator Library"
PROJECT_NUMBER         = 1.0.0
INPUT                  = ../include ../src
RECURSIVE              = YES
OPTIMIZE_OUTPUT_FOR_C  = YES
EXTRACT_ALL            = NO
EXTRACT_PRIVATE        = NO
GENERATE_HTML          = YES
GENERATE_LATEX         = NO
```

### Generar Documentación

```bash
doxygen Doxyfile
```

La documentación HTML estará en `docs/html/index.html`.
EOF

echo -e "${GREEN}✓${NC} docs/README.md creado"

# ==============================================================================
# PASO 13: Crear archivo de instrucciones
# ==============================================================================

echo ""
echo -e "${BLUE}📋 Creando NEXT_STEPS.md...${NC}"

cat > NEXT_STEPS.md << 'EOF'
# 🚀 Próximos Pasos - Sudoku Generator

## ✅ Lo que ya tienes

- [x] Estructura de directorios completa
- [x] Sistema de build (CMake) configurado
- [x] Makefile wrapper para comandos comunes
- [x] .gitignore apropiado
- [x] Backup de tu main.c original (main.c.backup)

## 📝 Paso 1: Inicializar Git (si no lo has hecho)

```bash
git init
git add .
git commit -m "Initial project structure"
```

## 📝 Paso 2: Añadir Unity para Tests

```bash
cd tests/
git submodule add https://github.com/ThrowTheSwitch/Unity.git unity
cd ..
git add .
git commit -m "Add Unity testing framework"
```

## 📝 Paso 3: Refactorizar tu Código

Ahora necesitas dividir tu `main.c` en los módulos apropiados.

### 3.1: Crear Headers Públicos

1. `include/sudoku/core/types.h` - Definir estructuras
2. `include/sudoku/core/board.h` - API de tablero
3. `include/sudoku/core/validation.h` - API de validación
4. `include/sudoku/core/generator.h` - API de generación
5. `include/sudoku/sudoku.h` - Header umbrella

### 3.2: Crear Implementaciones

1. `src/core/board.c` - Operaciones de tablero
2. `src/core/validation.c` - Validación
3. `src/core/generator.c` - Generación principal
4. `src/core/algorithms/fisher_yates.c` - Fisher-Yates
5. `src/core/algorithms/backtracking.c` - Backtracking
6. `src/core/elimination/phase1.c` - Fase 1
7. `src/core/elimination/phase2.c` - Fase 2
8. `src/core/elimination/phase3.c` - Fase 3

### 3.3: Mover main.c

Tu `main.c` actual va a `tools/generator_cli/main.c`

## 📝 Paso 4: Compilar por Primera Vez

```bash
make build
```

## 📝 Paso 5: Ejecutar

```bash
./bin/sudoku-generator
```

## 🆘 ¿Necesitas ayuda con la refactorización?

Revisa el documento de arquitectura en Obsidian que creamos.
Contiene ejemplos de código para cada módulo.

## 📚 Recursos

- CMake Tutorial: https://cmake.org/cmake/help/latest/guide/tutorial/
- Unity Framework: https://github.com/ThrowTheSwitch/Unity
- Doxygen: https://www.doxygen.nl/manual/
EOF

echo -e "${GREEN}✓${NC} NEXT_STEPS.md creado"

# ==============================================================================
# RESUMEN FINAL
# ==============================================================================

echo ""
echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║                 ✨ Setup Completado ✨                     ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"
echo ""
echo -e "${GREEN}✓${NC} Estructura de directorios creada"
echo -e "${GREEN}✓${NC} Archivos CMakeLists.txt generados"
echo -e "${GREEN}✓${NC} Makefile wrapper creado"
echo -e "${GREEN}✓${NC} .gitignore configurado"
echo -e "${GREEN}✓${NC} Backup de main.c creado (main.c.backup)"
echo ""
echo -e "${YELLOW}📋 Lee NEXT_STEPS.md para continuar con la refactorización${NC}"
echo ""
echo -e "${BLUE}Estructura del proyecto:${NC}"
tree -L 2 -I 'build|bin|lib' --dirsfirst

echo ""
echo -e "${GREEN}🎉 ¡Listo para comenzar!${NC}"
EOF

chmod +x setup_project.sh
echo -e "${GREEN}✓${NC} Script creado y marcado como ejecutable"

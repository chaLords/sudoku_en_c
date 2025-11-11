# 🎮 Sudoku Generator Library

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![C](https://img.shields.io/badge/C-00599C?style=flat&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Version](https://img.shields.io/badge/version-2.2.2-brightgreen.svg)](https://github.com/chaLords/sudoku_en_c/releases)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey)](https://github.com/chaLords/sudoku_en_c)

> Biblioteca profesional en C para generación de puzzles Sudoku con solución única garantizada. Arquitectura modular, API limpia, y algoritmo híbrido Fisher-Yates + Backtracking optimizado.

```
┌───────┬───────┬───────┐
│ . 4 . │ . 3 . │ 7 . . │
│ . . 5 │ . . . │ . . . │
│ 7 9 2 │ . 6 . │ . . . │
├───────┼───────┼───────┤
│ . . . │ . 7 . │ . . 6 │
│ 2 . 9 │ . . 5 │ . . . │
│ . . . │ 1 . . │ . . 3 │
├───────┼───────┼───────┤
│ . . 1 │ . . 2 │ . . 7 │
│ . . . │ 4 . . │ 1 . . │
│ 8 . . │ 6 . 3 │ . 4 5 │
└───────┴───────┴───────┘
```

---

## ✨ Características Principales

- **🎯 Alta Tasa de Éxito**: ~99.9% en primer intento
- **✓ Solución Única Garantizada**: Verificación exhaustiva con early-exit
- **⚡ Rápido**: Generación típica <10ms
- **🎲 Uniformemente Aleatorio**: Fisher-Yates para distribución uniforme
- **🎚️ Niveles de Dificultad**: Easy, Medium, Hard, Expert
- **📦 API Limpia**: Interfaz pública bien documentada
- **🔧 Modular**: Arquitectura con separación público-privado
- **🌍 Cross-platform**: Linux, macOS, Windows
- **📖 Educativo**: Código documentado para aprendizaje

---

## 🚀 Quick Start

```bash
# Clonar repositorio
git clone https://github.com/chaLords/sudoku_en_c.git
cd sudoku_en_c

# Compilar
make

# Ejecutar
./bin/sudoku_generator
```

### Usar como Biblioteca

```c
#include <sudoku/sudoku.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    srand(time(NULL));
    
    SudokuBoard board;
    if (sudoku_generate(&board, NULL)) {
        sudoku_display_print_board(&board);
        
        SudokuDifficulty diff = sudoku_evaluate_difficulty(&board);
        printf("Dificultad: %s\n", sudoku_difficulty_to_string(diff));
    }
    
    return 0;
}
```

```bash
gcc mi_programa.c -I include -L lib -lsudoku_core -o mi_programa
./mi_programa
```

---

## 📦 Instalación

### Prerrequisitos

- GCC/Clang (C11 o superior)
- Make o CMake
- Git

### Opción 1: Make (Recomendado)

```bash
# Compilar biblioteca y CLI
make

# Solo biblioteca
make lib

# Solo CLI
make cli

# Instalar system-wide (opcional)
sudo make install

# Ejecutar tests
make test
```

Esto genera:
- `lib/libsudoku_core.a` - Biblioteca estática
- `bin/sudoku_generator` - CLI ejecutable
- Headers en `include/sudoku/`

### Opción 2: CMake

```bash
mkdir build && cd build
cmake ..
make
sudo make install  # Opcional
```

### Opción 3: Compilación Manual

```bash
# Compilar biblioteca
gcc -c -I include src/core/*.c src/core/*/*.c
ar rcs libsudoku.a *.o

# Compilar CLI
gcc -I include tools/generator_cli/main.c -L. -lsudoku -o sudoku
```

---

## 📖 Uso Básico

### CLI - Modos de Verbosidad

```bash
# Modo 0: Minimal (solo resultado)
./sudoku_generator 0

# Modo 1: Compact (resumen de proceso) - Default
./sudoku_generator 1
./sudoku_generator

# Modo 2: Detailed (debugging completo)
./sudoku_generator 2
```

### API - Funciones Principales

#### Generar Puzzle

```c
SudokuBoard board;
SudokuGenerationStats stats;

if (sudoku_generate(&board, &stats)) {
    // Generación exitosa
    printf("Clues: %d, Empty: %d\n", board.clues, board.empty);
}
```

#### Validar Puzzle

```c
if (sudoku_validate_board(&board)) {
    printf("✓ Puzzle válido\n");
}
```

#### Verificar Solución Única

```c
int solutions = countSolutionsExact(&board, 2);
if (solutions == 1) {
    printf("✓ Solución única\n");
}
```

#### Configurar Verbosidad

```c
sudoku_set_verbosity(1);  // 0=minimal, 1=compact, 2=detailed
```

📖 **Documentación completa**: Ver [docs/API_REFERENCE.md](docs/API_REFERENCE.md)

---

## 🏗️ Estructura del Proyecto

```
sudoku_en_c/
├── include/sudoku/       # 🔓 API Pública
│   └── core/             # Headers públicos (types, generator, validation, board, display)
├── src/core/             # 🔒 Implementación
│   ├── algorithms/       # Backtracking, Fisher-Yates
│   ├── elimination/      # Sistema de 3 fases
│   └── internal/         # Headers privados
├── tests/unit/           # ✅ Tests por módulo
├── tools/                # 🛠️ CLI y utilidades
└── docs/                 # 📚 Documentación técnica
```

**Arquitectura modular** con API pública estable e implementación privada flexible.

📖 **Análisis detallado**: Ver [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)

---

## 🧠 Algoritmo

### Generación (Híbrido)

1. **Fisher-Yates Diagonal**: Llena subgrids independientes (0, 4, 8) con permutaciones aleatorias
2. **Backtracking**: Completa las 54 celdas restantes con backtracking randomizado
3. **3-Phase Elimination**:
   - **Phase 1**: Random selection (1 por subgrid)
   - **Phase 2**: No-alternatives elimination (iterativo)
   - **Phase 3**: Free elimination con verificación de unicidad

### Características Técnicas

- **Complejidad**: O(9^m) donde m = celdas vacías
- **Optimizaciones**: Early-exit, poda inteligente, randomización
- **Verificación**: Conteo exhaustivo con límite configurable
- **Distribución**: Uniformemente aleatoria gracias a Fisher-Yates

📖 **Análisis matemático**: Ver [docs/ALGORITHMS.md](docs/ALGORITHMS.md)

---

## 🗺️ Roadmap

### ✅ v2.2.2 (Actual - Noviembre 2025)
- Modularización completa
- API pública estable (`include/sudoku/`)
- Arquitectura preparada para v3.0

### 🚧 v2.3 (Q1 2025)
- Tests unitarios completos
- Sistema de configuración flexible
- Export/import de puzzles

### 🔮 v3.0 (Q2 2025)
- Soporte multi-tamaño (4x4, 16x16, 25x25)
- Punteros opacos
- Memoria dinámica
- Solver integrado
- Modo interactivo

---

## 🤝 Contribuir

¡Las contribuciones son bienvenidas! Para contribuir:

1. Fork el repositorio
2. Crea una branch (`git checkout -b feature/AmazingFeature`)
3. Commit tus cambios (`git commit -m 'Add AmazingFeature'`)
4. Push a la branch (`git push origin feature/AmazingFeature`)
5. Abre un Pull Request

📖 **Guía completa**: Ver [CONTRIBUTING.md](CONTRIBUTING.md)

---

## 📄 Licencia

Este proyecto está bajo la Licencia Apache 2.0 - ver [LICENSE](LICENSE) para detalles.

**Requisitos de Atribución**:
```
This project uses Sudoku Generator Library
Copyright 2025 Gonzalo Ramírez (@chaLords)
Licensed under Apache License 2.0
```

---

## 👤 Autor

**Gonzalo Ramírez**
- GitHub: [@chaLords](https://github.com/chaLords)
- Email: chaLords@icloud.com

---

## 📚 Documentación

- **[API Reference](docs/API_REFERENCE.md)** - Referencia completa de todas las funciones públicas
- **[Architecture](docs/ARCHITECTURE.md)** - Diseño arquitectónico y decisiones de implementación
- **[Algorithms](docs/ALGORITHMS.md)** - Análisis matemático y algoritmos
- **[Usage Guide](docs/USAGE.md)** - Ejemplos avanzados y casos de uso
- **[Contributing](CONTRIBUTING.md)** - Guía para contribuidores
- **[Changelog](CHANGELOG.md)** - Historial de versiones

---

## 🙏 Agradecimientos

- **Fisher-Yates Algorithm**: Ronald Fisher y Frank Yates (1938)
- **Inspiración**: Comunidad de programación competitiva
- **Referencias**: [GeeksforGeeks](https://www.geeksforgeeks.org/), [Wikipedia](https://en.wikipedia.org/wiki/Sudoku)

---

⭐ **Si este proyecto te fue útil, considera darle una estrella en GitHub!**

---

## 🌐 Otros Idiomas

- [English](README.en.md)

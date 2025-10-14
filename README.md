# 🎮 Generador de Sudoku en C

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![C](https://img.shields.io/badge/C-00599C?style=flat&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey)](https://github.com/chaLords/sudoku_en_c)
[![Version](https://img.shields.io/badge/version-2.2.1-brightgreen.svg)](https://github.com/chaLords/sudoku_en_c/releases)

> Un generador eficiente de sudokus completos utilizando un enfoque híbrido: algoritmo Fisher-Yates para subcuadrículas independientes y backtracking para completar el resto del tablero. Ahora con arquitectura refactorizada basada en estructuras de datos.

## 📋 Tabla de Contenidos

- [Características](#-características)
- [Demo](#-demo)
- [Instalación](#-instalación)
- [Modos de Verbosidad](#-modos-de-verbosidad)
- [Uso](#-uso)
- [Compatibilidad Multiplataforma](#-compatibilidad-multiplataforma)
- [Cómo Funciona](#-cómo-funciona)
- [Algoritmos](#-algoritmos)
- [Estructura del Código](#-estructura-del-código)
- [Roadmap](#-roadmap)
- [Contribuir](#-contribuir)
- [Licencia](#-licencia)
- [Autor](#-autor)

## ✨ Características

- ✅ **Generación completa de sudokus válidos 9x9**
- ✅ **Método híbrido optimizado** (Fisher-Yates + Backtracking)
- ✅ **Alta tasa de éxito** (~99.9%)
- ✅ **Permutaciones uniformemente aleatorias**
- ✅ **Validación automática** del tablero generado
- ✅ **Código limpio y bien documentado**
- ✅ **Interfaz visual en consola** con caracteres box-drawing
- ✅ **Sistema de eliminación de 3 fases** para crear puzzles jugables
- ✅ **Verificación de solución única**
- ✅ **Modos de verbosidad configurables** (0/1/2)
- ✅ **Compatible con Linux, macOS y Windows** (auto-configuración UTF-8)
- ✅ **Arquitectura refactorizada con estructuras de datos** (v2.2.1)
- ✅ **Gestión eficiente de memoria** con punteros y malloc/free
- ✅ **Código modular y mantenible** siguiendo principios SOLID

## 🎯 Demo

### Salida del Programa

```
═══════════════════════════════════════════════════════════════
    SUDOKU GENERATOR v2.2.1 – STRUCTURE-BASED REFACTORING
           Fisher-Yates + Backtracking + 3 Phases
═══════════════════════════════════════════════════════════════

🚀 ATTEMPT #1:
🎲 Filling diagonal with Fisher-Yates...
   SubGrid 0 (base: 0,0): 6 4 8 3 1 5 7 9 2 
   SubGrid 4 (base: 3,3): 9 7 4 3 8 5 1 2 6 
   SubGrid 8 (base: 6,6): 6 3 7 1 8 2 9 4 5 
✅ Diagonal successfully filled!

🔄 Backtracking in progress...
✅ Completed!
🎲 PHASE 1: Selecting numbers per subgrid with Fisher-Yates...
   Subgrid 0 (base: 0,0): 3 
   Subgrid 1 (base: 0,3): 7 
   Subgrid 2 (base: 0,6): 8 
   Subgrid 3 (base: 3,0): 6 
   Subgrid 4 (base: 3,3): 9 
   Subgrid 5 (base: 3,6): 5 
   Subgrid 6 (base: 6,0): 4 
   Subgrid 7 (base: 6,3): 1 
   Subgrid 8 (base: 6,6): 2 
✅ Phase 1 completed!
📊 PHASE 1 TOTAL: Removed 9 cells

--- ROUND 1 ---
🎲 PHASE 2: Selecting numbers without alternatives...
   Subgrid 0 (base: 0,0): 6 
   Subgrid 1 (base: 0,3): 2 
   Subgrid 2 (base: 0,6): 5 
   Subgrid 3 (base: 3,0): 1 
   Subgrid 4 (base: 3,3): 4 
   Subgrid 5 (base: 3,6): 2 
   Subgrid 6 (base: 6,0): 5 
   Subgrid 7 (base: 6,3): 8 
   Subgrid 8 (base: 6,6): 6 
✅ Phase 2 completed! Removed: 9

--- ROUND 2 ---
🎲 PHASE 2: Selecting numbers without alternatives...
   Subgrid 0 (base: 0,0): 8 
   Subgrid 1 (base: 0,3): 1 
   Subgrid 2 (base: 0,6): 9 
   Subgrid 3 (base: 3,0): 8 
   Subgrid 4 (base: 3,3): 3 
   Subgrid 5 (base: 3,6): 7 
   Subgrid 6 (base: 6,0): 9 
   Subgrid 7 (base: 6,3): 5 
   Subgrid 8 (base: 6,6): 3 
✅ Phase 2 completed! Removed: 9

--- ROUND 3 ---
🎲 PHASE 2: Selecting numbers without alternatives...
   Subgrid 0 (base: 0,0): 
   Subgrid 1 (base: 0,3): 4 
   Subgrid 2 (base: 0,6): 
   Subgrid 3 (base: 3,0): 
   Subgrid 4 (base: 3,3): 6 
   Subgrid 5 (base: 3,6): 1 
   Subgrid 6 (base: 6,0): 2 
   Subgrid 7 (base: 6,3): 7 
   Subgrid 8 (base: 6,6): 
✅ Phase 2 completed! Removed: 5

--- ROUND 4 ---
🎲 PHASE 2: Selecting numbers without alternatives...
   Subgrid 0 (base: 0,0): 
   Subgrid 1 (base: 0,3): 
   Subgrid 2 (base: 0,6): 
   Subgrid 3 (base: 3,0): 
   Subgrid 4 (base: 3,3): 
   Subgrid 5 (base: 3,6): 
   Subgrid 6 (base: 6,0): 7 
   Subgrid 7 (base: 6,3): 
   Subgrid 8 (base: 6,6): 
✅ Phase 2 completed! Removed: 1

--- ROUND 5 ---
🎲 PHASE 2: Selecting numbers without alternatives...
   Subgrid 0 (base: 0,0): 
   Subgrid 1 (base: 0,3): 
   Subgrid 2 (base: 0,6): 
   Subgrid 3 (base: 3,0): 
   Subgrid 4 (base: 3,3): 
   Subgrid 5 (base: 3,6): 
   Subgrid 6 (base: 6,0): 
   Subgrid 7 (base: 6,3): 
   Subgrid 8 (base: 6,6): 
✅ Phase 2 completed! Removed: 0

🛑 Cannot remove more numbers in PHASE 2


📊 PHASE 2 TOTAL: 4 rounds, removed 24 cells

🎲 PHASE 3: Free elimination with unique solution verification...
   Removed 2 at memAddr: 0x5f90ecc31410 (5,4) - Total: 1
   Removed 1 at memAddr: 0x5f90ecc31418 (2,7) - Total: 2
   Removed 4 at memAddr: 0x5f90ecc31420 (2,8) - Total: 3
   Removed 9 at memAddr: 0x5f90ecc31428 (5,7) - Total: 4
   Removed 9 at memAddr: 0x5f90ecc31430 (6,4) - Total: 5
   Removed 9 at memAddr: 0x5f90ecc31438 (8,6) - Total: 6
   Removed 9 at memAddr: 0x5f90ecc31440 (1,5) - Total: 7
   Removed 4 at memAddr: 0x5f90ecc31448 (4,6) - Total: 8
   Removed 8 at memAddr: 0x5f90ecc31450 (7,7) - Total: 9
   Removed 8 at memAddr: 0x5f90ecc31458 (2,5) - Total: 10
   Removed 5 at memAddr: 0x5f90ecc31460 (2,3) - Total: 11
   Removed 3 at memAddr: 0x5f90ecc31470 (3,2) - Total: 12
   Removed 5 at memAddr: 0x5f90ecc31478 (5,0) - Total: 13
   Removed 2 at memAddr: 0x5f90ecc31480 (1,6) - Total: 14
   Removed 8 at memAddr: 0x5f90ecc314a8 (5,6) - Total: 15
   Removed 3 at memAddr: 0x5f90ecc314b0 (2,6) - Total: 16
   Removed 8 at memAddr: 0x5f90ecc314b8 (4,4) - Total: 17
   Removed 4 at memAddr: 0x5f90ecc314c8 (5,2) - Total: 18
   Removed 7 at memAddr: 0x5f90ecc314d0 (5,1) - Total: 19
   Removed 3 at memAddr: 0x5f90ecc314e0 (7,1) - Total: 20
   Removed 6 at memAddr: 0x5f90ecc314f0 (7,2) - Total: 21
   Removed 1 at memAddr: 0x5f90ecc31510 (1,1) - Total: 22
   Removed 6 at memAddr: 0x5f90ecc31548 (1,7) - Total: 23
✅ Phase 3 completed! Removed: 23
✅ SUCCESS! Sudoku generated

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
📊 Empty: 56 | Clues: 25

🎉 VERIFIED! The puzzle is valid

📊 Difficulty level: HARD
```

## 📦 Instalación

### Requisitos Previos

- Compilador GCC (o cualquier compilador C estándar)
- Make (opcional)

### Clonar el Repositorio

```bash
git clone https://github.com/chaLords/sudoku_en_c.git
cd sudoku_en_c
```

### Compilar

#### 🐧 **Linux / macOS**
```bash
# Compilación simple
gcc main.c -o sudoku

# Con optimizaciones (recomendado)
gcc -O2 main.c -o sudoku -std=c11

# Con warnings detallados
gcc -Wall -Wextra main.c -o sudoku -std=c11
```

**Nota**: Se recomienda usar `-std=c11` para aprovechar características modernas del lenguaje.

#### 🪟 **Windows (VS Code / MinGW / MSYS2)**
```bash
# Compilación con soporte UTF-8 y C11
gcc -g main.c -o sudoku.exe -fexec-charset=UTF-8 -std=c11

# O simplemente (el programa se auto-configura)
gcc main.c -o sudoku.exe -std=c11
```
**Nota:** El programa detecta automáticamente Windows y configura UTF-8 en tiempo de ejecución. No necesitas configuraciones adicionales.

## 🎛️ Modos de Verbosidad

### Versión 2.2.1: Control de Salida Configurable

El generador soporta tres niveles de verbosidad controlables por línea de comandos:

#### Modo 0: Minimal
**Propósito:** Salida limpia para presentaciones o benchmarks

```bash
./sudoku 0
```

**Muestra:**
- Título del programa
- Tablero de Sudoku final
- Nivel de dificultad

**Tiempo de ejecución:** ~0.22s

**Ejemplo de salida:**
```
═══════════════════════════════════════════════════════════════
  SUDOKU GENERATOR v2.2.1 – STRUCTURE-BASED REFACTORING
═══════════════════════════════════════════════════════════════

┌───────┬───────┬───────┐
│ . . . │ . 2 1 │ . . . │
│ 5 . . │ 4 . . │ 7 . . │
│ . 7 . │ . 8 . │ . . . │
├───────┼───────┼───────┤
│ . 3 . │ . . . │ . . 6 │
│ . . . │ 5 . 9 │ 2 . 3 │
│ . . 4 │ . . 7 │ . . . │
├───────┼───────┼───────┤
│ . . . │ . . . │ . . 8 │
│ 6 5 9 │ . . 2 │ . . . │
│ . . . │ . . 4 │ 9 1 . │
└───────┴───────┴───────┘


📊 Difficulty level: EXPERT
```

---

#### Modo 1: Compact (Por Defecto)
**Propósito:** Uso normal con información resumida

```bash
./sudoku 1
# o simplemente:
./sudoku
```

**Muestra:**
- Resumen de fases de generación
- Estadísticas de eliminación
- Tablero final
- Conteo de celdas vacías/llenas
- Nivel de dificultad

**Tiempo de ejecución:** ~0.56s

**Ejemplo de salida:**
```
═══════════════════════════════════════════════════════════════
  SUDOKU GENERATOR v2.2.1 – STRUCTURE-BASED REFACTORING
═══════════════════════════════════════════════════════════════

🚀 ATTEMPT #1:
🎲 Diagonal + Backtracking...✅ Completed!
🎲 Phase 1: Fisher-Yates selection...✅ Phase 1 completed!
📊 PHASE 1 TOTAL: Removed 9 cells

🎲 Phase 2: Removal rounds...
📊 PHASE 2 TOTAL: 4 rounds, removed 23 cells

🎲 Phase 3: Free elimination...✅ Phase 3 completed! Removed: 25
✅ SUCCESS! Sudoku generated

┌───────┬───────┬───────┐
│ . . . │ . . . │ . . . │
│ . . . │ 5 . 1 │ 9 . . │
│ 7 . 9 │ . 3 . │ . 2 . │
├───────┼───────┼───────┤
│ . 4 . │ . . . │ . . 5 │
│ . 8 . │ 6 . 3 │ . . . │
│ . . 2 │ . . . │ . . . │
├───────┼───────┼───────┤
│ 1 . . │ . . 5 │ . 3 . │
│ . 6 . │ . . . │ . . . │
│ 4 5 . │ 1 2 7 │ 6 9 . │
└───────┴───────┴───────┘
📊 Empty: 57 | Clues: 24

🎉 VERIFIED! The puzzle is valid

📊 Difficulty level: EXPERT

```

---

#### Modo 2: Detailed
**Propósito:** Depuración completa y análisis detallado del algoritmo

```bash
./sudoku 2
```

**Muestra:**
- Todos los pasos de generación
- Números generados con Fisher-Yates
- Cada ronda de FASE 2
- Cada celda removida en FASE 3
- Debugging completo

**Tiempo de ejecución:** ~0.08s (variable)

**Ejemplo de salida (parcial):**
```
═══════════════════════════════════════════════════════════════
        SUDOKU GENERATOR v2.2.1 - HYBRID METHOD
           Fisher-Yates + Backtracking + 3 Phases
═══════════════════════════════════════════════════════════════

🚀 ATTEMPT #1:
🎲 Filling main diagonal with Fisher-Yates...
   Subgrid 0 (base: 0,0): 5 3 7 6 2 1 9 8 4 
   Subgrid 4 (base: 3,3): 8 1 6 4 5 7 9 2 3 
   Subgrid 8 (base: 6,6): 2 7 9 3 5 1 8 4 6 
✅ Diagonal completed!

🔄 Completing with backtracking...
🎲 PHASE 1: Selecting numbers per subgrid with Fisher-Yates...
   Subgrid 0 (base: 0,0): 4 
   Subgrid 1 (base: 0,3): 9 
   [...]

--- ROUND 1 ---
🎲 PHASE 2: Selecting numbers without alternatives...
   Subgrid 0 (base: 0,0): 1 
   [...]

🎲 PHASE 3: Free elimination with unique solution verification...
   Removed 8 at (6,0) - Total: 1
   Removed 6 at (8,6) - Total: 2
   [...]
✅ Phase 3 completed! Removed: 23

✅ SUCCESS! Sudoku generated
[Tablero]
```

---

### Comparación de Modos

| Característica | Modo 0 | Modo 1 | Modo 2 |
|---------------|--------|--------|--------|
| **Fases mostradas** | No | Resumen | Completas |
| **Estadísticas** | Mínimas | Totales | Detalladas |
| **Rondas FASE 2** | No | Total | Cada una |
| **Celdas FASE 3** | No | Total | Una por una |
| **Mejor para** | Presentaciones | Uso diario | Debugging |
| **Líneas de output** | ~15 | ~25 | ~80+ |

---

### Uso con Time

Mide el rendimiento del generador:

```bash
# Modo minimal para benchmarks limpios
time ./sudoku 0

# Modo compact para ver estadísticas
time ./sudoku 1

# Modo detailed para análisis profundo
time ./sudoku 2
```

---

### Cambiar Modo por Defecto

Si quieres cambiar el modo por defecto (actualmente modo 1), edita `main.c`:

```c
int VERBOSITY_LEVEL = 1;  // Cambiar a 0, 1, o 2
```

Luego recompila:
```bash
gcc -O2 main.c -o sudoku -std=c11
```

## 🚀 Uso

### Ejecución Básica

#### 🐧 Linux / macOS
```bash
./sudoku
```

#### 🪟 Windows
```bash
sudoku.exe
# o simplemente:
./sudoku.exe
```

El programa generará automáticamente un puzzle de Sudoku jugable y mostrará:
- El proceso de generación (diagonal + backtracking)
- Las 3 fases de eliminación de celdas
- El puzzle final con asteriscos (*) en celdas vacías
- Estadísticas de celdas vacías/llenas

### Integración en Otro Proyecto
```c
#include "main.c"

int main() {
    // Asignar memoria para el tablero
    SudokuBoard *board = (SudokuBoard *)malloc(sizeof(SudokuBoard));
    if(board == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria\n");
        return 1;
    }
    
    GenerationStats stats;
    
    // Generar puzzle jugable
    if(generateSudoku(board, &stats)) {
        printBoard(board);
        
        // Verificar validez
        if(validateBoard(board)) {
            printf("✅ Puzzle válido!\n");
            printf("📊 Dificultad: %s\n", evaluateDifficulty(board));
        }
    }
    
    // Liberar memoria
    free(board);
    return 0;
}
```

**Nota**: Este ejemplo demuestra el uso de las nuevas estructuras de datos y gestión de memoria dinámica introducidas en v2.2.1.

### Configurar Dificultad

Modifica la constante `PHASE3_TARGET` en `main.c`:

```c
#define PHASE3_TARGET 5   // Fácil (~35 celdas vacías)
#define PHASE3_TARGET 15  // Medio (~45 celdas vacías)
#define PHASE3_TARGET 25  // Difícil (~55 celdas vacías)
```

## 🌍 Compatibilidad Multiplataforma

### ✅ Auto-Configuración de UTF-8

El programa detecta automáticamente el sistema operativo y configura la codificación correcta:

```c
int main() {
    // Configurar codificación según el sistema operativo
    #ifdef _WIN32
        system("chcp 65001 > nul");  // UTF-8 en Windows
    #endif
    
    // ... resto del código
}
```

### 📝 ¿Qué significa esto?

- **🐧 Linux/macOS:** El código se compila sin las líneas de Windows. Sin overhead, sin cambios.
- **🪟 Windows:** El código configura automáticamente UTF-8 para mostrar correctamente los caracteres especiales (═, │, ┌, etc.).

### 🔧 Solución de Problemas en Windows

#### Problema: Caracteres corruptos (�������)

**Causa:** Terminal de Windows usando codificación incorrecta.

**Solución Automática:** El programa se auto-configura al ejecutar. No necesitas hacer nada.

**Solución Manual (si aún falla):**

1. **En VS Code Terminal:**
   - Abre `settings.json` (Ctrl+Shift+P → "Preferences: Open Settings (JSON)")
   - Añade:
   ```json
   {
       "terminal.integrated.defaultProfile.windows": "Command Prompt",
       "terminal.integrated.profiles.windows": {
           "Command Prompt": {
               "path": "C:\\Windows\\System32\\cmd.exe",
               "args": ["/K", "chcp 65001"]
           }
       }
   }
   ```

2. **En PowerShell:**
   ```powershell
   chcp 65001
   ./sudoku.exe
   ```

3. **En CMD:**
   ```cmd
   chcp 65001
   sudoku.exe
   ```

### 📋 Archivo .gitattributes

El proyecto incluye un archivo `.gitattributes` para mantener consistencia de saltos de línea:

```
*.c text eol=lf
*.h text eol=lf
*.md text eol=lf
```

Esto asegura que el código funcione correctamente al clonarse en cualquier sistema operativo.

## 🏗️ Arquitectura del Código (v2.2.1)

### Refactorización Basada en Estructuras

La versión 2.2.1 introduce una refactorización completa del código usando principios modernos de ingeniería de software:

#### Estructuras de Datos

**Position**: Abstrae coordenadas (fila, columna)
```c
typedef struct {
    int row;
    int col;
} Position;
```

**SudokuBoard**: Encapsula el tablero y sus metadatos
```c
typedef struct {
    int cells[SIZE][SIZE];
    int clues;
    int empty;
} SudokuBoard;
```

**SubGrid**: Representa regiones 3x3 del tablero
```c
typedef struct {
    int index;
    Position base;
} SubGrid;
```

**GenerationStats**: Agrupa estadísticas de generación
```c
typedef struct {
    int phase1_removed;
    int phase2_removed;
    int phase2_rounds;
    int phase3_removed;
} GenerationStats;
```

#### Ventajas de la Refactorización

**Modularidad mejorada**: Cada función tiene una responsabilidad clara y única.

**Eficiencia de memoria**: El uso de punteros reduce copias innecesarias. En lugar de pasar 324 bytes por valor, pasamos solo 8 bytes (dirección de memoria).

**Extensibilidad**: Agregar nuevas características al tablero (como timestamps o configuraciones) solo requiere modificar las estructuras, no las firmas de funciones.

**Mantenibilidad**: La separación clara de responsabilidades hace el código más fácil de entender, probar y modificar.

**Type safety**: El uso de `const` previene modificaciones accidentales y permite optimizaciones del compilador.

#### Mejoras Técnicas

- **Forward declarations**: Organización profesional del código con declaraciones adelantadas
- **Typedef**: Simplifica declaraciones y mejora legibilidad
- **Const correctness**: Parámetros de solo lectura marcados explícitamente
- **Memoria dinámica**: Uso educativo de malloc/free en fase 3
- **Documentación Doxygen**: Comentarios profesionales en todas las Funciones

## 🧠 Cómo Funciona

### Enfoque Híbrido + Sistema de 3 Fases

#### **GENERACIÓN: Híbrido Fisher-Yates + Backtracking**

**Fase 1: Fisher-Yates para la Diagonal**
Se llenan las subcuadrículas independientes (0, 4, 8) usando el algoritmo Fisher-Yates:

```
┌───────┬───────┬───────┐
│ [0]   │       │       │  ← Subcuadrícula 0
│       │       │       │
│       │       │       │
├───────┼───────┼───────┤
│       │ [4]   │       │  ← Subcuadrícula 4
│       │       │       │
│       │       │       │
├───────┼───────┼───────┤
│       │       │ [8]   │  ← Subcuadrícula 8
│       │       │       │
│       │       │       │
└───────┴───────┴───────┘
```

**Ventaja:** Estas subcuadrículas no comparten filas, columnas ni regiones, permitiendo llenado independiente sin conflictos.

**Fase 2: Backtracking para el Resto**
Se completan las celdas restantes usando backtracking recursivo con poda:

1. Buscar celda vacía
2. Probar números del 1-9 (aleatorizados)
3. Si es válido → recursión
4. Si falla → backtrack y probar siguiente número

#### **ELIMINACIÓN: Sistema de 3 Fases**

**FASE 1: Eliminación Aleatoria**
- Elimina exactamente 1 celda por cada subcuadrícula (9 total)
- Usa Fisher-Yates para selección aleatoria
- Garantiza distribución uniforme

**FASE 2: Eliminación Sin Alternativas**
- Elimina celdas cuyos números NO pueden ir en otra posición
- Se ejecuta en loop hasta que no pueda eliminar más
- Típicamente elimina 15-25 celdas adicionales

**FASE 3: Eliminación Libre Verificada**
- Elimina celdas libremente hasta alcanzar objetivo (configurable)
- Verifica que el puzzle mantenga solución única
- Usa `countSolutionsExact()` con early exit para eficiencia

### Ventajas del Método Híbrido

| Característica | Backtracking Puro | Fisher-Yates Puro | **Híbrido** |
|----------------|-------------------|-------------------|-------------|
| Velocidad | ⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐⭐ |
| Aleatoriedad | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Éxito | ⭐⭐⭐⭐⭐ | ⭐ | ⭐⭐⭐⭐⭐ |

## 🔧 Algoritmos

### Fisher-Yates Shuffle

**Complejidad:** O(n)

```c
void fisherYatesShuffle(int *array, int size, int start_value) {
    // Fill consecutively
    for(int i = 0; i < size; i++) {
        array[i] = start_value + i;
    }
    
    // Shuffle (Fisher-Yates backward)
    for(int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        // Swap using XOR arithmetic (educational alternative)
        if(i != j) {
            array[i] ^= array[j];
            array[j] ^= array[i];
            array[i] ^= array[j];
        }
    }
}
```

**Propiedades:**
- Genera permutaciones uniformemente aleatorias
- Cada permutación tiene la misma probabilidad
- Tiempo lineal O(n)

### Backtracking con Poda

**Complejidad:** O(9^m) donde m = celdas vacías

```c
bool completeSudoku(SudokuBoard *board) {
    Position pos;
    
    if(!findEmptyCell(board, &pos)) {
        return true; // Complete board
    }
    
    // Array of numbers to try (1-9) shuffled
    int numbers[SIZE];
    fisherYatesShuffle(numbers, SIZE, 1);
    
    for(int i = 0; i < SIZE; i++) {
        int num = numbers[i];
        
        if(isSafePosition(board, &pos, num)) {
            board->cells[pos.row][pos.col] = num;
            
            if(completeSudoku(board)) {
                return true;
            }
            
            board->cells[pos.row][pos.col] = 0; // Backtrack
        }
    }
    
    return false;
}
```

**Optimizaciones:**
- Poda temprana con `isSafePosition()`
- Verificación eficiente O(1) por celda
- Números aleatorizados para variedad

### Verificación de Solución Única

**Complejidad:** O(9^m) peor caso, O(9^k) típico con early exit

```c
int countSolutionsExact(SudokuBoard *board, int limit) {
    // Cuenta soluciones hasta límite
    // Si encuentra >= 2, para inmediatamente (early exit)
    // Speedup típico: 10^40 - 10^44 veces
}
```

## 📁 Estructura del Código

```
sudoku_en_c/
│
├── main.c                 # Código principal
├── README.md              # Este archivo (Español)
├── README.en.md           # README en inglés
├── LICENSE                # Licencia Apache 2.0
├── CHANGELOG.md           # Historial de versiones
├── NOTICE                 # Archivo de atribuciones
├── .gitignore             # Archivos a ignorar en Git
├── .gitattributes         # Normalización de saltos de línea
└── docs/
    ├── ALGORITHMS.md      # Análisis matemático detallado
    └── TECHNICAL.md       # Documentación técnica profunda
```

### Funciones Principales

| Función | Descripción | Complejidad |
|---------|-------------|-------------|
| `fisherYatesShuffle()` | Genera permutación aleatoria | O(n) |
| `isSafePosition()` | Valida si un número es válido | O(1) |
| `findEmptyCell()` | Busca siguiente celda vacía | O(n²) |
| `fillDiagonal()` | Llena subcuadrículas independientes | O(1) |
| `fillSubGrid()` | Llena una subcuadrícula específica | O(1) |
| `completeSudoku()` | Completa con backtracking | O(9^m) |
| `hasAlternative()` | Verifica si hay alternativas | O(1) |
| `phase1Elimination()` | FASE 1: Eliminación aleatoria | O(1) |
| `phase2Elimination()` | FASE 2: Eliminación sin alternativas | O(n²) |
| `phase3Elimination()` | FASE 3: Eliminación libre verificada | O(n² × 9^m) |
| `countSolutionsExact()` | Cuenta soluciones con early exit | O(9^m) |
| `generateSudoku()` | Orquesta todo el proceso | - |
| `printBoard()` | Imprime el tablero visualmente | O(n²) |
| `validateBoard()` | Valida el tablero completo | O(n²) |
| `evaluateDifficulty()` | Evalúa dificultad por pistas | O(1) |
| `initBoard()` | Inicializa tablero vacío | O(n²) |
| `updateBoardStats()` | Actualiza estadísticas | O(n²) |
| `createSubGrid()` | Crea estructura SubGrid | O(1) |
| `getPositionInSubGrid()` | Calcula posición en subgrid | O(1) |

## 🗺️ Roadmap

### ✅ Versión 2.2.1 (Actual - Enero 2025)
- [x] Generación completa de sudokus
- [x] Sistema de 3 fases de eliminación
- [x] Verificación de solución única
- [x] Validación automática
- [x] Interfaz de consola
- [x] Documentación completa
- [x] Código completamente en inglés
- [x] Compatibilidad multiplataforma (Linux/macOS/Windows)
- [x] Auto-configuración UTF-8
- [x] Modos de verbosidad configurables (0/1/2)
- [x] Parseo de argumentos de línea de comandos
- [x] **Refactorización con estructuras de datos**
- [x] **Forward declarations profesionales**
- [x] **Gestión de memoria con malloc/free**
- [x] **Arquitectura modular y extensible**

### 🚧 Versión 2.3 (Próximo - Q1 2025)
- [ ] Tests unitarios con framework de testing
- [ ] Configuración flexible (Config struct)
- [ ] Export/import de puzzles (save/load)
- [ ] Estadísticas detalladas de generación

### 🔮 Versión 3.0 (Futuro - Q2 2025)
- [ ] Soporte multi-tamaño (4x4, 16x16, 25x25)
- [ ] CLI mejorado con sistema de dos dígitos (`./sudoku 00`)
- [ ] Memoria dinámica para tableros variables
- [ ] Selector de dificultad integrado
- [ ] Solver automático
- [ ] GUI con ncurses
- [ ] Modo interactivo para jugar

## 🤝 Contribuir

¡Las contribuciones son bienvenidas! Si quieres mejorar el proyecto:

1. **Fork** el repositorio
2. Crea una **rama** para tu feature (`git checkout -b feature/AmazingFeature`)
3. **Commit** tus cambios (`git commit -m 'Add some AmazingFeature'`)
4. **Push** a la rama (`git push origin feature/AmazingFeature`)
5. Abre un **Pull Request**

### Ideas de Contribución

- 🐛 Reportar bugs
- 💡 Sugerir nuevas características
- 📝 Mejorar documentación
- ⚡ Optimizar algoritmos
- 🧪 Agregar tests
- 🌍 Mejorar compatibilidad multiplataforma

## 📄 Licencia

Este proyecto está bajo la Licencia Apache 2.0 - ver el archivo [LICENSE](LICENSE) para más detalles.

### ¿Qué significa?

✅ **Puedes:**
- Usar comercialmente
- Modificar el código
- Distribuir
- Uso privado
- Sublicenciar

⚠️ **Debes:**
- Incluir la licencia y copyright
- Incluir el archivo NOTICE
- Indicar cambios realizados
- Dar crédito apropiado

❌ **No puedes:**
- Hacer responsable al autor
- Usar marcas registradas sin permiso

### 📋 Créditos Requeridos

Si usas este código en tu proyecto, debes dar crédito apropiado:

```
Este proyecto utiliza el Generador de Sudoku en C
Copyright 2025 Gonzalo Ramírez (@chaLords)
Licenciado bajo Apache License 2.0
```

## 👤 Autor

**Gonzalo Ramírez**

- GitHub: [@chaLords](https://github.com/chaLords)
- Email: chaLords@icloud.com

## 🙏 Agradecimientos

- Algoritmo Fisher-Yates: Ronald Fisher y Frank Yates (1938)
- Inspiración: Comunidad de programación competitiva
- Recursos: [GeeksforGeeks](https://www.geeksforgeeks.org/), [Wikipedia](https://en.wikipedia.org/wiki/Sudoku)

## 📚 Referencias

- [Fisher-Yates Shuffle Algorithm](https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle)
- [Sudoku Solving Algorithms](https://en.wikipedia.org/wiki/Sudoku_solving_algorithms)
- [Backtracking](https://en.wikipedia.org/wiki/Backtracking)

---

⭐ Si este proyecto te fue útil, considera darle una estrella en GitHub!

---

**Nota:** Este es un proyecto educativo. El código está disponible libremente para aprender, modificar y usar.

---

## 🌐 Otros Idiomas

- [English](README.en.md)

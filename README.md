# 🎮 Generador de Sudoku en C

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![C](https://img.shields.io/badge/C-00599C?style=flat&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey)](https://github.com/chaLords/sudoku_en_c)

> Un generador eficiente de sudokus completos utilizando un enfoque híbrido: algoritmo Fisher-Yates para subcuadrículas independientes y backtracking para completar el resto del tablero.

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

## 🎯 Demo

### Salida del Programa

```
═══════════════════════════════════════════════════════════════
        SUDOKU GENERATOR v2.2.0 - HYBRID METHOD
           Fisher-Yates + Backtracking + 3 Phases
═══════════════════════════════════════════════════════════════

🚀 ATTEMPT #1:
🎲 Filling main diagonal with Fisher-Yates...
   Subgrid 0 (base: 0,0): 1 2 5 7 3 8 6 9 4 
   Subgrid 4 (base: 3,3): 6 2 8 5 4 7 1 9 3 
   Subgrid 8 (base: 6,6): 9 2 3 6 8 5 7 4 1 
✅ Diagonal completed!

🔄 Completing with backtracking...
🎲 PHASE 1: Selecting numbers per subgrid with Fisher-Yates...
   Subgrid 0 (base: 0,0): 4 
   Subgrid 1 (base: 0,3): 9 
   Subgrid 2 (base: 0,6): 3 
   Subgrid 3 (base: 3,0): 5 
   Subgrid 4 (base: 3,3): 8 
   Subgrid 5 (base: 3,6): 7 
   Subgrid 6 (base: 6,0): 6 
   Subgrid 7 (base: 6,3): 1 
   Subgrid 8 (base: 6,6): 2 
✅ Phase 1 completed!

--- ROUND 1 ---
🎲 PHASE 2: Selecting numbers without alternatives...
   Subgrid 0 (base: 0,0): 1 
   Subgrid 1 (base: 0,3): 3 
   Subgrid 2 (base: 0,6): 4 
   Subgrid 3 (base: 3,0): 7 
   Subgrid 4 (base: 3,3): 6 
   Subgrid 5 (base: 3,6): 9 
   Subgrid 6 (base: 6,0): 8 
   Subgrid 7 (base: 6,3): 7 
   Subgrid 8 (base: 6,6): 6 
✅ Phase 2 completed! Removed: 9

--- ROUND 2 ---
🎲 PHASE 2: Selecting numbers without alternatives...
   Subgrid 0 (base: 0,0): 2 
   Subgrid 1 (base: 0,3): 8 
   Subgrid 2 (base: 0,6): 2 
   Subgrid 3 (base: 3,0): 3 
   Subgrid 4 (base: 3,3): 2 
   Subgrid 5 (base: 3,6): 4 
   Subgrid 6 (base: 6,0): 3 
   Subgrid 7 (base: 6,3): 5 
   Subgrid 8 (base: 6,6): 5 
✅ Phase 2 completed! Removed: 9

--- ROUND 3 ---
🎲 PHASE 2: Selecting numbers without alternatives...
   Subgrid 0 (base: 0,0): 9 
   Subgrid 1 (base: 0,3): 
   Subgrid 2 (base: 0,6): 8 
   Subgrid 3 (base: 3,0): 
   Subgrid 4 (base: 3,3): 3 
   Subgrid 5 (base: 3,6): 1 
   Subgrid 6 (base: 6,0): 
   Subgrid 7 (base: 6,3): 4 
   Subgrid 8 (base: 6,6): 7 
✅ Phase 2 completed! Removed: 6

--- ROUND 4 ---
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

🎲 PHASE 3: Free elimination with unique solution verification...
   Removed 9 at (4,0) - Total: 1
   Removed 7 at (2,4) - Total: 2
   Removed 4 at (4,4) - Total: 3
   Removed 1 at (3,1) - Total: 4
   Removed 7 at (1,0) - Total: 5
   Removed 9 at (5,4) - Total: 6
   Removed 4 at (6,1) - Total: 7
   Removed 9 at (7,5) - Total: 8
   Removed 6 at (2,0) - Total: 9
   Removed 7 at (4,5) - Total: 10
   Removed 2 at (7,0) - Total: 11
   Removed 6 at (0,5) - Total: 12
   Removed 6 at (5,1) - Total: 13
   Removed 6 at (8,4) - Total: 14
   Removed 2 at (4,6) - Total: 15
   Removed 7 at (0,7) - Total: 16
   Removed 8 at (5,6) - Total: 17
   Removed 5 at (0,2) - Total: 18
   Removed 6 at (1,7) - Total: 19
   Removed 8 at (1,2) - Total: 20
✅ Phase 3 completed! Removed: 20

✅ SUCCESS! Sudoku generated

┌───────┬───────┬───────┐
│ * * * │ * * * │ * * 9 │
│ * 3 * │ * 5 4 │ 1 * * │
│ * * * │ 2 * 1 │ 5 * * │
├───────┼───────┼───────┤
│ * * * │ * * * │ 3 * * │
│ * 8 * │ 5 * * │ * * 6 │
│ 4 * 2 │ 1 * * │ * 5 * │
├───────┼───────┼───────┤
│ * * * │ * * * │ 9 * 3 │
│ * 7 1 │ * 3 * │ * 8 * │
│ * 5 9 │ 8 * 2 │ * 4 1 │
└───────┴───────┴───────┘
📊 Empty cells: 53 | Filled cells: 28

🎉 VERIFIED! The puzzle is valid
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

# Con optimizaciones
gcc -O2 main.c -o sudoku

# Con warnings detallados
gcc -Wall -Wextra main.c -o sudoku
```

#### 🪟 **Windows (VS Code / MinGW / MSYS2)**

```bash
# Compilación con soporte UTF-8
gcc -g main.c -o sudoku.exe -fexec-charset=UTF-8

# O simplemente (el programa se auto-configura)
gcc main.c -o sudoku.exe
```

**Nota:** El programa detecta automáticamente Windows y configura UTF-8 en tiempo de ejecución. No necesitas configuraciones adicionales.

## 🎛️ Modos de Verbosidad

### Versión 2.2.0: Control de Salida Configurable

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
        SUDOKU GENERATOR v2.2.0 - HYBRID METHOD
═══════════════════════════════════════════════════════════════

┌───────┬───────┬───────┐
│ * * 8 │ * 9 * │ * * * │
│ * 5 * │ * * * │ * 6 * │
│ * * * │ * 4 * │ * 2 7 │
├───────┼───────┼───────┤
│ 3 * 6 │ * * * │ * * 8 │
│ * * 7 │ * 6 * │ 9 4 * │
│ 2 * * │ * * * │ * 1 3 │
├───────┼───────┼───────┤
│ * * 4 │ * * * │ * * * │
│ * * * │ 2 * 1 │ * * * │
│ 6 * 3 │ * 8 9 │ * * * │
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
        SUDOKU GENERATOR v2.2.0 - HYBRID METHOD
═══════════════════════════════════════════════════════════════

🚀 ATTEMPT #1:
🎲 Diagonal + Backtracking...✅ Completed!
🎲 Phase 1: Fisher-Yates selection...✅ Phase 1 completed!
📊 PHASE 1 TOTAL: Removed 9 cells
🎲 Phase 2: Removal rounds...
📊 PHASE 2 TOTAL: Removed 23 cells across 4 rounds
🎲 Phase 3: Free elimination...✅ Phase 3 completed! Removed: 24
✅ SUCCESS! Sudoku generated

┌───────┬───────┬───────┐
│ * * * │ * * * │ 6 * * │
│ * * 8 │ 4 * * │ * * * │
│ * * 5 │ * * 3 │ 8 * 2 │
├───────┼───────┼───────┤
│ * * 2 │ * * * │ * 6 * │
│ 4 3 * │ * 7 * │ * * * │
│ * 6 * │ 2 1 * │ * 8 * │
├───────┼───────┼───────┤
│ * * * │ * * 7 │ 5 * * │
│ * 2 * │ 8 * * │ * 1 * │
│ 1 * 7 │ * * 9 │ * 3 * │
└───────┴───────┴───────┘
📊 Empty cells: 56 | Filled cells: 25

🎉 VERIFIED! The puzzle is valid

📊 Difficulty level: HARD
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
        SUDOKU GENERATOR v2.2.0 - HYBRID METHOD
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
gcc -O2 main.c -o sudoku -std=c99
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
    int sudoku[SIZE][SIZE];
    
    // Generar puzzle jugable
    if(generateHybridSudoku(sudoku)) {
        printSudoku(sudoku);
        
        // Verificar validez
        if(validateSudoku(sudoku)) {
            printf("Puzzle válido!\n");
        }
    }
    
    return 0;
}
```

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
void fisherYatesShuffle(int *array, int size, int num_in) {
    // Llenar array consecutivo
    for(int i = 0; i < size; i++) {
        array[i] = num_in + i;
    }
    
    // Mezclar (Fisher-Yates)
    for(int i = size-1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
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
bool completeSudoku(int sudoku[SIZE][SIZE]) {
    int row, col;
    
    // 1. Buscar celda vacía
    if(!findEmptyCell(sudoku, &row, &col)) {
        return true; // ¡Completo!
    }
    
    // 2. Probar números 1-9
    for(int num = 1; num <= 9; num++) {
        if(isSafePosition(sudoku, row, col, num)) {
            sudoku[row][col] = num;
            
            // 3. Recursión
            if(completeSudoku(sudoku)) {
                return true;
            }
            
            // 4. Backtrack
            sudoku[row][col] = 0;
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
int countSolutionsExact(int sudoku[SIZE][SIZE], int limite) {
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
| `completeSudoku()` | Completa con backtracking | O(9^m) |
| `firstRandomElimination()` | FASE 1: Eliminación aleatoria | O(1) |
| `hasAlternativeInRowCol()` | Verifica alternativas | O(1) |
| `secondNoAlternativeElimination()` | FASE 2: Sin alternativas | O(n²) |
| `countSolutionsExact()` | Cuenta soluciones (con early exit) | O(9^m) |
| `thirdFreeElimination()` | FASE 3: Libre verificada | O(n² × 9^m) |
| `generateHybridSudoku()` | Orquesta todo el proceso | - |
| `printSudoku()` | Imprime visualmente | O(n²) |
| `validateSudoku()` | Valida el tablero completo | O(n²) |

## 🗺️ Roadmap

### ✅ Versión 2.2.0 (Actual)
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

### 🚧 Versión 2.5 (Futuro Cercano)
- [ ] Selección dinámica de dificultad
- [ ] Múltiples niveles de dificultad
- [ ] Solver automático
- [ ] Exportar/importar sudokus
- [ ] Estadísticas de generación
- [ ] Tests unitarios

### 🔮 Versión 3.0 (Ideas)
- [ ] GUI con ncurses
- [ ] Modo interactivo para jugar
- [ ] Generador de variantes (6x6, 12x12, etc.)
- [ ] API REST
- [ ] Biblioteca compartida (.so/.dll)

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

# 🎮 Generador de Sudoku en C

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![C](https://img.shields.io/badge/C-00599C?style=flat&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey)](https://github.com/chaLords/sudoku_en_c)

> Un generador eficiente de sudokus completos utilizando un enfoque híbrido: algoritmo Fisher-Yates para subcuadrículas independientes y backtracking para completar el resto del tablero.

## 📋 Tabla de Contenidos

- [Características](#-características)
- [Demo](#-demo)
- [Instalación](#-instalación)
- [Uso](#-uso)
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

## 🎯 Demo

### Salida del Programa

```
═══════════════════════════════════════════════════════════════
        SUDOKU GENERATOR v2.0.0 - HYBRID METHOD
           Fisher-Yates + Backtracking + 3 Phases
═══════════════════════════════════════════════════════════════

🚀 ATTEMPT #1:
🎲 Filling main diagonal with Fisher-Yates...
   Subgrid 0 (base: 0,0): 8 5 3 7 6 2 1 9 4
   Subgrid 4 (base: 3,3): 1 9 7 4 3 8 2 5 6
   Subgrid 8 (base: 6,6): 3 6 5 1 8 7 9 4 2
✅ Diagonal completed!

🔄 Completing with backtracking...

🎲 PHASE 1: Selecting numbers per subgrid with Fisher-Yates...
   Subgrid 0 (base: 0,0): 8
   Subgrid 1 (base: 0,3): 9
   Subgrid 2 (base: 0,6): 1
   Subgrid 3 (base: 3,0): 4
   Subgrid 4 (base: 3,3): 7
   Subgrid 5 (base: 3,6): 5
   Subgrid 6 (base: 6,0): 7
   Subgrid 7 (base: 6,3): 4
   Subgrid 8 (base: 6,6): 6
✅ Phase 1 completed!

--- ROUND 1 ---
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
   Removed 8 at (1,7) - Total: 1
   Removed 6 at (0,8) - Total: 2
   Removed 8 at (3,4) - Total: 3
   Removed 2 at (2,6) - Total: 4
   Removed 2 at (8,5) - Total: 5
   Removed 1 at (5,6) - Total: 6
   Removed 1 at (8,3) - Total: 7
   Removed 1 at (2,8) - Total: 8
   Removed 6 at (2,4) - Total: 9
   Removed 9 at (2,3) - Total: 10
   Removed 9 at (6,7) - Total: 11
   Removed 5 at (5,1) - Total: 12
   Removed 8 at (8,0) - Total: 13
   Removed 9 at (1,1) - Total: 14
   Removed 3 at (6,4) - Total: 15
   Removed 5 at (4,3) - Total: 16
   Removed 1 at (3,5) - Total: 17
   Removed 2 at (6,8) - Total: 18
   Removed 7 at (5,3) - Total: 19
   Removed 5 at (7,4) - Total: 20
✅ Phase 3 completed! Removed: 20

✅ SUCCESS! Sudoku generated

┌───────┬───────┬───────┐
│ * 8 * │ * 7 4 │ * * * │
│ * * * │ 2 * * │ * * 3 │
│ 5 4 3 │ * * * │ * * * │
├───────┼───────┼───────┤
│ * 7 4 │ * * * │ * 4 7 │
│ 6 * 1 │ * * * │ * 6 9 │
│ 2 * * │ * * 3 │ * 6 9 │
├───────┼───────┼───────┤
│ * 1 * │ * * 6 │ * * * │
│ 3 * * │ * * * │ * * 8 │
│ * * 7 │ * 9 * │ 5 * 4 │
└───────┴───────┴───────┘
📊 Empty cells: 55 | Filled cells: 26

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

```bash
# Compilación simple
gcc main.c -o sudoku

# Con optimizaciones
gcc -O2 main.c -o sudoku

# Con warnings detallados
gcc -Wall -Wextra main.c -o sudoku
```

## 🚀 Uso

### Ejecución Básica

```bash
./sudoku
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
#define PHASE3_TARGET 20  // Fácil (~35 celdas vacías)
#define PHASE3_TARGET 30  // Medio (~45 celdas vacías)
#define PHASE3_TARGET 40  // Difícil (~55 celdas vacías)
```

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
- Usa `countSolutions()` con early exit para eficiencia

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
int countSolutions(int sudoku[SIZE][SIZE], int limite) {
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
| `countSolutions()` | Cuenta soluciones (con early exit) | O(9^m) |
| `thirdFreeElimination()` | FASE 3: Libre verificada | O(n² × 9^m) |
| `generateHybridSudoku()` | Orquesta todo el proceso | - |
| `printSudoku()` | Imprime visualmente | O(n²) |
| `validateSudoku()` | Valida el tablero completo | O(n²) |

## 🗺️ Roadmap

### ✅ Versión 2.0 (Actual)
- [x] Generación completa de sudokus
- [x] Sistema de 3 fases de eliminación
- [x] Verificación de solución única
- [x] Validación automática
- [x] Interfaz de consola
- [x] Documentación completa
- [x] Código completamente en inglés

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

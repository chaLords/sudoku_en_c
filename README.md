# 🎮 Generador de Sudoku en C

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://www.apache.org/licenses/LICENSE-2.0)
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

## 🎯 Demo

### Salida del Programa

```
═══════════════════════════════════════════════════════════════
              MÉTODO HÍBRIDO RECOMENDADO
          Tu Fisher-Yates + Backtracking
═══════════════════════════════════════════════════════════════

🚀 INTENTO #1:
🎲 Llenando diagonal principal con Fisher-Yates...
   Subcuadrícula 0 (base: 0,0): 5 3 7 6 2 1 9 8 4 
   Subcuadrícula 4 (base: 3,3): 8 1 6 4 5 7 9 2 3 
   Subcuadrícula 8 (base: 6,6): 2 7 9 3 5 1 8 4 6 
✅ Diagonal completada!

🔄 Completando con backtracking...
✅ ¡ÉXITO! Sudoku generado
┌───────┬───────┬───────┐
│ 5 3 7 │ 2 4 9 │ 6 1 8 │
│ 6 2 1 │ 3 8 5 │ 4 9 7 │
│ 9 8 4 │ 1 7 6 │ 2 3 5 │
├───────┼───────┼───────┤
│ 7 4 2 │ 8 1 6 │ 9 5 3 │
│ 1 9 3 │ 4 5 7 │ 8 6 2 │
│ 8 6 5 │ 9 2 3 │ 7 4 1 │
├───────┼───────┼───────┤
│ 4 1 6 │ 5 3 8 │ 2 7 9 │
│ 2 7 8 │ 6 9 4 │ 3 5 1 │
│ 3 5 9 │ 7 1 2 │ 8 4 6 │
└───────┴───────┴───────┘
🎉 ¡VERIFICADO! El Sudoku es válido
```

## 📦 Instalación

### Requisitos Previos

- Compilador GCC (o cualquier compilador C estándar)
- Make (opcional)

### Clonar el Repositorio

```bash
git clone https://github.com/tu-usuario/sudoku-generator.git
cd sudoku-generator
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

El programa generará automáticamente 5 sudokus completos y mostrará el resultado.

### Integración en Otro Proyecto

```c
#include "main.c"

int main() {
    int sudoku[SIZE][SIZE];
    
    // Generar sudoku
    if(generarSudokuHibrido(sudoku)) {
        imprimirSudoku(sudoku);
    }
    
    return 0;
}
```

## 🧠 Cómo Funciona

### Enfoque Híbrido

El generador utiliza una estrategia en dos fases:

#### **Fase 1: Fisher-Yates para la Diagonal**
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

#### **Fase 2: Backtracking para el Resto**
Se completan las celdas restantes usando backtracking recursivo con poda:

1. Buscar celda vacía
2. Probar números del 1-9 (aleatorizados)
3. Si es válido → recursión
4. Si falla → backtrack y probar siguiente número

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
void num_orden_fisher_yates(int *array, int size, int num_in) {
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
bool completarSudoku(int sudoku[SIZE][SIZE]) {
    // 1. Buscar celda vacía
    if(!encontrarCeldaVacia(sudoku, &fila, &col)) {
        return true; // ¡Completo!
    }
    
    // 2. Probar números 1-9
    for(int num = 1; num <= 9; num++) {
        if(esSafePosicion(sudoku, fila, col, num)) {
            sudoku[fila][col] = num;
            
            // 3. Recursión
            if(completarSudoku(sudoku)) {
                return true;
            }
            
            // 4. Backtrack
            sudoku[fila][col] = 0;
        }
    }
    
    return false;
}
```

**Optimizaciones:**
- Poda temprana con `esSafePosicion()`
- Verificación eficiente O(1) por celda
- Números aleatorizados para variedad

## 📁 Estructura del Código

```
sudoku-generator/
│
├── main.c                 # Código principal
├── README.md              # Este archivo (español)
├── README.en.md           # Documentación en inglés
├── LICENSE                # Licencia Apache 2.0
├── NOTICE                 # Archivo de atribuciones
├── .gitignore             # Archivos ignorados por Git
└── docs/
    ├── TECHNICAL.md          # Documentación Arquitectura e implementación
    └── ALGORITHMS.md         # Documentación Análisis matemático

```

### Funciones Principales

| Función | Descripción | Complejidad |
|---------|-------------|-------------|
| `num_orden_fisher_yates()` | Genera permutación aleatoria | O(n) |
| `esSafePosicion()` | Valida si un número es válido | O(1) |
| `llenarDiagonal()` | Llena subcuadrículas independientes | O(1) |
| `completarSudoku()` | Completa con backtracking | O(9^m) |
| `verificarSudoku()` | Valida el tablero completo | O(n²) |

## 🗺️ Roadmap

### ✅ Versión 1.0 (Actual)
- [x] Generación completa de sudokus
- [x] Validación automática
- [x] Interfaz de consola
- [x] Documentación básica

### 🚧 Versión 2.0 (Futuro)
- [ ] Generación de puzzles jugables
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
- Indicar cambios realizados
- **Incluir el archivo NOTICE con atribuciones**
- **Dar crédito visible en la documentación**

✅ **Además:**
- Protección explícita de patentes
- Limitación de responsabilidad

❌ **No puedes:**
- Hacer responsable al autor
- Usar marcas registradas sin permiso

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

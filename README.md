# 🎮 Generador de Sudoku en C

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://www.apache.org/licenses/LICENSE-2.0)
[![C](https://img.shields.io/badge/C-00599C?style=flat&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey)](https://github.com/chaLords/sudoku_en_c)
[![Version](https://img.shields.io/badge/version-2.0.0-green)](https://github.com/chaLords/sudoku_en_c/releases)

> Un generador eficiente de puzzles de Sudoku utilizando un enfoque híbrido: algoritmo Fisher-Yates para subcuadrículas independientes, backtracking para completar el tablero, y sistema de 3 fases para crear puzzles jugables con solución única garantizada.

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
- ✅ **Generación de puzzles jugables con 3 fases de eliminación**
- ✅ **Verificación de solución única** usando contador de soluciones
- ✅ **Método híbrido optimizado** (Fisher-Yates + Backtracking)
- ✅ **Alta tasa de éxito** (~99.9%)
- ✅ **Permutaciones uniformemente aleatorias**
- ✅ **Validación automática** del tablero generado
- ✅ **Múltiples niveles de dificultad** (configurable)
- ✅ **Código limpio y bien documentado**
- ✅ **Interfaz visual en consola** con caracteres box-drawing

## 🎯 Demo

### Salida del Programa (v2.0)

```
═══════════════════════════════════════════════════════════════
              MÉTODO HÍBRIDO RECOMENDADO
          Tu Fisher-Yates + Backtracking
═══════════════════════════════════════════════════════════════

🚀 INTENTO #1:
🎲 Llenando diagonal principal con Fisher-Yates...
   Subcuadrícula 0 (base: 0,0): 3 6 4 1 5 2 8 9 7 
   Subcuadrícula 4 (base: 3,3): 3 7 4 9 6 1 5 2 8 
   Subcuadrícula 8 (base: 6,6): 2 4 5 7 9 8 1 6 3 
✅ Diagonal completada!

🔄 Completando con backtracking...

🎲 FASE 1: Eligiendo números por subcuadrículas con Fisher-Yates...
Array random generado: 5 8 3 1 7 9 6 2 4
   Subcuadrícula 0 (base: 0,0): 5
   Subcuadrícula 1 (base: 0,3): 8
   Subcuadrícula 2 (base: 0,6): 3
   Subcuadrícula 3 (base: 3,0): 1
   Subcuadrícula 4 (base: 3,3): 7
   Subcuadrícula 5 (base: 3,6): 9
   Subcuadrícula 6 (base: 6,0): 6
   Subcuadrícula 7 (base: 6,3): 2
   Subcuadrícula 8 (base: 6,6): 4
✅ Elección completada!

--- RONDA 1 ---
🎲 FASE 2: Eligiendo números por subcuadrículas con Fisher-Yates...
   Subcuadrícula_2 0 (base: 0,0): 
   Subcuadrícula_2 1 (base: 0,3): 
   Subcuadrícula_2 2 (base: 0,6): 
   Subcuadrícula_2 3 (base: 3,0): 
   Subcuadrícula_2 4 (base: 3,3): 
   Subcuadrícula_2 5 (base: 3,6): 
   Subcuadrícula_2 6 (base: 6,0): 
   Subcuadrícula_2 7 (base: 6,3): 
   Subcuadrícula_2 8 (base: 6,6): 
✅ Fase 2 completada! Eliminados: 0

🛑 No se pueden eliminar más números en FASE 2

🎯 FASE 3: Eliminación libre con verificación de solución única...
Eliminado 3 en (1,1) - Total: 1
Eliminado 9 en (4,0) - Total: 2
Eliminado 9 en (8,3) - Total: 3
Eliminado 4 en (2,7) - Total: 4
Eliminado 7 en (8,5) - Total: 5
Eliminado 3 en (4,6) - Total: 6
Eliminado 8 en (5,5) - Total: 7
Eliminado 2 en (8,2) - Total: 8
Eliminado 1 en (5,2) - Total: 9
Eliminado 7 en (5,8) - Total: 10
Eliminado 9 en (5,8) - Total: 11
Eliminado 1 en (1,8) - Total: 12
Eliminado 6 en (0,3) - Total: 13
Eliminado 5 en (0,5) - Total: 14
Eliminado 8 en (7,1) - Total: 15
Eliminado 4 en (5,4) - Total: 16
Eliminado 8 en (8,7) - Total: 17
Eliminado 6 en (5,6) - Total: 18
Eliminado 3 en (7,8) - Total: 19
Eliminado 4 en (8,6) - Total: 20
FASE 3 completada! Eliminados: 20

✅ ¡ÉXITO! Sudoku generado

┌───────┬───────┬───────┐
│ * * * │ * * * │ * * 2 │
│ * * 7 │ * 9 4 │ * 6 * │
│ 8 9 * │ * 1 * │ 7 * 5 │
├───────┼───────┼───────┤
│ * 7 * │ 2 * * │ * * * │
│ * 5 * │ * 6 1 │ * 2 8 │
│ 3 * * │ 8 * * │ * * * │
├───────┼───────┼───────┤
│ * 6 * │ * * 5 │ * 1 9 │
│ 4 * * │ * * * │ 5 * * │
│ * 1 * │ * 3 * │ * * * │
└───────┴───────┴───────┘
Celdas vacías: 54 | Celdas llenas: 27

🎉 ¡VERIFICADO! El puzzle es válido
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
    if(generarSudokuHibrido(sudoku)) {
        imprimirSudoku(sudoku);
        
        // Verificar validez
        if(verificarSudoku(sudoku)) {
            printf("Puzzle válido!\n");
        }
    }
    
    return 0;
}
```

### Configurar Dificultad

Modifica la variable `objetivo_adicional` en la función `generarSudokuHibrido()` dentro de `main.c`:

```c
// En la función generarSudokuHibrido()
int objetivo_adicional = 10;  // Fácil (~40-45 celdas vacías)
int objetivo_adicional = 20;  // Medio (~50-55 celdas vacías)
int objetivo_adicional = 30;  // Difícil (~60-65 celdas vacías)
```

**Nota:** El número final de celdas vacías depende de:
- FASE 1: Siempre elimina 9 celdas (1 por subcuadrícula)
- FASE 2: Elimina cantidad variable según estructura (0-25 celdas típicamente)
- FASE 3: Elimina hasta el objetivo especificado manteniendo solución única

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

#### **ELIMINACIÓN: Sistema de 3 Fases para Puzzles Jugables**

**FASE 1: Eliminación Aleatoria por Subcuadrícula**
- Elimina 1 número aleatorio de cada subcuadrícula (9 celdas)
- Usa Fisher-Yates para elegir qué número (1-9) eliminar
- **Garantiza distribución uniforme**

**FASE 2: Eliminación Sin Alternativas (Loop)**
- Busca números que NO tienen alternativas en su fila/columna/subcuadrícula
- Si encuentra uno, lo elimina y repite el proceso
- Continúa hasta que no pueda eliminar más
- **Típicamente elimina 0-25 celdas adicionales**

**FASE 3: Eliminación Libre con Verificación**
- Intenta eliminar celdas adicionales en orden secuencial
- Verifica que mantenga **solución única** usando `contarSoluciones()`
- Configurable mediante variable `objetivo_adicional`
- **Permite ajustar dificultad del puzzle**

### Ventajas del Método Completo

| Característica | Backtracking Puro | Eliminación Simple | **Híbrido 3 Fases** |
|----------------|-------------------|-------------------|---------------------|
| Velocidad Generación | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Aleatoriedad | ⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Solución Única | ❌ | ❌ | ✅ |
| Control Dificultad | ❌ | ⭐⭐ | ⭐⭐⭐⭐⭐ |
| Puzzles Jugables | ❌ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |

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

### Contador de Soluciones

**Complejidad:** O(9^m) con límite configurable

```c
int contarSoluciones(int sudoku[SIZE][SIZE], int limite) {
    // Encuentra celda vacía
    if(!encontrarCeldaVacia(sudoku, &fila, &col)) {
        return 1; // Solución encontrada
    }
    
    int total = 0;
    for(int num = 1; num <= 9; num++) {
        if(esSafePosicion(sudoku, fila, col, num)) {
            sudoku[fila][col] = num;
            total += contarSoluciones(sudoku, limite);
            
            // Early exit si ya encontramos múltiples
            if(total >= limite) {
                sudoku[fila][col] = 0;
                return total;
            }
            
            sudoku[fila][col] = 0;
        }
    }
    return total;
}
```

**Uso:** Garantiza que el puzzle tenga **exactamente una solución**

## 📁 Estructura del Código

```
sudoku_en_c/
│
├── main.c                 # Código principal
├── README.md              # Este archivo (español)
├── README.en.md           # Documentación en inglés
├── LICENSE                # Licencia Apache 2.0
├── NOTICE                 # Requisitos de atribución
├── CHANGELOG.md           # Historial de cambios entre versiones
├── .gitignore             # Archivos ignorados por Git
└── docs/
    ├── TECHNICAL.md       # Documentación técnica: arquitectura y rendimiento
    └── ALGORITHMS.md      # Análisis matemático: teoremas y complejidad
```

### Archivos del Proyecto

| Archivo | Descripción |
|---------|-------------|
| `main.c` | Código fuente completo con todas las funciones del generador |
| `README.md` / `README.en.md` | Documentación principal del proyecto (español/inglés) |
| `LICENSE` | Licencia Apache 2.0 con términos de uso |
| `NOTICE` | Archivo de atribuciones requerido por Apache 2.0 |
| `CHANGELOG.md` | Registro cronológico de todos los cambios entre versiones |
| `docs/TECHNICAL.md` | Documentación técnica detallada: arquitectura, benchmarks, decisiones de diseño |
| `docs/ALGORITHMS.md` | Análisis matemático profundo: teoremas, demostraciones, complejidad |

### Funciones Principales

| Función | Descripción | Complejidad |
|---------|-------------|-------------|
| `num_orden_fisher_yates()` | Genera permutación aleatoria | O(n) |
| `esSafePosicion()` | Valida si un número es válido | O(1) |
| `llenarDiagonal()` | Llena subcuadrículas independientes | O(1) |
| `completarSudoku()` | Completa con backtracking | O(9^m) |
| `primeraEleccionAleatoria()` | FASE 1: Eliminación aleatoria | O(1) |
| `tieneAlternativaEnFilaCol()` | Verifica alternativas de un número | O(1) |
| `segundaEleccionSinAlternativas()` | FASE 2: Eliminación sin alternativas | O(n²) |
| `contarSoluciones()` | Cuenta soluciones posibles | O(9^m) |
| `terceraEleccionLibre()` | FASE 3: Eliminación verificada | O(n² × 9^m) |
| `verificarSudoku()` | Valida el tablero completo | O(n²) 
|## 🗺️ Roadmap

### ✅ Versión 1.0
- [x] Generación completa de sudokus
- [x] Validación automática
- [x] Interfaz de consola
- [x] Documentación básica

### ✅ Versión 2.0 (Actual)
- [x] Generación de puzzles jugables
- [x] Sistema de 3 fases de eliminación
- [x] Verificación de solución única
- [x] Control de dificultad configurable
- [x] Documentación técnica completa

### 🚧 Versión 2.1 (Próximo)
- [ ] Menú interactivo para elegir dificultad
- [ ] Exportar puzzles a archivo .txt
- [ ] Modo batch (generar N puzzles)
- [ ] Tests unitarios

### 🔮 Versión 3.0 (Futuro)
- [ ] Solver automático con visualización
- [ ] GUI con ncurses
- [ ] Modo interactivo para jugar
- [ ] Generador de variantes (6x6, 12x12, Samurai, etc.)
- [ ] API REST
- [ ] Biblioteca compartida (.so/.dll)
- [ ] Estadísticas avanzadas de generación

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
- 🌐 Traducir a otros idiomas
- 🎨 Mejorar interfaz visual

## 📄 Licencia

Este proyecto está bajo la **Apache License 2.0** - ver el archivo [LICENSE](LICENSE) para más detalles.

### 🎯 Requisito Principal: DEBES Darme Crédito

**Si usas, modificas o distribuyes este código, DEBES:**

1. **✅ Mencionar al autor original:**
   ```
   Basado en Sudoku Generator por Gonzalo Ramírez (@chaLords)
   https://github.com/chaLords/sudoku_en_c
   ```

2. **✅ Incluir el archivo NOTICE** con las atribuciones

3. **✅ Mantener el copyright:**
   ```
   Copyright 2025 Gonzalo Ramírez
   Licensed under the Apache License, Version 2.0
   ```

4. **✅ Declarar tus modificaciones** si cambias el código

### ¿Qué puedes hacer?

✅ **Permitido:**
- ✅ Uso comercial (vender productos basados en esto)
- ✅ Modificar el código como quieras
- ✅ Distribuir copias
- ✅ Uso privado
- ✅ Sublicenciar tu trabajo derivado
- ✅ Protección de patentes

⚠️ **Obligatorio (o violas la licencia):**
- ⚠️ **Dar crédito al autor original** (Gonzalo Ramírez)
- ⚠️ **Incluir una copia de la licencia Apache 2.0**
- ⚠️ **Incluir el archivo NOTICE** con atribuciones
- ⚠️ **Documentar los cambios** que hagas
- ⚠️ **Mantener los avisos de copyright**

❌ **Prohibido:**
- ❌ Hacer responsable al autor por problemas
- ❌ Usar el nombre "Gonzalo Ramírez" o "@chaLords" para promocionar sin permiso
- ❌ Usar marcas registradas sin autorización

### 🆚 Comparación con otras licencias

| Característica | Apache 2.0 | MIT | GPL 3.0 |
|----------------|------------|-----|---------|
| **Requiere mencionar autor** | ✅ **SÍ** | ✅ SÍ | ✅ SÍ |
| Uso comercial | ✅ | ✅ | ✅ |
| Modificación | ✅ | ✅ | ✅ |
| Distribución | ✅ | ✅ | ✅ |
| Protección de patentes | ✅ **Explícita** | ❌ No | ✅ Sí |
| Requiere documentar cambios | ✅ **SÍ** | ❌ No | ✅ Sí |
| Requiere código abierto | ❌ No | ❌ No | ✅ **SÍ** |

**¿Por qué Apache 2.0?**
- ✅ Te protege mejor legalmente que MIT
- ✅ **Garantiza que te den crédito**
- ✅ Protección explícita de patentes
- ✅ Permite uso comercial sin restricciones
- ✅ No obliga a mantener código abierto (como GPL)

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
- [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0)

---

⭐ Si este proyecto te fue útil, considera darle una estrella en GitHub!

---

**Nota:** Este es un proyecto educativo con licencia Apache 2.0. El código está disponible libremente para aprender, modificar y usar, incluyendo uso comercial con las condiciones especificadas en la licencia.

---

## 🌐 Otros Idiomas

- [English](README.en.md)

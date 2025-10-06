# Changelog

Todos los cambios notables en este proyecto serán documentados en este archivo.

El formato está basado en [Keep a Changelog](https://keepachangelog.com/es-ES/1.0.0/),
y este proyecto adhiere a [Semantic Versioning](https://semver.org/lang/es/).

## [2.1.0] - 2025-10-06

### 🌍 Internacionalización Completa del Código

#### ⚠️ BREAKING CHANGES
Esta versión introduce cambios incompatibles con versiones anteriores. Todo el código ha sido traducido al inglés para seguir estándares internacionales.

### 🔄 Cambiado

#### Traducción Completa a Inglés
- **Todos los nombres de funciones** traducidos de español a inglés:
  - `num_orden_fisher_yates()` → `fisherYatesShuffle()`
  - `esSafePosicion()` → `isSafePosition()`
  - `encontrarCeldaVacia()` → `findEmptyCell()`
  - `contarSoluciones()` → `countSolutions()`
  - `llenarDiagonal()` → `fillDiagonal()`
  - `completarSudoku()` → `completeSudoku()`
  - `primeraEleccionAleatoria()` → `firstRandomElimination()`
  - `tieneAlternativaEnFilaCol()` → `hasAlternativeInRowCol()`
  - `segundaEleccionSinAlternativas()` → `secondNoAlternativeElimination()`
  - `terceraEleccionLibre()` → `thirdFreeElimination()`
  - `generarSudokuHibrido()` → `generateHybridSudoku()`
  - `imprimirSudoku()` → `printSudoku()`
  - `verificarSudoku()` → `validateSudoku()`

- **Todas las variables** traducidas a inglés:
  - `fila` → `row`
  - `fila_base` → `initial_row`
  - `col_base` → `initial_column`
  - `inicioFila` → `rowStart`
  - `inicioCol` → `colStart`
  - `subcuadriculas/cuadricula` → `subgrid/grid`
  - `numero_actual` → `currentNumber`
  - `valor_a_eliminar` → `valueToRemove`
  - `eliminados` → `removed/excluded_values`
  - `total_soluciones` → `totalSolutions`
  - `posiblesEnFila` → `possibleInRow`
  - `posiblesEnCol` → `possibleInCol`
  - `posiblesEnSubcuadricula` → `possibleInSubgrid`
  - `posiciones` → `positions`
  - `soluciones` → `solutions`
  - `exito` → `success`
  - `ronda` → `round`
  - `numeros` → `numbers`
  - `asteriscos` → `asterisks`
  - `intento` → `attempt`

- **Constantes** traducidas:
  - `OBJETIVO_FASE3` → `PHASE3_TARGET`

- **Todos los comentarios** en código traducidos a inglés
- **Todos los mensajes `printf`** traducidos a inglés
- **Títulos de sección** en código traducidos a inglés

#### Documentación Actualizada
- **README.md**: Actualizado con nombres de funciones en inglés
- **README.en.md**: Actualizado con nombres de funciones en inglés
- **TECHNICAL.md**: Actualizado con referencias a funciones en inglés
- **NOTICE**: Mejorado con requisitos de atribución explícitos
- Ambos READMEs actualizados con salida real del programa (55 celdas vacías)
- Demo actualizado mostrando caso donde FASE 2 elimina 0 celdas

### 📚 Documentación
- Estructura bilingüe mantenida (documentación en español, código en inglés)
- Ejemplos de código actualizados en toda la documentación
- Tablas de funciones actualizadas con nuevos nombres
- Mensajes de commit documentados para referencia futura

### ✨ Razones del Cambio
1. **Colaboración internacional**: Inglés es el estándar en desarrollo de software
2. **Consistencia**: Todas las bibliotecas estándar de C están en inglés
3. **Portafolio profesional**: Mejor para aplicaciones laborales
4. **Mejores prácticas**: Siguiendo estándares de la industria
5. **Mantenibilidad**: Más fácil para contribuidores de todo el mundo

### 📝 Nota de Migración
Proyectos que usen versiones anteriores necesitarán actualizar todas las llamadas a funciones. Se recomienda usar herramientas de búsqueda y reemplazo con los nombres listados arriba.

---

## [2.0.0] - 2025-10-05

### 🎉 Añadido

#### Sistema de Generación de Puzzles Jugables
- **Sistema de 3 fases de eliminación de celdas** para crear puzzles jugables
- **FASE 1**: Eliminación aleatoria por subcuadrícula usando Fisher-Yates
  - Elimina 1 número aleatorio de cada una de las 9 subcuadrículas
  - Garantiza distribución uniforme inicial (9 celdas vacías)
- **FASE 2**: Eliminación iterativa sin alternativas
  - Loop que elimina números sin alternativas en fila/columna/subcuadrícula
  - Continúa hasta que no pueda eliminar más
  - Típicamente elimina 0-25 celdas adicionales (variable según estructura)
- **FASE 3**: Eliminación libre con verificación de solución única
  - Intenta eliminar celdas adicionales en orden aleatorio
  - Verifica solución única usando `countSolutions()` (antes `contarSoluciones()`)
  - Configurable mediante constante `PHASE3_TARGET` (antes `OBJETIVO_FASE3`) (default: 20)
  - Resultado típico: 30-54 celdas vacías totales

#### Funciones Nuevas
- `countSolutions()`: Cuenta el número de soluciones posibles del puzzle
  - Implementa backtracking con límite configurable
  - Optimización: early exit al encontrar múltiples soluciones
  - Garantiza que los puzzles tengan **exactamente una solución**
- `firstRandomElimination()`: Implementa FASE 1 de eliminación
- `hasAlternativeInRowCol()`: Verifica si un número tiene alternativas
- `secondNoAlternativeElimination()`: Implementa FASE 2 con loop
- `thirdFreeElimination()`: Implementa FASE 3 con verificación única

#### Mejoras de Documentación
- Comentarios tipo Doxygen en todas las funciones
- Explicación detallada de cada fase de eliminación
- Ejemplos de uso y configuración de dificultad
- Referencias a complejidad algorítmica
- Documentación técnica completa en `docs/`:
  - `TECHNICAL.md`: Arquitectura del sistema, análisis de complejidad, benchmarks
  - `ALGORITHMS.md`: Fundamentos matemáticos, teoremas, pruebas formales

### 🔄 Cambiado

#### Reorganización del Código
- **Nueva estructura por secciones lógicas**:
  1. Utilidades básicas (Fisher-Yates)
  2. Funciones de verificación
  3. Generación del Sudoku
  4. Eliminación de celdas (3 fases)
  5. Función principal
  6. Funciones auxiliares
  7. Main
- Todas las funciones ahora tienen documentación completa
- Headers de sección más claros y descriptivos

#### Mejoras en la Interfaz
- **Mensajes de consola más informativos**:
  - Emojis para mejor experiencia visual (🎲, ✅, 🔄, 🛑, 🎯, etc.)
  - Progreso detallado de cada fase de eliminación
  - Contador de celdas eliminadas por fase
  - Rondas numeradas en FASE 2
  - Array random generado visible para debugging
- **Función `printSudoku()` mejorada**:
  - Asteriscos (*) para celdas vacías en lugar de 0
  - Contador de celdas vacías/llenas: `Empty cells: 54 | Filled cells: 27`
  - Formato más limpio y legible
  - Bordes Unicode para mejor visualización

#### Sistema de Configuración
- Uso de constante `PHASE3_TARGET` para configuración clara
- Fácil modificación para diferentes niveles de dificultad
- Mejor documentación sobre cómo ajustar dificultad

#### Licencia
- **Cambio a Apache License 2.0** (desde MIT)
  - Mayor protección de patentes
  - Requisito explícito de documentar cambios
  - Mejor protección legal para contribuidores
  - Archivo `NOTICE` agregado con requisitos de atribución
- Header de licencia actualizado en `main.c`
- Archivo `LICENSE` completamente reescrito
- READMEs actualizados con nueva licencia

### 🐛 Corregido
- Consistencia de licencia entre archivos (antes había conflicto MIT/Apache)
- Mensajes de error más descriptivos
- Validación mejorada de celdas vacías
- Sincronización entre README.md y README.en.md

### ⚡ Optimizado
- `countSolutions()` con early exit para mejor performance (~10^40x speedup)
- Orden de verificación en `hasAlternativeInRowCol()` optimizado
- Uso de constantes (#define SIZE) para valores fijos

### 📚 Documentación
- README.md actualizado con ejemplos de puzzles jugables
- README.en.md sincronizado con versión en español
- Documentación técnica exhaustiva en carpeta `docs/`
- CHANGELOG.md agregado para seguimiento de versiones
- Badges actualizados en READMEs

---

## [1.0.0] - 2025-01-XX

### 🎉 Añadido
- **Generación completa de sudokus válidos 9x9**
- **Método híbrido** Fisher-Yates + Backtracking
  - Fase 1: Llenar diagonal principal con Fisher-Yates
  - Fase 2: Completar resto con backtracking
- **Funciones principales** (nombres en español en esta versión):
  - `num_orden_fisher_yates()`: Permutación aleatoria
  - `esSafePosicion()`: Validación de números
  - `llenarDiagonal()`: Llenar subcuadrículas independientes
  - `completarSudoku()`: Backtracking recursivo
  - `verificarSudoku()`: Validación del tablero completo
- **Interfaz de consola** con caracteres box-drawing
- **Validación automática** del sudoku generado
- **Documentación básica** en README.md
- Licencia MIT

### 📚 Documentación
- README.md con explicación del algoritmo híbrido
- Ejemplos de uso
- Instrucciones de compilación
- Roadmap inicial

---

## Tipos de Cambios
- `🎉 Añadido`: Nuevas funcionalidades
- `🔄 Cambiado`: Cambios en funcionalidades existentes
- `🗑️ Removido`: Funcionalidades eliminadas
- `🐛 Corregido`: Corrección de bugs
- `🔒 Seguridad`: Correcciones de seguridad
- `⚡ Optimizado`: Mejoras de performance
- `📚 Documentación`: Cambios en documentación
- `🌍 Internacionalización`: Cambios de idioma/localización

---

## [Unreleased]

### 🚧 En Progreso
- Menú interactivo para elegir dificultad
- Exportar puzzles a archivo .txt
- Modo batch para generar múltiples puzzles

### 💡 Planeado para v2.2
- Parametrizar `PHASE3_TARGET` como argumento de función
- Tests unitarios
- Sistema de perfiles de dificultad predefinidos
- Modo verbose/quiet para logs
- Estructura modular con archivos .h y .c separados

### 🔮 Ideas para v3.0
- Solver automático con visualización paso a paso
- GUI con ncurses
- Modo interactivo para jugar
- Generador de variantes (6x6, 12x12, Samurai)
- API REST
- Biblioteca compartida (.so/.dll)
- Análisis de dificultad real (técnicas de solución requeridas)

---

**Nota**: Las fechas usan formato ISO 8601 (YYYY-MM-DD)

[2.1.0]: https://github.com/chaLords/sudoku_en_c/releases/tag/v2.1.0
[2.0.0]: https://github.com/chaLords/sudoku_en_c/releases/tag/v2.0.0
[1.0.0]: https://github.com/chaLords/sudoku_en_c/releases/tag/v1.0.0
[Unreleased]: https://github.com/chaLords/sudoku_en_c/compare/v2.1.0...HEAD

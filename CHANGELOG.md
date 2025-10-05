# Changelog

Todos los cambios notables en este proyecto serán documentados en este archivo.

El formato está basado en [Keep a Changelog](https://keepachangelog.com/es-ES/1.0.0/),
y este proyecto adhiere a [Semantic Versioning](https://semver.org/lang/es/).

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
  - Intenta eliminar celdas adicionales en orden secuencial
  - Verifica solución única usando `contarSoluciones()`
  - Configurable mediante variable `objetivo_adicional` dentro de `generarSudokuHibrido()` (default: 20)
  - Resultado típico: 40-65 celdas vacías totales

#### Funciones Nuevas
- `contarSoluciones()`: Cuenta el número de soluciones posibles del puzzle
  - Implementa backtracking con límite configurable
  - Optimización: early exit al encontrar múltiples soluciones
  - Garantiza que los puzzles tengan **exactamente una solución**
- `primeraEleccionAleatoria()`: Implementa FASE 1 de eliminación
- `tieneAlternativaEnFilaCol()`: Verifica si un número tiene alternativas
- `segundaEleccionSinAlternativas()`: Implementa FASE 2 con loop
- `terceraEleccionLibre()`: Implementa FASE 3 con verificación única

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
- **Función `imprimirSudoku()` mejorada**:
  - Asteriscos (*) para celdas vacías en lugar de 0
  - Contador de celdas vacías/llenas: `Celdas vacías: 54 | Celdas llenas: 27`
  - Formato más limpio y legible
  - Bordes Unicode para mejor visualización

#### Sistema de Configuración
- Uso de variable local `objetivo_adicional` en lugar de constante global
- Mayor flexibilidad para futuras extensiones (selección dinámica de dificultad)
- Mejor encapsulación del parámetro de configuración

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
- `contarSoluciones()` con early exit para mejor performance (~10^40x speedup)
- Orden de verificación en `tieneAlternativaEnFilaCol()` optimizado
- Uso de constantes (#define SIZE) para valores fijos
- Variable local para configuración flexible de dificultad

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
- **Funciones principales**:
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

---

## [Unreleased]

### 🚧 En Progreso
- Menú interactivo para elegir dificultad
- Exportar puzzles a archivo .txt
- Modo batch para generar múltiples puzzles

### 💡 Planeado para v2.1
- Parametrizar `objetivo_adicional` como argumento de función
- Tests unitarios
- Sistema de perfiles de dificultad predefinidos
- Modo verbose/quiet para logs

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

[2.0.0]: https://github.com/chaLords/sudoku_en_c/releases/tag/v2.0.0
[1.0.0]: https://github.com/chaLords/sudoku_en_c/releases/tag/v1.0.0
[Unreleased]: https://github.com/chaLords/sudoku_en_c/compare/v2.0.0...HEAD

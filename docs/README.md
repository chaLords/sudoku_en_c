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

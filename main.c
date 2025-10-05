#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

/*
 * Sudoku Generator - Hybrid Fisher-Yates + Backtracking
 * 
 * Copyright 2025 Gonzalo Ramírez (@chaLords)
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define SIZE 9
#define OBJETIVO_FASE3 20  // Constante para fase 3

// ═══════════════════════════════════════════════════════════════════
//                    1. UTILIDADES BÁSICAS
// ═══════════════════════════════════════════════════════════════════

/**
 * Genera una permutación aleatoria usando Fisher-Yates
 * @param array: Array a llenar
 * @param size: Tamaño del array
 * @param num_in: Número inicial (usualmente 1)
 */
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

// ═══════════════════════════════════════════════════════════════════
//                    2. FUNCIONES DE VERIFICACIÓN
// ═══════════════════════════════════════════════════════════════════

/**
 * Verifica si es seguro colocar un número en una posición
 * @return true si es válido, false si hay conflicto
 */
bool esSafePosicion(int sudoku[SIZE][SIZE], int fila, int col, int num) {
    // Verificar fila
    for(int x = 0; x < SIZE; x++) {
        if(sudoku[fila][x] == num) return false;
    }
    
    // Verificar columna
    for(int x = 0; x < SIZE; x++) {
        if(sudoku[x][col] == num) return false;
    }
    
    // Verificar subcuadrícula 3x3
    int inicioFila = fila - fila % 3;
    int inicioCol = col - col % 3;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(sudoku[i + inicioFila][j + inicioCol] == num) return false;
        }
    }
    
    return true;
}

/**
 * Encuentra la siguiente celda vacía (con valor 0)
 * @param fila: Puntero para guardar fila encontrada
 * @param col: Puntero para guardar columna encontrada
 * @return true si encuentra celda vacía, false si está completo
 */
bool encontrarCeldaVacia(int sudoku[SIZE][SIZE], int *fila, int *col) {
    for(*fila = 0; *fila < SIZE; (*fila)++) {
        for(*col = 0; *col < SIZE; (*col)++) {
            if(sudoku[*fila][*col] == 0) return true;
        }
    }
    return false;
}

/**
 * Cuenta el número de soluciones posibles del sudoku
 * @param limite: Límite de soluciones a buscar (usualmente 2)
 * @return Número de soluciones encontradas
 */
int contarSoluciones(int sudoku[SIZE][SIZE], int limite) {
    int fila, col;
    
    // Si no hay celdas vacías, encontramos una solución
    if(!encontrarCeldaVacia(sudoku, &fila, &col)) {
        return 1;
    }
    
    int total_soluciones = 0;
    
    // Probar números del 1-9
    for(int num = 1; num <= 9; num++) {
        if(esSafePosicion(sudoku, fila, col, num)) {
            sudoku[fila][col] = num;
            
            // Acumula soluciones encontradas
            total_soluciones += contarSoluciones(sudoku, limite);
            
            // Optimización: si ya encontramos 2+, no seguir buscando
            if(total_soluciones >= limite) {
                sudoku[fila][col] = 0;
                return total_soluciones;
            }
            
            sudoku[fila][col] = 0; // Backtrack
        }
    }
    
    return total_soluciones;
}

// ═══════════════════════════════════════════════════════════════════
//                    3. GENERACIÓN DEL SUDOKU
// ═══════════════════════════════════════════════════════════════════

/**
 * Llena las subcuadrículas de la diagonal principal (0, 4, 8)
 * Estas no interfieren entre sí, permitiendo llenado independiente
 */
void llenarDiagonal(int sudoku[SIZE][SIZE]) {
    printf("🎲 Llenando diagonal principal con Fisher-Yates...\n");
    
    int subcuadriculas[] = {0, 4, 8};
    
    for(int idx = 0; idx < 3; idx++) {
        int cuadricula = subcuadriculas[idx];
        int random[SIZE];
        num_orden_fisher_yates(random, SIZE, 1);
        
        int fila_base = (cuadricula/3) * 3;
        int col_base = (cuadricula%3) * 3;
        
        printf("   Subcuadrícula %d (base: %d,%d): ", cuadricula, fila_base, col_base);
        
        for(int i = 0; i < SIZE; i++) {
            int fila = fila_base + (i/3);
            int col = col_base + (i%3);
            sudoku[fila][col] = random[i];
            printf("%d ", random[i]);
        }
        printf("\n");
    }
    printf("✅ Diagonal completada!\n\n");
}

/**
 * Completa el sudoku usando backtracking recursivo
 * @return true si logra completar, false si no hay solución
 */
bool completarSudoku(int sudoku[SIZE][SIZE]) {
    int fila, col;
    
    if(!encontrarCeldaVacia(sudoku, &fila, &col)) {
        return true; // ¡Completado!
    }
    
    // Números aleatorios para variedad
    int numeros[9] = {1,2,3,4,5,6,7,8,9};
    
    // Mezclar para variedad
    for(int i = 8; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = numeros[i];
        numeros[i] = numeros[j];
        numeros[j] = temp;
    }
    
    // Probar cada número
    for(int i = 0; i < 9; i++) {
        int num = numeros[i];
        
        if(esSafePosicion(sudoku, fila, col, num)) {
            sudoku[fila][col] = num;
            
            if(completarSudoku(sudoku)) {
                return true;
            }
            
            sudoku[fila][col] = 0; // Backtrack
        }
    }
    
    return false;
}

// ═══════════════════════════════════════════════════════════════════
//                    4. ELIMINACIÓN DE CELDAS (3 FASES)
// ═══════════════════════════════════════════════════════════════════

/**
 * FASE 1: Elimina un número aleatorio por cada subcuadrícula
 * Usa Fisher-Yates para elegir qué número (1-9) eliminar en cada una
 */
void eleccionNumerosSubCuadriculas(int sudoku[SIZE][SIZE]) {
    printf("🎲 FASE 1: Eligiendo números por subcuadrículas con Fisher-Yates...\n");
    
    int subcuadriculas[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    int random[SIZE];
    num_orden_fisher_yates(random, SIZE, 1);
    
    for(int idx = 0; idx < 9; idx++) {
        int cuadricula = subcuadriculas[idx];
        int fila_base = (cuadricula/3) * 3;
        int col_base = (cuadricula%3) * 3;
        
        printf("   Subcuadrícula %d (base: %d,%d): ", cuadricula, fila_base, col_base);
        
        int valor_a_eliminar = random[idx];
        
        for(int i = 0; i < SIZE; i++) {
            int fila = fila_base + (i/3);
            int col = col_base + (i%3);
            
            if(sudoku[fila][col] == valor_a_eliminar) {
                sudoku[fila][col] = 0;
                printf("%d ", valor_a_eliminar);
                break;
            }
        }
        printf("\n");
    }
    printf("✅ Fase 1 completada!\n\n");
}

/**
 * Verifica si un número tiene alternativas en su fila, columna o subcuadrícula
 * @return true si HAY alternativas (no se debe eliminar), false si NO HAY (se puede eliminar)
 */
bool tieneAlternativaEnFilaCol(int sudoku[SIZE][SIZE], int fila, int col, int num) {
    int temp = sudoku[fila][col];
    sudoku[fila][col] = 0;
    
    int posiblesEnFila = 0;
    int posiblesEnCol = 0;
    int posiblesEnSubcuadricula = 0;
    
    // Verificar otra posición en la FILA
    for(int x = 0; x < SIZE; x++) {
        if(x != col && sudoku[fila][x] == 0) {
            if(esSafePosicion(sudoku, fila, x, num)) {
                posiblesEnFila++;
            }
        }
    }
    
    // Verificar otra posición en la COLUMNA
    for(int x = 0; x < SIZE; x++) {
        if(x != fila && sudoku[x][col] == 0) {
            if(esSafePosicion(sudoku, x, col, num)) {
                posiblesEnCol++;
            }
        }
    }
    
    // Verificar otra posición en la SUBCUADRÍCULA 3x3
    int inicioFila = fila - fila % 3;
    int inicioCol = col - col % 3;
    
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            int f = inicioFila + i;
            int c = inicioCol + j;
            
            // Saltar la posición original
            if(f == fila && c == col) continue;
            
            if(sudoku[f][c] == 0 && esSafePosicion(sudoku, f, c, num)) {
                posiblesEnSubcuadricula++;
            }
        }
    }
    
    sudoku[fila][col] = temp; // Restaurar
    
    return (posiblesEnFila > 0) || (posiblesEnCol > 0) || (posiblesEnSubcuadricula > 0);
}

/**
 * FASE 2: Elimina números que NO tienen alternativas en su fila/columna/subcuadrícula
 * Se ejecuta en loop hasta que no pueda eliminar más
 * @return Cantidad de números eliminados en esta ronda
 */
int segundaEleccionNumerosSubCuadriculas(int sudoku[SIZE][SIZE]) {
    printf("🎲 FASE 2: Eligiendo números sin alternativas...\n");
    
    int subcuadriculas[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    int eliminados = 0;
    
    for(int idx = 0; idx < 9; idx++) {
        int cuadricula = subcuadriculas[idx];
        int fila_base = (cuadricula/3) * 3;
        int col_base = (cuadricula%3) * 3;
        
        printf("   Subcuadrícula %d (base: %d,%d): ", cuadricula, fila_base, col_base);
        
        for(int i = 0; i < SIZE; i++) {
            int fila = fila_base + (i/3);
            int col = col_base + (i%3);
            
            if(sudoku[fila][col] != 0) {
                int numero_actual = sudoku[fila][col];
                
                if(!tieneAlternativaEnFilaCol(sudoku, fila, col, numero_actual)) {
                    sudoku[fila][col] = 0;
                    printf("%d ", numero_actual);
                    eliminados++;
                    break;
                }
            }
        }
        printf("\n");
    }
    
    printf("✅ Fase 2 completada! Eliminados: %d\n\n", eliminados);
    return eliminados;
}

/**
 * FASE 3: Eliminación libre hasta alcanzar objetivo
 * Verifica que mantenga solución única usando contarSoluciones()
 * @param objetivo: Cantidad adicional de celdas a vaciar
 * @return Cantidad de números eliminados
 */
int terceraEleccionLibre(int sudoku[SIZE][SIZE], int objetivo) {
    printf("🎲 FASE 3: Eliminación libre con verificación de solución única...\n");
    
    // Estructura para almacenar posiciones
    typedef struct {
        int fila;
        int col;
    } Posicion;
    
    Posicion posiciones[81];
    int count = 0;
    
    // Recolectar todas las posiciones con números
    for(int f = 0; f < SIZE; f++) {
        for(int c = 0; c < SIZE; c++) {
            if(sudoku[f][c] != 0) {
                posiciones[count].fila = f;
                posiciones[count].col = c;
                count++;
            }
        }
    }
    
    // Barajear posiciones (Fisher-Yates)
    for(int i = count-1; i > 0; i--) {
        int j = rand() % (i + 1);
        Posicion temp = posiciones[i];
        posiciones[i] = posiciones[j];
        posiciones[j] = temp;
    }
    
    // Intentar eliminar en orden aleatorio
    int eliminados = 0;
    for(int i = 0; i < count && eliminados < objetivo; i++) {
        int fila = posiciones[i].fila;
        int col = posiciones[i].col;
        
        int temp = sudoku[fila][col];
        sudoku[fila][col] = 0;
        
        // Verificar que mantenga solución única
        int soluciones = contarSoluciones(sudoku, 2);
        
        if(soluciones == 1) {
            eliminados++;
            printf("   Eliminado %d en (%d,%d) - Total: %d\n", 
                   temp, fila, col, eliminados);
        } else {
            sudoku[fila][col] = temp; // Restaurar si hay múltiples soluciones
        }
    }
    
    printf("✅ Fase 3 completada! Eliminados: %d\n\n", eliminados);
    return eliminados;
}

// ═══════════════════════════════════════════════════════════════════
//                    5. FUNCIÓN PRINCIPAL
// ═══════════════════════════════════════════════════════════════════

/**
 * Genera un sudoku jugable usando método híbrido + eliminación de celdas
 * @return true si genera exitosamente, false en caso de error
 */
bool generarSudokuHibrido(int sudoku[SIZE][SIZE]) {
    // Inicializar todas las celdas con 0
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            sudoku[i][j] = 0;
        }
    }
    
    // PASO 1: Llenar diagonal principal con Fisher-Yates
    llenarDiagonal(sudoku);
    
    // PASO 2: Completar con backtracking
    printf("🔄 Completando con backtracking...\n");
    bool exito = completarSudoku(sudoku);
    
    if(exito) {
        // PASO 3: FASE 1 - Eliminar 1 por subcuadrícula
        eleccionNumerosSubCuadriculas(sudoku);
        
        // PASO 4: FASE 2 - Loop de eliminación sin alternativas
        int ronda = 1;
        int eliminados;
        do {
            printf("--- RONDA %d ---\n", ronda);
            eliminados = segundaEleccionNumerosSubCuadriculas(sudoku);
            ronda++;
        } while(eliminados > 0);
        
        printf("🛑 No se pueden eliminar más números en FASE 2\n\n");
        
        // PASO 5: FASE 3 - Eliminación libre hasta objetivo
        terceraEleccionLibre(sudoku, OBJETIVO_FASE3);
    }
    
    return exito;
}

// ═══════════════════════════════════════════════════════════════════
//                    6. FUNCIONES AUXILIARES
// ═══════════════════════════════════════════════════════════════════

/**
 * Imprime el sudoku en formato visual con bordes
 * Muestra asteriscos (*) para celdas vacías
 */
void imprimirSudoku(int sudoku[SIZE][SIZE]) {
    int asteriscos = 0;
    
    printf("┌───────┬───────┬───────┐\n");
    for(int i = 0; i < SIZE; i++) {
        printf("│");
        for(int j = 0; j < SIZE; j++) {
            if(sudoku[i][j] == 0) {
                printf(" *");
                asteriscos++;
            } else {
                printf(" %d", sudoku[i][j]);
            }
            if((j + 1) % 3 == 0) printf(" │");
        }
        printf("\n");
        if((i + 1) % 3 == 0 && i < 8) {
            printf("├───────┼───────┼───────┤\n");
        }
    }
    printf("└───────┴───────┴───────┘\n");
    printf("📊 Celdas vacías: %d | Celdas llenas: %d\n", asteriscos, 81 - asteriscos);
}

/**
 * Verifica que el sudoku sea válido (sin conflictos)
 * @return true si es válido, false si hay errores
 */
bool verificarSudoku(int sudoku[SIZE][SIZE]) {
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            if(sudoku[i][j] == 0) continue; // Saltar celdas vacías
            
            int num = sudoku[i][j];
            sudoku[i][j] = 0; // Temporal
            
            if(!esSafePosicion(sudoku, i, j, num)) {
                sudoku[i][j] = num; // Restaurar
                return false;
            }
            
            sudoku[i][j] = num; // Restaurar
        }
    }
    return true;
}

// ═══════════════════════════════════════════════════════════════════
//                           7. MAIN
// ═══════════════════════════════════════════════════════════════════

int main() {
    // Inicializar semilla aleatoria
    srand(time(NULL));
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("        GENERADOR DE SUDOKU v2.0.0 - MÉTODO HÍBRIDO\n");
    printf("           Fisher-Yates + Backtracking + 3 Fases\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    int sudoku[SIZE][SIZE];
    
    // Generar sudoku
    for(int intento = 1; intento <= 5; intento++) {
        printf("🚀 INTENTO #%d:\n", intento);
        
        if(generarSudokuHibrido(sudoku)) {
            printf("✅ ¡ÉXITO! Sudoku generado\n\n");
            imprimirSudoku(sudoku);
            printf("\n");
            
            if(verificarSudoku(sudoku)) {
                printf("🎉 ¡VERIFICADO! El puzzle es válido\n");
            } else {
                printf("❌ Error en verificación\n");
            }
            break;
        } else {
            printf("❌ Falló (muy raro con este método)\n\n");
        }
    }
    
    return 0;
}

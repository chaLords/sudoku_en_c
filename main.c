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

// ═══════════════════════════════════════════════════════════════════
//                    FUNCIÓN FISHER-YATES
// ═══════════════════════════════════════════════════════════════════

void num_orden_fisher_yates(int *array, int size, int num_in) {
    for(int i = 0; i < size; i++) {
        array[i] = num_in + i;
    }
    for(int i = size-1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// ═══════════════════════════════════════════════════════════════════
//                    FUNCIONES DE VERIFICACIÓN
// ═══════════════════════════════════════════════════════════════════

bool esSafePosicion(int sudoku[SIZE][SIZE], int fila, int col, int num) {
    // Verificación de fila si se repite algun numero...si se repite marca false...
    for(int x = 0; x < SIZE; x++) {
        if(sudoku[fila][x] == num) return false;
    }
    
    // Verificación de columna se repite algun número, si se repite marca false...
    for(int x = 0; x < SIZE; x++) {
        if(sudoku[x][col] == num) return false;
    }
    
    // Verificación de subcuadrícula 3x3 , si se repite algun número...
    int inicioFila = fila - fila % 3;
    int inicioCol = col - col % 3;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(sudoku[i + inicioFila][j + inicioCol] == num) return false;
        }
    }
    
    return true;
}

bool encontrarCeldaVacia(int sudoku[SIZE][SIZE], int *fila, int *col) {
    for(*fila = 0; *fila < SIZE; (*fila)++) {
        for(*col = 0; *col < SIZE; (*col)++) {
            if(sudoku[*fila][*col] == 0) return true;
        }
    }
    return false;
}

// ═══════════════════════════════════════════════════════════════════
//                    MÉTODO HÍBRIDO RECOMENDADO
// ═══════════════════════════════════════════════════════════════════

// PASO 1: Llenar diagonal principal con método Fisher-Yates...
void llenarDiagonal(int sudoku[SIZE][SIZE]) {
    printf("🎲 Llenando diagonal principal con Fisher-Yates...\n");
    
    // Subcuadrículas 0, 4, 8 (no interfieren entre sí)
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

// PASO 2: Backtracking para completar el resto
bool completarSudoku(int sudoku[SIZE][SIZE]) {
    int fila, col;
    
    if(!encontrarCeldaVacia(sudoku, &fila, &col)) {
        return true; // ¡Completado!
    }
    
    // Números aleatorios para variedad
    int numeros[9] = {1,2,3,4,5,6,7,8,9};
    
    // Mezclar para variedad (opcional)
    for(int i = 8; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = numeros[i];
        numeros[i] = numeros[j];
        numeros[j] = temp;
    }
   //prueba cada numero... 
    for(int i = 0; i < 9; i++) {
        int num = numeros[i];
       //5. pregunta: "Puedo poner num en sudoku"??
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

// FUNCIÓN PRINCIPAL: Método híbrido completo
bool generarSudokuHibrido(int sudoku[SIZE][SIZE]) {
    
  //Iniciar todas las celdas con 0...
    for(int i=0; i<SIZE; i++){
      for(int j=0; j<SIZE; j++){
        sudoku[i][j] = 0;
    }
  }
    // PASO 1: Método para diagonal (100% confiable)
    llenarDiagonal(sudoku);
    
    // PASO 2: Backtracking para el resto (100% confiable)
    printf("🔄 Completando con backtracking...\n");
    return completarSudoku(sudoku);
}

// ═══════════════════════════════════════════════════════════════════
//                    FUNCIONES AUXILIARES
// ═══════════════════════════════════════════════════════════════════

void imprimirSudoku(int sudoku[SIZE][SIZE]) {
    printf("┌───────┬───────┬───────┐\n");
    for(int i = 0; i < SIZE; i++) {
        printf("│");
        for(int j = 0; j < SIZE; j++) {
            printf(" %d", sudoku[i][j]);
            if((j + 1) % 3 == 0) printf(" │");
        }
        printf("\n");
        if((i + 1) % 3 == 0 && i < 8) {
            printf("├───────┼───────┼───────┤\n");
        }
    }
    printf("└───────┴───────┴───────┘\n");
}

bool verificarSudoku(int sudoku[SIZE][SIZE]) {
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            if(sudoku[i][j] == 0) continue; //<-- SALTA CELDAS VACÍAS

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
//                           MAIN
// ═══════════════════════════════════════════════════════════════════

int main() {
    srand(time(NULL));
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("              MÉTODO HÍBRIDO RECOMENDADO\n");
    printf("          Tu Fisher-Yates + Backtracking\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    //Inicia el array AQUÍ!!!! 
    int sudoku[SIZE][SIZE];
    
    // Probar varias generaciones
    for(int intento = 1; intento <= 5; intento++) {
        printf("🚀 INTENTO #%d:\n", intento);
        
        if(generarSudokuHibrido(sudoku)) {
            printf("✅ ¡ÉXITO! Sudoku generado\n");
            imprimirSudoku(sudoku);
            
            if(verificarSudoku(sudoku)) {
                printf("🎉 ¡VERIFICADO! El Sudoku es válido\n");
            } else {
                printf("❌ Error en verificación\n");
            }
            break;
        } else {
            printf("❌ Falló (muy raro con este método)\n");
        }
        
        printf("\n");
    }
    
    return 0;
}

/*
VENTAJAS DEL MÉTODO HÍBRIDO:

1. **Aprovecha Fisher-Yates**: Para subcuadrículas independientes
2. **Usa backtracking**: Solo donde es necesario  
3. **Alta probabilidad de éxito**: ~99.9%
4. **Eficiente**: Más rápido que backtracking puro
5. **Escalable**: Fácil de expandir para generar puzzles

PRÓXIMOS PASOS:

1. Implementar código base
2. Pruébalo y ajústalo  
3. Agrega eliminación de celdas para crear puzzles
4. Implementa verificación de solución única
5. Crea interface para diferentes dificultades

*/

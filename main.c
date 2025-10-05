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
    // Verificación de fila si se repite algun numero, false...
    for(int x = 0; x < SIZE; x++) {
        if(sudoku[fila][x] == num) return false;
    }
    
    // Verificación de columna se repite algun número, false...
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
// ═══════════════════════════════════════════════════════════════════
//                    MÉTODOS ELIMINACIÓN CELDAS
// ═══════════════════════════════════════════════════════════════════

void eleccionNumerosSubCuadriculas(int sudoku[SIZE][SIZE]){

    printf("🎲 FASE 1: Eligiendo números por subcuadrículas con Fisher-Yates...\n");

    int subcuadriculas[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    int random[SIZE];
    num_orden_fisher_yates(random, SIZE, 1);

    // Agregar esta línea de debug:
    printf("Array random generado: ");
    for(int i = 0; i < 9; i++) printf("%d ", random[i]);
    printf("\n\n");



    for(int idx = 0; idx < 9; idx++) {
        int cuadricula = subcuadriculas[idx];
        
        int fila_base = (cuadricula/3) * 3;
        int col_base = (cuadricula%3) * 3;
        
        
        
        printf("   Subcuadrícula %d (base: %d,%d): ", cuadricula, fila_base, col_base);
        
          for(int i = 0; i < SIZE; i++) {
            int fila = fila_base + (i/3);
            int col = col_base + (i%3);
            int valor_a_eliminar = random[idx];

            if(sudoku[fila][col] == valor_a_eliminar){
              sudoku[fila][col] = 0;
              printf("%d ", valor_a_eliminar);
              break;
            }
            
          }
        printf("\n");
    }
    printf("✅ Elección completada!\n\n");

}

bool tieneAlternativaEnFilaCol(int sudoku[SIZE][SIZE], int fila, int col, int num) {


    int temp = sudoku[fila][col];
    sudoku[fila][col] = 0;
    int posiblesEnFilas = 0;
    int posiblesEnCol = 0;
    int posiblesEnSubcuadricula = 0;
  
    // Verificación otra posición en la FILA
    for(int x = 0; x < SIZE; x++) {
        if(x != col && sudoku[fila][x] == 0){
            if(esSafePosicion(sudoku, fila, x, num)){
              posiblesEnFilas++;//Contar, no retornar inmediatamente

           }  
        }
    }
    //Verificación otra posición en la COLUMNA
    for(int x = 0; x < SIZE; x++){
        if(x != fila && sudoku[x][col] == 0){
            if(esSafePosicion(sudoku, x, col, num)){
              posiblesEnCol++;
            }
        }

    }
    //Verificación otra posición en las SUBCUADRÍCULAS 3X3
    int inicioFila = fila - fila % 3;
    int inicioCol = col - col % 3;
    
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            int f = inicioFila + i;
            int c = inicioCol + j;
            
            //Saltar la posición original
            if(f == fila && c == col) continue;

            //Si no es la posición original y está vacías
            if(sudoku[f][c] == 0 && esSafePosicion(sudoku, f, c, num)){
                    posiblesEnSubcuadricula++;
                
            }
        }
    }
  
    sudoku[fila][col] = temp; //Restaurar...
    return (posiblesEnFilas > 0) || (posiblesEnCol > 0) || (posiblesEnSubcuadricula > 0); //No Existen Alternativas se puede quitar....

}

int segundaEleccionNumerosSubCuadriculas(int sudoku[SIZE][SIZE]){


    printf("🎲 FASE 2: Eligiendo números por subcuadrículas con Fisher-Yates...\n");

    int subcuadriculas[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    int eliminados = 0; //Contador


    for(int idx = 0; idx < 9; idx++) {
        int cuadricula = subcuadriculas[idx];
        int fila_base = (cuadricula/3) * 3;
        int col_base = (cuadricula%3) * 3;
        
        
        
        printf("   Subcuadrícula_2 %d (base: %d,%d): ", cuadricula, fila_base, col_base);
        
        for(int i = 0; i < SIZE; i++) {
            int fila = fila_base + (i/3);
            int col = col_base + (i%3);
            
            if(sudoku[fila][col] != 0){
              int numero_actual = sudoku[fila][col];


              if(!tieneAlternativaEnFilaCol(sudoku, fila, col, numero_actual)){
                sudoku[fila][col] = 0;
                printf("%d", numero_actual);//Imprime segunda corrida de números...
                eliminados++; //Incrementar contador
                break;
              }
            }
          }
          printf("\n");
    }
    printf("✅ Elección completada! Eliminados: %d\n\n", eliminados);
    return eliminados; //Retornar Cuántos eliminó

}

int terceraEleccionLibre(int sudoku[SIZE][SIZE], int num){

    printf("🎯 FASE 3: Eliminación libre hasta objetivo...\n");

    int eliminados = 0;
    int intentos = 0;
    int max_intentos = 200;

    while(eliminados < num && intentos < max_intentos){
        bool elimino_num = false;
        
        //Recorrer TODO el tablero
        for(int fila=0; fila<SIZE && eliminados < num; fila++){
          for(int col=0; col<SIZE && eliminados < num; col++){

            if(sudoku[fila][col] != 0){
                int num_actual = sudoku[fila][col];
                if(!tieneAlternativaEnFilaCol(sudoku, fila, col, num_actual)){
                    sudoku[fila][col] = 0;
                    eliminados++;
                    elimino_num = true;
                    printf("Eliminado %d en (%d,%d) - Total: %d\n", num_actual, fila, col, eliminados);
                }
            }
          }
        }
        if(!elimino_num) break; //Si una pasada completa no eliminó nada, salir...
        intentos++;
    }
    printf("✅ FASE 3 completada! Eliminados: %d\n\n", eliminados);
    return eliminados;
}
// ═══════════════════════════════════════════════════════════════════
//            FUNCIÓN PRINCIPAL: Método híbrido completo    
// ═══════════════════════════════════════════════════════════════════
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
    bool exito = completarSudoku(sudoku);
    if(exito){
        eleccionNumerosSubCuadriculas(sudoku);    
        // Loop FASE 2
        int ronda = 1;
        int eliminados;
        do {
        printf("--- RONDA %d ---\n", ronda);
        eliminados = segundaEleccionNumerosSubCuadriculas(sudoku);
        ronda++;
        } while(eliminados > 0);  // Continuar mientras elimine algo    
      printf("🛑 No se pueden eliminar más números\n");
      //FASE 3: Libre hasta objetivo
      int objetivo_adicional = 15;
      terceraEleccionLibre(sudoku, objetivo_adicional);
    }  return exito;
}
// ═══════════════════════════════════════════════════════════════════
//                    FUNCIONES AUXILIARES
// ═══════════════════════════════════════════════════════════════════

void imprimirSudoku(int sudoku[SIZE][SIZE]) {
    int asteriscos = 0;
    printf("┌───────┬───────┬───────┐\n");
    for(int i = 0; i < SIZE; i++) {
        printf("│");
        for(int j = 0; j < SIZE; j++) {
            if(sudoku[i][j] == 0){
              printf(" *");
              asteriscos++;

            } else{
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
    printf("Total celdas vacías %d\n", asteriscos); //Mostrar total
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
    //Inicia semilla random!!!
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

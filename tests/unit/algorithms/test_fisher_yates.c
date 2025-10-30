/*
 * Archivo: test_fisher_yates.c
 * Pruebas unitarias para la función sudoku_generate_permutation
 * en el proyecto de generador de Sudoku
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

// Si usas Unity, incluye su header
// #include "unity.h"

#define SIZE 9

// Declaración de tu función (ajusta según tu header)
void sudoku_generate_permutation(int *array, int size, int start);

// ═══════════════════════════════════════════════════════════════
//                    FUNCIONES AUXILIARES
// ═══════════════════════════════════════════════════════════════

// Función de comparación para qsort
int comparar_int(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// Imprime un arreglo (útil para depuración)
void imprimir_arreglo(int *arr, int size) {
    printf("[");
    for(int i = 0; i < size; i++) {
        printf("%d", arr[i]);
        if(i < size-1) printf(", ");
    }
    printf("]\n");
}

// ═══════════════════════════════════════════════════════════════
//          PRUEBA 1: TODOS LOS ELEMENTOS PRESENTES
// ═══════════════════════════════════════════════════════════════

bool test_todos_elementos_presentes(void) {
    printf("\n[TEST 1] Verificando que todos los elementos estén presentes...\n");
    
    int arr[SIZE];
    int arr_ordenado[SIZE];
    bool exito = true;
    
    // Ejecutamos la función
    sudoku_generate_permutation(arr, SIZE, 1);
    
    printf("  Arreglo mezclado: ");
    imprimir_arreglo(arr, SIZE);
    
    // Copiamos y ordenamos
    memcpy(arr_ordenado, arr, SIZE * sizeof(int));
    qsort(arr_ordenado, SIZE, sizeof(int), comparar_int);
    
    printf("  Arreglo ordenado: ");
    imprimir_arreglo(arr_ordenado, SIZE);
    
    // Verificamos que después de ordenar tengamos [1,2,3,4,5,6,7,8,9]
    for(int i = 0; i < SIZE; i++) {
        if(arr_ordenado[i] != (i + 1)) {
            printf("  ✗ ERROR: Se esperaba %d pero se encontró %d\n", 
                   i + 1, arr_ordenado[i]);
            exito = false;
        }
    }
    
    if(exito) {
        printf("  ✓ ÉXITO: Todos los elementos del 1 al 9 están presentes\n");
    }
    
    return exito;
}

// ═══════════════════════════════════════════════════════════════
//          PRUEBA 2: SIN DUPLICADOS
// ═══════════════════════════════════════════════════════════════

bool test_sin_duplicados(void) {
    printf("\n[TEST 2] Verificando que no haya duplicados...\n");
    
    int arr[SIZE];
    bool exito = true;
    
    sudoku_generate_permutation(arr, SIZE, 1);
    
    printf("  Arreglo mezclado: ");
    imprimir_arreglo(arr, SIZE);
    
    // Ordenamos para facilitar la búsqueda de duplicados
    qsort(arr, SIZE, sizeof(int), comparar_int);
    
    // Verificamos que no haya elementos consecutivos iguales
    for(int i = 1; i < SIZE; i++) {
        if(arr[i-1] == arr[i]) {
            printf("  ✗ ERROR: Duplicado encontrado: %d\n", arr[i]);
            exito = false;
        }
    }
    
    if(exito) {
        printf("  ✓ ÉXITO: No hay elementos duplicados\n");
    }
    
    return exito;
}

// ═══════════════════════════════════════════════════════════════
//          PRUEBA 3: GENERA RESULTADOS DIFERENTES
// ═══════════════════════════════════════════════════════════════

bool test_genera_diferentes_resultados(void) {
    printf("\n[TEST 3] Verificando que genera resultados diferentes...\n");
    
    int arr1[SIZE], arr2[SIZE];
    bool son_diferentes = false;
    
    // Generamos dos mezclas
    sudoku_generate_permutation(arr1, SIZE, 1);
    sudoku_generate_permutation(arr2, SIZE, 1);
    
    printf("  Primera mezcla:  ");
    imprimir_arreglo(arr1, SIZE);
    printf("  Segunda mezcla:  ");
    imprimir_arreglo(arr2, SIZE);
    
    // Verificamos si son diferentes en al menos una posición
    for(int i = 0; i < SIZE; i++) {
        if(arr1[i] != arr2[i]) {
            son_diferentes = true;
            break;
        }
    }
    
    if(son_diferentes) {
        printf("  ✓ ÉXITO: Las mezclas son diferentes (el algoritmo es aleatorio)\n");
        return true;
    } else {
        printf("  ⚠ ADVERTENCIA: Las dos mezclas fueron idénticas\n");
        printf("  Esto puede ocurrir por azar. Probando 10 veces más...\n");
        
        // Damos más oportunidades
        for(int intento = 0; intento < 10; intento++) {
            sudoku_generate_permutation(arr1, SIZE, 1);
            sudoku_generate_permutation(arr2, SIZE, 1);
            
            for(int i = 0; i < SIZE; i++) {
                if(arr1[i] != arr2[i]) {
                    printf("  ✓ ÉXITO: Encontramos diferencias en el intento %d\n", 
                           intento + 1);
                    return true;
                }
            }
        }
        
        printf("  ✗ ERROR: Después de 12 intentos, todas las mezclas fueron idénticas\n");
        printf("  Posible problema: rand() no inicializado o algoritmo determinista\n");
        return false;
    }
}

// ═══════════════════════════════════════════════════════════════
//      PRUEBA 4: DETECCIÓN DEL ERROR DE SATTOLO (LA IMPORTANTE)
// ═══════════════════════════════════════════════════════════════

bool test_elementos_pueden_quedarse_en_lugar(void) {
    printf("\n[TEST 4] Verificando que elementos PUEDEN quedarse en su lugar original...\n");
    printf("  (Esta prueba detecta el error de Sattolo)\n");
    
    const int NUM_PRUEBAS = 10000;
    int elementos_que_quedaron_en_lugar[SIZE] = {0};
    int total_elementos_que_quedaron = 0;
    
    printf("  Ejecutando %d mezclas...\n", NUM_PRUEBAS);
    
    // Ejecutamos muchas mezclas
    for(int prueba = 0; prueba < NUM_PRUEBAS; prueba++) {
        int arr[SIZE];
        sudoku_generate_permutation(arr, SIZE, 1);
        
        // Verificamos qué elementos quedaron en su posición original
        for(int i = 0; i < SIZE; i++) {
            if(arr[i] == (i + 1)) {  // El elemento (i+1) está en posición i
                if(elementos_que_quedaron_en_lugar[i] == 0) {
                    elementos_que_quedaron_en_lugar[i] = 1;
                    total_elementos_que_quedaron++;
                }
            }
        }
        
        // Optimización: si ya todos quedaron al menos una vez, podemos parar
        if(total_elementos_que_quedaron == SIZE) {
            printf("  ✓ Completado en %d iteraciones (optimizado)\n", prueba + 1);
            break;
        }
    }
    
    // Mostramos resultados detallados
    printf("\n  Resultados por elemento:\n");
    bool todos_quedaron = true;
    for(int i = 0; i < SIZE; i++) {
        if(elementos_que_quedaron_en_lugar[i]) {
            printf("    ✓ Elemento %d quedó en posición %d al menos una vez\n", 
                   i+1, i);
        } else {
            printf("    ✗ Elemento %d NUNCA quedó en posición %d\n", i+1, i);
            todos_quedaron = false;
        }
    }
    
    printf("\n  Resumen: %d de %d elementos quedaron en su lugar al menos una vez\n",
           total_elementos_que_quedaron, SIZE);
    
    if(todos_quedaron) {
        printf("\n  ✓ ÉXITO: Fisher-Yates correcto (elementos pueden quedarse)\n");
        printf("  Esto significa que tu código usa: j = rand() %% (i + 1)\n");
        return true;
    } else {
        printf("\n  ✗ ERROR DE SATTOLO DETECTADO\n");
        printf("  Algunos elementos NUNCA quedaron en su posición original.\n");
        printf("  Esto indica que probablemente tienes: j = rand() %% i\n");
        printf("  Deberías cambiarlo a: j = rand() %% (i + 1)\n");
        return false;
    }
}

// ═══════════════════════════════════════════════════════════════
//      PRUEBA 5 (BONUS): DISTRIBUCIÓN UNIFORME SIMPLIFICADA
// ═══════════════════════════════════════════════════════════════

bool test_distribucion_basica(void) {
    printf("\n[TEST 5 BONUS] Verificando distribución básica...\n");
    printf("  (Cuenta cuántas veces cada elemento aparece en primera posición)\n");
    
    const int NUM_PRUEBAS = 9000;  // 1000 por cada elemento
    int conteo_primera_posicion[SIZE] = {0};
    
    printf("  Ejecutando %d mezclas...\n", NUM_PRUEBAS);
    
    for(int prueba = 0; prueba < NUM_PRUEBAS; prueba++) {
        int arr[SIZE];
        sudoku_generate_permutation(arr, SIZE, 1);
        
        // Contamos qué elemento quedó en la primera posición
        conteo_primera_posicion[arr[0] - 1]++;
    }
    
    printf("\n  Frecuencia de cada elemento en primera posición:\n");
    printf("  (Esperado: ~%d veces cada uno para distribución uniforme)\n", 
           NUM_PRUEBAS / SIZE);
    
    double esperado = (double)NUM_PRUEBAS / SIZE;
    bool distribucion_razonable = true;
    
    for(int i = 0; i < SIZE; i++) {
        double porcentaje = (conteo_primera_posicion[i] * 100.0) / NUM_PRUEBAS;
        double desviacion = ((conteo_primera_posicion[i] - esperado) / esperado) * 100.0;
        
        printf("    Elemento %d: %4d veces (%.1f%%, desviación: %+.1f%%)\n", 
               i+1, conteo_primera_posicion[i], porcentaje, desviacion);
        
        // Una desviación mayor al 30% sería preocupante
        if(desviacion > 30.0 || desviacion < -30.0) {
            distribucion_razonable = false;
        }
    }
    
    if(distribucion_razonable) {
        printf("\n  ✓ La distribución parece razonablemente uniforme\n");
        return true;
    } else {
        printf("\n  ⚠ ADVERTENCIA: Hay desviaciones significativas en la distribución\n");
        printf("  Esto podría indicar un problema sutil en el algoritmo o en rand()\n");
        return false;
    }
}

// ═══════════════════════════════════════════════════════════════
//                      FUNCIÓN PRINCIPAL DE PRUEBAS
// ═══════════════════════════════════════════════════════════════

int main(void) {
    // Inicializar el generador de números aleatorios
    srand(time(NULL));
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("       SUITE DE PRUEBAS PARA FISHER-YATES (SUDOKU)\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    int pruebas_exitosas = 0;
    int total_pruebas = 5;
    
    // Ejecutar todas las pruebas
    if(test_todos_elementos_presentes()) pruebas_exitosas++;
    if(test_sin_duplicados()) pruebas_exitosas++;
    if(test_genera_diferentes_resultados()) pruebas_exitosas++;
    if(test_elementos_pueden_quedarse_en_lugar()) pruebas_exitosas++;  // ← La de Sattolo
    if(test_distribucion_basica()) pruebas_exitosas++;
    
    // Resumen final
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("                    RESUMEN DE PRUEBAS\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  Pruebas exitosas: %d / %d\n", pruebas_exitosas, total_pruebas);
    
    if(pruebas_exitosas == total_pruebas) {
        printf("\n  🎉 ¡TODAS LAS PRUEBAS PASARON!\n");
        printf("  Tu implementación de Fisher-Yates es correcta.\n");
        return 0;
    } else {
        printf("\n  ⚠️  ALGUNAS PRUEBAS FALLARON\n");
        printf("  Revisa los errores arriba y corrige tu implementación.\n");
        return 1;
    }
}

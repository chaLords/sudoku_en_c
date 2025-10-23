// src/core/algorithms/fisher_yates.c
#include <stdlib.h>
#include "../internal/algorithms_internal.h"

void sudoku_generate_permutation(int *array, int size, int start) {
 // ═══════════════════════════════════════════════════════════════════
//                    FISHER-YATES ALGORITHM
// ═══════════════════════════════════════════════════════════════════
/**
 * Shuffles an array using Fisher-Yates algorithm
 * Parameters:
 * - array: pointer to the array to shuffle (modifies in-place)
 * - size: array size
 * - start_value: initial value to fill (usually 1)
 */
    // Fill consecutively
    for(int i = 0; i < size; i++) {
        array[i] = start + i;
    }
    
    // Shuffle (Fisher-Yates backward)
    for(int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}


/**
 * Memory Management Utilities
 * Version: 3.0.0
 * 
 * Helper functions for dynamic 2D array allocation
 * with proper error handling.
 */

#include <stdlib.h>
#include <stdio.h>

/**
 * Allocates 2D integer array
 * 
 * @param rows Number of rows
 * @param cols Number of columns
 * @return Pointer to allocated array or NULL on failure
 */
int** allocate2DArray(int rows, int cols) {
    // Allocate array of row pointers
    int **array = (int **)malloc(rows * sizeof(int *));
    if(array == NULL) {
        fprintf(stderr, "Error: Failed to allocate row pointers\n");
        return NULL;
    }
    
    // Allocate each row
    for(int i = 0; i < rows; i++) {
        array[i] = (int *)calloc(cols, sizeof(int));
        if(array[i] == NULL) {
            fprintf(stderr, "Error: Failed to allocate row %d\n", i);
            
            // Free previously allocated rows
            for(int j = 0; j < i; j++) {
                free(array[j]);
            }
            free(array);
            return NULL;
        }
    }
    
    return array;
}

/**
 * Frees 2D integer array
 * 
 * @param array Array to free
 * @param rows Number of rows
 */
void free2DArray(int **array, int rows) {
    if(array == NULL) return;
    
    for(int i = 0; i < rows; i++) {
        free(array[i]);
    }
    free(array);
}

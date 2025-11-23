/**
 * @file phase1.c (ADAPTED FOR CONFIGURABLE BOARD SIZES)
 * @brief Phase 1: Balanced elimination across all subgrids
 * @author Gonzalo Ramírez
 * @date 2025-11-21
 * 
 * ARCHITECTURAL CHANGES FOR CONFIGURABLE SIZES:
 * 
 * 1. REPLACED: Hardcoded SUDOKU_SIZE constant
 *    WITH: Dynamic board->board_size access
 *    
 * 2. REPLACED: Static array int numbers[9]
 *    WITH: Dynamic allocation based on actual board size
 *    
 * 3. MAINTAINED: Event-driven architecture (no printf)
 * 4. MAINTAINED: Encapsulation through getters/setters where possible
 * 
 * KEY INSIGHT: Phase 1's algorithm (remove one random number per subgrid)
 * is inherently size-agnostic. The only changes needed are replacing
 * hardcoded loop bounds and array sizes with dynamic values.
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "algorithms_internal.h"
#include "elimination_internal.h"
//#include "board_internal.h"
#include "events_internal.h"
#include "sudoku/core/board.h"

/**
 * @brief Phase 1: Remove one random number from each subgrid
 * 
 * ADAPTATIONS FOR CONFIGURABLE SIZES:
 * 
 * The core algorithm remains unchanged: select a random number per subgrid
 * and remove its first occurrence. However, we must dynamically allocate
 * the permutation array to match the board size.
 * 
 * MATHEMATICAL REASONING:
 * - For an N×N board with k×k subgrids, there are k² subgrids
 * - Each subgrid contains N cells (k² cells technically, but N numbers 1..N)
 * - We need an array of size N to hold the permutation
 * 
 * MEMORY MANAGEMENT:
 * - Allocates int[board_size] on the heap
 * - Must free after use to prevent memory leak
 * - Allocation failure handled gracefully (returns 0)
 * 
 * @param[in,out] board Board with complete solution
 * @param[in] index Array of subgrid indices (typically shuffled)
 * @param[in] count Number of subgrids to process
 * @return Number of cells successfully removed
 * 
 * @pre board != NULL && index != NULL
 * @pre Board is completely filled with valid solution
 * @post Exactly one cell per processed subgrid is emptied
 * 
 * Time Complexity: O(k² × N) where k² = number of subgrids, N = board size
 * Space Complexity: O(N) for the permutation array
 */
int phase1Elimination(SudokuBoard *board, const int *index, int count) {
    // Emit phase start event
    emit_event(SUDOKU_EVENT_PHASE1_START, board, 1, 0);
    
    // ✅ ADAPTACIÓN 1: Obtener tamaño dinámico del tablero
    // ANTES: Asumía board_size = 9
    // AHORA: Consulta el valor real del tablero
    int board_size = sudoku_board_get_board_size(board);
    
    // ✅ ADAPTACIÓN 2: Asignación dinámica del array de permutación
    // ANTES: int numbers[SUDOKU_SIZE] con SUDOKU_SIZE = 9
    // AHORA: malloc(board_size * sizeof(int)) para cualquier tamaño
    // 
    // RAZONAMIENTO: En un tablero N×N, los números válidos van de 1 a N.
    // Necesitamos espacio para N números en la permutación.
    int *numbers = (int *)malloc(board_size * sizeof(int));
    
    // Handle memory allocation failure gracefully
    if (numbers == NULL) {
        // In production, this should propagate error upward
        // For now, return 0 (no cells removed) and emit error event
        fprintf(stderr, "Error: Failed to allocate memory for Phase 1\n");
        return 0;
    }
    
    // Generate random permutation of numbers 1..board_size
    // This function is already adaptive because we pass board_size as parameter
    sudoku_generate_permutation(numbers, board_size, 1);
    
    int removed = 0;
    
    // Process each subgrid in the order specified by index array
    for (int idx = 0; idx < count; idx++) {
        SudokuSubGrid subgrid = sudoku_subgrid_create(index[idx], board->subgrid_size);        
        // Select which number to remove from this subgrid
        // Uses the idx-th element of the permutation
        int target_value = numbers[idx];
        
        // ✅ ADAPTACIÓN 3: Usar board_size en lugar de SUDOKU_SIZE
        // ANTES: for (int cell_idx = 0; cell_idx < SUDOKU_SIZE; cell_idx++)
        // AHORA: for (int cell_idx = 0; cell_idx < board_size; cell_idx++)
        //
        // RAZONAMIENTO: Cada subcuadrícula contiene board_size celdas.
        // Para 4×4: subgrid tiene 4 celdas (2×2)
        // Para 9×9: subgrid tiene 9 celdas (3×3)
        // Para 16×16: subgrid tiene 16 celdas (4×4)
        for (int cell_idx = 0; cell_idx < board_size; cell_idx++) {
            SudokuPosition pos = sudoku_subgrid_get_position(&subgrid, cell_idx);
            
            // ✅ NOTA: Aquí mantenemos acceso directo a board->cells
            // porque estamos en código interno (elimination_internal).
            // Si fuera API pública, usaríamos sudoku_board_get_cell().
            if (board->cells[pos.row][pos.col] == target_value) {
                // Save value before removing (needed for event emission)
                int removed_value = board->cells[pos.row][pos.col];
                
                // Remove the cell
                board->cells[pos.row][pos.col] = 0;
                removed++;
                
                // Emit cell selected event
                emit_event_cell(SUDOKU_EVENT_PHASE1_CELL_SELECTED, board, 1,
                               removed, pos.row, pos.col, removed_value);
                
                break;  // Only remove one occurrence per subgrid
            }
        }
    }
    
    // ✅ CRÍTICO: Liberar memoria dinámica
    // Olvidar este free() causa memory leak
    free(numbers);
    
    // Emit phase complete event
    emit_event(SUDOKU_EVENT_PHASE1_COMPLETE, board, 1, removed);
    
    return removed;
}

/**
 * EDUCATIONAL NOTES ON ADAPTATIONS:
 * 
 * 1. DYNAMIC vs STATIC ARRAYS:
 *    
 *    Static (old):  int numbers[9];
 *    - Allocated on stack at compile-time
 *    - Size fixed forever
 *    - Fast but inflexible
 *    
 *    Dynamic (new): int *numbers = malloc(board_size * sizeof(int));
 *    - Allocated on heap at runtime
 *    - Size determined by board_size variable
 *    - Requires explicit free()
 *    - Flexible but requires memory management
 * 
 * 2. WHY board_size NOT subgrid_count FOR THE LOOP?
 *    
 *    Each subgrid contains board_size cells (not subgrid_size²).
 *    Example for 9×9 board (3×3 subgrids):
 *    - board_size = 9
 *    - subgrid_size = 3
 *    - cells per subgrid = subgrid_size × subgrid_size = 9
 *    
 *    We iterate 0..8 (board_size) because there are 9 cells to check
 *    in each 3×3 subgrid, numbered linearly 0-8.
 * 
 * 3. MEMORY ALLOCATION ERROR HANDLING:
 *    
 *    Always check if malloc() returns NULL. On modern systems with
 *    virtual memory, this is rare, but can happen if:
 *    - System is out of memory
 *    - Requesting absurdly large allocation
 *    - Memory fragmentation prevents allocation
 *    
 *    Proper error handling prevents segmentation faults from dereferencing
 *    NULL pointers.
 * 
 * 4. WHY THIS ADAPTATION IS SUFFICIENT:
 *    
 *    The algorithm's logic is size-independent:
 *    - "Remove one random number per subgrid"
 *    
 *    This statement is true whether we have:
 *    - 4 subgrids in 4×4 board
 *    - 9 subgrids in 9×9 board
 *    - 16 subgrids in 16×16 board
 *    
 *    The only size-dependent parts were:
 *    - Array size for permutation → solved with malloc
 *    - Loop bound for cell iteration → solved with board_size
 *    
 *    Everything else (subgrid creation, position calculation, event
 *    emission) was already size-agnostic through the API.
 */

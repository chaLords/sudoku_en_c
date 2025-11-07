/**
 * @file display.c
 * @brief Display and visualization functions for Sudoku boards
 * @author Gonzalo Ramírez
 * @date 2025-11-06
 * 
 * This module handles all visual representation of Sudoku boards,
 * including formatted printing to console. Separating display logic
 * from data management allows for multiple output formats and
 * easier maintenance of presentation layer.
 * 
 * The current implementation focuses on console output with Unicode
 * box-drawing characters. Future versions will add file export
 * capabilities in various formats (HTML, PDF, plain text).
 */

#include <stdio.h>
#include "sudoku/core/display.h"
#include "sudoku/core/types.h"
#include "internal/config_internal.h"  

// ═══════════════════════════════════════════════════════════════
//                    CONSOLE OUTPUT FUNCTIONS
// ═══════════════════════════════════════════════════════════════

/**
 * @brief Print the Sudoku board with formatted box-drawing characters
 * 
 * Creates a visually appealing representation of the board using Unicode
 * box-drawing characters for a professional console output. Empty cells
 * are displayed as dots (.), filled cells show their numbers (1-9).
 * The 3x3 subgrid structure is clearly delineated with thicker borders.
 * 
 * Output format uses Unicode box-drawing characters:
 * - ┌ ┬ ┐ └ ┴ ┘ for corners
 * - ─ for horizontal lines
 * - │ for vertical lines
 * - ├ ┼ ┤ for intersections
 * 
 * The display automatically includes statistics (empty cells, clues)
 * when the global verbosity level is set to 1 or higher.
 * 
 * @param[in] board Pointer to the board to display
 * 
 * @pre board != NULL
 * @pre board is properly initialized (via sudoku_board_init or sudoku_generate)
 * 
 * @note Requires UTF-8 terminal support for proper character rendering.
 *       On Windows, set code page 65001: `chcp 65001`
 * @note Output goes to stdout. For file output, redirect or use alternative
 *       export functions when available
 * @note The display format is optimized for 80-column terminals
 * 
 * @warning On terminals without Unicode support, box-drawing characters
 *          may render as question marks or other replacement characters
 * 
 * Example output:
 * @code
 * ┌───────┬───────┬───────┐
 * │ . 2 7 │ . 4 . │ . . . │
 * │ . 5 . │ 7 . . │ 2 8 4 │
 * │ 9 1 . │ . 8 . │ 3 . 5 │
 * ├───────┼───────┼───────┤
 * │ . 8 2 │ . 3 7 │ . 1 9 │
 * │ . 3 9 │ 4 1 5 │ 6 2 . │
 * │ . . 1 │ 2 8 9 │ 7 . . │
 * ├───────┼───────┼───────┤
 * │ . . 8 │ . . 6 │ . 5 2 │
 * │ . 7 . │ . . 4 │ 8 . 1 │
 * │ 1 4 5 │ 3 . 8 │ . 6 7 │
 * └───────┴───────┴───────┘
 * 📊 Empty: 35 | Clues: 46
 * @endcode
 * 
 * @see sudoku_set_verbosity() to control statistics display
 * @see sudoku_board_init() for board initialization
 */
void sudoku_display_print_board(const SudokuBoard *board) {
    // Top border with corners and horizontal lines
    printf("┌───────┬───────┬───────┐\n");
    
    // Process each row of the 9x9 grid
    for(int i = 0; i < SUDOKU_SIZE; i++) {
        printf("│");
        
        // Print each cell in the current row
        for(int j = 0; j < SUDOKU_SIZE; j++) {
            if(board->cells[i][j] == 0) {
                printf(" .");  // Empty cell represented as dot
            } else {
                printf(" %d", board->cells[i][j]);  // Filled cell shows number
            }
            
            // Vertical separator after every 3rd column (subgrid boundary)
            if((j + 1) % 3 == 0) {
                printf(" │");
            }
        }
        printf("\n");
        
        // Horizontal separator after every 3rd row (subgrid boundary)
        // Skip after the last row (row 8) to use bottom border instead
        if((i + 1) % 3 == 0 && i < SUDOKU_SIZE - 1) {
            printf("├───────┼───────┼───────┤\n");
        }
    }
    
    // Bottom border with corners and horizontal lines
    printf("└───────┴───────┴───────┘\n");
    
    // Optional statistics display controlled by verbosity level
    if(VERBOSITY_LEVEL >= 1) {
        printf("📊 Empty: %d | Clues: %d\n", board->empty, board->clues);
    }
}

// ═══════════════════════════════════════════════════════════════
//                    FUTURE: FILE OUTPUT FUNCTIONS
// ═══════════════════════════════════════════════════════════════

/*
 * Future functions planned for this section:
 * - sudoku_display_save_to_file()    - Save board to text file
 * - sudoku_display_export_ascii()    - Plain ASCII version (no Unicode)
 * - sudoku_display_export_html()     - HTML table format
 * - sudoku_display_export_json()     - JSON representation
 */

// ═══════════════════════════════════════════════════════════════
//                    FUTURE: FORMATTED STRING FUNCTIONS
// ═══════════════════════════════════════════════════════════════

/*
 * Future functions planned for this section:
 * - sudoku_display_to_string()       - Return board as formatted string
 * - sudoku_display_compact_format()  - One-line representation
 * - sudoku_display_debug_format()    - Verbose format with metadata
 */

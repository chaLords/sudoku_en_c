/**
 * @file display.c
 * @brief Display and visualization functions for Sudoku boards (Multi-size)
 * @author Gonzalo Ramírez
 * @date 2025-12-11
 * 
 * FIXED VERSION v3.0.2c - Square aspect ratio
 */

#include <stdio.h>
#include <math.h>
#include "sudoku/core/display.h"
#include "sudoku/core/types.h"
#include "sudoku/core/board.h"
#include "internal/board_internal.h"

// ═══════════════════════════════════════════════════════════════
//                    HELPER FUNCTIONS
// ═══════════════════════════════════════════════════════════════

static int calculate_cell_width(int board_size) {
    if (board_size <= 9) return 1;
    if (board_size <= 99) return 2;
    return 3;
}

/**
 * @brief Print horizontal border with square aspect ratio
 * 
 * Each cell occupies (cell_width + 1) characters:
 * - 1 leading space
 * - cell_width for the number
 * No trailing space (makes it more compact and square)
 */
static void print_horizontal_border(char border_type, int subgrid_size, 
                                   int cell_width) {
    const char *left, *junction, *right;
    const char *horizontal = "─";
    
    switch (border_type) {
        case 'T':
            left = " ┌";
            junction = "─┬";
            right = "─┐";
            break;
        case 'M':
            left = " ├";
            junction = "─┼";
            right = "─┤";
            break;
        case 'B':
            left = " └";
            junction = "─┴";
            right = "─┘";
            break;
        default:
            return;
    }
    
    printf("%s", left);
    
    for (int sg_col = 0; sg_col < subgrid_size; sg_col++) {
        for (int cell = 0; cell < subgrid_size; cell++) {
            // ✅ Each cell = (cell_width + 1) for square aspect
            for (int w = 0; w < (cell_width + 1); w++) {
                printf("%s", horizontal);
            }
        }
        
        if (sg_col < subgrid_size - 1) {
            printf("%s", junction);
        } else {
            printf("%s\n", right);
        }
    }
}

// ═══════════════════════════════════════════════════════════════
//                    PUBLIC DISPLAY FUNCTION
// ═══════════════════════════════════════════════════════════════

void sudoku_display_print_board(const SudokuBoard *board) {
    if (board == NULL) {
        fprintf(stderr, "Error: Cannot display NULL board\n");
        return;
    }
    
    int subgrid_size = sudoku_board_get_subgrid_size(board);
    int board_size = sudoku_board_get_board_size(board);
    
    if (subgrid_size < 2 || board_size < 4) {
        fprintf(stderr, "Error: Invalid board dimensions\n");
        return;
    }
    
    int cell_width = calculate_cell_width(board_size);
    
    // Top border
    print_horizontal_border('T', subgrid_size, cell_width);
    
    // Board rows
    for (int row = 0; row < board_size; row++) {
        printf(" │");
        
        for (int col = 0; col < board_size; col++) {
            int value = sudoku_board_get_cell(board, row, col);
            
            // ✅ Format: " %*d" = 1 space + cell_width (NO trailing space)
            if (value == 0) {
                printf(" %*s", cell_width, ".");
            } else {
                printf(" %*d", cell_width, value);
            }
            
            // Vertical separator at subgrid boundaries
            if ((col + 1) % subgrid_size == 0) {
                printf(" │");
            }
        }
        
        printf("\n");
        
        // Horizontal separator at subgrid boundaries
        if ((row + 1) % subgrid_size == 0 && row < board_size - 1) {
            print_horizontal_border('M', subgrid_size, cell_width);
        }
    }
    
    // Bottom border
    print_horizontal_border('B', subgrid_size, cell_width);
    
    // Statistics
    int clues = sudoku_board_get_clues(board);
    int empty = sudoku_board_get_empty(board);
    
    printf("\n📊 Empty: %d | Clues: %d\n", empty, clues);
}

// ═══════════════════════════════════════════════════════════════
//                    COMPACT DISPLAY (OPTIONAL)
// ═══════════════════════════════════════════════════════════════

void sudoku_display_print_compact(const SudokuBoard *board) {
    if (board == NULL) return;
    
    int subgrid_size = sudoku_board_get_subgrid_size(board);
    int board_size = sudoku_board_get_board_size(board);
    int cell_width = calculate_cell_width(board_size);
    
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            int value = sudoku_board_get_cell(board, row, col);
            
            if (value == 0) {
                printf("%*s", cell_width, ".");
            } else {
                printf("%*d", cell_width, value);
            }
            
            printf(" ");
            
            if ((col + 1) % subgrid_size == 0 && col < board_size - 1) {
                printf("| ");
            }
        }
        printf("\n");
        
        if ((row + 1) % subgrid_size == 0 && row < board_size - 1) {
            for (int i = 0; i < board_size * (cell_width + 1) + subgrid_size; i++) {
                printf("─");
            }
            printf("\n");
        }
    }
}

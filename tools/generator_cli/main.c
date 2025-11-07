// tools/generator_cli/main.c - REFACTORED VERSION

/**
 * @file main.c
 * @brief CLI tool for generating Sudoku puzzles
 * @author Gonzalo Ramírez
 * @date 2025-11-06
 * 
 * This command-line interface demonstrates how to use the Sudoku
 * generation library. It serves as both a practical tool and an
 * example of proper library usage.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Include ONLY public headers from the library
#include "sudoku/core/types.h"
#include "sudoku/core/generator.h"
#include "sudoku/core/display.h"
#include "sudoku/core/validation.h"

// ═══════════════════════════════════════════════════════════════════
//                           MAIN FUNCTION
// ═══════════════════════════════════════════════════════════════════

int main(int argc, char *argv[]) {
    // Configure UTF-8 encoding on Windows
    #ifdef _WIN32
        system("chcp 65001 > nul");
    #endif
    
    // Initialize random number generator
    srand((unsigned int)time(NULL));
    
    // Default verbosity level (1 = compact mode)
    int verbosity_level = 1;
    
    // Parse command-line arguments
    if(argc > 1) {
        int level = atoi(argv[1]);
        if(level >= 0 && level <= 2) {
            verbosity_level = level;
        } else {
            fprintf(stderr, "❌ Invalid verbosity level: %s\n", argv[1]);
            printf("\nUsage: %s [level]\n", argv[0]);
            printf("  level: 0 (minimal), 1 (compact - default), 2 (detailed)\n\n");
            printf("Examples:\n");
            printf("  %s       - Run with default mode (compact)\n", argv[0]);
            printf("  %s 0     - Run in minimal mode\n", argv[0]);
            printf("  %s 1     - Run in compact mode\n", argv[0]);
            printf("  %s 2     - Run in detailed mode\n", argv[0]);
            printf("  time %s 0 - Run in minimal mode with timing\n", argv[0]);
            return 1;
        }
    }
    
    // Use the public API to configure verbosity
    sudoku_set_verbosity(verbosity_level);
    
    // Get current verbosity level for display logic
    int current_level = sudoku_get_verbosity();
    
    // Allocate memory for the board (demonstrates dynamic allocation)
    SudokuBoard *board = (SudokuBoard *)malloc(sizeof(SudokuBoard));
    if(board == NULL) {
        fprintf(stderr, "❌ Error: Could not allocate memory for the board\n");
        return 1;
    }
    
    // Structure to store generation statistics
    SudokuGenerationStats stats;
    
    // Display title based on verbosity level
    if(current_level == 0 || current_level == 1) {
        printf("═══════════════════════════════════════════════════════════════\n");
        printf("  SUDOKU GENERATOR v2.3.0 – MODULAR ARCHITECTURE\n");
        printf("═══════════════════════════════════════════════════════════════\n\n");
    } else if(current_level == 2) {
        printf("═══════════════════════════════════════════════════════════════\n");
        printf("    SUDOKU GENERATOR v2.3.0 – MODULAR ARCHITECTURE\n");
        printf("           Fisher-Yates + Backtracking + 3 Phases\n");
        printf("═══════════════════════════════════════════════════════════════\n\n");
    }

    bool success = false;
    
    // Try up to 5 attempts to generate a valid Sudoku
    for(int attempt = 1; attempt <= 5; attempt++) {
        if(current_level >= 1) {
            printf("🚀 ATTEMPT #%d:\n", attempt);
        }
        
        // Call the library's generation function
        if(sudoku_generate(board, &stats)) {
            if(current_level >= 1) {
                printf("✅ SUCCESS! Sudoku generated\n\n");
            }
            
            // Display the generated puzzle
            sudoku_display_print_board(board);
            printf("\n");
            
            // Validate the puzzle for correctness
            if(sudoku_validate_board(board)) {
                if(current_level >= 1) {
                    printf("🎉 VERIFIED! The puzzle is valid\n");
                }
            }
            
            // Evaluate and display difficulty level
            if(current_level >= 0) {
                SudokuDifficulty difficulty = sudoku_evaluate_difficulty(board);
                printf("\n📊 Difficulty level: %s\n", 
                       sudoku_difficulty_to_string(difficulty));
            }
            
            success = true;
            break;
        } else {
            // Only show failures in detailed mode
            if(current_level == 2) {
                printf("❌ Failed (very rare with this method)\n\n");
            }
        }
    }
    
    // Free allocated memory
    free(board);
    
    // Report final status
    if(!success) {
        fprintf(stderr, "\n❌ ERROR: Could not generate a valid Sudoku after multiple attempts!\n");
        return 1;
    }
    
    return 0;
}

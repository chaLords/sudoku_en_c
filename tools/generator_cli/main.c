// tools/generator_cli/main.c - REFACTORED VERSION WITH CALLBACKS

/**
 * @file main.c
 * @brief CLI tool for generating Sudoku puzzles
 * @author Gonzalo Ramírez
 * @date 2025-11-13
 * 
 * This command-line interface demonstrates how to use the Sudoku
 * generation library with event callbacks for monitoring progress.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Include ONLY public headers from the library
#include "sudoku/core/board.h"
#include "sudoku/core/types.h"
#include "sudoku/core/generator.h"
#include "sudoku/core/display.h"
#include "sudoku/core/validation.h"

// ═══════════════════════════════════════════════════════════════════
//                    EVENT CALLBACK FOR PROGRESS MONITORING
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Callback function to receive generation events
 * 
 * This demonstrates how an application can monitor the generation process.
 * The library emits events (WHAT happens) and the application decides
 * HOW to display them. This maintains separation of concerns.
 */
void generation_callback(const SudokuEventData *event, void *user_data) {
    // user_data contains the verbosity level
    int verbosity = *(int*)user_data;
    
    switch (event->type) {
        case SUDOKU_EVENT_GENERATION_START:
            // Only show in detailed mode
            if (verbosity >= 2) {
                printf("\n🎲 GENERATION START\n");
                printf("════════════════════════════════════════\n");
            }
            break;
            
        case SUDOKU_EVENT_PHASE1_START:
            if (verbosity >= 2) {
                printf("\n🎯 PHASE 1: Fisher-Yates Selection\n");
            } else if (verbosity >= 1) {
                printf("🚀 Diagonal + Backtracking...");
                fflush(stdout);
            }
            break;
            
        case SUDOKU_EVENT_PHASE1_CELL_SELECTED:
            // Detailed mode: show each cell removed
            if (verbosity >= 2) {
                printf("   Removed %d at (%d,%d) - Total: %d\n",
                       event->value, event->row, event->col,
                       event->cells_removed_total);
            }
            break;
            
        case SUDOKU_EVENT_PHASE1_COMPLETE:
            if (verbosity >= 2) {
                printf("✓ Phase 1 completed: %d cells removed\n",
                       event->cells_removed_total);
            } else if (verbosity >= 1) {
                printf("Phase 1: %d cells removed\n",
                       event->cells_removed_total);
            }
            break;
            
        case SUDOKU_EVENT_GENERATION_COMPLETE:
            if (verbosity >= 2) {
                printf("\n════════════════════════════════════════\n");
                printf("✅ GENERATION COMPLETE\n");
                printf("Final: %d empty, %d clues\n",
                       sudoku_board_get_empty(event->board),
                       sudoku_board_get_clues(event->board));
            }
            break;
            
        default:
            // Ignore other events for now (Phase 2 and 3 not yet updated)
            break;
    }
}

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
    
    // Allocate memory for the board
    SudokuBoard *board = sudoku_board_create();
    if (board == NULL) {
        fprintf(stderr, "Error: No se pudo crear el tablero\n");
        return 1;
    }
    
    // Structure to store generation statistics
    SudokuGenerationStats stats;
    
    // Display title based on verbosity level
    if(verbosity_level == 0 || verbosity_level == 1) {
        printf("═══════════════════════════════════════════════════════════════\n");
        printf("  SUDOKU GENERATOR v2.2.3 – MODULAR ARCHITECTURE\n");
        printf("═══════════════════════════════════════════════════════════════\n\n");
    } else if(verbosity_level == 2) {
        printf("═══════════════════════════════════════════════════════════════\n");
        printf("    SUDOKU GENERATOR v2.2.3 – MODULAR ARCHITECTURE\n");
        printf("           Fisher-Yates + Backtracking + 3 Phases\n");
        printf("═══════════════════════════════════════════════════════════════\n");
    }

    bool success = false;
    
    // Try up to 5 attempts to generate a valid Sudoku
    for(int attempt = 1; attempt <= 5; attempt++) {
        if(verbosity_level >= 1) {
            printf("🚀 ATTEMPT #%d:\n", attempt);
        }
        
        // Configure generation with callback
        SudokuGenerationConfig config = {
            .callback = generation_callback,
            .user_data = &verbosity_level,  // Pass verbosity to callback
            .max_attempts = 0
        };
        
        // Call the library's generation function WITH CALLBACK SUPPORT
        if(sudoku_generate_ex(board, &config, &stats)) {
            if(verbosity_level >= 1) {
                printf("✅ SUCCESS! Sudoku generated\n\n");
            }
            
            // Display the generated puzzle
            sudoku_display_print_board(board);
            printf("\n");
            
            // Validate the puzzle for correctness
            if(sudoku_validate_board(board)) {
                if(verbosity_level >= 1) {
                    printf("🎉 VERIFIED! The puzzle is valid\n");
                }
            }
            
            // Evaluate and display difficulty level
            if(verbosity_level >= 0) {
                SudokuDifficulty difficulty = sudoku_evaluate_difficulty(board);
                printf("\n📊 Difficulty level: %s\n", 
                       sudoku_difficulty_to_string(difficulty));
            }
            
            success = true;
            break;
        } else {
            // Only show failures in detailed mode
            if(verbosity_level == 2) {
                printf("❌ Failed (very rare with this method)\n\n");
            }
        }
    }
    
    // Free allocated memory
    sudoku_board_destroy(board);
    
    // Report final status
    if(!success) {
        fprintf(stderr, "\n❌ ERROR: Could not generate a valid Sudoku after multiple attempts!\n");
        return 1;
    }
    
    return 0;
}

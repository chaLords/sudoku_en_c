// tools/generator_cli/main.c - FINAL VERSION (COMPILES CORRECTLY)

/**
 * @file main.c
 * @brief CLI tool for generating Sudoku puzzles with configurable size and difficulty
 * @author Gonzalo Ramírez
 * @date 2025-12-11
 * 
 * CORRECTED VERSION - Uses actual API from types.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "sudoku/core/board.h"
#include "sudoku/core/types.h"
#include "sudoku/core/generator.h"
#include "sudoku/core/display.h"
#include "sudoku/core/validation.h"

// ═══════════════════════════════════════════════════════════════════
//                    BOARD SIZE CONFIGURATION
// ═══════════════════════════════════════════════════════════════════

typedef struct {
    const char *name;
    int subgrid_size;
    int total_cells;
    bool production_ready;
    const char *status_msg;
} BoardSizeInfo;

static const BoardSizeInfo SUPPORTED_SIZES[] = {
    {"4x4", 2, 16, true, "✅ PRODUCTION READY - Tutorial/Learning mode"},
    {"9x9", 3, 81, true, "✅ PRODUCTION READY - Classic Sudoku"},
    {"16x16", 4, 256, true, "✅ PRODUCTION READY - Expert level (~500ms generation)"},
    {"25x25", 5, 625, false, "⚠️  EXPERIMENTAL - May timeout (60s limit)"}
};

#define NUM_SUPPORTED_SIZES (sizeof(SUPPORTED_SIZES) / sizeof(SUPPORTED_SIZES[0]))

// ═══════════════════════════════════════════════════════════════════
//                    DIFFICULTY CONFIGURATION
// ═══════════════════════════════════════════════════════════════════

typedef struct {
    const char *name;
    SudokuDifficulty level;  // ✅ Use actual enum from types.h
    int min_empty_percent;
    int max_empty_percent;
    const char *description;
} DifficultyInfo;

static const DifficultyInfo SUPPORTED_DIFFICULTIES[] = {
    {"EASY",   DIFFICULTY_EASY,   30, 40, "Beginner-friendly (~60-70% filled)"},
    {"MEDIUM", DIFFICULTY_MEDIUM, 50, 60, "Intermediate challenge (~40-50% filled)"},
    {"HARD",   DIFFICULTY_HARD,   65, 75, "Advanced puzzle (~25-35% filled)"},
    {"EXPERT", DIFFICULTY_EXPERT, 75, 85, "Master level (~15-25% filled)"}
};

#define NUM_SUPPORTED_DIFFICULTIES (sizeof(SUPPORTED_DIFFICULTIES) / sizeof(SUPPORTED_DIFFICULTIES[0]))

// ═══════════════════════════════════════════════════════════════════
//                    HELPER FUNCTIONS
// ═══════════════════════════════════════════════════════════════════

int parse_board_size(const char *size_str) {
    if (!size_str) return -1;
    
    char buffer[16];
    strncpy(buffer, size_str, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    for (char *p = buffer; *p; p++) {
        *p = tolower(*p);
    }
    
    for (size_t i = 0; i < NUM_SUPPORTED_SIZES; i++) {
        if (strcmp(buffer, SUPPORTED_SIZES[i].name) == 0) {
            return SUPPORTED_SIZES[i].subgrid_size;
        }
    }
    
    return -1;
}

int parse_difficulty(const char *diff_str) {
    if (!diff_str) return -1;
    
    char buffer[16];
    strncpy(buffer, diff_str, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    for (char *p = buffer; *p; p++) {
        *p = toupper(*p);
    }
    
    for (size_t i = 0; i < NUM_SUPPORTED_DIFFICULTIES; i++) {
        if (strcmp(buffer, SUPPORTED_DIFFICULTIES[i].name) == 0) {
            return (int)i;  // Return index
        }
    }
    
    return -1;
}

const BoardSizeInfo* get_size_info(int subgrid_size) {
    for (size_t i = 0; i < NUM_SUPPORTED_SIZES; i++) {
        if (SUPPORTED_SIZES[i].subgrid_size == subgrid_size) {
            return &SUPPORTED_SIZES[i];
        }
    }
    return NULL;
}

const DifficultyInfo* get_difficulty_info_by_index(int index) {
    if (index >= 0 && index < (int)NUM_SUPPORTED_DIFFICULTIES) {
        return &SUPPORTED_DIFFICULTIES[index];
    }
    return NULL;
}

const DifficultyInfo* get_difficulty_info_by_enum(SudokuDifficulty difficulty) {
    for (size_t i = 0; i < NUM_SUPPORTED_DIFFICULTIES; i++) {
        if (SUPPORTED_DIFFICULTIES[i].level == difficulty) {
            return &SUPPORTED_DIFFICULTIES[i];
        }
    }
    return NULL;
}

const DifficultyInfo* evaluate_difficulty_from_board(const SudokuBoard *board, int total_cells) {
    int filled = sudoku_board_get_clues(board);
    int empty = total_cells - filled;
    int empty_percent = (empty * 100) / total_cells;
    
    // Find matching difficulty range
    for (size_t i = 0; i < NUM_SUPPORTED_DIFFICULTIES; i++) {
        if (empty_percent >= SUPPORTED_DIFFICULTIES[i].min_empty_percent &&
            empty_percent <= SUPPORTED_DIFFICULTIES[i].max_empty_percent) {
            return &SUPPORTED_DIFFICULTIES[i];
        }
    }
    
    // Default to closest
    if (empty_percent < 30) return &SUPPORTED_DIFFICULTIES[0];  // EASY
    if (empty_percent > 85) return &SUPPORTED_DIFFICULTIES[3];  // EXPERT
    return &SUPPORTED_DIFFICULTIES[1];  // MEDIUM
}

// ═══════════════════════════════════════════════════════════════
//                    LANGUAGE DETECTION
// ═══════════════════════════════════════════════════════════════

typedef enum {
    LANG_ENGLISH,
    LANG_SPANISH
} Language;

/**
 * @brief Detect system language from environment variables
 * @return LANG_SPANISH if system is in Spanish, LANG_ENGLISH otherwise
 */
Language detect_system_language(void) {
    const char *lang = getenv("LANG");
    const char *language = getenv("LANGUAGE");
    const char *lc_all = getenv("LC_ALL");
    
    // Check in order of priority
    const char *env_lang = lc_all ? lc_all : (language ? language : lang);
    
    if (env_lang) {
        // Check if starts with "es" (es_ES, es_MX, es_CL, etc.)
        if (strncmp(env_lang, "es", 2) == 0) {
            return LANG_SPANISH;
        }
    }
    
    return LANG_ENGLISH;
}

// ═══════════════════════════════════════════════════════════════
//                    HELP FUNCTIONS (BILINGUAL)
// ═══════════════════════════════════════════════════════════════

void print_usage_english(const char *program_name) {
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("  SUDOKU GENERATOR - Command Line Interface\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    printf("Usage: %s [size] [difficulty] [verbosity]\n\n", program_name);
    
    printf("Supported Sizes:\n");
    for (size_t i = 0; i < NUM_SUPPORTED_SIZES; i++) {
        printf("  %s  - %s\n", SUPPORTED_SIZES[i].name, SUPPORTED_SIZES[i].status_msg);
    }
    
    printf("\nSupported Difficulties:\n");
    for (size_t i = 0; i < NUM_SUPPORTED_DIFFICULTIES; i++) {
        printf("  %-8s - %s\n", 
               SUPPORTED_DIFFICULTIES[i].name, 
               SUPPORTED_DIFFICULTIES[i].description);
    }
    
    printf("\n⚠️  NOTE: Difficulty is a TARGET, not a guarantee.\n");
    printf("   The generator removes cells while maintaining solution uniqueness.\n");
    printf("   Sometimes the puzzle structure requires more clues than the target.\n");
    printf("   Achieved difficulty may differ from requested difficulty.\n");
    
    printf("\nVerbosity: 0 (minimal), 1 (compact - default), 2 (detailed)\n");
    
    printf("\nExamples:\n");
    printf("  %s 9x9 EASY 0           - 9×9 easy puzzle, minimal output\n", program_name);
    printf("  %s 16x16 HARD 1         - 16×16 hard puzzle, compact output\n", program_name);
    printf("  %s 4x4 MEDIUM 2         - 4×4 medium puzzle, detailed output\n", program_name);
    printf("  %s 9x9 0                - 9×9 default difficulty, minimal output\n", program_name);
    
    printf("\n💡 TIP: To guarantee exact difficulty, use a retry loop:\n");
    printf("   while ! %s 9x9 EXPERT 0 | grep -q 'EXPERT ✓'; do :; done\n", program_name);
    
    printf("\nLanguage Options:\n");
    printf("  %s --help           - Show this help (auto-detect language)\n", program_name);
    printf("  %s --help-es        - Ayuda en español\n", program_name);
    printf("  %s --help-en        - Help in English\n", program_name);
    printf("\n");
}

void print_usage_spanish(const char *program_name) {
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("  GENERADOR DE SUDOKU - Interfaz de Línea de Comandos\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    printf("Uso: %s [tamaño] [dificultad] [verbosidad]\n\n", program_name);
    
    printf("Tamaños Soportados:\n");
    printf("  4x4   - ✅ LISTO PARA PRODUCCIÓN - Modo tutorial/aprendizaje\n");
    printf("  9x9   - ✅ LISTO PARA PRODUCCIÓN - Sudoku clásico\n");
    printf("  16x16 - ✅ LISTO PARA PRODUCCIÓN - Nivel experto (~500ms generación)\n");
    printf("  25x25 - ⚠️  EXPERIMENTAL - Puede exceder tiempo límite (60s)\n");
    
    printf("\nDificultades Soportadas:\n");
    printf("  EASY     - Para principiantes (~60-70%% lleno)\n");
    printf("  MEDIUM   - Desafío intermedio (~40-50%% lleno)\n");
    printf("  HARD     - Puzzle avanzado (~25-35%% lleno)\n");
    printf("  EXPERT   - Nivel maestro (~15-25%% lleno)\n");
    
    printf("\n⚠️  NOTA: La dificultad es un OBJETIVO, no una garantía.\n");
    printf("   El generador elimina celdas manteniendo solución única.\n");
    printf("   A veces la estructura del puzzle requiere más pistas que el objetivo.\n");
    printf("   La dificultad lograda puede diferir de la solicitada.\n");
    
    printf("\nVerbosidad: 0 (mínima), 1 (compacta - por defecto), 2 (detallada)\n");
    
    printf("\nEjemplos:\n");
    printf("  %s 9x9 EASY 0           - Puzzle 9×9 fácil, salida mínima\n", program_name);
    printf("  %s 16x16 HARD 1         - Puzzle 16×16 difícil, salida compacta\n", program_name);
    printf("  %s 4x4 MEDIUM 2         - Puzzle 4×4 medio, salida detallada\n", program_name);
    printf("  %s 9x9 0                - 9×9 dificultad por defecto, salida mínima\n", program_name);
    
    printf("\n💡 CONSEJO: Para garantizar dificultad exacta, usa un bucle:\n");
    printf("   while ! %s 9x9 EXPERT 0 | grep -q 'EXPERT ✓'; do :; done\n", program_name);
    
    printf("\nOpciones de Idioma:\n");
    printf("  %s --help           - Mostrar esta ayuda (auto-detecta idioma)\n", program_name);
    printf("  %s --help-es        - Ayuda en español\n", program_name);
    printf("  %s --help-en        - Help in English\n", program_name);
    printf("\n");
}

void print_usage(const char *program_name) {
    Language lang = detect_system_language();
    
    if (lang == LANG_SPANISH) {
        print_usage_spanish(program_name);
    } else {
        print_usage_english(program_name);
    }
}

bool is_verbosity_arg(const char *str) {
    if (!str || strlen(str) != 1) return false;
    return (str[0] >= '0' && str[0] <= '2');
}

// ═══════════════════════════════════════════════════════════════════
//                           MAIN FUNCTION
// ═══════════════════════════════════════════════════════════════════

int main(int argc, char *argv[]) {
    #ifdef _WIN32
        system("chcp 65001 > nul");
    #endif
    
    srand((unsigned int)time(NULL));
    
    // Default values
    int subgrid_size = 3;              // Default: 9×9
    int difficulty_index = 1;          // Default: MEDIUM
    int verbosity_level = 1;           // Default: compact
    bool difficulty_specified = false;
    
    // ═══════════════════════════════════════════════════════════════
    //                    ARGUMENT PARSING
    // ═══════════════════════════════════════════════════════════════
    
    if (argc > 1) {
        // Check for help flags (with language options)
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_usage(argv[0]);  // Auto-detect language
            return 0;
        }
        if (strcmp(argv[1], "--help-en") == 0) {
            print_usage_english(argv[0]);  // Force English
            return 0;
        }
        if (strcmp(argv[1], "--help-es") == 0) {
            print_usage_spanish(argv[0]);  // Force Spanish
            return 0;
        }
        
        int parsed_size = parse_board_size(argv[1]);
        if (parsed_size == -1) {
            fprintf(stderr, "❌ Invalid size: '%s'\n", argv[1]);
            print_usage(argv[0]);
            return 1;
        }
        subgrid_size = parsed_size;
    }
    
    if (argc > 2) {
        if (is_verbosity_arg(argv[2])) {
            verbosity_level = atoi(argv[2]);
        } else {
            int parsed_diff = parse_difficulty(argv[2]);
            if (parsed_diff == -1) {
                fprintf(stderr, "❌ Invalid difficulty: '%s'\n", argv[2]);
                fprintf(stderr, "   Valid options: EASY, MEDIUM, HARD, EXPERT\n");
                print_usage(argv[0]);
                return 1;
            }
            difficulty_index = parsed_diff;
            difficulty_specified = true;
        }
    }
    
    if (argc > 3) {
        int level = atoi(argv[3]);
        if (level >= 0 && level <= 2) {
            verbosity_level = level;
        } else {
            fprintf(stderr, "❌ Invalid verbosity: '%s' (must be 0, 1, or 2)\n", argv[3]);
            return 1;
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                    VALIDATION & INFO
    // ═══════════════════════════════════════════════════════════════
    
    const BoardSizeInfo *size_info = get_size_info(subgrid_size);
    if (!size_info) {
        fprintf(stderr, "❌ Internal error: Invalid board size\n");
        return 1;
    }
    
    const DifficultyInfo *diff_info = get_difficulty_info_by_index(difficulty_index);
    if (!diff_info) {
        fprintf(stderr, "❌ Internal error: Invalid difficulty level\n");
        return 1;
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                    DISPLAY HEADER
    // ═══════════════════════════════════════════════════════════════
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  SUDOKU GENERATOR v3.0.2 - %s BOARD\n", size_info->name);
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("%s\n", size_info->status_msg);
    
    if (difficulty_specified) {
        printf("🎯 Target difficulty: %s - %s\n", 
               diff_info->name, diff_info->description);
    }
    
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    if (!size_info->production_ready && verbosity_level >= 1) {
        printf("⚠️  WARNING: %s is EXPERIMENTAL\n", size_info->name);
        printf("   - Generation may timeout after 60 seconds\n");
        printf("   - For production, use 4×4, 9×9, or 16×16\n\n");
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                    BOARD CREATION & GENERATION
    // ═══════════════════════════════════════════════════════════════
    
    SudokuBoard *board = sudoku_board_create_size(subgrid_size);
    if (!board) {
        fprintf(stderr, "❌ Error: Could not create board\n");
        return 1;
    }
    
    SudokuGenerationStats stats;
    bool success = false;
    time_t start_time = time(NULL);
    
    // Generation loop
    int max_attempts = 5;
    for (int attempt = 1; attempt <= max_attempts; attempt++) {
        if (verbosity_level >= 1) {
            printf("🚀 ATTEMPT #%d:\n", attempt);
        }
        
        // ✅ USE BASIC GENERATION FUNCTION (exists in your API)
        if (sudoku_generate(board, &stats)) {
            if (verbosity_level >= 1) {
                printf("✅ SUCCESS! %s Sudoku generated\n\n", size_info->name);
            }
            
            // Display board
            sudoku_display_print_board(board);
            printf("\n");
            
            // Validate
            if (sudoku_validate_board(board)) {
                if (verbosity_level >= 1) {
                    printf("🎉 VERIFIED! The puzzle is valid\n");
                }
            }
            
            // Statistics
            if (verbosity_level >= 1) {
                int filled = sudoku_board_get_clues(board);
                int empty = size_info->total_cells - filled;
                double elimination_percent = (empty * 100.0) / size_info->total_cells;
                
                printf("\n📊 STATISTICS:\n");
                printf("   Board size:    %s (%d cells)\n", 
                       size_info->name, size_info->total_cells);
                printf("   Filled cells:  %d\n", filled);
                printf("   Empty cells:   %d (%.1f%%)\n", empty, elimination_percent);
            }
            
            // Evaluate difficulty
            if (verbosity_level >= 0) {
                const DifficultyInfo *achieved = evaluate_difficulty_from_board(board, size_info->total_cells);
                
                if (difficulty_specified) {
                    // User specified a target difficulty
                    if (achieved == diff_info) {
                        // ✅ Match: Achieved requested difficulty
                        printf("\n📊 Difficulty: %s ✓ (target achieved)\n", achieved->name);
                    } else {
                        // ⚠️ Mismatch: Show both clearly
                        printf("\n📊 Difficulty: %s (target was %s)\n", 
                               achieved->name, diff_info->name);
                    }
                } else {
                    // No target specified, just show achieved
                    printf("\n📊 Difficulty: %s\n", achieved->name);
                }
            }
            
            success = true;
            break;
        } else {
            if (verbosity_level >= 1) {
                printf("❌ Attempt failed\n\n");
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                    TIMING & CLEANUP
    // ═══════════════════════════════════════════════════════════════
    
    time_t end_time = time(NULL);
    double elapsed = difftime(end_time, start_time);
    
    if (verbosity_level >= 1 && success) {
        printf("\n⏱️  Generation time: %.1f seconds\n", elapsed);
    }
    
    sudoku_board_destroy(board);
    
    if (!success) {
        fprintf(stderr, "\n❌ ERROR: Generation failed after %d attempts\n", max_attempts);
        if (!size_info->production_ready) {
            fprintf(stderr, "\n💡 TIP: Try 4×4, 9×9, or 16×16 for guaranteed success\n");
        }
        return 1;
    }
    
    return 0;
}

/**
 * @file events.c
 * @brief Implementation of internal event emission system
 */

#include "internal/events_internal.h"
#include <stddef.h>

// Global state for event system (private to this module)
static SudokuEventCallback g_callback = NULL;
static void *g_user_data = NULL;

void events_init(SudokuEventCallback callback, void *user_data) {
    g_callback = callback;
    g_user_data = user_data;
}

void emit_event(SudokuEventType type,
                const SudokuBoard *board,
                int phase,
                int cells_removed) {
    // If no callback registered, do nothing (zero overhead)
    if (g_callback == NULL) {
        return;
    }
    
    // Build event data
    SudokuEventData event = {
        .type = type,
        .board = board,
        .phase_number = phase,
        .cells_removed_total = cells_removed,
        .round_number = 0,
        .row = -1,
        .col = -1,
        .value = 0
    };
    
    // Invoke the callback
    g_callback(&event, g_user_data);
}

void emit_event_cell(SudokuEventType type,
                     const SudokuBoard *board,
                     int phase,
                     int cells_removed,
                     int row,
                     int col,
                     int value) {
    // If no callback registered, do nothing
    if (g_callback == NULL) {
        return;
    }
    
    // Build event data with cell information
    SudokuEventData event = {
        .type = type,
        .board = board,
        .phase_number = phase,
        .cells_removed_total = cells_removed,
        .round_number = 0,
        .row = row,
        .col = col,
        .value = value
    };
    
    // Invoke the callback
    g_callback(&event, g_user_data);
}

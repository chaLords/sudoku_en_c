/**
 * @file events_internal.h
 * @brief Internal event emission system for generation monitoring
 * @author Gonzalo Ramírez
 * @date 2025-11-13
 * 
 * This header provides the internal infrastructure for emitting events
 * during puzzle generation. Used by generator.c and elimination phases.
 * 
 * INTERNAL USE ONLY - Not part of public API
 */

#ifndef SUDOKU_EVENTS_INTERNAL_H
#define SUDOKU_EVENTS_INTERNAL_H

#include "sudoku/core/types.h"

/**
 * @brief Initialize the event system with a callback
 * 
 * Must be called before any event emission. Pass NULL to disable events.
 * 
 * @param callback Callback function to receive events (NULL to disable)
 * @param user_data Custom data to pass to callback
 * 
 * @internal Called by sudoku_generate_ex()
 */
void events_init(SudokuEventCallback callback, void *user_data);

/**
 * @brief Emit a simple event (no cell-specific data)
 * 
 * Use this for events that only need the event type and basic phase info.
 * 
 * @param type Event type
 * @param board Current board state (can be NULL for some events)
 * @param phase Phase number (1, 2, or 3; use 0 if not applicable)
 * @param cells_removed Total cells removed in this phase
 * 
 * @internal Used throughout generation code
 */
void emit_event(SudokuEventType type,
                const SudokuBoard *board,
                int phase,
                int cells_removed);

/**
 * @brief Emit a cell-specific event
 * 
 * Use this for events related to a specific cell (removal, testing, etc.)
 * 
 * @param type Event type
 * @param board Current board state
 * @param phase Phase number (1, 2, or 3)
 * @param cells_removed Total cells removed so far
 * @param row Row of the cell
 * @param col Column of the cell
 * @param value Value in the cell
 * 
 * @internal Used in elimination phases
 */
void emit_event_cell(SudokuEventType type,
                     const SudokuBoard *board,
                     int phase,
                     int cells_removed,
                     int row,
                     int col,
                     int value);

#endif // SUDOKU_EVENTS_INTERNAL_H

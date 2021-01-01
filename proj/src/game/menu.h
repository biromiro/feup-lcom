#ifndef MENU_H
#define MENU_H

#include <lcom/lcf.h>
#include "../periferals/video_gr.h"

/** @defgroup menu menu
 * @{
 *
 * Functions for changing menus
 */

/**
 * @brief Represents the Game's States
 * 
 */
enum {
    START,
    INSTRUCTIONS,
    GAME,
    GAMEOVER,
    EXIT,
    COOP
} typedef gameState;

/**
 * @brief Handles the mouse selections on the menu
 * 
 * @param cursor the current cursor
 */
void handle_menu_mouse(xpm_object *cursor);

/**
 * @brief Maps the current menu state to the game state
 * 
 * @return gameState 
 */
gameState return_game();

#endif

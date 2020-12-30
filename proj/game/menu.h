#ifndef MENU_H
#define MENU_H

#include <lcom/lcf.h>
#include "../periferals/video_gr.h"

enum {
    START,
    INSTRUCTIONS,
    GAME,
    GAMEOVER,
    EXIT,
    COOP
} typedef gameState;


void handle_menu_mouse(xpm_object *cursor);

gameState return_game();

#endif

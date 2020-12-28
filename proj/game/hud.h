#ifndef HUD_H
#define HUD_H

#include <lcom/lcf.h>
#include "../periferals/video_gr.h"
#include "enemies.h"
#include "magic_blast.h"
#include "game_state.h"
#include "../img/font/font.h"
#include "../img/hud_overlay_wave.xpm"
#include "../img/hud_overlay_score.xpm"
#include "../img/hud_overlay_charges.xpm"

void draw_current_hud();

void set_hud_values();

void draw_score();

void draw_waves();

void draw_charges();

void draw_digit(xpm_object* xpm);

void free_xpm_mem(xpm_object* xpm);

void reset_hud();

#endif

#ifndef HUD_H
#define HUD_H

#include <lcom/lcf.h>
#include "../peripherals/video_gr.h"
#include "enemies.h"
#include "magic_blast.h"
#include "game_state.h"
#include "../img/font/font.h"
#include "../img/hud_overlay_wave.xpm"
#include "../img/hud_overlay_score.xpm"
#include "../img/hud_overlay_charges.xpm"
#include "../peripherals/rtc.h"

/** @defgroup hud hud
 * @{
 *
 * Functions for dealing with the hud
 */

/**
 * @brief draws the current HUD
 * 
 */
void draw_current_hud();

/**
 * @brief Sets the hud values (score, ammo and wave) to their correct values
 * 
 */
void set_hud_values();

/**
 * @brief Draws the current score
 * 
 */
void draw_score();

/**
 * @brief Draws the current wave
 * 
 */
void draw_waves();

/**
 * @brief Draws the current charge
 * 
 */
void draw_charges();

/**
 * @brief Draws a given digit
 * 
 * @param xpm the digit to draw
 */
void draw_digit(xpm_object* xpm);

/**
 * @brief Frees the memory related to a given xpm
 * 
 * @param xpm the xpm to free
 */
void free_xpm_mem(xpm_object* xpm);

/**
 * @brief Resets the hud
 * 
 */
void reset_hud();

/**
 * @brief Draws the date to the screen
 * 
 */
void draw_date();

/**
 * @brief Draws the time to the screen
 * 
 */
void draw_time();

#endif

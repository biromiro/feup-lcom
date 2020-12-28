#ifndef ENEMIES_H
#define ENEMIES_H

#include <lcom/lcf.h>
#include "../periferals/video_gr.h"
#include "../img/Wraith_02/Spells_Effect.xpm"

/**
 * @brief Set up the enemies available (10)
 * 
 */
void set_enemies_available();

/**
 * @brief Free memory related to the enemies
 * 
 */
void free_enemies();

/**
 * @brief Creates a "Magic Blast" XPM, calculates its speed and
 * updates the available blasts
 */
void throw_enemies();

/**
 * @brief Prints the blasts
 * If a blast is off-screen, frees its memory and reindexes
 * the blasts
 */
void print_enemies();

/**
 * @brief Reindexes the magic blasts
 * 
 * @param positions Position of freed up blast object
 */
void reindex_enemies(size_t positions);

/**
 * @brief Checks collisions between active blasts and active enemies
 * and active enemies and main character
 * 
 * Frees the allocated memory of the respective blasts and enemies,
 * if there was in fact a collision
 * 
 * @return 0 if there is no collision between player&  enemy, 1 if there is  
 */
int checking_collision(xpm_object **magic_blasts);

/**
 * @brief Checks if there was a collision between an enemy and 
 * a projectile
 * 
 * @param magic_blasts Magic blast
 * @param enemy Enemy
 * 
 * @return 0 if there is no collision, 1 if there is 
 */
int enemy_collision(xpm_object *magic_blast, xpm_object *enemy);

uint8_t get_score();

void reset_enemies();

#endif

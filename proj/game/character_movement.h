#ifndef CHARACTER_MOVEMENT_H
#define CHARACTER_MOVEMENT_H

#include <lcom/lcf.h>
#include "../periferals/video_gr.h"
#include "../periferals/i8042.h"
#include "../periferals/serial_port.h"
#include "img/Wraith_01/Idle/Wraith_Idle.h"
#include "img/Wraith_01/Walking/Wraith_Walking.h"
#include "img/Wraith_01/Attacking/Wraith_Attack.h"

enum {
    START,
    INSTRUCTIONS,
    GAME,
    GAMEOVER,
    EXIT,
    COOP
} typedef gameState;

/**
 * @brief Creates the game objects (sprites)
 * 
 */
void create_game_objects();

/**
 * @brief Free memory related to the game objects
 * 
 */
void free_game_objects();

/**
 * @brief Updates the character movement
 * 
 * @param counter 
 * @return 0 on success 
 */
int update_character_movement(int counter);

int update_character_movement_player(int counter, animated_xpm_object*** player, animated_xpm_object** current_sprite, bool* attackAnimation);

/**
 * @brief Updates the sprites position, depending on
 * keyboard input
 * 
 * @param scancode Key Pressed (WASD)
 */
void handle_button_presses(uint8_t scancode, bool player1_press);

void handle_ingame_scancode(uint8_t scancode, animated_xpm_object*** player, animated_xpm_object** current_sprite, bool* attackAnimation, bool player1_pp);

/**
 * @brief Calls throw_magic_blast if LB is pressed
 * 
 * @param cursor XPM of mouse cursor
 * @param pp Mouse Packet
 */
void handle_mouse_packet(xpm_object* cursor, struct packet *pp, bool player1);

void handle_mouse_packet_player(xpm_object* cursor, struct packet *pp, animated_xpm_object*** player, animated_xpm_object** current_sprite, bool* attackAnimation, bool* sent, bool player1_pp);

/**
 * @brief Get the current character object
 * 
 * @return xpm_object* of current character
 */
xpm_object* get_current_character_p1();

xpm_object* get_current_character_p2();

/**
 * @brief Checks if the character has hit a wall
 * 
 * @param current_sprite Character sprite
 * 
 * @return 0 if there is no collision, 1 if there is 
 */
int wall_collision(animated_xpm_object *current_sprite);

void swap_characters();

#endif

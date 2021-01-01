#ifndef CHARACTER_MOVEMENT_H
#define CHARACTER_MOVEMENT_H

#include <lcom/lcf.h>
#include "../periferals/video_gr.h"
#include "../periferals/i8042.h"
#include "../periferals/serial_port.h"
#include "../periferals/rtc.h"
#include "magic_blast.h"
#include "menu.h"
#include "img/Wraith_01/Idle/Wraith_Idle.h"
#include "img/Wraith_01/Walking/Wraith_Walking.h"
#include "img/Wraith_01/Attacking/Wraith_Attack.h"
#include "img/Wraith_02/Attacking/Wraith_Attack.h"
#include "img/Wraith_02/Idle/Wraith_Idle.h"
#include "img/Wraith_02/Walking/Wraith_Walking.h"

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
 * @param counter the timer counter in ticks
 * @return 0 on success 
 */
int update_character_movement(int counter);


/**
 * @brief Updates the movement of a specific player
 * 
 * @param counter the time counter in ticks
 * @param player the player to update
 * @param current_sprite the current sprite of the player to update
 * @param attackAnimation defines if the player is attacking or not
 * @return 0 on success
 */
int update_character_movement_player(int counter, animated_xpm_object*** player, animated_xpm_object** current_sprite, bool* attackAnimation);

/**
 * @brief Updates the sprites position, depending on
 * keyboard input
 * 
 * @param scancode Key Pressed (WASD)
 * @param player1_press defines if it was the player1 who pressed a key or not
 */
void handle_button_presses(uint8_t scancode, bool player1_press);

/**
 * @brief Updates the sprite position of a given player, depending on keyboard input and sends, if needed, the information to the serial port
 * 
 * @param scancode Key pressed (WASD)
 * @param player the player who pressed the key
 * @param current_sprite the current sprite of the player who pressed the key
 * @param attackAnimation defines if the player is attacking or not
 * @param player1_pp defines if it was the player1 who pressed a key or not
 */
void handle_ingame_scancode(uint8_t scancode, animated_xpm_object*** player, animated_xpm_object** current_sprite, bool* attackAnimation, bool player1_pp);

/**
 * @brief Handles mouse movement and button presses for all characters and sends, if needed, the information to the serial port
 * 
 * @param cursor XPM of mouse cursor
 * @param pp Mouse Packet
 * @param player1 defines if it was the player1 who pressed a key or not
 */
void handle_mouse_packet(xpm_object* cursor, struct packet *pp, bool player1_pp);

/**
 * @brief Handles mouse movement and button presses for a given characters
 * 
 * @param cursor the cursor of the current character 
 * @param pp the packet to unpack
 * @param player the player whose movement is to be handled
 * @param current_sprite the current sprite of the current player
 * @param attackAnimation defines if the player is attacking or not
 * @param sent defines if a magic ball was already sent in this button press
 * @param player1_pp defines if it was the player1 who pressed a key or not  
 */
void handle_mouse_packet_player(xpm_object* cursor, struct packet *pp, animated_xpm_object*** player, animated_xpm_object** current_sprite, bool* attackAnimation, bool* sent, bool player1_pp);

/**
 * @brief Get the current character object
 * 
 * @return xpm_object* of current character
 */
xpm_object* get_current_character_p1();

/**
 * @brief Get the current character p2 object
 * 
 * @return xpm_object* 
 */
xpm_object* get_current_character_p2();

/**
 * @brief Checks if the character has hit a wall
 * 
 * @param current_sprite Character sprite
 * 
 * @return 0 if there is no collision, 1 if there is 
 */
int wall_collision(animated_xpm_object *current_sprite);

/**
 * @brief swaps character animated_xpm_object pointers (for multiplayer)
 * 
 */
void swap_characters();

/**
 * @brief resets character's starting position, speed and current sprite
 * 
 */
void reset_characters();

#endif

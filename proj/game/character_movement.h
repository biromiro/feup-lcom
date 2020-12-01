#include <lcom/lcf.h>
#include "../periferals/video_gr.h"
#include "../periferals/i8042.h"
#include "img/Wraith_01/Idle/Wraith_Idle.h"
#include "img/Wraith_01/Walking/Wraith_Walking.h"
#include "img/Wraith_01/Attacking/Wraith_Attack.h"


/**
 * @brief Creates the game objects (sprites)
 * 
 */
void create_game_objects();

/**
 * @brief Updates the character movement
 * 
 * @param counter 
 * @return 0 on success 
 */
int update_character_movement(int counter);

/**
 * @brief Updates the sprites position, depending on
 * keyboard input
 * 
 * @param scancode Key Pressed (WASD)
 */
void handle_button_presses(uint8_t scancode);

/**
 * @brief Calls throw_magic_blast if LB is pressed
 * 
 * @param cursor XPM of mouse cursor
 * @param pp Mouse Packet
 */
void handle_mouse_packet(xpm_object* cursor, struct packet *pp);

/**
 * @brief Get the current character object
 * 
 * @return xpm_object* of current character
 */
xpm_object* get_current_character();

/**
 * @brief Checks if the character has hit a wall
 * 
 * @param current_sprite Character sprite
 * 
 * @return 0 if there is no collision, 1 if there is 
 */
int wall_collision(animated_xpm_object *current_sprite);

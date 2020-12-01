#include <lcom/lcf.h>
#include "../periferals/video_gr.h"
#include "../periferals/i8042.h"
#include "img/Wraith_01/Idle/Wraith_Idle.h"
#include "img/Wraith_01/Walking/Wraith_Walking.h"
#include "img/Wraith_01/Attacking/Wraith_Attack.h"

void create_game_objects();

int update_character_movement(int counter);

void handle_button_presses(uint8_t scancode);

void handle_mouse_packet(xpm_object* cursor, struct packet *pp);

xpm_object* get_current_character();

#include "character_movement.h"
#include "../periferals/video_gr.h"
#include "../periferals/i8042.h"

static animated_xpm_object **object;
static animated_xpm_object *current_sprite;
static bool mirrored = false;

void create_game_objects(){
  object = malloc(sizeof(animated_xpm_object*)*2);
  object[0] = create_animated_sprite(wraithIdle, sizeof(wraithIdle)/sizeof(const char *), "Wraith Idle", 400, 400, 4);
  object[1] = create_animated_sprite(wraithWalking, sizeof(wraithWalking)/sizeof(const char *), "Wraith Walking", 0, 0, 4);

  current_sprite = object[0];
}

int update_character_movement(int counter){
    if(counter%2 == 0){
        current_sprite->cur_speed += 1;

        if(current_sprite->obj->speed != 0) current_sprite->obj->x += current_sprite->obj->speed;
        print_animated_sprite(current_sprite, mirrored);

        if(OK != swap_buffer()){
            printf("Unable to swap buffers!");
            return 1;
        }
    }
    return 0;
}

void handle_button_presses(uint8_t scancode){

  switch (scancode){
  case KBC_MK_D_KEY:
      object[1]->obj->x = current_sprite->obj->x;
      object[1]->obj->y = current_sprite->obj->y;

      current_sprite = object[1];
      current_sprite->obj->speed = 4;
      mirrored = false;      
      break;

  case KBC_MK_A_KEY:
      object[1]->obj->x = current_sprite->obj->x;
      object[1]->obj->y = current_sprite->obj->y;

      current_sprite = object[1];
      current_sprite->obj->speed = -4;
      mirrored = true;
      break;
  case KBC_BRK_A_KEY:
  case KBC_BRK_D_KEY:
      object[0]->obj->x = current_sprite->obj->x;
      object[0]->obj->y = current_sprite->obj->y;

      current_sprite = object[0];
      current_sprite->obj->speed = 0;
      break;
  default:
      break;
  }
}

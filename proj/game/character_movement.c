#include "character_movement.h"
#include "magic_blast.h"

static animated_xpm_object **object;
static animated_xpm_object *current_sprite;
static bool mirrored = false, sent = false, attackAnimation = false;

void create_game_objects(){
  object = malloc(sizeof(animated_xpm_object*)*3);
  object[0] = create_animated_sprite(wraithIdle, sizeof(wraithIdle)/sizeof(const char *), "Wraith Idle", 400, 400, 4);
  object[1] = create_animated_sprite(wraithWalking, sizeof(wraithWalking)/sizeof(const char *), "Wraith Walking", 0, 0, 4);
  object[2] = create_animated_sprite(wraithAttack, sizeof(wraithAttack)/sizeof(const char *), "Wraith Attacking", 0, 0,4);
  current_sprite = object[0];
}

int update_character_movement(int counter){
    current_sprite->cur_speed += 1;

    if(attackAnimation && ((current_sprite->cur_fig + 1) == current_sprite->num_fig)){
      current_sprite->cur_fig = 0;
      object[0]->obj->x = current_sprite->obj->x;
      object[0]->obj->y = current_sprite->obj->y;

      current_sprite = object[0];
      attackAnimation = false;
    }
    if(current_sprite->obj->x_speed != 0) current_sprite->obj->x += current_sprite->obj->x_speed;
    print_animated_sprite(current_sprite, mirrored);

    return 0;
}

void handle_button_presses(uint8_t scancode){

  switch (scancode){
  case KBC_MK_D_KEY:
      object[1]->obj->x = current_sprite->obj->x;
      object[1]->obj->y = current_sprite->obj->y;

      current_sprite = object[1];
      current_sprite->obj->x_speed = 4;
      mirrored = false;      
      break;

  case KBC_MK_A_KEY:
      object[1]->obj->x = current_sprite->obj->x;
      object[1]->obj->y = current_sprite->obj->y;

      current_sprite = object[1];
      current_sprite->obj->x_speed = -4;
      mirrored = true;
      break;
  case KBC_BRK_A_KEY:
  case KBC_BRK_D_KEY:
      object[0]->obj->x = current_sprite->obj->x;
      object[0]->obj->y = current_sprite->obj->y;

      current_sprite = object[0];
      current_sprite->obj->x_speed = 0;
      break;
  default:
      break;
  }
}

void handle_mouse_packet(xpm_object* cursor, struct packet *pp){
    if(pp->lb){
        if(!sent){
            throw_magic_blast(cursor, get_current_character());
            object[2]->obj->x = current_sprite->obj->x;
            object[2]->obj->y = current_sprite->obj->y;

            current_sprite = object[2];
            sent = true;
            attackAnimation = true;
        }
    }else{
        sent = false;
    }
}

xpm_object* get_current_character(){
    return current_sprite->obj;
}

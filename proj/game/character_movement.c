#include "character_movement.h"
#include "magic_blast.h"

static animated_xpm_object **object;
static animated_xpm_object *current_sprite;
gameState gs;
static bool sent = false, attackAnimation = false;

void create_game_objects() {
  object = malloc(sizeof(animated_xpm_object *) * 3);
  object[0] = create_animated_sprite(wraithIdle, sizeof(wraithIdle) / sizeof(const char *), "Wraith Idle", get_h_resolution()/2 - 150, get_v_resolution() - 350,3);
  object[1] = create_animated_sprite(wraithWalking, sizeof(wraithWalking) / sizeof(const char *), "Wraith Walking", 0, 0, 3);
  object[2] = create_animated_sprite(wraithAttack, sizeof(wraithAttack) / sizeof(const char *), "Wraith Attacking", 0, 0, 3);
  current_sprite = object[0];
}

void free_game_objects(){
  for(size_t i=0; i<3; i++){
    free(object[i]);
  }
  free(object);
}

int update_character_movement(int counter) {
  current_sprite->cur_speed += 1;

  if (attackAnimation && ((current_sprite->cur_fig + 1) == current_sprite->num_fig)) {
    current_sprite->cur_fig = 0;
    object[0]->obj->x = current_sprite->obj->x;
    object[0]->obj->y = current_sprite->obj->y;
    object[0]->obj->mirrored = current_sprite->obj->mirrored;

    current_sprite = object[0];
    attackAnimation = false;
  }
  if (current_sprite->obj->x_speed != 0)
    if (!wall_collision(current_sprite))
      current_sprite->obj->x += current_sprite->obj->x_speed;
  print_animated_sprite(current_sprite);

  return 0;
}

void handle_button_presses(uint8_t scancode) {
    if(gs==GAMEOVER || gs==INSTRUCTIONS){
      gs=START;
    }
    else if(gs==GAME){
      switch (scancode) {
        case KBC_MK_D_KEY:
          object[1]->obj->x = current_sprite->obj->x;
          object[1]->obj->y = current_sprite->obj->y;
          if(!attackAnimation){
            current_sprite = object[1];
          }
          current_sprite->obj->x_speed = 8;
          current_sprite->obj->mirrored = false;
          break;

        case KBC_MK_A_KEY:
          object[1]->obj->x = current_sprite->obj->x;
          object[1]->obj->y = current_sprite->obj->y;
          if(!attackAnimation){
              current_sprite = object[1];
          }
          current_sprite->obj->x_speed = -8;
          current_sprite->obj->mirrored = true;
          break;
        case KBC_BRK_A_KEY:
        case KBC_BRK_D_KEY:
          object[0]->obj->x = current_sprite->obj->x;
          object[0]->obj->y = current_sprite->obj->y;
          object[0]->obj->mirrored = current_sprite->obj->mirrored;
          if(!attackAnimation){
            current_sprite = object[0];
          }
          current_sprite->obj->x_speed = 0;
          break;
        default:
          break;
      }
    }
    
}

void handle_mouse_packet(xpm_object *cursor, struct packet *pp) {
  if (pp->lb) {
    if(gs==START){
      if(cursor->x<=690 && cursor->x>=460 && cursor->y>=425 && cursor->y<=485){
        gs=GAME;
      }
      else if(cursor->x<=820 && cursor->x>=325 && cursor->y>=505 && cursor->y<=565){
        gs=INSTRUCTIONS;
      }else if(cursor->x<=650 && cursor->x>=495 && cursor->y>=595 && cursor->y<=655){
        gs=EXIT;
      }

      
    }
    else if(gs==GAME){
      if (!sent) {
        throw_magic_blast(cursor, get_current_character());
        object[2]->obj->x = current_sprite->obj->x;
        object[2]->obj->y = current_sprite->obj->y;
        object[2]->obj->mirrored = current_sprite->obj->mirrored;

        current_sprite = object[2];
        current_sprite->obj->x_speed = 0;

        sent = true;
        attackAnimation = true;
      }
    }
  }
  else {
    sent = false;
  }
}

xpm_object *get_current_character() {
  return current_sprite->obj;
}

int wall_collision(animated_xpm_object *current_sprite) {
  if((current_sprite->obj->x < 0 && !(current_sprite->obj->mirrored)) ||
   ((current_sprite->obj->x + (current_sprite->obj->img).width) > get_h_resolution() && current_sprite->obj->mirrored)) return 0;
  for (size_t i = 0; i < (current_sprite->obj->img).height; i++) {
    for (size_t k = 0; k < (current_sprite->obj->img).width; k++) {
      uint32_t color = 0;
       size_t j = 0;
      if(current_sprite->obj->mirrored) j = (current_sprite->obj->img).width - k - 1;
      else j = k; 
      for (size_t byte = 0; byte < get_bytes_per_pixel(); byte++) {
        color |= (*(current_sprite->obj->map + (j + i * (current_sprite->obj->img).width) * (get_bytes_per_pixel()) + byte)) << (byte * 8);
      }
      if (color == xpm_transparency_color((current_sprite->obj->img).type))
        continue;
      if ((k + current_sprite->obj->x) + current_sprite->obj->x_speed > get_h_resolution())
        return 1;
      if ((k + current_sprite->obj->x) + current_sprite->obj->x_speed <= 0)
        return 1;
    }
  }
  return 0;
}

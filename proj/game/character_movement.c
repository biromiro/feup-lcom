#include "character_movement.h"
#include "magic_blast.h"

static animated_xpm_object **player1, **player2;
static animated_xpm_object *current_sprite_player1, *current_sprite_player2;
gameState gs;
static bool sentP1 = false, sentP2 = false, attackAnimationP1 = false, attackAnimationP2 = false;
bool coop = false;

void create_game_objects() {
  player1 = malloc(sizeof(animated_xpm_object *) * 3);
  player2 = malloc(sizeof(animated_xpm_object *) * 3);
  player1[0] = create_animated_sprite(wraithIdle, sizeof(wraithIdle) / sizeof(const char *), "Wraith Idle 1", get_h_resolution()/2 - 300, get_v_resolution() - 350,3);
  player1[1] = create_animated_sprite(wraithWalking, sizeof(wraithWalking) / sizeof(const char *), "Wraith Walking 1", 0, 0, 3);
  player1[2] = create_animated_sprite(wraithAttack, sizeof(wraithAttack) / sizeof(const char *), "Wraith Attacking 1", 0, 0, 3);
  player2[0] = create_animated_sprite(wraithIdle, sizeof(wraithIdle) / sizeof(const char *), "Wraith Idle 2", get_h_resolution()/2 + 100, get_v_resolution() - 350,3);
  player2[1] = create_animated_sprite(wraithWalking, sizeof(wraithWalking) / sizeof(const char *), "Wraith Walking 2", 0, 0, 3);
  player2[2] = create_animated_sprite(wraithAttack, sizeof(wraithAttack) / sizeof(const char *), "Wraith Attacking 2", 0, 0, 3);
  current_sprite_player1 = player1[0];
  current_sprite_player2 = player2[0];
}

void free_game_objects(){
  for(size_t i=0; i<3; i++){
    free(player1[i]);
    free(player2[i]);
  }
  free(player1);
  free(player2);
}

int update_character_movement(int counter) {
  if(coop) update_character_movement_player(counter, &player2, &current_sprite_player2, &attackAnimationP2);
  update_character_movement_player(counter, &player1, &current_sprite_player1, &attackAnimationP1);
  return 0;
}

int update_character_movement_player(int counter, animated_xpm_object*** player, animated_xpm_object** current_sprite, bool* attackAnimation){
  (*current_sprite)->cur_speed += 1;

  if (*attackAnimation && (((*current_sprite)->cur_fig + 1) == (*current_sprite)->num_fig)) {
    (*current_sprite)->cur_fig = 0;
    (*player)[0]->obj->x = (*current_sprite)->obj->x;
    (*player)[0]->obj->y = (*current_sprite)->obj->y;
    (*player)[0]->obj->mirrored = (*current_sprite)->obj->mirrored;

    (*current_sprite) = (*player)[0];
    *attackAnimation = false;
  }
  if ((*current_sprite)->obj->x_speed != 0){
    if (!wall_collision((*current_sprite))) (*current_sprite)->obj->x += (*current_sprite)->obj->x_speed;
  }
  print_animated_sprite((*current_sprite));
  return OK;
}


void handle_button_presses(uint8_t scancode, bool player1_pp) {
  if(player1_pp){
    if(gs==GAMEOVER || gs==INSTRUCTIONS){
      gs=START;
    }
    else if(gs==GAME){
      handle_ingame_scancode(scancode, &player1, &current_sprite_player1, &attackAnimationP1, player1_pp);
    }
  }else handle_ingame_scancode(scancode, &player2, &current_sprite_player2, &attackAnimationP2, player1_pp);
}

void handle_ingame_scancode(uint8_t scancode, animated_xpm_object*** player, animated_xpm_object** current_sprite, bool *attackAnimation, bool player1_pp){
  switch (scancode) {
    case KBC_MK_D_KEY:
      (*player)[1]->obj->x = (*current_sprite)->obj->x;
      (*player)[1]->obj->y = (*current_sprite)->obj->y;
      if(!(*attackAnimation)){
        (*current_sprite) = (*player)[1];
      }
      (*current_sprite)->obj->x_speed = 8;
      (*current_sprite)->obj->mirrored = false;
      break;

    case KBC_MK_A_KEY:
      (*player)[1]->obj->x = (*current_sprite)->obj->x;
      (*player)[1]->obj->y = (*current_sprite)->obj->y;
      if(!(*attackAnimation)){
          (*current_sprite) = (*player)[1];
      }
      (*current_sprite)->obj->x_speed = -8;
      (*current_sprite)->obj->mirrored = true;
      break;
    case KBC_BRK_A_KEY:
    case KBC_BRK_D_KEY:
      (*player)[0]->obj->x = (*current_sprite)->obj->x;
      (*player)[0]->obj->y = (*current_sprite)->obj->y;
      (*player)[0]->obj->mirrored = (*current_sprite)->obj->mirrored;
      if(!(*attackAnimation)){
        (*current_sprite) = (*player)[0];
      }
      (*current_sprite)->obj->x_speed = 0;
      break;
    default:
      return;
  }
  if(player1_pp && coop) send_scancode(scancode);
}

void handle_mouse_packet(xpm_object *cursor, struct packet *pp, bool player1_pp) {
  if(player1_pp){
    if (pp->lb) {
      if(gs==START){
        if(cursor->x<=690 && cursor->x>=460 && cursor->y>=425 && cursor->y<=485){
          gs=GAME;
        }
        else if(cursor->x<=820 && cursor->x>=325 && cursor->y>=505 && cursor->y<=565){
          gs=INSTRUCTIONS;
        }else if(cursor->x<=650 && cursor->x>=495 && cursor->y>=595 && cursor->y<=655){
          gs=EXIT;
        }else if(cursor->x<=820 && cursor->x>=325 && cursor->y>=700){
          gs=COOP;
          send_byte(0x53);
        }
      }else if(gs == GAME){
        if(!sentP1 && coop) send_mouse_info(cursor);
      }
    }
    handle_mouse_packet_player(cursor, pp, &player1, &current_sprite_player1, &attackAnimationP1, &sentP1, player1_pp);
  }else{
    handle_mouse_packet_player(cursor, pp, &player2, &current_sprite_player2, &attackAnimationP2, &sentP2, player1_pp);
    sentP2 = false;
  }
}

void handle_mouse_packet_player(xpm_object* cursor, struct packet *pp, animated_xpm_object*** player, animated_xpm_object** current_sprite, bool* attackAnimation, bool* sent, bool player1_pp){
  if(pp->lb){
    if(gs==GAME){
      if (!(*sent)) {
        if(player1_pp) throw_magic_blast(cursor, get_current_character_p1());
        else throw_magic_blast(cursor, get_current_character_p2());
        (*player)[2]->obj->x = (*current_sprite)->obj->x;
        (*player)[2]->obj->y = (*current_sprite)->obj->y;
        (*player)[2]->obj->mirrored = (*current_sprite)->obj->mirrored;

        (*current_sprite) = (*player)[2];
        (*current_sprite)->obj->x_speed = 0;

        *sent = true;
        *attackAnimation = true;
      }
    }
  }
  else {
    *sent = false;
  }
}

xpm_object *get_current_character_p1() {
  return current_sprite_player1->obj;
}

xpm_object *get_current_character_p2() {
  return current_sprite_player2->obj;
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

void swap_characters(){
  animated_xpm_object **temp = player1;
  animated_xpm_object *temp2 = current_sprite_player1;
  bool temp3 = sentP1, temp4 = attackAnimationP1;
  player1 = player2;
  player2 = temp;
  current_sprite_player1 = current_sprite_player2;
  current_sprite_player2 = temp2;
  sentP1 = sentP2;
  sentP2 = temp3;
  attackAnimationP1 = attackAnimationP2;
  attackAnimationP2 = temp4;
}

#include "magic_blast.h"
#include "character_movement.h"
#include <math.h>

static xpm_object **magic_blasts ;
static size_t total_blasts = 10;
static size_t available_blasts;
static int PROJECTILE_VELOCITY = 8;

void set_magic_blasts_available() {
  available_blasts = total_blasts;
  magic_blasts = malloc(sizeof(xpm_object *) * total_blasts);
}

void free_magic_blasts(){
  for(size_t i=0; i<(total_blasts-available_blasts); i++){
    free(magic_blasts[i]);
  }
  free(magic_blasts);
}

void throw_magic_blast(xpm_object *cursor, xpm_object *character) {
  if (available_blasts == 0)
    return;

  int x_offset = (character->mirrored) ? 90 : (160);
  int y_offset = 215; 
  xpm_object *new_magic_blast = create_sprite(Spells_Effect, "Magic Blast", character->x + x_offset, character->y + y_offset);

  int dx = (cursor->x - (character->x + x_offset));
  int dy = (cursor->y - (character->y + y_offset));

  double alpha = atan2((double)dy, (double)dx);
  
  new_magic_blast->x_speed =(int)(cos(alpha)*(PROJECTILE_VELOCITY));
  new_magic_blast->y_speed =(int)(sin(alpha)*(PROJECTILE_VELOCITY));

  magic_blasts[total_blasts - available_blasts] = new_magic_blast;
  available_blasts--;
}

void print_magic_blasts() {

  for (size_t i = 0; i < (total_blasts - available_blasts); i++) {

    if ((magic_blasts[i]->x < -20) || (magic_blasts[i]->x > (get_h_resolution() + 20)) || (magic_blasts[i]->y < -20) || (magic_blasts[i]->y > (get_v_resolution() + 20))) {
      free(magic_blasts[i]);
      reindex_magic_blasts(i);
      i--;
      continue;
    }
    print_xpm(magic_blasts[i]);

    magic_blasts[i]->x += magic_blasts[i]->x_speed;
    magic_blasts[i]->y += magic_blasts[i]->y_speed;
  }
}

void reindex_magic_blasts(size_t position) {
  for (size_t i = position; i < total_blasts - 1; i++)
    magic_blasts[i] = magic_blasts[i + 1];
  available_blasts++;
}

xpm_object **get_magic_blasts() {
  return magic_blasts;
}

size_t get_total_blasts() {
  return total_blasts;
}

size_t get_available_blasts() {
  return available_blasts;
}

void decrement_available_blasts() {
  available_blasts--;
}

void reset_magic_blasts(){
  free_magic_blasts();
  set_magic_blasts_available();
}

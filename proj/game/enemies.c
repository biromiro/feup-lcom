#include "enemies.h"

#include "character_movement.h"
#include "magic_blast.h"

static xpm_object **enemies;
static size_t total_enemies = 10;
static size_t available_enemies;
static int ENEMIES_MAX_VELOCITY = 5;

void set_enemies_available() {
  srand(time(NULL));
  available_enemies = total_enemies;
  enemies = malloc(sizeof(xpm_object *) * total_enemies);
}

void throw_enemies() {
  if (available_enemies == 0)
    return;
  int pos_x = rand() % (get_h_resolution() + 1);
  xpm_object *new_enemies = create_sprite(Enemy, "Enemy", pos_x, rand() % 10 - 10);

  new_enemies->x_speed = pos_x <= (get_h_resolution() / 2.0) ? rand() % ENEMIES_MAX_VELOCITY : rand() % ENEMIES_MAX_VELOCITY - 5;
  new_enemies->y_speed = 2;

  enemies[total_enemies - available_enemies] = new_enemies;
  available_enemies--;
}

void print_enemies() {

  for (size_t i = 0; i < (total_enemies - available_enemies); i++) {

    if ((enemies[i]->x < -95) || (enemies[i]->x > (get_h_resolution() + 1)) || (enemies[i]->y < -20) || (enemies[i]->y > (get_v_resolution() + 20))) {
      free(enemies[i]);
      reindex_enemies(i);
      i--;
      continue;
    }
    print_xpm(enemies[i], false);

    enemies[i]->x += enemies[i]->x_speed;
    enemies[i]->y += enemies[i]->y_speed;
  }
}

void reindex_enemies(size_t position) {
  for (size_t i = position; i < total_enemies - 1; i++)
    enemies[i] = enemies[i + 1];
  available_enemies++;
}

void checking_collision(xpm_object **magic_blasts) {

  bool collided = false;

  for (size_t blastIndex = 0; blastIndex < (get_total_blasts() - get_available_blasts()); blastIndex++) {
    for (size_t enemiesIndex = 0; enemiesIndex < total_enemies - available_enemies; enemiesIndex++) {
      if (blast_collision(magic_blasts[blastIndex], enemies[enemiesIndex])) {
        free(enemies[enemiesIndex]);
        reindex_enemies(enemiesIndex);
        collided = true;
        break;
      }
    }
    if (collided) {
      free(magic_blasts[blastIndex]);
      reindex_magic_blasts(blastIndex);
      collided = false;
      blastIndex--;
    }
  }
}

int blast_collision(xpm_object *magic_blast, xpm_object *enemy) {

  if ( // condition where there will be no collision from the get go
    magic_blast->x + (magic_blast->img).width < enemy->x ||
    enemy->x + (enemy->img).width < magic_blast->x ||
    magic_blast->y + (magic_blast->img).height < enemy->y ||
    enemy->y + (enemy->img).height < magic_blast->y)
    return 0;

  // Calculating search area (for optimization purposes)

  size_t offsetX_magic = 0, offsetY_magic = 0, offsetX_enemy = 0, offsetY_enemy = 0, search_width = 0, search_height = 0;

  if ((magic_blast->x <= enemy->x) && magic_blast->x + (magic_blast->img).width > enemy->x) { // Start of X of enemy is inside of blast
    search_width = magic_blast->x + (magic_blast->img).width - enemy->x;
    offsetX_magic = (magic_blast->img).width - search_width;
  }
  else if ((enemy->x <= magic_blast->x) && enemy->x + (enemy->img).width > magic_blast->x) { // Start of X of blast is inside of enemy
    search_width = enemy->x + (enemy->img).width - magic_blast->x;
    offsetX_enemy = (enemy->img).width - search_width;
  }

  if ((magic_blast->y <= enemy->y) && magic_blast->y + (magic_blast->img).height > enemy->y) { // Start of Y of enemy is inside of blast
    search_height = magic_blast->y + (magic_blast->img).height - enemy->y;
    offsetY_magic = (magic_blast->img).height - search_height;
  }
  else if ((enemy->y <= magic_blast->y) && enemy->y + (enemy->img).height > magic_blast->y) { // Start of Y of blast is inside of enemy
    search_height = enemy->y + (enemy->img).height - magic_blast->y;
    offsetY_enemy = (enemy->img).height - search_height;
  }

  for (size_t i = 0; i < search_height; i++) {
    for (size_t j = 0; j < search_width; j++) {
      uint32_t color_blast = 0, color_enemy = 0;
      for (size_t byte = 0; byte <= get_bytes_per_pixel(); byte++) {
        color_blast |= (*(magic_blast->map + ((j + offsetX_magic) + (i + offsetY_magic) * (magic_blast->img).width) * (get_bytes_per_pixel()) + byte)) << (byte * 8);
        color_enemy |= (*(enemy->map + ((j + offsetX_enemy) + (i + offsetY_enemy) * (enemy->img).width) * (get_bytes_per_pixel()) + byte)) << (byte * 8);
      }
      if (color_blast != xpm_transparency_color((magic_blast->img).type) &&
          color_enemy != xpm_transparency_color((enemy->img).type))
        return 1;
    }
  }

  return 0;
}

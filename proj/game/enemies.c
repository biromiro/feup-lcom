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
    print_xpm(enemies[i]);

    enemies[i]->x += enemies[i]->x_speed;
    enemies[i]->y += enemies[i]->y_speed;
  }
}

void reindex_enemies(size_t position) {
  for (size_t i = position; i < total_enemies - 1; i++)
    enemies[i] = enemies[i + 1];
  available_enemies++;
}

int checking_collision(xpm_object **magic_blasts) {

  bool collided = false;
  for (size_t enemiesIndex = 0; enemiesIndex < total_enemies - available_enemies; enemiesIndex++) {
    for (size_t blastIndex = 0; blastIndex < (get_total_blasts() - get_available_blasts()); blastIndex++) {
      if (enemy_collision(magic_blasts[blastIndex], enemies[enemiesIndex])) {
        free(magic_blasts[blastIndex]);
        reindex_magic_blasts(blastIndex);
        collided = true;
        break;
      }
    }

    if (collided) {
      free(enemies[enemiesIndex]);
      reindex_enemies(enemiesIndex);
      collided = false;
      enemiesIndex--;
      continue;
    }
    if (enemy_collision(get_current_character(), enemies[enemiesIndex])) {
      free(enemies[enemiesIndex]);
      reindex_enemies(enemiesIndex);
      return 1;
    }
  }
  return 0;
}

int enemy_collision(xpm_object *object, xpm_object *enemy) {

  if ( // condition where there will be no collision from the get go
    object->x + (object->img).width < enemy->x ||
    enemy->x + (enemy->img).width < object->x ||
    object->y + (object->img).height < enemy->y ||
    enemy->y + (enemy->img).height < object->y)
    return 0;

  // Calculating search area (for optimization purposes)

  size_t offsetX_magic = 0, offsetY_magic = 0, offsetX_enemy = 0, offsetY_enemy = 0, search_width = 0, search_height = 0;

  if ((object->x <= enemy->x) && object->x + (object->img).width > enemy->x) { // Start of X of enemy is inside of blast
    search_width = object->x + (object->img).width - enemy->x;
    offsetX_magic = (object->img).width - search_width;
  }
  else if ((enemy->x <= object->x) && enemy->x + (enemy->img).width > object->x) { // Start of X of blast is inside of enemy
    search_width = enemy->x + (enemy->img).width - object->x;
    offsetX_enemy = (enemy->img).width - search_width;
  }

  if ((object->y <= enemy->y) && object->y + (object->img).height > enemy->y) { // Start of Y of enemy is inside of blast
    search_height = object->y + (object->img).height - enemy->y;
    offsetY_magic = (object->img).height - search_height;
  }
  else if ((enemy->y <= object->y) && enemy->y + (enemy->img).height > object->y) { // Start of Y of blast is inside of enemy
    search_height = enemy->y + (enemy->img).height - object->y;
    offsetY_enemy = (enemy->img).height - search_height;
  }

  for (size_t i = 0; i < search_height; i++) {
    for (size_t j = 0; j < search_width; j++) {
      uint32_t color_blast = 0, color_enemy = 0;
      for (size_t byte = 0; byte <  get_bytes_per_pixel(); byte++) {
        color_blast |= (*(object->map + ((j + offsetX_magic) + (i + offsetY_magic) * (object->img).width) * (get_bytes_per_pixel()) + byte)) << (byte * 8);
        color_enemy |= (*(enemy->map + ((j + offsetX_enemy) + (i + offsetY_enemy) * (enemy->img).width) * (get_bytes_per_pixel()) + byte)) << (byte * 8);
      }
      if (color_blast != xpm_transparency_color((object->img).type) &&
          color_enemy != xpm_transparency_color((enemy->img).type))
        return 1;
    }
  }

  return 0;
}

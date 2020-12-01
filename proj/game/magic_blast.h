#include <lcom/lcf.h>
#include "../periferals/video_gr.h"
#include "../img/Wraith_01/Spells_Effect.xpm"

/**
 * @brief Set up the magic blasts available (10)
 * 
 */
void set_magic_blasts_available();

/**
 * @brief Creates a "Magic Blast" XPM, calculates its speed and
 * updates the available blasts
 * @param cursor Mouse cursor
 * @param character Player character
 */
void throw_magic_blast(xpm_object *cursor, xpm_object *character);

/**
 * @brief Prints the blasts
 * If a blast is off-screen, frees its memory and reindexes
 * the blasts
 */
void print_magic_blasts();

/**
 * @brief Reindexes the magic blasts
 * 
 * @param positions Position of freed up blast object
 */
void reindex_magic_blasts(size_t positions);

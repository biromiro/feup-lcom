#ifndef MAGIC_BLAST_H
#define MAGIC_BLAST_H

#include "../img/Wraith_01/Spells_Effect.xpm"
#include "../periferals/video_gr.h"
#include <lcom/lcf.h>

/**
 * @brief Set up the magic blasts available (10)
 * 
 */
void set_magic_blasts_available();

/**
 * @brief Frees all memory related to the magic blasts
 * 
 */
void free_magic_blasts();
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

/**
 * @brief Get the magic blasts
 * 
 * @return XPM objects array 
 */
xpm_object **get_magic_blasts();

/**
 * @brief Get the Total Blasts
 * 
 * @return Number of total blasts 
 */
size_t get_total_blasts();

/**
 * @brief Get the Available Blasts
 * 
 * @return Number of available blasts 
 */
size_t get_available_blasts();

/**
 * @brief Decrements the available blasts variable
 * 
 */
void decrement_available_blasts();

/**
 * @brief Resets the magic blasts array to its initial values
 * 
 */
void reset_magic_blasts();

#endif

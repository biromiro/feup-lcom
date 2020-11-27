#include <lcom/lcf.h>

int (_vbe_get_mode_info)(uint16_t mode, vbe_mode_info_t* vm);

/**
 * @brief Draws a given pixel with a given color
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @param color the color to draw
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);

/**
 * @brief Calculates a similar 16 bit RGB color from a 32 bit one
 *
 * @param color the color to convert
 * 
 * @return the 16 bit approximation
 */
uint16_t (get_16_bit_color) (uint32_t color);

/**
 * @brief Gets the 8 bit red color component from a 32 bit RGB color
 *
 * @param color the original color
 * 
 * @return the red component
 */
uint8_t (get_red) (uint32_t color);

/**
 * @brief Gets the 8 bit green color component from a 32 bit RGB color
 *
 * @param color the original color
 * 
 * @return the green component
 */
uint8_t (get_green) (uint32_t color);

/**
 * @brief Gets the 8 bit blue color component from a 32 bit RGB color
 *
 * @param color the original color
 * 
 * @return the blue component
 */
uint8_t (get_blue) (uint32_t color);

/**
 * @brief Gets the 8 bit alpha color component from a 32 bit RGB color
 *
 * @param color the original color
 * 
 * @return the alpha component
 */
uint8_t (get_alpha) (uint32_t color);

/**
 * @brief Prints a 'rainbow' matrix filling the screen given a number of rectanges and its first * color
 *
 * @param indexed true if the color mode is indexed
 * @param no_rectangles the number of rectangles in both x and y directions
 * @param first the color of the first rectangle of the matrix
 * @param step a value to calculate following rectangle's color
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
void vg_print_matrix(bool indexed, uint8_t no_rectangles, uint32_t first, uint8_t step);

/**
 * @brief Gets the corresponding indexed color given parameters
 * 
 * @param row the current row of the rectangle
 * @param col the current column of the rectangle
 * @param no_rectangles the number of rectangles in both x and y directions
 * @param first the color of the first rectangle of the matrix
 * @param step a value to calculate following rectangle's color
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
uint8_t get_indexed_color(unsigned row, unsigned col, uint32_t first, uint8_t no_rectangles, uint8_t step);

/**
 * @brief Gets the corresponding color (in RGB format) given parametersr
 *
 * @param row the current row of the rectangle
 * @param col the current column of the rectangled
 * @param no_rectangles the number of rectangles in both x and y directions
 * @param first the color of the first rectangle of the matrix
 * @param step a value to calculate following rectangle's color
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
uint32_t get_color(unsigned row, unsigned col, uint32_t first, uint8_t no_rectangles, uint8_t step);

uint16_t get_h_resolution();

uint16_t get_v_resolution();

int swap_buffer();

int get_current_buffer(struct reg86 *r);

uint8_t* loadXPM(xpm_map_t xpm, enum xpm_image_type type, xpm_image_t *img);

void print_xpm(uint16_t x, uint16_t y,  uint8_t* map, xpm_image_t *img);

void erase_xpm(uint16_t x, uint16_t y, uint8_t* map, xpm_image_t *img);

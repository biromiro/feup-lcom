#include <lcom/lcf.h>

#ifndef XPM_OBJECTS
#define XPM_OBJECTS
/**
 * @brief Struct to store our XPM objects
 * 
 */
struct {
  char *ID; ///< ID of xpm_object
  uint8_t *map; ///< Video Mem allocated for XPM 
  xpm_image_t img; ///< Image of XPM
  int x,y, x_speed,y_speed; ///< Position and Speeds
}typedef xpm_object;

/**
 *@brief Struct to store an animated XPM 
 */
struct{
  xpm_object* obj; ///<
  int aspeed, cur_speed, num_fig, cur_fig;
  char **map;
}typedef animated_xpm_object;
#endif

/**
 * @brief Gets the VBE information
 * 
 * @param mode VBE mode
 * @param vm struct that stores the retrieved infomation
 * 
 * @return 0 on success, 1 otherwise
 * 
 */
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
 * @brief Gets the corresponding color (in RGB format) given parameters
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

/**
 * @brief Gets the horizontal resolution
 * 
 * @return Horizontal resolution
 */
uint16_t get_h_resolution();

/**
 * @brief Gets the vertical resolution
 * 
 * @return Vertical Resolution 
 */
uint16_t get_v_resolution();

/**
 * @brief Get the bytes per pixel of the mode
 * 
 * @return bytes per pixel 
 */
unsigned get_bytes_per_pixel();

/**
 * @brief Swaps between buffers
 * 
 * @return 0 on success, -1 otherwise 
 */
int swap_buffer();

/**
 * @brief Gets the current buffer
 * 
 * @param r REG86 struct
 * @return 0 on success 
 */
int get_current_buffer(struct reg86 *r);

/**
 * @brief Loads an XPM
 * 
 * @param xpm XPM map
 * @param type XPM type
 * @param img XPM image
 * @return XPM map
 */
uint8_t* loadXPM(xpm_map_t xpm, enum xpm_image_type type, xpm_image_t *img);

/**
 * @brief Prints a XPM
 * 
 * @param xpm XPM object to be printed
 * @param mirrored Control flag, for displaying mirrored XPMs
 */
void print_xpm(xpm_object* xpm, bool mirrored);

/**
 * @brief Prints over a XPM with the transparent colour
 * 
 * @param xpm XPM to be covered
 */
void erase_xpm(xpm_object* xpm);

/**
 * @brief Creates a sprite object
 * 
 * @param sprite XMP map
 * @param ID ID of object
 * @param x X position
 * @param y Y position
 * @return xpm_object* 
 */
xpm_object *create_sprite(xpm_map_t sprite, char *ID, int x, int y);

/**
 * @brief Creates a animated sprite object
 * 
 * @param xpms Array of XPM maps
 * @param num_of_sprites Number of sprites in array
 * @param ID ID of object
 * @param x X position
 * @param y Y position
 * @param aspeed Speed of the animation
 * @return animated_xpm_object* 
 */
animated_xpm_object* create_animated_sprite(xpm_map_t* xpms, int num_of_sprites , char *ID,int x, int y, int aspeed);

/**
 * @brief Prints an animated sprite
 * 
 * @param animated_sprite Animated spite object
 * @param mirrored Control flag, for displaying mirrored XPMs
 */
void print_animated_sprite(animated_xpm_object* animated_sprite, bool mirrored);

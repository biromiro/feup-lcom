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
  bool mirrored;
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

void set_background(xpm_object* background);

void set_hud(xpm_object* xpm);

void draw_static_buffer_pixels(uint16_t x, uint16_t y, uint32_t color, char* buffer);

void print_background();

void print_hud();

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
void print_xpm(xpm_object* xpm);

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
 */
void print_animated_sprite(animated_xpm_object* animated_sprite);

void* get_buffer_to_draw();

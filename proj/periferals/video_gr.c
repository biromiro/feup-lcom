#include "video_gr.h"

vbe_mode_info_t vmi_p;
static void *video_mem; /* frame-buffer VM addres (static global variable*/
static void *video_mem_sec;
static void *video_mem_third;
static char* background;
static unsigned bytes_per_pixel;
static uint16_t h_res;
static uint16_t v_res;

int(_vbe_get_mode_info)(uint16_t mode, vbe_mode_info_t *vm) {
  struct reg86 r;
  mmap_t map;

  lm_alloc(sizeof(vbe_mode_info_t), &map);

  memset(&r, 0, sizeof(r)); /* zero the structure */

  r.ax = 0x4F01;
  r.cx = mode;
  r.es = PB2BASE(map.phys);
  r.di = PB2OFF(map.phys);
  r.intno = 0x10;

  if (sys_int86(&r) != OK) {
    printf("\tvbe_get_mode_info(): sys_int86() failed \n");
    return -1;
  }

  *vm = *(vbe_mode_info_t *) map.virt;
  lm_free(&map);
  return 0;
}

void *(vg_init)(uint16_t mode) {

  _vbe_get_mode_info(mode, &vmi_p);

  v_res = vmi_p.YResolution;
  h_res = vmi_p.XResolution;

  struct minix_mem_range mr, mr_secondary, mr_third;
  unsigned int vram_base; /* VRAM's physical addresss */
  unsigned int vram_base_secondary_buffer;
  unsigned int vram_base_third_buffer;
  unsigned int vram_size; /* VRAM's size, but you can use
              the frame-buffer size, instead */
  int r;

  /* Use VBE function 0x01 to initialize vram_base and vram_size */

  bytes_per_pixel = (vmi_p.BitsPerPixel + 7) / 8;

  vram_base = vmi_p.PhysBasePtr;
  vram_size = h_res * v_res * bytes_per_pixel;
  vram_base_secondary_buffer = vmi_p.PhysBasePtr + vram_size;
  vram_base_third_buffer = vmi_p.PhysBasePtr + (vram_size)*2;

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size * 3;

  mr_secondary.mr_base = (phys_bytes) vram_base_secondary_buffer;
  mr_third.mr_base = (phys_bytes) vram_base_third_buffer;

  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

  video_mem_sec = vm_map_phys(SELF, (void *) mr_secondary.mr_base, vram_size);

  video_mem_third = vm_map_phys(SELF, (void *) mr_third.mr_base, vram_size);

  if (video_mem == MAP_FAILED || video_mem_sec == MAP_FAILED || video_mem_third == MAP_FAILED) {
    panic("couldn't map video memory");
    return NULL;
  }

  memset(video_mem, 0, vram_size);
  memset(video_mem_sec, 0, vram_size);
  memset(video_mem_third, 0, vram_size);

  struct reg86 r86;

  /* Specify the appropriate register values */

  memset(&r86, 0, sizeof(r86)); /* zero the structure */

  r86.intno = 0x10; /* BIOS video services */
  r86.ah = 0x4F;    /* Set Video Mode function */
  r86.al = 0x02;    /* 80x25 graphics mode*/
  r86.bx = mode | BIT(14);

  /* Make the BIOS call */

  if (sys_int86(&r86) != OK) {
    printf("\tvg_init(): sys_int86() failed \n");
    return NULL;
  }

  background = malloc(sizeof(char)*vram_size);
  
  return video_mem;
}

void set_background(xpm_object* xpm){
  for (size_t i = 0; i < (xpm->img).height; i++) {
    for (size_t j = 0; j < (xpm->img).width; j++) {
      uint32_t color = 0;
      for (size_t byte = 0; byte < bytes_per_pixel; byte++) {
        color |= (*(xpm->map + (j + i * (xpm->img).width) * bytes_per_pixel + byte)) << (byte * 8);
      }
      if (color == xpm_transparency_color((xpm->img).type))
        continue;
      draw_static_buffer_pixels(xpm->x + j, xpm->y + i, color, background);
    }
  }
}

void draw_static_buffer_pixels(uint16_t x, uint16_t y, uint32_t color, char* buffer){
  if (x >= h_res || y >= v_res)
    return;
  memcpy(buffer + (h_res*y + x)*bytes_per_pixel, &color, bytes_per_pixel);
}

void print_background(){
  memcpy(video_mem_sec, background, h_res*v_res*bytes_per_pixel);
}

int(vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
  if (x >= h_res || y >= v_res)
    return 1;
  memcpy((char*) video_mem_sec + (h_res*y + x)*bytes_per_pixel, &color, bytes_per_pixel);
  return 0;
}

uint16_t get_h_resolution() {
  return h_res;
}

uint16_t get_v_resolution() {
  return v_res;
}

unsigned get_bytes_per_pixel() {
  return bytes_per_pixel;
}

int swap_buffer() {

  struct reg86 r;

  memset(&r, 0, sizeof(struct reg86));

  get_current_buffer(&r);

  if (r.dx == 0) {
    memset(&r, 0, sizeof(struct reg86));
    r.dx = v_res;
  }
  else if (r.dx == v_res){
    memset(&r, 0, sizeof(struct reg86));
    r.dx = 2*v_res;
  }else{
    memset(&r, 0, sizeof(struct reg86));
    r.dx = 0;
  }

  r.ax = 0x4F07;
  r.bx = 0x00;
  r.cx = 0x00;
  r.intno = 0x10;

  if (sys_int86(&r) != OK) {
    printf("\tsetCurrentBUffer(): sys_int86() failed \n");
    return -1;
  }

  void *temp = video_mem;
  video_mem = video_mem_sec;
  video_mem_sec = video_mem_third;
  video_mem_third = temp;
  return 0;
}

int get_current_buffer(struct reg86 *r) {

  r->ax = 0x4F07;
  r->bx = 0x01;
  r->intno = 0x10;

  if (sys_int86(r) != OK) {
    printf("\tgetCurrentBUffer(): sys_int86() failed \n");
    return -1;
  }

  return 0;
}

uint8_t *loadXPM(xpm_map_t xpm, enum xpm_image_type type, xpm_image_t *img) {
  return xpm_load(xpm, type, img);
}

void print_xpm(xpm_object *xpm) {
  for (size_t i = 0; i < (xpm->img).height; i++) {
    for (size_t j = 0; j < (xpm->img).width; j++) {
      uint32_t color = 0;
      for (size_t byte = 0; byte < bytes_per_pixel; byte++) {
        color |= (*(xpm->map + (j + i * (xpm->img).width) * bytes_per_pixel + byte)) << (byte * 8);
      }
      if (color == xpm_transparency_color((xpm->img).type))
        continue;
      if (xpm->mirrored)
        vg_draw_pixel(xpm->x + ((xpm->img).width - j-1), xpm->y + i, color);
      else
        vg_draw_pixel(xpm->x + j, xpm->y + i, color);
    }
  }
}

void erase_xpm(xpm_object *xpm) {

  for (size_t i = 0; i < (xpm->img).height; i++) {
    //memset(buffer + (x+(y+i)*h_res)*bytes_per_pixel,*(map+(i*img.width)*bytes_per_pixel),img.width*bytes_per_pixel);
    for (size_t j = 0; j < (xpm->img).width; j++) {
      vg_draw_pixel(xpm->x + j, xpm->y + i, xpm_transparency_color((xpm->img).type));
    }
  }
}

xpm_object *create_sprite(xpm_map_t sprite, char *ID, int x, int y) {

  xpm_object *object = malloc(sizeof(xpm_object));

  xpm_image_t img;
  uint8_t *map = loadXPM(sprite, XPM_8_8_8_8, &img);

  object->ID = ID;
  object->map = map;
  object->img = img;
  object->x = x;
  object->y = y;
  object->x_speed = 0;
  object->y_speed = 0;
  object->mirrored = false;

  return object;
}

animated_xpm_object *create_animated_sprite(xpm_map_t *xpms, int num_of_sprites, char *ID, int x, int y, int aspeed) {

  animated_xpm_object *animated_sprite = malloc(sizeof(animated_xpm_object));

  animated_sprite->obj = create_sprite(xpms[0], ID, x, y);

  animated_sprite->map = malloc(sizeof(char *) * num_of_sprites);

  animated_sprite->map[0] = (char *) animated_sprite->obj->map;

  animated_sprite->aspeed = aspeed;
  animated_sprite->num_fig = num_of_sprites;
  animated_sprite->cur_speed = 0;
  animated_sprite->cur_fig = 0;

  for (size_t i = 0; i < (size_t) num_of_sprites; i++) {
    xpm_map_t view = xpms[i];
    xpm_image_t imgView;
    uint8_t *map = loadXPM(view, XPM_8_8_8_8, &imgView);

    animated_sprite->map[i] = (char *) map;
  }

  return animated_sprite;
}

void print_animated_sprite(animated_xpm_object *animated_sprite) {
  xpm_object current_sprite;

  if (animated_sprite->cur_speed == animated_sprite->aspeed) {
    animated_sprite->cur_speed = 0;
    animated_sprite->cur_fig = (animated_sprite->cur_fig + 1) % animated_sprite->num_fig;
  }

  current_sprite.x = animated_sprite->obj->x;
  current_sprite.y = animated_sprite->obj->y;
  current_sprite.mirrored = animated_sprite->obj->mirrored;
  current_sprite.img = animated_sprite->obj->img;
  current_sprite.map = (uint8_t *) animated_sprite->map[animated_sprite->cur_fig];

  print_xpm(&current_sprite);
}

void* get_buffer_to_draw(){
  return video_mem_sec;
}

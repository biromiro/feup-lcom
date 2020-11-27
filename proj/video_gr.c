#include "video_gr.h"

vbe_mode_info_t vmi_p;
static void *video_mem;         /* frame-buffer VM addres (static global variable*/ 
static void *video_mem_sec;
static unsigned bytes_per_pixel;
static uint16_t h_res;
static uint16_t v_res;

int (_vbe_get_mode_info)(uint16_t mode, vbe_mode_info_t* vm){
    struct reg86 r;
    mmap_t map;

    lm_alloc(sizeof(vbe_mode_info_t), &map);

    memset(&r, 0, sizeof(r));	/* zero the structure */

    r.ax = 0x4F01;
    r.cx = mode;
    r.es = PB2BASE(map.phys);
    r.di = PB2OFF(map.phys);
    r.intno = 0x10;

    if( sys_int86(&r) != OK ) { 
      printf("\tvbe_get_mode_info(): sys_int86() failed \n");
      return -1;
    }

    *vm = *(vbe_mode_info_t*) map.virt;
    lm_free(&map);
    return 0;
}

void* (vg_init)(uint16_t mode) {

  _vbe_get_mode_info(mode,&vmi_p);

  v_res = vmi_p.YResolution;
  h_res = vmi_p.XResolution;

  struct minix_mem_range mr, mr_secondary;
  unsigned int vram_base;  /* VRAM's physical addresss */
  unsigned int vram_base_secondary_buffer;
  unsigned int vram_size;  /* VRAM's size, but you can use
              the frame-buffer size, instead */
  int r;				    

  /* Use VBE function 0x01 to initialize vram_base and vram_size */

  bytes_per_pixel = (vmi_p.BitsPerPixel+7)/8;

  vram_base = vmi_p.PhysBasePtr;
  vram_base_secondary_buffer = vmi_p.PhysBasePtr + h_res*v_res*bytes_per_pixel;
  vram_size = h_res * v_res * bytes_per_pixel;

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes) vram_base;	
  mr.mr_limit = mr.mr_base + vram_size*2;  

  mr_secondary.mr_base = (phys_bytes) vram_base_secondary_buffer;

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  video_mem_sec = vm_map_phys(SELF, (void *)mr_secondary.mr_base, vram_size);

  if(video_mem == MAP_FAILED || video_mem_sec == MAP_FAILED) {
    panic("couldn't map video memory");
    return NULL;
  }

  memset(video_mem,0,vram_size);
  memset(video_mem_sec,0,vram_size);

  struct reg86 r86;

  /* Specify the appropriate register values */
  
  memset(&r86, 0, sizeof(r86));	/* zero the structure */

  r86.intno = 0x10; /* BIOS video services */
  r86.ah = 0x4F;    /* Set Video Mode function */
  r86.al = 0x02;    /* 80x25 graphics mode*/
  r86.bx = mode | BIT(14);
  
  /* Make the BIOS call */

  if( sys_int86(&r86) != OK ) {
    printf("\tvg_init(): sys_int86() failed \n");
    return NULL;
  }

  return video_mem;
}

int (vg_draw_rectangle) (uint16_t	x, uint16_t	y, uint16_t	width, uint16_t	height,uint32_t color) {
  for(int i=0; i<height;i++){
    vg_draw_hline(x,y+i,width,color);
  }
  return 0;
} 		

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len,uint32_t color) {
  for(int i=0; i<len;i++) {
    vg_draw_pixel(x+i,y,color);
  }
  return 0;
}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
  if(x>h_res || y>v_res) return 1;
  char* buffer = (char*) video_mem_sec;
  uint8_t color_byte;
  if(bytes_per_pixel == 2) color = get_16_bit_color(color);
  for(unsigned j=0;j<bytes_per_pixel;j++) {
    color_byte = (uint8_t) color;
    buffer[(h_res*y + x)*bytes_per_pixel + j] = color_byte;
    color = color >> 8;
  }
  return 0;
}

uint16_t (get_16_bit_color) (uint32_t color){
  uint8_t a,r,g,b;
  r = get_red(color) >> 3;
  g = ((vmi_p.RsvdMaskSize) == 0) ? get_green(color) >> 2 : get_green(color) >> 3;
  b = get_blue(color) >> 3;
  a = get_alpha(color) >> 7;
  if((vmi_p.RsvdMaskSize) == 0) return (r << 11) | (g << 5) | (b);
  else return (a<<15) | (r << 10) | (g << 5) | (b);
}

uint8_t (get_red) (uint32_t color){
  return (color << 8) >> 24;
}

uint8_t (get_green) (uint32_t color){
  return (color << 16) >> 24;
}

uint8_t (get_blue) (uint32_t color){
  return (color << 24) >> 24;
}

uint8_t (get_alpha) (uint32_t color){
  return color >> 24;
}

void vg_print_matrix( bool indexed, uint8_t no_rectangles, uint32_t first, uint8_t step){
  unsigned size_of_col = h_res/no_rectangles, size_of_row = v_res/no_rectangles;
  unsigned extra_col = h_res%no_rectangles, extra_row = v_res%no_rectangles;
  unsigned row, col;
  uint32_t black_color = 0x00, color_to_fill;

  for(row = 0; row<no_rectangles; row++){
    for(col = 0; col<no_rectangles; col++){
      color_to_fill = indexed ? get_indexed_color(row,col,first,no_rectangles,step) : get_color(row,col,first,no_rectangles,step);
      vg_draw_rectangle(col*size_of_col, row*size_of_row, size_of_col, size_of_row, color_to_fill);     
    }
    vg_draw_rectangle(col*size_of_col, row*size_of_row, extra_col, extra_row, black_color);
  }
  vg_draw_rectangle(0,row*size_of_row,vmi_p.XResolution,extra_row,black_color);

}

uint8_t get_indexed_color(unsigned row, unsigned col, uint32_t first, uint8_t no_rectangles, uint8_t step){
    return (first + (row * no_rectangles + col) * step) % (1 << vmi_p.BitsPerPixel);
}

uint32_t get_color(unsigned row, unsigned col, uint32_t first, uint8_t no_rectangles, uint8_t step){

  uint8_t r,g,b;
  r = (get_red(first) + col * step) % (1 << vmi_p.RedMaskSize);
  g = (get_green(first) + row * step) % (1 << vmi_p.GreenMaskSize); 
  b = (get_blue(first) + (col + row) * step) % (1 << vmi_p.BlueMaskSize);

  return (r << 16 | g << 8 | b);
}

uint16_t get_h_resolution(){
  return h_res;
}

uint16_t get_v_resolution(){
  return v_res;
}

int swap_buffer(){

  struct reg86 r;

  memset(&r,0,sizeof(struct reg86));
  
  get_current_buffer(&r);

  if(r.dx == 0){
    memset(&r,0,sizeof(struct reg86));
    r.dx = v_res;
  }else{
    memset(&r,0,sizeof(struct reg86));
    r.dx = 0;
  }

  r.ax = 0x4F07;
  r.bx = 0x00;
  r.cx = 0x00;
  r.intno = 0x10;

  if( sys_int86(&r) != OK ) { 
      printf("\tsetCurrentBUffer(): sys_int86() failed \n");
      return -1;
    }

  void* temp = video_mem;
  video_mem = video_mem_sec;
  video_mem_sec = temp;
  memcpy(video_mem_sec,video_mem,h_res*v_res*bytes_per_pixel);
  return 0;
}

int get_current_buffer(struct reg86 *r){

  r->ax = 0x4F07;  
  r->bx = 0x01;
  r->intno = 0x10;

   if( sys_int86(r) != OK ) { 
      printf("\tsetCurrentBUffer(): sys_int86() failed \n");
      return -1;
    }

  return 0;
}

uint8_t* loadXPM(xpm_map_t xpm, enum xpm_image_type type, xpm_image_t *img) {
  return xpm_load(xpm,type,img);
}

void print_xpm(uint16_t x, uint16_t y,  uint8_t* map, xpm_image_t *img){
  for(size_t i=0; i<img->height; i++){
      //memset(buffer + (x+(y+i)*h_res)*bytes_per_pixel,*(map+(i*img.width)*bytes_per_pixel),img.width*bytes_per_pixel);
      for(size_t j=0; j<img->width; j++){
        vg_draw_pixel(x+j, y+i,*(map + (j+i*img->width)*bytes_per_pixel));
      }
  }
}

void erase_xpm(uint16_t x, uint16_t y, uint8_t* map, xpm_image_t *img){

  for(size_t i=0; i<img->height; i++){
      //memset(buffer + (x+(y+i)*h_res)*bytes_per_pixel,*(map+(i*img.width)*bytes_per_pixel),img.width*bytes_per_pixel);
      for(size_t j=0; j<img->width; j++){
        vg_draw_pixel(x+j, y+i,xpm_transparency_color(img->type));
      }
  }
}

struct xpm_object *loadXPMs(){

  struct  xpm_object *objects = malloc(sizeof(struct xpm_object)*20);

  xpm_map_t character=character_xpm;
  xpm_image_t img;
  uint8_t* map = loadXPM(character,XPM_8_8_8_8,&img);

  objects[0].ID = "Character"; objects[0].map = map;
  objects[0].img = img;


  return objects;
}

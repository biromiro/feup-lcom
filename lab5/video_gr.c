#include "video_gr.h"

vbe_mode_info_t vmi_p;
static void *video_mem;         /* frame-buffer VM addres (static global variable*/ 
static unsigned bytesPerPixel;
           
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

  struct minix_mem_range mr;
  unsigned int vram_base;  /* VRAM's physical addresss */
  unsigned int vram_size;  /* VRAM's size, but you can use
              the frame-buffer size, instead */
  int r;				    

  /* Use VBE function 0x01 to initialize vram_base and vram_size */

  bytesPerPixel = (vmi_p.BitsPerPixel+7)/8;

  vram_base = (phys_bytes) vmi_p.PhysBasePtr;
  vram_size = vmi_p.XResolution * vmi_p.YResolution * bytesPerPixel;

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes) vram_base;	
  mr.mr_limit = mr.mr_base + vram_size;  

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  if(video_mem == MAP_FAILED) {
    panic("couldn't map video memory");
    return NULL;
  }

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
  uint8_t *copy_mem= video_mem;
  uint8_t colorByte;
  if(bytesPerPixel == 2) color = get16BitColor(color);
  for(unsigned j=0;j<bytesPerPixel;j++) {
    colorByte = color & 0xFF;
    copy_mem[(vmi_p.XResolution*y + x)*bytesPerPixel + j] = colorByte;
    color = color >> 8;
  }
  return 0;
}

uint16_t (get16BitColor) (uint32_t color){
  uint8_t a,r,g,b;
  r = getRed(color) >> 3;
  g = ((vmi_p.RsvdMaskSize) == 0) ? getGreen(color) >> 2 : getGreen(color) >> 3;
  b = getBlue(color) >> 3;
  a = getAlpha(color) >> 7;
  if((vmi_p.RsvdMaskSize) == 0) return (r << 11) | (g << 5) | (b);
  else return (a<<15) | (r << 10) | (g << 5) | (b);
}

uint8_t (getRed) (uint32_t color){
  return (color << 8) >> 24;
}

uint8_t (getGreen) (uint32_t color){
  return (color << 16) >> 24;
}

uint8_t (getBlue) (uint32_t color){
  return (color << 24) >> 24;
}

uint8_t (getAlpha) (uint32_t color){
  return color >> 24;
}

void vg_print_matrix( bool indexed, uint8_t no_rectangles, uint32_t first, uint8_t step){
  unsigned sizeOfCol = vmi_p.XResolution/no_rectangles, sizeOfRow = vmi_p.YResolution/no_rectangles;
  unsigned extraCol = vmi_p.XResolution%no_rectangles, extraRow = vmi_p.YResolution%no_rectangles;
  unsigned row, col;
  uint32_t blackColor = 0x00, colorToFill;

  for(row = 0; row<no_rectangles; row++){
    for(col = 0; col<no_rectangles; col++){
      colorToFill = indexed ? getIndexedColor(row,col,first,no_rectangles,step) : getColor(row,col,first,no_rectangles,step);
      vg_draw_rectangle(col*sizeOfCol, row*sizeOfRow, sizeOfCol, sizeOfRow, colorToFill);     
    }
    vg_draw_rectangle(col*sizeOfCol, row*sizeOfRow, extraCol, extraRow, blackColor);
  }
  vg_draw_rectangle(0,row*sizeOfRow,vmi_p.XResolution,extraRow,blackColor);

}

uint8_t getIndexedColor(unsigned row, unsigned col, uint32_t first, uint8_t no_rectangles, uint8_t step){
    return (first + (row * no_rectangles + col) * step) % (1 << vmi_p.BitsPerPixel);
}

uint32_t getColor(unsigned row, unsigned col, uint32_t first, uint8_t no_rectangles, uint8_t step){
  uint8_t r,g,b;
  r = (getRed(first) + col * step) % (1 << vmi_p.RedMaskSize);
  g = (getGreen(first) + row * step) % (1 << vmi_p.GreenMaskSize); 
  b = (getBlue(first) + (col + row) * step) % (1 << vmi_p.BlueMaskSize);

  return (r << 16 | g << 8 | b);
}

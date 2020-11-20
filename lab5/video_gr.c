#include "video_gr.h"

vbe_mode_info_t vmi_p;
static void *video_mem;         /* frame-buffer VM addres (static global variable*/ 
           
void* (vg_init)(uint16_t mode) {

  vbe_get_mode_info(mode,&vmi_p);

  struct minix_mem_range mr;
  unsigned int vram_base;  /* VRAM's physical addresss */
  unsigned int vram_size;  /* VRAM's size, but you can use
              the frame-buffer size, instead */
  int r;				    

  /* Use VBE function 0x01 to initialize vram_base and vram_size */

  vram_base = (phys_bytes) vmi_p.PhysBasePtr;
  vram_size = vmi_p.XResolution * vmi_p.YResolution * (vmi_p.BitsPerPixel+7)/8;

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

//600x400 x=599 width=10
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
  char *copy_mem= video_mem;
  int RGB=0;
  uint8_t colour;
  bool reserved=vmi_p.RsvdMaskSize==0;
  if(!reserved) RGB=1;
  for(int j=0;j<(vmi_p.BitsPerPixel+7)/8;j++) {
    uint8_t colourByte = (uint8_t) (color>>(j*8));
    switch(RGB){
      case 3:
        colour=(colourByte>>(8-vmi_p.RedMaskSize));
        break;
      case 2:
        colour=(colourByte>>(8-vmi_p.GreenMaskSize));
        break;
      case 1:
        colour=(colourByte>>(8-vmi_p.BlueMaskSize));
        break;
      case 0:
        colour=(colourByte>>(8-vmi_p.RsvdMaskSize));
        break;
      default:
      break;
    }
    RGB++;
    copy_mem[(vmi_p.XResolution*y+x)*((vmi_p.BitsPerPixel+7)/8)+j]=colour;
    if((RGB==4 && reserved)) RGB=1;
    else if((RGB==4&&!reserved)) RGB=0;
  }

  return 0;
}




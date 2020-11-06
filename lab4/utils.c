//
// Created by biromiro on 23/10/20.
//
#include <lcom/lcf.h>

#ifdef LAB3
uint32_t cnt = 0;
#endif

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {

  *lsb = (uint8_t) val;

  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {

  val = val >> 8;
  *msb = (uint8_t) val;

  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {

  //getting word from designated port
  uint32_t val = 0;
  int res = sys_inb(port, &val);
  #ifdef LAB3
  cnt++;
  #endif
  //checks if the read was valid
  if(res != 0){
    printf("Invalid sys_inb");
  }

  *value = (uint8_t) val;

  return res;
}



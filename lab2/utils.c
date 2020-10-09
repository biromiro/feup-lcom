#include <lcom/lcf.h>

#include <stdint.h>

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

  uint32_t val = 0;
  int res = sys_inb(port, &val);

  if(res != 0){
    printf("Invalid sys_inb");
  }

  *value = (uint8_t) val;

  return res;
}

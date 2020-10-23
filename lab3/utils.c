//
// Created by biromiro on 23/10/20.
//
#include "utils.h"

int cnt = 0;

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



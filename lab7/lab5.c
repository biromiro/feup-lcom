// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "serial_port.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab7/g06/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab7/g06/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  uint8_t count = 5;
  ser_init();
  do{
    if(read_byte() == OK){
      uint8_t val = pop(get_received_queue());
      printf("value = %x\n", val);
      count--;
    }
  }while(count);
  ser_exit();
  return 0;
}

int(video_test_controller)() {
  ser_init();
  send_byte(0x65);
  send_byte(0x66);
  send_byte(0x67);
  send_byte(0x68);
  send_byte(0x69);
  ser_exit();
  return 0;
}

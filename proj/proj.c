// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/proj.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "periferals/interrupt_handler.h"

extern bool finished;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/g06/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g06/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}
int(proj_main_loop)(int argc, char *argv[]) {

  if (OK != initialize()) {
    printf("Could not initialize video mode and/or interrupts!");
    return 1;
  }

  while (!finished) {
    interrupt_call_receiver();
  }

  if (OK != finish()) {
    printf("Could not fully reset the state of Minix!");
    return 1;
  }

  return 0;
}

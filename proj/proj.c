// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/proj.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "periferals/interrupt_handler.h"
#include "periferals/rtc.h"

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
  /*if (OK != initialize()) {
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

  return 0;*/
  int ipc_status,r;
  uint8_t irq_set,counter=0;
  message msg;

  if(rtc_subscribe_int(&irq_set)!=0) {
    printf("Error subscribing timer\n");
    return 1;
  }

  set_interrupts(ALARM,1);

  printf("subscribed");
  while(counter < 10) {

    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }else{
      printf("i'm free!");
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: 
          if (msg.m_notify.interrupts &irq_set) {
            rtc_updater();
            char* date = print_date();
             printf("%s\n", date);
             counter++;
          }
          break;
        default:
          break;
      }
    } else {
      
    }
  }

  rtc_unsubscribe_int();

  

  return 0;
}
